# ¿Qué es un sistema embebido?

## Definición

Un **sistema embebido** es un sistema de cómputo **diseñado para realizar funciones específicas** dentro de un producto mayor, que interactúa con el mundo físico mediante sensores y actuadores.

!!! important "Importante" 
    Debe ser un producto final, no una PC independiente.

### Rasgos característicos

- **Propósito específico**: realiza una tarea o conjunto acotado de tareas.
- **Interacción físico-digital**: adquisición de variables (sensores) y acción (actuadores).
- **Restricciones duras**: consumo energético, memoria, procesamiento, costo, tamaño.
- **Confiabilidad y disponibilidad**: ciclos de vida largos, operación continua.
- **Tiempo real (frecuente)**: respuestas dentro de límites temporales definidos.
- **Co-diseño HW/SW**: decisión conjunta de electrónica, firmware y software.
- **Seguridad y ciberseguridad**: protección del usuario y del entorno (funcional y digital).

### Comparativa PC vs Sistema Embebido

| Criterio                    | PC propósito general              | Sistema embebido                                   |
| --------------------------- | --------------------------------- | -------------------------------------------------- |
| Alcance funcional           | Amplio, multitarea                | Específico a la aplicación                         |
| Interfaz con usuario        | Teclado, mouse, pantalla          | Puede carecer de UI; LEDs, botones, HMI dedicada   |
| Recursos (CPU/RAM/almacen.) | Abundantes                        | Limitados por costo/energía/espacio                |
| SO                          | Windows/macOS/Linux de escritorio | Bare-metal/RTOS/Linux embebido                     |
| Tiempo real                 | No determinista                   | Frecuentemente determinista                        |
| Energía                     | Red eléctrica                     | Batería/ultrabajo de potencia                      |
| Robustez/ambiental          | Moderada                          | Alta (temperatura, vibración, EMI)                 |
| Ciclo de vida               | Corto/mediano                     | Largo (años o décadas)                             |
| Ciberseguridad funcional    | Importante                        | Crítica (seguridad física + seguridad informática) |

### ¿Qué no es un sistema embebido?

- Una PC de escritorio usada tal cual (no integrada a un producto).
- Un servidor genérico de propósito general.
- Un script en laptop que lee un sensor USB (salvo que la laptop sea parte del producto final).

### Casos de uso

| Dominio      | Producto/Función          | Requisito clave                   | Tiempo real |
| ------------ | ------------------------- | --------------------------------- | ----------- |
| Automotriz   | ECU de frenos (ABS/ESP)   | Latencia y seguridad funcional    | Duro        |
| Médico       | Marcapasos                | Confiabilidad y biocompatibilidad | Duro        |
| Industrial   | Control de motor en línea | Determinismo y robustez           | Duro        |
| Consumo      | Termostato inteligente    | Conectividad y eficiencia         | Blando      |
| IoT portátil | Wearable (smartwatch)     | Bajo consumo y UX                 | Blando      |
| Domótica     | Cerradura inteligente     | Seguridad/cifrado                 | Variable    |


_**Tiempo real duro**_: incumplir una deadline implica fallo del sistema (p. ej., Freno ABS).  
_**Tiempo real blando**_: las demoras degradan la calidad pero no implican fallo (p. ej., audio).
---

## Estructura típica de un sistema embebido

- **MCU Microcontrolador**: CPU + memoria + periféricos en un mismo chip, optimizado para control de E/S en tiempo real.

- **MPU MicroProcesador**: CPU “sola” (memoria y periféricos externos). SoC: término paraguas que puede incluir GPU, radios, etc.

### Componentes Clave:


***Diagrama 1 — Visión de alto nivel***

