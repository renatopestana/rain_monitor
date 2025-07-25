#include <stdio.h>
#include "rain_gauge.h"
#include "config.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

static volatile uint32_t rain_pulses = 0;
static absolute_time_t last_pulse_time;

static void rain_sensor_isr(uint gpio, uint32_t events) {
    if (gpio == RAIN_SENSOR_GPIO) {
        absolute_time_t now = get_absolute_time();
        if (absolute_time_diff_us(last_pulse_time, now) > DEBOUNCE_TIME_US) {
            rain_pulses++;
            last_pulse_time = now;
            printf("Rain pulse detected! Pulses count: %lu\n", rain_pulses);
        }
    }
}

void rain_gauge_init(void) {
    gpio_init(RAIN_SENSOR_GPIO);
    gpio_set_dir(RAIN_SENSOR_GPIO, GPIO_IN);
    gpio_pull_up(RAIN_SENSOR_GPIO);
    gpio_set_irq_enabled_with_callback(RAIN_SENSOR_GPIO, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &rain_sensor_isr);
    last_pulse_time = get_absolute_time();
}

uint32_t rain_gauge_get_pulses(void) {
    return rain_pulses;
}

void rain_gauge_reset_pulses(void) {
    rain_pulses = 0;
}