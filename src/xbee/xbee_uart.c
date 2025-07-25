#include "xbee_uart.h"
#include "config.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include <stdio.h>

static uint8_t uart_rx_raw_buffer[RX_BUFFER_SIZE];
static volatile uint16_t uart_rx_raw_head = 0;
static volatile uint16_t uart_rx_raw_tail = 0;
static volatile bool escape_next_byte_isr = false;

static void uart_rx_isr(void) {
    while (uart_is_readable(XBEE_UART_ID)) {
        uint8_t char_received = uart_getc(XBEE_UART_ID);
        printf("%X ",char_received);
        
        if (escape_next_byte_isr) {
            char_received ^= XOR_MASK;
            escape_next_byte_isr = false;
        } else if (char_received == ESCAPE_BYTE) {
            escape_next_byte_isr = true;
            continue;
        }

        uart_rx_raw_buffer[uart_rx_raw_head] = char_received;
        uart_rx_raw_head = (uart_rx_raw_head + 1) % RX_BUFFER_SIZE;

        if (uart_rx_raw_head == uart_rx_raw_tail) {
            printf("Error: UART Buffer RX full.\n");
            uart_rx_raw_tail = (uart_rx_raw_tail + 1) % RX_BUFFER_SIZE;
        }
    }
}

void xbee_uart_init(void) {
    uart_init(XBEE_UART_ID, XBEE_BAUD_RATE);
    gpio_set_function(XBEE_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(XBEE_RX_PIN, GPIO_FUNC_UART);
    
    irq_set_exclusive_handler(UART0_IRQ, uart_rx_isr); 
    irq_set_enabled(UART0_IRQ, true); 
    uart_set_irq_enables(XBEE_UART_ID, true, false); 
}

uint16_t xbee_uart_bytes_available(void) {
    if (uart_rx_raw_head >= uart_rx_raw_tail) {
        return uart_rx_raw_head - uart_rx_raw_tail;
    } else {
        return RX_BUFFER_SIZE - (uart_rx_raw_tail - uart_rx_raw_head);
    }
}

uint8_t xbee_uart_read_byte(void) {
    if (xbee_uart_bytes_available() == 0) {
        return 0;
    }
    uint8_t byte = uart_rx_raw_buffer[uart_rx_raw_tail];
    uart_rx_raw_tail = (uart_rx_raw_tail + 1) % RX_BUFFER_SIZE;
    return byte;
}

void xbee_uart_putc_escaped(uart_inst_t *uart, uint8_t byte) {
    if (byte == START_DELIMITER || byte == ESCAPE_BYTE || byte == 0x11 || byte == 0x13) {
        uart_putc(uart, ESCAPE_BYTE);
        uart_putc(uart, byte ^ XOR_MASK);
    } else {
        uart_putc(uart, byte);
    }
}

void xbee_uart_send_bytes_escaped(uart_inst_t *uart, const uint8_t *data, uint16_t length) {
    for (int i = 0; i < length; i++) {
        xbee_uart_putc_escaped(uart, data[i]);
    }
}