```mermaid
flowchart LR
  %% ---------- External signals ----------
  CLK["Reloj / PLL"]
  RST["Reset"]
  PWR["Gestión de energía<br/>(LDO / DCDC / Sleep)"]
  EXT_IO["Sensores / Actuadores externos"]

  %% ---------- MCU boundary ----------
  subgraph MCU["Microcontrolador (SoC)"]
    direction LR

    %% Central interconnect
    BUS[["Interconexión SoC<br/>(AXI / AHB / APB)"]]

    %% CPU subsystem (keep it simple at high level)
    subgraph CPU["CPU"]
      direction TB
      CORE["Núcleo (Core)"]
      INTCTRL["Control de interrupciones (NVIC/INTC)"]
      REGFILE["Registros"]
      ALU["ALU (FPU/DSP opcional)"]
      CORE --- INTCTRL
      CORE --- REGFILE
      CORE --- ALU
    end

    %% Memory
    subgraph MEM["Memoria"]
      direction TB
      FLASH[(Flash - Código)]
      SRAM[(SRAM - Datos)]
      EEPROM[(EEPROM/Emulada - Parámetros)]
      TCM[(Cache / TCM opcional)]
    end

    %% Peripherals (grouped)
    subgraph PER["Periféricos"]
      direction TB
      GPIO[[GPIO / I/O digital]]
      TIM[[Timers / PWM / RTC]]
      ANA[[ADC / DAC / Comparador]]
      COM[[UART · SPI · I2C · CAN · USB · ETH]]
      WDT[[Watchdog]]
      DMA[[DMA]]
    end
  end

  %% ---------- Clean wiring via hub ----------
  CPU --- BUS
  MEM --- BUS
  PER --- BUS
  DMA --- BUS

  %% External world
  EXT_IO --- GPIO

  %% Service signals (limit fan-out to reduce clutter)
  CLK --> CPU
  CLK --> PER
  RST --> CPU
  RST --> PER
  PWR --> CPU
  PWR --> MEM
```

| Componente            | Función principal                          | Puntos de diseño / riesgo típico                                           |
|-----------------------|---------------------------------------------|----------------------------------------------------------------------------|
| ALU / FPU / DSP       | Cálculo entero, flotante, señal             | Latencia, precisión, consumo; ¿se justifica FPU?                           |
| Unidad de Control     | Secuenciar instrucciones                    | Soporte ISA                           |
| Registros/PC/SP       | Estado interno y flujo                      | Tamaño de banco, llamadas/ISR anidadas                                     |
| NVIC/INTC             | Gestión de interrupciones                   | Prioridades, latencias, determinismo                                       |
| DMA                   | Transferencia sin CPU                       | Configuración correcta, coherencia de caché                                |
| Flash                 | Almacenar firmware                          | Endurance, tiempos de programación/lectura                                 |
| SRAM                  | Datos en ejecución                          | Tamaño/costos, inicialización y protección                                 |
| EEPROM                | Parámetros persistentes                     | Ciclos de escritura, wear leveling                                         |
| GPIO                  | I/O digital                                 | Pull-ups, debounce, protección ESD                                         |
| Timers/PWM/RTC        | Tiempo, captura/compare, control            | Resolución, jitter, sincronización con ADC                                 |
| ADC/DAC/Comp.         | Interfaces analógicas                       | Ruido, referencia, tasa de muestreo, linealidad                            |
| UART/SPI/I²C/CAN…     | Comunicaciones                              | Velocidad, errores, EMC, protocolos superiores                             |
| Reloj/PLL             | Base de tiempo                              | Estabilidad, arranque, consumo                                             |
| Reset/Watchdog        | Recuperación de fallos                      | Ventanas, falsas activaciones, cobertura de fallos                         |
| Power Mgmt            | Modos sleep/standby                         | Latencias de wake-up, retención de estado                                  |


### Buses de interconexión

Dentro de un sistema embebido, los bloques funcionales se comunican entre sí a través de buses de interconexión. Estos buses permiten la transferencia de datos, direcciones y señales de control entre los diferentes componentes del sistema. Existen varios tipos de buses, cada uno con sus propias características y propósitos:

!!! note "Nota"
    Un bus es el conjunto de conexiones físicas que permiten la comunicación entre los diferentes componentes de un sistema.

1. **Bus de datos**: Transporta la información entre los componentes.
2. **Bus de direcciones**: Lleva las direcciones de memoria a las que se accede.
3. **Bus de control**: Transmite señales de control que coordinan las operaciones del sistema.

Así mismo podemos denominar:

- **Maestros/Iniciadores** (AMBA: Initiator): Dispositivos que inician las transferencias de datos (p. ej., CPU, DMA).
- **Esclavos/Objetivos** (AMBA: Target): Dispositivos que responden a las solicitudes de los maestros (p. ej., memoria, periféricos).


