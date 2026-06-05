# Introduccion

## Objetivos de aprendizaje

- Conocer la estructura básica de un microcontrolador (reloj, bus, memoria, periféricos SIO).

- Instalar y configurar el toolchain (Pico SDK, CMake, OpenOCD, GDB).

- Comprender el uso de registros para el control de GPIO.

- Implementar FSM con antirrebote.

- Configurar SysTick (1 ms) y medir latencias/jitter vía trazas.

## Materiales sugeridos

- Hardware: 
    - Raspberry Pi Pico
    - protoboard
    - LED
    - resistencia (330–1kΩ)
    - Push Button 
    - Resistencias de 10kΩ
    - cables 
    - osciloscopio

- Software: 
    - Visual Studio Code
    - Pico SDK
    - CMake (≥3.13) 
    - arm-none-eabi-gcc
    - OpenOCD
    - GDB

## Material de apoyo

- Documentación del Pico SDK (GPIO, clocks, depuración, etc.).

- Capítulos introductorios de Curso práctico para programación de AVR (refuerza bases de bitwise/registries/GPIO).

- Apuntes sobre temporización y FSM simples (este repo).

