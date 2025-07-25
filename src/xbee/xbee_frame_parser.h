#ifndef XBEE_FRAME_PARSER_H
#define XBEE_FRAME_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa o parser de frames XBee, resetando seu estado.
 */
void xbee_frame_parser_init(void);

/**
 * @brief Processa um unico byte recebido da UART para montar um frame XBee.
 * Deve ser chamada com bytes "unescaped".
 * @param byte O byte a ser processado.
 */
void xbee_frame_parser_process_byte(uint8_t byte);

/**
 * @brief Verifica se um frame XBee completo e valido foi recebido.
 * @param frame_data Ponteiro para um ponteiro que recebera o endereco do buffer do frame.
 * @param frame_length Ponteiro para uma variavel que recebera o comprimento do frame.
 * @return True se um frame completo foi recebido e validado, False caso contrario.
 */
bool xbee_frame_parser_get_frame(uint8_t **frame_data, uint16_t *frame_length);

#ifdef __cplusplus
}
#endif

#endif // XBEE_FRAME_PARSER_H