```mermaid
flowchart LR
  subgraph Fabric[Matriz / Bus interno]
    CPU2["CPU (Maestro)"]
    DMA2["DMA (Maestro)"]
    MEM2["Memorias (Esclavos): Flash/SRAM"]
    P1["Periféricos APB (Esclavos)"]
    P2["Periféricos rápidos (Esclavos)"]
  end

  CPU2 -->|Dirección/Control| MEM2
  MEM2 -->|Datos| CPU2

  CPU2 <---> P1
  CPU2 <---> P2
  DMA2 <---> MEM2
  DMA2 <---> P2
```

## Memorias: tipos y usos

En un sistema embebido elegimos memorias según **volatilidad**, **latencia/ancho de banda**, **endurance** (ciclos de escritura), **tamaño** y **costo/energía**. Aquí un mapa práctico.

### Volátiles (rápidas, se pierden al apagar)

| Tipo             | Volátil | Lectura/Escritura | Endurance aprox. | Tamaño típico en MCUs | Uso típico                                  | Riesgos / diseño |
|------------------|:------:|-------------------|------------------:|-----------------------:|---------------------------------------------|------------------|
| **SRAM**         |  Sí    | Muy rápida / rápida| Ilimitada (lógica)| 2–512 KiB              | Variables en ejecución, buffers             | Consumo en sleep; tamaño limitado |
| **TCM (ITCM/DTCM)** | Sí | **Muy rápida** (casi 1 ciclo) | Ilimitada | 16–512 KiB | Código crítico o datos de ISR en tiempo real | Tamaño pequeño; requiere linker script |
| **Caché (I/D)**  |  Sí    | Transparente (aceleración) | — | — | Acelerar acceso a Flash/externa            | Inval./coherencia con DMA |
| **PSRAM**        |  Sí    | Media / media      | Ilimitada        | 2–16 MiB (externa)     | Framebuffers, UI, ML ligero                 | Latencia mayor que SRAM; consumo |
| **SDRAM/DDR**    |  Sí    | Alta BW / latencia media | Ilimitada | 16–1024 MiB (MPU/SoC) | Linux/GUI, visión/ML, grandes buffers       | Controlador complejo; refresh, EMC |

### No volátiles (persisten sin energía)

| Tipo                 | Volátil | Lectura/Escritura         | Endurance aprox. | Tamaño típico | Uso típico                                        | Riesgos / diseño |
|----------------------|:------:|---------------------------|------------------:|--------------:|---------------------------------------------------|------------------|
| **Flash interna (NOR)** | No  | Lectura rápida; **escritura/borrado por páginas/bloques** | 10³–10⁵ ciclos | 64 KiB–2 MiB | **Firmware**, constantes, a veces logs            | Tamaño de página; _wait states_; **wear leveling** para logs |
| **QSPI NOR externa** | No     | Lectura rápida; XIP posible | 10³–10⁵          | 4–256 MiB     | Código XIP, assets (UI), modelos pequeños         | Latencia > interna; líneas QSPI; protección/firmware seguro |
| **NAND (eMMC/SD)**   | No     | BW alto secuencial; aleatorio lento | 10³–10⁵ | 4–256 GiB | Datos masivos: archivos, audio/video, _databases_ | Sistema de archivos, **wear leveling**, integridad (journaling) |
| **EEPROM**           | No     | Escritura por bytes/páginas (simple) | 10⁵–10⁶       | 512 B–256 KiB | **Parámetros** de calibración/configuración       | Endurance: distribuir escrituras; tiempo de escritura |
| **FRAM**             | No     | **Casi SRAM** (rápida) lectura/escritura | 10¹²–10¹⁴ | 4–1024 KiB | Logs frecuentes, contadores, estado crítico       | Costo/KB; disponibilidad |
| **MRAM** *(opc.)*    | No     | Rápida; no volátil        | 10¹²+            | 128 KiB–16 MiB | Estado seguro ante fallos de energía              | Costo; oferta limitada |
| **OTP / eFuse**      | No     | Programación única        | 1                | Decenas–cientos bits | IDs, claves, configuración de arranque        | **Irreversible**; planificar bien campos |

!!! tip "Reglas rápidas"
    - **Código**: Flash interna; si no cabe o quieres **XIP**, QSPI NOR externa.  
    - **Datos en tiempo real** (colas ISR, filtros): SRAM/TCM.  
    - **Parámetros** que cambian poco: EEPROM / FRAM (si cambian mucho).  
    - **Logs frecuentes**: FRAM o estrategia de wear leveling en Flash.  
    - **Activos grandes** (imágenes, audio): QSPI NOR o SD/eMMC.  
    - **Linux/GUI/ML pesado**: SDRAM/DDR + almacenamiento masivo.

