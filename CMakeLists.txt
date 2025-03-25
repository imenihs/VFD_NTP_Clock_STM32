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
set(CMAKE_CXX_STANDARD 17)  # C++17を使用
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME VFD_NTP_Clock)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)
# enable_language(CXX ASM)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# C++ コンパイルオプション（STM32F103最適化）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections")

# C コンパイルオプション（STM32F103最適化）
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -ffunction-sections -fdata-sections")

# リンク時の最適化（不要なコードを削除）
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -u _printf_float") # printfにfloat変換機能を追加する
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")


# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# C++ コンパイルオプション 特定フォルダ内の警告抑制
# Core/Src/ フォルダ内のすべてのCファイルに適用
file(GLOB SRC_WARN_OFF Core/Src/*.c)
foreach(source_file ${SRC_WARN_OFF})
    set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
endforeach()

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    
)
# .cppのソースファイルを自動検知してコンパイル対象に追加する
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    User/*.cpp
    User/*.c
    Core/Src/*.cpp
    Core/Src/*.c
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${SRC_FILES}

)

# すべての .c ファイルを C++ としてコンパイル
# set_source_files_properties(${SRC_FILES} PROPERTIES LANGUAGE C CXX)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    C:/ST/STM32CubeCLT_1.15.1/GNU-tools-for-STM32/arm-none-eabi/include
    C:/ST/STM32CubeCLT_1.15.1/GNU-tools-for-STM32/arm-none-eabi/include/c++/12.3.1
    C:/ST/STM32CubeCLT_1.15.1/GNU-tools-for-STM32/arm-none-eabi/include/c++/12.3.1/arm-none-eabi
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    stdc++  # C++ standard library

    # Add user defined libraries
)

# target_link_options(${CMAKE_PROJECT_NAME} PRIVATE --specs=nosys.specs)
