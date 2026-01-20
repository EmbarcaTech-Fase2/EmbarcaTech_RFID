# Controle de Servo com RFID – Raspberry Pi Pico (RP2040)

<p align="center">
  <img src="image.png" alt="EmbarcaTech" width="420">
</p>

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Raspberry Pi Pico](https://img.shields.io/badge/Raspberry%20Pi%20Pico-00A040?style=for-the-badge&logo=raspberrypi&logoColor=white)
![Pico SDK](https://img.shields.io/badge/Pico%20SDK-1.5%2B-blue?style=for-the-badge)

## Descrição do Projeto

Projeto embarcado que lê uma tag/cartão RFID (RC522) e aciona um servo motor para simular a abertura de uma porta. Um display OLED SSD1306 mostra o estado do sistema e o UID do cartão detectado. O firmware é escrito em C utilizando o Pico SDK para o microcontrolador RP2040 (Raspberry Pi Pico).

## Visão Geral

- Arquivo principal: [RFID.c](RFID.c) — inicializa PWM do servo, SPI para o RC522 e I2C para o display; lê o UID e aciona o servo conforme regras.
- Periféricos:
  - Servo no PWM a 50 Hz (GPIO28).
  - RC522 via SPI0.
  - SSD1306 via I2C1 (400 kHz), endereço 0x3C.
- Saída: mensagens no OLED e `printf` via USB/serial.

## Hardware e Ligações

Raspberry Pi Pico (RP2040):

- Servo motor:
  - Sinal: GPIO28.
  - Alimentação externa recomendada (5V) e GND comum com o Pico.

- Módulo RFID RC522 (SPI0):
  - `SCK` → GPIO18
  - `MOSI` → GPIO19
  - `MISO` → GPIO16
  - `SS`/`CS` → GPIO17
  - `RST` → GPIO20
  - `3V3` e `GND` conforme o módulo

- Display OLED SSD1306 (I2C1):
  - `SDA` → GPIO14
  - `SCL` → GPIO15
  - Endereço: `0x3C`

## Funcionamento

- O sistema exibe tela de espera no OLED e aguarda aproximação de um cartão/tag.
- Ao detectar um cartão, mostra o UID e verifica se corresponde ao UID autorizado.
- Para o UID autorizado, o servo gira para a posição de abertura e retorna após um tempo.
- Parâmetros principais no código:
  - `MIN_PULSE = 500µs` e `MAX_PULSE = 2500µs` (mapeamento 0–180°).
  - I2C1 em 400 kHz; PWM a 50 Hz; SPI0 em 4 MHz.

UID de exemplo autorizado (ajustável no código): `00 FC 95 7C`.

## Como Compilar e Gravar

### Via Tarefas do VS Code

- Compile: Task “Compile Project”.
- Carregue `.uf2` via `picotool`: Task “Run Project”.
- Flash via depurador CMSIS‑DAP/OpenOCD: Task “Flash”.

### Observações

- Certifique-se de ter o Pico SDK configurado e as ferramentas (`ninja`, `picotool`, `openocd`) disponíveis conforme as tasks do workspace.
- O artefato gerado principal é `build/RFID.uf2`.

## Personalização

- Altere o UID autorizado em [RFID.c](RFID.c) na verificação dentro do laço principal.
- Ajuste `MIN_PULSE`/`MAX_PULSE` conforme seu servo.
- Atualize pinos se necessário nos headers da biblioteca do RC522.
- Se desejar outro endereço do OLED, ajuste a constante `endereco` em [RFID.c](RFID.c).

## Estrutura

- Código da aplicação: [RFID.c](RFID.c)
- Bibliotecas: `lib/mfrc522.*`, `lib/ssd1306.*`, `lib/font.h`
- Build: pasta `build/` (CMake/Ninja), gera `RFID.uf2`

## Créditos

- Biblioteca MFRC522 adaptada para Pico SDK.