!!! note "¿Qué es XIP (Execute-In-Place)?"
    Ejecutar código **directamente** desde una memoria externa (p. ej., QSPI NOR) sin copiarlo a SRAM. Ahorra SRAM, a costa de latencia; ideal para código no crítico.


### Checklist de memoria 
- Tamaño de **firmware** (Flash) y **datos en ejecución** (SRAM/TCM).  
- ¿Necesitas **XIP**? ¿Qué latencia tolera tu bucle crítico?  
- **Endurance** esperado (parámetros/logs) y estrategia de **wear leveling**.  
- ¿DMA y caché? Plan de coherencia/inval.  
- Consumo en **sleep/retención** y tiempos de **wake-up**.  
- Integridad/seguridad: **firmware firmado**, protección de lectura/escritura.





## Modelos de memoria

Los sistemas embebidos utilizan diferentes modelos de memoria para gestionar el almacenamiento y acceso a datos. 

Esto lo hace a través de instrucciones y datos que se almacenan en diferentes tipos de memoria.

- Instrucciones: Son las operaciones que la CPU debe realizar y se almacenan en la memoria de programa.
- Datos: Son la información sobre la que opera la CPU y se almacenan en la memoria de datos.

Algunos de los modelos más comunes son:

1. **Modelo de memoria von Neumann**: La CPU usa una sola memoria para instrucciones y datos, lo que simplifica el diseño, pero limita el rendimiento porque no puede acceder a ambos al mismo tiempo.

2. **Modelo de memoria Harvard**: Este modelo utiliza memorias separadas para instrucciones y datos, lo que permite accesos simultáneos y mejora el rendimiento. Sin embargo, es más complejo de implementar.

```mermaid
flowchart LR

%% =======================
%%   VON NEUMANN (izq.)
%% =======================
subgraph VN["Von Neumann"]
direction TB
  VN_MEM["Memoria (Datos + Código)"]
  VN_CPU["CPU"]
  VN_IO["Dispositivos I/O"]

  VN_MEM -->|Datos| VN_CPU
  VN_CPU -->|Dirección| VN_MEM
  VN_CPU -->|Datos| VN_IO
end

%% =======================
%%   HARVARD (der.)
%% =======================
subgraph HV["Harvard"]
direction TB
  HV_PM["Memoria de Programa"]
  HV_DM["Memoria de Datos"]
  HV_CPU["CPU"]
  HV_IO["Dispositivos I/O"]

  %% Mantener PM y DM en la misma fila superior
  HV_PM --- HV_DM

  %% Instrucciones (PM -> CPU) y su dirección (CPU -> PM)
  HV_PM -->|Instrucción| HV_CPU
  HV_CPU -.->|Dirección de instrucción| HV_PM

  %% Datos (DM -> CPU) y su dirección (CPU -> DM)
  HV_DM -->|Datos| HV_CPU
  HV_CPU -.->|Dirección de variable| HV_DM

  %% I/O debajo de la CPU
  HV_CPU -->|Datos| HV_IO
end

%% ==== Estilo opcional (puedes quitarlo si no lo necesitas) ====
classDef box fill:#f7f7f7,stroke:#444,stroke-width:1px;
class VN_MEM,VN_CPU,VN_IO,HV_PM,HV_DM,HV_CPU,HV_IO box;
linkStyle default stroke:#444,stroke-width:1px;
```

## "Tamaño de la arquitectura"

El “tamaño de la arquitectura” suele referirse al ancho de palabra de la CPU (número de bits de sus registros generales y de la ALU: 8, 16, 32, 64 bits). Sin embargo, para decidir hardware también interesan:

- **Ancho del bus de direcciones** (cuántas direcciones distintas puede emitir la CPU/DMA).
- **Ancho del bus de datos** (cuántos bits se pueden transferir en paralelo).
- **Tamaño de punteros** (cuántos bits se utilizan para representar una dirección de memoria).

!!! note "Nota"
    Puede haber combinaciones, es decir CPU de 32 bits con 24 bits de direcciones, o buses de datos hacia perifericos de 8/16 bits.

#### Efectos  del Tamaño:

