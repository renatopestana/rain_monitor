#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "hardware/uart.h"

#define RAIN_SENSOR_GPIO            22      // Pino GPIO para o sensor de efeito hall
#define DHT22_GPIO_PIN              14      // Pino GPIO para o sensor DHT22
#define XBEE_UART_ID                uart0   // UART a ser utilizada (uart0 ou uart1)
#define XBEE_TX_PIN                 0       // Pino GPIO para TX da UART
#define XBEE_RX_PIN                 1       // Pino GPIO para RX da UART
#define XBEE_BAUD_RATE              230400  //Taxa de baud para o XBee

#define PULSES_PER_MM               5.5     // Exemplo: Quantos pulsos para 1mm de chuva. CALIBRE!
#define DEBOUNCE_TIME_US            50000   // 50ms de debounce para o sensor

#define START_DELIMITER             0x7E
#define ESCAPE_BYTE                 0x7D
#define XOR_MASK                    0x20
#define API_FRAME_TYPE_TX_REQUEST   0x10  // Transmit Request Frame (0x10)
#define API_FRAME_TYPE_RX_PACKET    0x90  // ZB Receive Packet (0x90)
#define API_FRAME_TYPE_TX_STATUS    0x8B  // Transmit Status Frame (0x8B)

#define RX_BUFFER_SIZE              256     // Tamanho do buffer de bytes brutos recebidos da UART (para ISR)
#define MAX_XBEE_FRAME_SIZE         250     // Tamanho maximo de um frame XBee (payload + overhead)
#define MAX_PAYLOAD_TEXT_SIZE       (MAX_XBEE_FRAME_SIZE - 12) // Tamanho maximo para payload de texto em 0x90

#define BROADCAST_INTERVAL_SECONDS  30                  // Enviar mensagem de identificacao a cada 30 segundos
#define DEVICE_ID                   "RAIN_MONITOR_001"  // Identificador unico do dispositivo

#define WATCHDOG_TIMEOUT_MS         5000                // WDT 5 segundos

#endif // CONFIG_H