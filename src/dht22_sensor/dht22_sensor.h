#ifndef DHT22_SENSOR_H
#define DHT22_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inicializa o pino GPIO para comunicacao com o sensor DHT22.
 * @param gpio_pin O nÃƒÂºmero do pino GPIO conectado ao DATA do DHT22.
 */
void dht22_sensor_init(uint8_t gpio_pin);

/**
 * @brief Le os dados de umidade e temperatura do sensor DHT22.
 * Esta duncao e bloqueante e sensivel a tempo.
 * @param humidity Ponteiro para uma variavel float onde a umidade sera armazenada.
 * @param temperature Ponteiro para uma variavel float onde a temperatura sera armazenada.
 * @return true se a leitura for bem-sucedida e o checksum for valido, false caso contrario.
 */
bool dht22_sensor_read_data(float *humidity, float *temperature);

#ifdef __cplusplus
}
#endif

#endif // DHT22_SENSOR_H