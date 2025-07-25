#include "xbee_frame_parser.h"
#include "config.h"
#include <stdio.h>

typedef enum {
    WAITING_FOR_START_DELIMITER,
    READING_LENGTH_MSB,
    READING_LENGTH_LSB,
    READING_FRAME_DATA,
    READING_CHECKSUM
} XbeeFrameParsingState;

static XbeeFrameParsingState current_parsing_state;
static uint8_t xbee_frame_buffer[MAX_XBEE_FRAME_SIZE];
static uint16_t xbee_frame_index;
static uint16_t xbee_expected_frame_length;


/**
 * @brief Calcula o checksum de uma sequencia de bytes.
 * O checksum e calculado somando todos os bytes, exceto o Start Delimiter e Length,
 * e subtraindo o resultado de 0xFF.
 * @param data Ponteiro para os dados do frame (API ID + Payload).
 * @param length O comprimento dos dados para o calculo do checksum.
 * @return O valor do checksum calculado.
 */
static uint8_t calculate_checksum(uint8_t *data, uint16_t length) {
    uint8_t checksum_byte = 0;
    for (int i = 0; i < length; i++) {
        checksum_byte += data[i];
    }
    return 0xFF - checksum_byte;
}

void xbee_frame_parser_init(void) {
    current_parsing_state = WAITING_FOR_START_DELIMITER;
    xbee_frame_index = 0;
    xbee_expected_frame_length = 0;
}

void xbee_frame_parser_process_byte(uint8_t byte) {
    switch (current_parsing_state) {
        case WAITING_FOR_START_DELIMITER:
            if (byte == START_DELIMITER) {
                xbee_frame_index = 0;
                xbee_expected_frame_length = 0;
                current_parsing_state = READING_LENGTH_MSB;
                printf("XBee Parser: Start delimiter (0x7E) detected.\n");
            }
            break;

        case READING_LENGTH_MSB:
            xbee_expected_frame_length = (uint16_t)byte << 8;
            current_parsing_state = READING_LENGTH_LSB;
            break;

        case READING_LENGTH_LSB:
            xbee_expected_frame_length |= byte;
            printf("XBee Parser: Frame lenght (MSB+LSB): %d bytes.\n", xbee_expected_frame_length);
            if (xbee_expected_frame_length > 0 && xbee_expected_frame_length <= MAX_XBEE_FRAME_SIZE) {
                current_parsing_state = READING_FRAME_DATA;
            } else {
                printf("XBee Parser: Error frame lenght (%d). Reseting.\n", xbee_expected_frame_length);
                xbee_frame_parser_init(); // Resetar o parser
            }
            break;

        case READING_FRAME_DATA:
            if (xbee_frame_index < xbee_expected_frame_length) {
                xbee_frame_buffer[xbee_frame_index++] = byte;
            }

            if (xbee_frame_index == xbee_expected_frame_length) {
                current_parsing_state = READING_CHECKSUM;
            } else if (xbee_frame_index >= MAX_XBEE_FRAME_SIZE) { // Buffer overflow durante a leitura de dados
                printf("XBee Parser: Frame Buffer full during data reading. Reseting.\n");
                xbee_frame_parser_init(); // Resetar o parser
            }
            break;

        case READING_CHECKSUM:
            {
                uint8_t received_checksum = byte;
                uint8_t calculated_checksum = calculate_checksum(xbee_frame_buffer, xbee_expected_frame_length);

                if (received_checksum == calculated_checksum) {
                    printf("XBee Parser: Checksum OK. Frame Ready.\n");
                } else {
                    printf("XBee Parser: Checksum Error! Received: 0x%02X, Calculated: 0x%02X. Dircarding frame.\n",
                           received_checksum, calculated_checksum);
                    xbee_frame_parser_init(); // Resetar o parser
                }
                // Em ambos os casos (checksum OK ou erro), o estado de parsing deve ser resetado
                // para aguardar o prÃ³ximo frame.
                current_parsing_state = WAITING_FOR_START_DELIMITER;
            }
            break;
    }
}

bool xbee_frame_parser_get_frame(uint8_t **frame_data, uint16_t *frame_length) {
    if (current_parsing_state == WAITING_FOR_START_DELIMITER && xbee_expected_frame_length > 0) {
        *frame_data = xbee_frame_buffer;
        *frame_length = xbee_expected_frame_length;
        xbee_expected_frame_length = 0; // Marcar como lido
        return true;
    }
    return false;
}