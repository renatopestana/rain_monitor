#include "app_commands.h"
#include "rain_gauge/rain_gauge.h"
#include <string.h>
#include <stdio.h>


void app_commands_handle_command(const char *command_payload) {
    if (strcmp(command_payload, "RESET_PULSE_COUNTER") == 0) {
        rain_gauge_reset_pulses();
        printf(">>> 'RESET_PULSE_COUNTER' command received. Rain counter reset. <<<\n");
    } 

    // Adicionar novos comandos aqui no futuro

    else if (strcmp(command_payload, "GET_STATUS") == 0) {
        printf(">>> 'GET_STATUS' command received. Status: %lu pulses. <<<\n", rain_gauge_get_pulses());
    }
    else {
        printf("Unrecognized command received: '%s'\n", command_payload);
    }
}