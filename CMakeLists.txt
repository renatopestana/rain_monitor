# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rain_monitor C CXX)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Adicione os arquivos fonte de cada módulo
set(SOURCE_FILES
    src/main.c
    src/rain_gauge/rain_gauge.c
    src/dht22_sensor/dht22_sensor.c
    src/xbee/xbee_uart.c
    src/xbee/xbee_frame_parser.c
    src/xbee/xbee_frame_handler.c
    src/app_commands/app_commands.c
)

# Add executable. Default name is the project name, version 0.1
# Esta linha deve vir ANTES de pico_set_program_name
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES}) # Usar ${SOURCE_FILES} aqui, não apenas main.c

pico_set_program_name(${CMAKE_PROJECT_NAME} "rain_monitor")
pico_set_program_version(${CMAKE_PROJECT_NAME} "0.1")

# Adicione os diretórios de inclusão para os módulos
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/rain_gauge
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dht22_sensor 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xbee
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_commands
)

pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)
pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)

# Link com as bibliotecas do Pico SDK
target_link_libraries(${CMAKE_PROJECT_NAME}
    pico_stdlib
    hardware_gpio
    hardware_uart
)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})