1. El ancho de direcciones limita la memoria maxima
2. Mas bits permiten operaciones aritmeticas y de memoria en menos ciclos
3. Las arquitecturas pequenas pueden ahorrar energia y precio.

| **Ancho de direcciones** | **Máximo direccionable** |
| -----------------------: | -----------------------: |
|                    8 bit |                    256 B |
|                   16 bit |                   64 KiB |
|                   24 bit |                   16 MiB |
|                   32 bit |                    4 GiB |
|                   40 bit |                    1 TiB |
|                   48 bit |                  256 TiB |
|                   64 bit |         16 EiB (teórico) |


#### Comparacion entre Tamaños

| Criterio                                | **8/16 bits**                                                                            | **32 bits (MCU/MPU)**                                                                                                                                          | **64 bits (SoC)**                                                                                                                                                            |
| --------------------------------------- | ---------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Complejidad / costo HW**              | Muy bajo                                                                                 | Bajo–medio                                                                                                                                | Medio–alto                                                                                                                                                                   |
| **Consumo energético**                  | Muy bajo (frecuencias modestas)                                                          | Bajo con buen rendimiento/Hz                                                                                                                                   | Mayor en promedio                                                                                                                                                            |
| **Espacio direccionable (orientativo)** | 8–16 b: 256 B–64 KiB                                                                     | Hasta 4 GiB (teórico)                                                                                                                                          | Hasta 16 EiB (teórico)                                                                                                                                                       |
| **Tamaño de puntero típico (C/C++)**    | 16 b                                                                                     | 32 b                                                                                                                                                           | 64 b                                                                                                                                                                         |
| **Desempeño en enteros/32 b**           | Limitado (multi-ciclo)                                                                   | Muy bueno (nativo)                                                                                                                                             | Excelente                                                                                                                                                                    |
| **FPU/DSP / Criptografía**              | Raro / externo                                                                           | Frecuente (M4/M7/RV32-F/D; aceleradores AES/SHA)                                                                                                               | Frecuente / avanzado (SIMD, AES-NI, etc.)                                                                                                                                    |
| **RTOS / Tiempo real**                  | Posible, con límites                                                                     | Muy bueno (determinismo + periféricos modernos)                                                                                                                | Menos determinista en SoC complejos                                                                                                                                          |
| **SO viable**                           | Bare-metal/RTOS                                                                          | Bare-metal/RTOS; Linux en 32 b con **MMU** (Cortex-A, etc.)                                                                                                    | Linux/Unix de pleno derecho                                                                                                                                                  |
| **Conectividad moderna (TLS/OTA)**      | Limitada                                                                                 | Sólida (Wi-Fi/BLE/Cell + TLS)                                                                                                                                  | Completa (redes, contenedores, etc.)                                                                                                                                         |
| **Temporizadores (resolución/rango)**   | Resolución buena, rango corto                                                            | Excelente equilibrio (timers 32 b)                                                                                                                             | Depende del SoC; no foco principal                                                                                                                                           |
| **Time-to-market (ecosistema)**         | Menor oferta actual                                                                      | Muy alto (toolchains, HAL, stacks, RTOS)                                                                                                                       | Alto, pero mayor complejidad                                                                                                                                                 |
| **Casos típicos**                       | Control simple, legacy, ultra-bajo costo                                                 | IoT, control de motor, audio básico, gateways ligeros, HMI simple                                                                                              | Linux, UI compleja, visión/ML ligero, RAM > 1–2 GiB                                                                                                                          |
| **Riesgos**                             | Tope de memoria, software limitado                                                       | Elegir de más/menos (over/under-spec)                                                                                                                          | Consumo, costo, integración compleja                                                                                                                                         |
| **Ejemplos comerciales**                | *Microchip PIC16/PIC18*, *AVR ATmega328P/ATmega32U4*, *TI MSP430* (16 b), *Renesas RL78* | *ST STM32* (F0/G0/F4/H7…), *NXP LPC55Sxx / Kinetis*, *Microchip SAMD21/SAMC21/SAME5x*, *Nordic nRF52/nRF53*, *Espressif ESP32/ESP32-C3*, *Raspberry Pi RP2040* | *Broadcom BCM2711 (Raspberry Pi 4, Cortex-A72)*, *NXP i.MX 8M (Cortex-A53)*, *TI Sitara AM64x (Cortex-A53)*, *Rockchip RK3566/68 (Cortex-A55)*, *Allwinner A64 (Cortex-A53)* |


