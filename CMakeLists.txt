# Generated Cmake Pico project file
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# == Configuração automática da Extensão Raspberry Pi Pico ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ==========================================================

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Importa o SDK
include(pico_sdk_import.cmake)

project(jogo_multidiscip C CXX ASM)

# Inicializa o SDK
pico_sdk_init()

# --- ARQUIVOS FONTE ---
# Adicione aqui todos os arquivos .c que compõem seu projeto
add_executable(jogo_multidiscip 
    Brand_Quiz.c 
    ssd1306_libs/ssd1306.c 
)

pico_set_program_name(jogo_multidiscip "jogo_multidiscip")
pico_set_program_version(jogo_multidiscip "0.1")

# USB habilitada para o Serial Monitor
pico_enable_stdio_uart(jogo_multidiscip 0)
pico_enable_stdio_usb(jogo_multidiscip 1)

# --- BIBLIOTECAS DO SDK ---
target_link_libraries(jogo_multidiscip
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    hardware_i2c
    hardware_adc
    hardware_pwm
)

# --- DIRETÓRIOS DE INCLUDE ---
# Isso diz ao compilador para procurar arquivos .h nestas pastas
target_include_directories(jogo_multidiscip PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/ssd1306_libs
)

pico_add_extra_outputs(jogo_multidiscip)