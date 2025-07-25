#ifndef XBEE_UART_H
#define XBEE_UART_H

#include <stdint.h>
#include <stdbool.h>
#include "hardware/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa a UART para comunicacao com o modulo XBee.
 */
void xbee_uart_init(void);

/**
 * @brief Retorna o numero de bytes disponiveis no buffer de recepcao da UART.
 * @return O numero de bytes disponiveis
 */
uint16_t xbee_uart_bytes_available(void);

/**
 * @brief Le um byte do buffer de recepcao da UART.
 * @return O byte lido.
 */
uint8_t xbee_uart_read_byte(void);

/**
 * @brief Envia um byte pela UART, aplicando a logica de escape se necessario (API Mode 2).
 * @param uart instancai da UART (e.g., XBEE_UART_ID).
 * @param byte O byte a ser enviado.
 */
void xbee_uart_putc_escaped(uart_inst_t *uart, uint8_t byte);

/**
 * @brief Envia um buffer de bytes pela UART, aplicando a logica de escape para cada byte.
 * @param uart instancia da UART (e.g., XBEE_UART_ID).
 * @param data Ponteiro para o array de bytes a ser enviado.
 * @param length O numero de bytes no array.
 */
void xbee_uart_send_bytes_escaped(uart_inst_t *uart, const uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif // XBEE_UART_H