## Ejemplos de aplicaciones

### Misma palabra (32 bits), Von Neumann vs. Harvard

Asumamos la siguiente operacion de programacion

```c
uint32_t sum = 0;
for (size_t i = 0; i < N; i++) {
    sum += A[i];
}
```

Para este codigo tendremos los siguientes supuestos:

- Carga de 32 bits = 1 acceso de datos.
- ADD = 1 ciclo de ALU.
- Bifurcación/final de bucle = 1 instrucción (se cuenta su fetch).
- Sin caché, sin esperas de memoria, pipeline simple.
- En Von Neumann hay un bus único (instrucciones y datos compiten).
- En Harvard hay dos buses (instrucciones y datos en paralelo).

| Concepto                       | Von Neumann (bus único) |              Harvard (buses I/D) |
| ------------------------------ | ----------------------: | -------------------------------: |
| Fetch `LDR` (leer instrucción) |                 1 ciclo |                  1 ciclo (I-bus) |
| Lectura `A[i]` (dato 32b)      |                 1 ciclo | 1 ciclo (D-bus, **en paralelo**) |
| Fetch `ADD`                    |                 1 ciclo |                  1 ciclo (I-bus) |
| Fetch `BNE`/fin de bucle       |                 1 ciclo |                  1 ciclo (I-bus) |
| **Total por iteración**        |          **≈ 4 ciclos** |                   **≈ 3 ciclos** |

```mermaid
%%{init: {'gantt': {'axisFormat': '%Q'}} }%%
gantt
  dateFormat  x
  title Von Neumann 32 bits (ciclos por iteración)
  section Bus único
  Fetch LDR             :a1, 0, 1
  Fetch de datos (A[i]) :a2, 1, 1
  Fetch ADD             :a3, 2, 1
  Fetch BNE/loop        :a4, 3, 1

```

```mermaid
%%{init: {'gantt': {'axisFormat': '%Q'}} }%%
gantt
  dateFormat  x
  title Harvard 32 bits (ciclos por iteración)
  section I-bus (instrucciones)
  Fetch LDR        :i1, 0, 1
  Fetch ADD        :i2, 1, 1
  Fetch BNE/loop   :i3, 2, 1
  section D-bus (datos)
  Read A[i]        :d1, 0, 1

```

### Misma arquitectura (Harvard), distinto tamaño de palabra

```c
uint32_t sum = 0;
for (size_t i = 0; i < N; i++) {
    sum += A[i];
}
```

Tomaremos los siguientes supuestos donde w es el tamaño de palabra:

- Cargas: 32 / w accesos de datos
- Suma: 32 / w operaciones de ALU
- Bifurcación: ≈ 1 instrucción
Ciclos ≈ 2·(32/w) + 1

| Tamaño de palabra **w** | Cargas (32/w) | Sumas (32/w) | Branch | **Total aprox.** |
| ----------------------: | ------------: | -----------: | -----: | ---------------: |
|              **8 bits** |             4 |            4 |      1 |          **≈ 9** |
|             **16 bits** |             2 |            2 |      1 |          **≈ 5** |
|             **32 bits** |             1 |            1 |      1 |          **≈ 3** |


!!! note "Nota"
    En la práctica abundan diseños **Harvard modificados** (separación I/D con caminos de cruce o regiones compartidas para facilitar DMA/bootloader).

---

## ISA y microarquitectura: RISC vs CISC para embebidos

- ISA (Instruction Set Architecture) es el conjunto de instrucciones que un procesador puede entender y ejecutar, así como el manejo de excepciones/interrupciones.
- Una microarquitectura es la manera en que una arquitectura del conjunto de instrucciones (ISA) se implementa físicamente en un procesador que caen en principalmente dos categorías.
  - RISC (Reduced Instruction Set Computer).
  - CISC (Complex Instruction Set Computer).

### RISC (Reduced Instruction Set Computer).

La idea principal detrás de esto es simplificar el hardware mediante el uso de un conjunto de instrucciones de unos pocos pasos basicos para cargar, evaluar y almacenar operación, por ejemplo un comando de carga (LOAD) o un comando de almacenamiento (STORE).

Sus principales características son:

