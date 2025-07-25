#include "xbee_frame_handler.h"
#include "config.h"
#include "xbee_uart.h"
#include "app_commands/app_commands.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief Calcula o checksum de uma sequencia de bytes para envio.
 * @param data Ponteiro para os dados do frame (API ID + Payload).
 * @param length O comprimento dos dados para o calculo do checksum.
 * @return O valor do checksum calculado.
 */
static uint8_t calculate_checksum_for_tx(uint8_t *data, uint16_t length) {
    uint8_t checksum_byte = 0;
    for (int i = 0; i < length; i++) {
        checksum_byte += data[i];
    }
    return 0xFF - checksum_byte;
}

void xbee_frame_handler_process_frame(uint8_t *frame_data, uint16_t frame_length) {
    printf("\n--- FRAME XBEE RECEIVED (Handler) ---\n");
    printf("Payload lenght + API ID: %d bytes\n", frame_length);
    printf("Frame Data (API ID + Payload): ");
    for (int i = 0; i < frame_length; i++) {
        printf("%02X ", frame_data[i]);
    }
    printf("\n-------------------------------------\n");

    if (frame_length > 0) {
        uint8_t api_frame_id = frame_data[0];
        printf("API Frame ID: 0x%02X\n", api_frame_id);

        switch (api_frame_id) {
            case API_FRAME_TYPE_RX_PACKET: // ZB Receive Packet (0x90)
                if (frame_length >= 12) { // 1 (API ID) + 8 (64-bit Addr) + 2 (16-bit Addr) + 1 (Options) = 12
                    printf("Frame Type: ZB Received Packet (0x90)\n");
                    printf("Source Address 64-bit: %02X%02X%02X%02X%02X%02X%02X%02X\n",
                           frame_data[1], frame_data[2], frame_data[3], frame_data[4],
                           frame_data[5], frame_data[6], frame_data[7], frame_data[8]);
                    printf("Source Address 16-bit: %02X%02X\n", frame_data[9], frame_data[10]);
                    printf("Receive Options: 0x%02X\n", frame_data[11]);
                    
                    uint16_t payload_start_index = 12;
                    if (frame_length > payload_start_index) {
                        printf("Payload RF Data: ");
                        for (int i = payload_start_index; i < frame_length; i++) {
                            printf("%02X ", frame_data[i]);
                        }
                        printf("\n");
                        
                        char payload_text[MAX_PAYLOAD_TEXT_SIZE]; 
                        uint16_t actual_payload_len = frame_length - payload_start_index;
                        if (actual_payload_len < sizeof(payload_text)) {
                            memcpy(payload_text, &frame_data[payload_start_index], actual_payload_len);
                            payload_text[actual_payload_len] = '\0'; // Null-terminate the string
                            printf("Payload (text): %s\n", payload_text);

                            app_commands_handle_command(payload_text);

                        } else {
                            printf("Payload text too big!\n");
                        }
                    }
                } else {
                    printf("Error: ZB Received Packet (0x90) with invalid lenght.\n");
                }
                break;

            case API_FRAME_TYPE_TX_STATUS: // TX Status (0x8B)
                if (frame_length >= 6) {
                    printf("Frame type: TX Status (0x8B)\n");
                    printf("Frame ID: 0x%02X\n", frame_data[1]);
                    printf("16-bit Destination Address: %02X%02X\n", frame_data[2], frame_data[3]);
                    printf("Transmit Retry Count: %d\n", frame_data[4]);
                    printf("Delivery Status: 0x%02X ", frame_data[5]);
                    
                    // InterpretaÃƒÂ§ÃƒÂ£o do Delivery Status
                    switch (frame_data[5]) {
                        case 0x00: printf("(Success)\n"); break;
                        case 0x01: printf("(MAC ACK Failure)\n"); break;
                        case 0x02: printf("(CCA Failure)\n"); break;
                        case 0x03: printf("(Invalid Destination Endpoint)\n"); break;
                        case 0x04: printf("(Network ACK Failure)\n"); break;
                        case 0x05: printf("(Not Joined to Network)\n"); break;
                        case 0x15: printf("(Invalid Binding Table Index)\n"); break;
                        case 0x18: printf("(Resource Error)\n"); break;
                        case 0x21: printf("(No Free Buffers)\n"); break;
                        case 0x22: printf("(Last Hop ACK Failure)\n"); break;
                        case 0x23: printf("(Indirect Message Unaddressed)\n"); break;
                        case 0x74: printf("(Data Payload Too Large)\n"); break;
                        case 0x75: printf("(Indirect Message Dropped)\n"); break;
                        default: printf("(Unknown Status)\n"); break;
                    }
                    printf("Discovery Status: 0x%02X ", frame_data[6]);
                    switch (frame_data[6]) {
                        case 0x00: printf("(No Discovery Overhead)\n"); break;
                        case 0x01: printf("(Address Discovery)\n"); break;
                        case 0x02: printf("(Route Discovery)\n"); break;
                        case 0x03: printf("(Address and Route Discovery)\n"); break;
                        case 0x40: printf("(Extended Timeout Discovery)\n"); break;
                        default: printf("(Unknown Status)\n"); break;
                    }
                    printf("\n");
                } else {
                    printf("Error: TX Status (0x8B) with invalid lenght.\n");
                }
                break;

            default:
                printf("Frame ID 0x%02X not implemented.\n", api_frame_id);
                break;
        }
    }
}

