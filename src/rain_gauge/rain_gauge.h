#ifndef RAIN_GAUGE_H
#define RAIN_GAUGE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa o GPIO para o sensor de chuva e configura a ISR.
 */
void rain_gauge_init(void);

/**
 * @brief Retorna o numero atual de pulsos de chuva.
 * @return O numero de pulsos de chuva.
 */
uint32_t rain_gauge_get_pulses(void);

/**
 * @brief Reseta o contador de pulsos de chuva para zero.
 */
void rain_gauge_reset_pulses(void);

#ifdef __cplusplus
}
#endif

#endif // RAIN_GAUGE_H