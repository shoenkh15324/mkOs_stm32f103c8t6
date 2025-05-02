cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME mkOs_stm32f103c8t6)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

# Compile Flag
add_compile_options(-Wno-unused-parameter)
add_compile_options(-Wno-format)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    Core/mkOS/source/application/*.cpp
    Core/mkOS/source/application/project/stm32f103/*.cpp
    Core/mkOS/source/system/*.cpp
    Core/mkOS/source/oal/*.cpp
    Core/mkOS/source/hal/*.cpp
    Core/mkOS/source/hal/uart/*.cpp
    Core/mkOS/source/hal/led/*.cpp
    Core/mkOS/external/freeRtos/*.c
    Core/mkOS/external/freeRtos/portable/ARM_CM3/*.c
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    ${SRC_FILES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    Core/mkOS
    Core/mkOS/include
    Core/mkOS/include/application
    Core/mkOS/include/application/project/stm32f103
    Core/mkOS/include/system
    Core/mkOS/include/oal
    Core/mkOS/include/hal
    Core/mkOS/include/hal/uart
    Core/mkOS/include/hal/led
    Core/mkOS/external
    Core/mkOS/external/freeRtos/include
    Core/mkOS/external/freeRtos/portable/ARM_CM3
    #Core/mkOS/external/freeRtos/portable/ARM_CM4F
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
    MKOS_ENABLE
    PROJECT_F103
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