void xbee_frame_handler_send_broadcast_frame(const char *payload_str) {
    uint16_t payload_len = strlen(payload_str);
    
    // Transmit Request (0x10) frame structure:
    // Start Delimiter (1 byte) + Length (2 bytes) + Frame Data (n bytes) + Checksum (1 byte)
    // Frame Data:
    // [API Frame ID (1)] [Frame ID (1)] [64-bit Dest Addr (8)] [16-bit Dest Net Addr (2)] [Broadcast Radius (1)] [Transmit Options (1)] [RF Data (n)]

    // Comprimento da parte "Frame Data" (API Frame ID ate o final do RF Data)
    // 1 (API ID 0x10) + 1 (Frame ID) + 8 (64-bit Dest Addr) + 2 (16-bit Dest Net Addr) + 1 (Broadcast Radius) + 1 (Transmit Options) + payload_len
    uint16_t frame_data_len = 1 + 1 + 8 + 2 + 1 + 1 + payload_len;
    
    // Buffer temporario para construir o "Frame Data" antes do escape e checksum
    // Tamanho maximo possivel (MAX_XBEE_FRAME_SIZE - 2 para Length - 1 para Checksum)
    uint8_t temp_frame_data[MAX_XBEE_FRAME_SIZE - 3]; 
    uint16_t temp_frame_data_idx = 0;

    // Preencher o temp_frame_data
    temp_frame_data[temp_frame_data_idx++] = API_FRAME_TYPE_TX_REQUEST; // API Frame ID (0x10)
    temp_frame_data[temp_frame_data_idx++] = 0x01; // Frame ID (use 0x01 para habilitar resposta de TX Status)

    // 64-bit Destination Address (Broadcast = 0x000000000000FFFF)
    temp_frame_data[temp_frame_data_idx++] = 0x00; temp_frame_data[temp_frame_data_idx++] = 0x00;
    temp_frame_data[temp_frame_data_idx++] = 0x00; temp_frame_data[temp_frame_data_idx++] = 0x00;
    temp_frame_data[temp_frame_data_idx++] = 0x00; temp_frame_data[temp_frame_data_idx++] = 0x00;
    temp_frame_data[temp_frame_data_idx++] = 0xFF; temp_frame_data[temp_frame_data_idx++] = 0xFF;

    // 16-bit Destination Network Address (Broadcast = 0xFFFE)
    temp_frame_data[temp_frame_data_idx++] = 0xFF; temp_frame_data[temp_frame_data_idx++] = 0xFE;

    temp_frame_data[temp_frame_data_idx++] = 0x00; // Broadcast Radius (0x00 = max hops)
    temp_frame_data[temp_frame_data_idx++] = 0x00; // Transmit Options (0x00 = default)

    // RF Data (Payload)
    for (int i = 0; i < payload_len; i++) {
        if (temp_frame_data_idx < (MAX_XBEE_FRAME_SIZE - 3)) { // Evitar overflow
             temp_frame_data[temp_frame_data_idx++] = (uint8_t)payload_str[i];
        } else {
            printf("Error: Payload too big for XBEE frame buffer.\n");
            return;
        }
    }

    // Calcular Checksum sobre temp_frame_data
    uint8_t checksum = calculate_checksum_for_tx(temp_frame_data, frame_data_len);

    // Iniciar envio do frame pela UART
    uart_putc(XBEE_UART_ID, START_DELIMITER); // Start Delimiter (0x7E)

    // Enviar Length (MSB, LSB) - estes bytes tambem precisam de escape se forem especiais
    xbee_uart_putc_escaped(XBEE_UART_ID, (uint8_t)((frame_data_len >> 8) & 0xFF)); // MSB
    xbee_uart_putc_escaped(XBEE_UART_ID, (uint8_t)(frame_data_len & 0xFF));       // LSB

    // Enviar todos os bytes do "Frame Data" com escape
    xbee_uart_send_bytes_escaped(XBEE_UART_ID, temp_frame_data, frame_data_len);

    // Enviar o Checksum com escape
    xbee_uart_putc_escaped(XBEE_UART_ID, checksum);

    printf("XBee: Broadcast Frame (0x10) sent with payload of %d bytes.\n", payload_len);
}