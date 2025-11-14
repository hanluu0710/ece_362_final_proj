cmake_minimum_required(VERSION 3.13)

# Set path to Pico SDK (relative path since your folder already has pico-sdk/)
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/pico-sdk")

# Import the SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ece_362_final_proj C CXX ASM)

# Initialize the SDK
pico_sdk_init()

# Create executable
add_executable(ece_362_final_proj display.c)

# Link libraries
target_link_libraries(ece_362_final_proj pico_stdlib hardware_spi hardware_gpio)

# Create additional build outputs (UF2, BIN, etc.)
pico_add_extra_outputs(ece_362_final_proj)
