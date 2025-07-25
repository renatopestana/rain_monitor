#include "dht22_sensor.h"
#include "config.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"
#include <stdio.h>

static uint8_t dht_gpio_pin;

static bool wait_for_gpio_state(uint expected_state, uint32_t timeout_us) {
    absolute_time_t start_time = get_absolute_time();
    while (gpio_get(dht_gpio_pin) != expected_state) {
        if (absolute_time_diff_us(start_time, get_absolute_time()) > timeout_us) {
            return false; // Timeout
        }
    }
    return true;
}

void dht22_sensor_init(uint8_t gpio_pin) {
    dht_gpio_pin = gpio_pin;
    gpio_init(dht_gpio_pin);
    gpio_pull_up(dht_gpio_pin);
    printf("DHT22 Sensor initialized GPIO %d.\n", dht_gpio_pin);
}

bool dht22_sensor_read_data(float *humidity, float *temperature) {
    uint8_t data[5] = {0};
    uint8_t checksum = 0;
    bool success = false;

    uint32_t ints = save_and_disable_interrupts();

    gpio_set_dir(dht_gpio_pin, GPIO_OUT);
    gpio_put(dht_gpio_pin, 0); // Pull LOW for at least 1ms (18ms recommended)
    sleep_ms(18); // Recommended 18ms
    gpio_put(dht_gpio_pin, 1); // Pull HIGH for 20-40us
    sleep_us(40);
    gpio_set_dir(dht_gpio_pin, GPIO_IN); // Set as input to receive response

    if (!wait_for_gpio_state(0, 100)) { // Wait for LOW
        printf("DHT22: Answer not detected (LOW).\n");
        goto end_read;
    }
    if (!wait_for_gpio_state(1, 100)) { // Wait for HIGH
        printf("DHT22: Answer not detected (HIGH).\n");
        goto end_read;
    }
    
    for (int i = 0; i < 5; i++) { // Para cada um dos 5 bytes
        for (int j = 7; j >= 0; j--) { // Para cada bit (MSB primeiro)
            if (!wait_for_gpio_state(0, 70)) { // Wait for LOW start of bit
                printf("DHT22: Error waiting LOW for bit %d byte %d.\n", j, i);
                goto end_read;
            }
            
            sleep_us(30);

            if (gpio_get(dht_gpio_pin) == 1) {
                data[i] |= (1 << j);
                if (!wait_for_gpio_state(0, 90)) { // Aguarda o fim do pulso HIGH (total ~70us)
                    printf("DHT22: Error waiting end HIGH for bit 1.\n");
                    goto end_read;
                }
            } else {
                if (!wait_for_gpio_state(0, 40)) { // Apenas uma pequena espera de seguranca
                    printf("DHT22: Error waiting end HIGH for bit 0.\n");
                    goto end_read;
                }
            }
        }
    }

    checksum = data[0] + data[1] + data[2] + data[3];
    if (data[4] == checksum) {
        *humidity = (float)((data[0] << 8) | data[1]) / 10.0f;
        *temperature = (float)(((data[2] & 0x7F) << 8) | data[3]) / 10.0f;
        if (data[2] & 0x80) { // Se o bit 7 do byte 3 for 1, temperatura e negativa
            *temperature *= -1.0f;
        }
        success = true;
    } else {
        printf("DHT22: Checksum error! Received: 0x%02X, Calculated: 0x%02X\n", data[4], checksum);
    }

end_read:
    restore_interrupts(ints); // Reabilitar interrupcao
    return success;
}