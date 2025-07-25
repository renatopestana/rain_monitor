// src/app_commands/app_commands.h
#ifndef APP_COMMANDS_H
#define APP_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trata comandos especifico da aplicacao recebidos via XBee.
 * @param command_payload A string do comando recebido.
 */
void app_commands_handle_command(const char *command_payload);

#ifdef __cplusplus
}
#endif

#endif // APP_COMMANDS_H