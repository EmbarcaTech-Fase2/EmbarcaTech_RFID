#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "mfrc522.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define SERVO_PIN 28
#define MIN_PULSE 500  // Pulso para ~0 graus
#define MAX_PULSE 2500 // Pulso para ~180 graus

long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main()
{
    stdio_init_all();

    // Servo: PWM a 50 Hz (20 ms)
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 19999);
    pwm_init(slice_num, &config, true);

    // Começa em 0°
    int angle = 0;
    int pulse = map(angle, 0, 180, MIN_PULSE, MAX_PULSE);
    pwm_set_gpio_level(SERVO_PIN, pulse);

    // Inicializa MFRC522
    MFRC522Ptr_t mfrc = MFRC522_Init();
    PCD_Init(mfrc, spi0);
    PCD_AntennaOn(mfrc); // Liga antena
    sleep_ms(500);

    // Inicializa display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    char uid_str[24]; // Buffer para UID formatado
    bool cor = true;

    while (1)
    {

        printf("Waiting for card...\n");
        ssd1306_fill(&ssd, !cor);                  // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
        ssd1306_line(&ssd, 3, 25, 123, 25, cor);           // Desenha uma linha
        ssd1306_line(&ssd, 3, 37, 123, 37, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6); // Desenha uma string
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);  // Desenha uma string
        ssd1306_draw_string(&ssd, "RFID RC522", 10, 28);   // Desenha uma string
        ssd1306_draw_string(&ssd, "Aproxime o", 8, 41);    // Desenha uma string
        ssd1306_draw_string(&ssd, "  CARTAO", 8, 52);      // Desenha uma string
        ssd1306_send_data(&ssd);

        while (!PICC_IsNewCardPresent(mfrc))
        {
            sleep_ms(500);
        }

        printf("Card detected! Trying to read UID...\n");

        if (PICC_ReadCardSerial(mfrc))
        {
            // Formata UID em string "XX XX XX XX ..."
            int offset = 0;
            for (int i = 0; i < mfrc->uid.size; i++)
            {
                offset += sprintf(&uid_str[offset], "%02X ", mfrc->uid.uidByte[i]);
            }
            printf("UID: %s\n", uid_str);

            ssd1306_draw_string(&ssd, "UID:       ", 8, 41); // Desenha uma string
            ssd1306_draw_string(&ssd, uid_str, 8, 52);  // Desenha uma string
            ssd1306_send_data(&ssd);                     // Atualiza o display      

            // Verifica UID para acionar o servo motor
            if (mfrc->uid.size == 4) // Certifica-se que tem 4 bytes no UID
            {
                // Cartão UID 00 FC 95 7C aciona o servo motor
                if (mfrc->uid.uidByte[0] == 0x00 &&
                    mfrc->uid.uidByte[1] == 0xFC &&
                    mfrc->uid.uidByte[2] == 0x95 &&
                    mfrc->uid.uidByte[3] == 0x7C)
                {
                    angle = 180;
                    pulse = map(angle, 0, 180, MIN_PULSE, MAX_PULSE);
                    pwm_set_gpio_level(SERVO_PIN, pulse);
                    sleep_ms(3000);
                    pulse = 0;
                    pulse = map(angle, 0, 180, MIN_PULSE, MAX_PULSE);
                }
            }
        }
        else
        {
            printf("Failed to read UID\n");
            ssd1306_draw_string(&ssd, "Falha na", 8, 41);  // Desenha uma string
            ssd1306_draw_string(&ssd, " Leitura", 8, 52);   // Desenha uma string
            ssd1306_send_data(&ssd);
        }
        sleep_ms(2000);
    }
}
