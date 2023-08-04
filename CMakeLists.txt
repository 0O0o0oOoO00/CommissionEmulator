cmake_minimum_required(VERSION 3.25)
project(CommissionEmulator C)

set(CMAKE_C_STANDARD 17)

add_executable(emulator main.c Commission.h CommissionData.h Config.h Emulator.c Emulator.h)
