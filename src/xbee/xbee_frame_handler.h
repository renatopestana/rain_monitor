// src/xbee/xbee_frame_handler.h
#ifndef XBEE_FRAME_HANDLER_H
#define XBEE_FRAME_HANDLER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Processa um frame XBee completo e validado, decodificando seu tipo
 * e extraindo as informacoes relevantes.
 * @param frame_data Ponteiro para os dados do frame (API ID + Payload).
 * @param frame_length O comprimento dos dados do frame.
 */
void xbee_frame_handler_process_frame(uint8_t *frame_data, uint16_t frame_length);

/**
 * @brief constroi e envia um frame XBee Transmit Request (0x10) para broadcast.
 * @param payload_str A string de dados a ser enviada como payload.
 */
void xbee_frame_handler_send_broadcast_frame(const char *payload_str);

#ifdef __cplusplus
}
#endif

#endif // XBEE_FRAME_HANDLER_H