#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "config.h"
#include "hardware/watchdog.h"
#include "rain_gauge/rain_gauge.h"
#include "dht22_sensor/dht22_sensor.h"
#include "xbee/xbee_uart.h"
#include "xbee/xbee_frame_parser.h"
#include "xbee/xbee_frame_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

int main() {
    stdio_init_all(); 

    rain_gauge_init();
    dht22_sensor_init(DHT22_GPIO_PIN);
    xbee_uart_init();
    xbee_frame_parser_init();

    printf("Rain Monitor - Iniciado!\n");
    printf("Device ID: %s\n", DEVICE_ID);
    printf("UART Baud Rate: %d\n", XBEE_BAUD_RATE);
    printf("DHT22 sensor GPIO: %d\n", DHT22_GPIO_PIN);


    watchdog_enable(WATCHDOG_TIMEOUT_MS, false);
    printf("Watchdog Timer enabled with timeout %d ms.\n", WATCHDOG_TIMEOUT_MS);

    if (watchdog_caused_reboot()) {
        printf("Restart caused by Watchdog Timer!\n");
    }

    uint64_t last_broadcast_time = time_us_64();

    while (1) {
        watchdog_update();

        while (xbee_uart_bytes_available() > 0) {
            uint8_t received_byte = xbee_uart_read_byte();
            xbee_frame_parser_process_byte(received_byte);
        }

        uint8_t *received_frame_data;
        uint16_t received_frame_length;
        if (xbee_frame_parser_get_frame(&received_frame_data, &received_frame_length)) {
            xbee_frame_handler_process_frame(received_frame_data, received_frame_length);
        }

        if (time_us_64() - last_broadcast_time >= (BROADCAST_INTERVAL_SECONDS * 1000000ULL)) {
            float rain_mm = (float)rain_gauge_get_pulses() / PULSES_PER_MM;
            float humidity = -99.9f;
            float temperature = -99.9f;
            
            if (dht22_sensor_read_data(&humidity, &temperature)) {
               printf("Humidity: %.1f%%, Temperature: %.1fC\n", humidity, temperature);
            } 
            else {
                printf("Failed reading sensor.\n");
            }

            char payload_tx_buffer[MAX_PAYLOAD_TEXT_SIZE];
            snprintf(payload_tx_buffer, sizeof(payload_tx_buffer), "ID:%s,Rain:%.2fmm,Pulses:%lu,Humidity:%.1f%%,Temp:%.1fC", 
                     DEVICE_ID, rain_mm, rain_gauge_get_pulses(), humidity, temperature);
            
            xbee_frame_handler_send_broadcast_frame(payload_tx_buffer);
            
            rain_gauge_reset_pulses();
            last_broadcast_time = time_us_64();
        }
        
        sleep_ms(10); 
    }
    return 0;
}

#ifdef __cplusplus
}
#endif