- Instrucciones más simples, por lo tanto decodificación de instrucciones sencilla.
- La instrucción viene en un tamaño inferior a una palabra.
- La instrucción permite un pipeline corto; idealmente ~1 instrucción/ciclo.
- Registros más generales.
- Modos de direccionamiento simples.
- Menos tipos de datos.
- Es posible crear un pipeline.

### CISC (Complex Instruction Set Computer).

La idea principal es que una sola instrucción realizará todas las operaciones de carga, evaluación y almacenamiento, tal como un comando de multiplicación hará cosas como cargar datos, evaluarlos y almacenarlos, por lo tanto, es complejo.

Sus principales características son:

- Instrucción compleja, por lo tanto decodificación de instrucciones complejas.
- Las instrucciones tienen un tamaño mayor a una palabra.
- La instrucción puede tardar más de un ciclo de reloj para ejecutarse.
- Históricamente menor número de registros de propósito general, ya que las operaciones se realizan en la propia memoria.
- Modos de direccionamiento complejos.
- Más tipos de datos.

### Comparativa RISC vs CISC


| Categoría                          | RISC                                                                 | CISC                                                                                 |
|-----------------------------------|----------------------------------------------------------------------|--------------------------------------------------------------------------------------|
| **Tamaño de código**              | Mayor (se requieren más instrucciones).                              | Menor (instrucciones complejas reducen líneas de código).                           |
| **Velocidad de ejecución**        | Más rápida (instrucciones simples, fácil decodificación).            | Más lenta (instrucciones complejas, mayor tiempo de decodificación).                |
| **Consumo de energía**            | Menor (ventaja para dispositivos portátiles/embebidos).              | Mayor (complejidad del conjunto de instrucciones).                                   |
| **Uso de memoria (programa)**     | Mayor (más instrucciones para tareas complejas).                     | Menor / más eficiente (menos instrucciones para tareas complejas).                   |
| **Complejidad del diseño/ISA**    | Menor (conjunto más pequeño y regular).                              | Mayor (conjunto amplio y heterogéneo; diseño y fabricación más complejos).          |
| **Número de instrucciones**       | Se necesitan más instrucciones para tareas complejas.                | Menos instrucciones para la misma tarea (cada instrucción hace más).                |
| **Costo de desarrollo/fabricación** | Puede ser mayor (según tu nota original).                            | Puede ser menor relativo a RISC (según tu nota original).                            |
| **Ejemplos de ISAs/familias y usos típicos** | **ARM** (Cortex-M/A/R): STMicro (STM32), NXP, TI, Microchip (SAM), Nordic, Renesas, Samsung Exynos, Qualcomm Snapdragon, Broadcom (Raspberry Pi), Apple M-series; **RISC-V**: SiFive, Espressif (ESP32-C3/C6/H2), Kendryte K210, Bouffalo Lab BL602/604, StarFive; **MIPS** (hist.): Microchip PIC32, Loongson (GP); **Power/PowerPC**: NXP MPC5xxx (auto), IBM (servidor); **SPARC** (LEON en aeroespacial); **SuperH** (Renesas SH); **AVR** (Atmel/Microchip 8-bit); **MSP430** (TI 16-bit); **ARC** (Synopsys); **Xtensa** (Cadence, p.ej. ESP32 “clásico” LX6/LX7); **OpenRISC**; **Nios II** (Intel FPGA), **MicroBlaze** (Xilinx FPGA). | **x86/x86-64**: Intel (Core/Atom), AMD (Ryzen/EPYC); **Motorola 68000/ColdFire** (hist./embebidos clásicos), **VAX** (hist.); **Z80** (Zilog, clásico/retro/embebido simple), **6502** (WDC 65C02/65C816, retro/industrial), **8051** (MCS-51, múltiples fabricantes como Nuvoton/Silicon Labs), **Renesas RX/RL78** (CISC 32/16-bit), **68HC11/HC12** (clásicos). |

## Checklist para elegir un MCU

**Checklist rápido para elegir MCU**

- Requisitos de tiempo real (duro/blando) y latencia de E/S
- Consumo/energía y modos sleep
- Periféricos clave (ADC, PWM, DMA, comms)
- Memoria necesaria (Flash/SRAM) + seguridad (WDT, MPUs)
- Ecosistema (HAL, RTOS, toolchain, comunidad)
- Coste y disponibilidad (vida útil del producto)
