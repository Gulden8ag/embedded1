# Introduction

## Learning objectives

- Understand the basic structure of a microcontroller (clock, bus, memory, SIO peripherals).

- Install and configure the toolchain (Pico SDK, CMake, OpenOCD, GDB).

- Understand the use of registers for GPIO control.

- Implement FSMs with debouncing.

- Configure SysTick (1 ms) and measure latency/jitter via traces.

## Suggested materials

- Hardware:
    - Raspberry Pi Pico
    - breadboard
    - LED
    - resistor (330–1kΩ)
    - push button
    - 10kΩ resistors
    - jumper wires
    - oscilloscope

- Software:
    - Visual Studio Code
    - Pico SDK
    - CMake (≥3.13)
    - arm-none-eabi-gcc
    - OpenOCD
    - GDB

## Supporting material

- Pico SDK documentation (GPIO, clocks, debugging, etc.).

- Introductory chapters of *Curso práctico para programación de AVR* (reinforces bitwise/register/GPIO fundamentals).

- Notes on timing and simple FSMs (this repo).
