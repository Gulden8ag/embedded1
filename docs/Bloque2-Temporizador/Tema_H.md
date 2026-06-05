# UART — Comunicación serial

---

## 2) Conceptos 

### 2.1 ¿Qué es UART?

**UART (Universal Asynchronous Receiver/Transmitter)** transmite datos bit a bit sin reloj compartido. Cada extremo configura **baudios**, **bits de datos**, **paridad** y **stop**. Señales: **TX** (emite) y **RX** (recibe).

**Trama típica:**

![Trama UART](../../../images/uart_frame.png)

* **Start bit:** 0 → marca inicio del byte.
* **Bits de datos:** 5–9 (normalmente 8).
* **Paridad:** None/Even/Odd para detección simple de error.
* **Stop bits:** 1 o 2 (nivel alto).

> En 8N1, un byte ocupa **10 bits** (1 start + 8 data + 1 stop).

### 2.2 bps vs baudios

* **bps (bits por segundo):** tasa de bits reales (incluye start/stop/paridad).
* **Baudios (símbolos/s):** en UART cada símbolo = 1 bit ⇒ **baudios = bps**.
* En sistemas modulados un símbolo puede valer varios bits (ahí **baudios ≠ bps**).

**Ejemplo 115200 baudios:**
`T_bit ≈ 1/115200 ≈ 8.68 µs`. Un carácter 8N1 (10 bits) tarda ~**86.8 µs** ⇒ ~**11.5 kB/s**.

**Velocidad maxima Teorica** 

Según el datasheet, puede alcanzar hasta **~1.5 Mbaud** sin errores apreciables con un cable corto y hardware estable.
En práctica, los valores típicos estables son:

- 9600 / 19200 / 38400 / 57600 / 115200 bps (estándares de PC).
- 1 000 000 bps (usado a veces en comunicación MCU-MCU).

A mayor velocidad, el margen de error y el ruido aumentan significativamente, entre mas largo el cable mayor probabilidad de errores.

UART puede funcionar correctamente si el error entre el emisor y el receptor se mantiene por debajo del ±3 %.


### 2.3 UART vs USART

| Característica | UART                      | USART                                |
| -------------- | ------------------------- | ------------------------------------ |
| Reloj          | No (asíncrona)            | Puede sí (síncrona) o no (asíncrona) |
| Líneas         | TX, RX                    | TX, RX, **CLK** (modo síncrono)      |
| Complejidad    | Menor                     | Mayor, más flexible                  |
| Uso típico     | Consolas, módulos simples | Modems/links que requieren reloj     |



### 2.4 Hardware de UART Pi Pico 2

**UARTs hardware:** **UART0** y **UART1** (pueden usarse simultáneamente).

**Pines por defecto:**

* UART0 → TX **GP0**, RX **GP1**
* UART1 → TX **GP8**, RX **GP9**

**Pines alternativos compatibles:**

| UART  | TX posibles           | RX posibles           |
| ----- | --------------------- | --------------------- |
| UART0 | GP0, GP12, GP16, GP28 | GP1, GP13, GP17, GP29 |
| UART1 | GP4, GP8, GP20        | GP5, GP9, GP21        |

![Wiring UART](../../../images/wiring_uart.png)



### 2.5 ASCII (American Standard Code for Information Interchange)

El estándar ASCII define 128 símbolos (0–127) que incluyen:

| Tipo                  | Ejemplo          | Rango decimal | Ejemplo binario       |
| --------------------- | ---------------- | ------------- | --------------------- |
| **Control**           | `\n`, `\r`, `\t` | 0–31          | `00001010` (LF)       |
| **Números**           | `0`–`9`          | 48–57         | `00110000`–`00111001` |
| **Letras mayúsculas** | `A`–`Z`          | 65–90         | `01000001`–`01011010` |
| **Letras minúsculas** | `a`–`z`          | 97–122        | `01100001`–`01111010` |
| **Símbolos comunes**  | `, . ; ? !` etc. | 33–47 y otros | `00100001` (¡)        |

Cada carácter (char) es un entero de 8 bits (0–255).

Cuando haces uart_putc(UART_ID, 'A');, se envía el valor 65 decimal (0x41).

Cuando recibes un byte (uart_getc() devuelve un char), puedes interpretarlo como número o letra:

```makefile
Valor: 65, Carácter: A
```

### 2.6 FIFO (First In First Out)

FIFO (First In, First Out) es una pequeña cola de almacenamiento dentro del periférico UART.
En el RP2350, cada UART tiene una FIFO de 16 bytes tanto para RX como para TX.

**Funcionamiento:**

- Cada byte recibido entra a la FIFO RX.
- El CPU (o DMA) los va extrayendo con uart_getc().
- Si el CPU se retrasa y la FIFO se llena → los bytes nuevos se pierden (overrun).

**Importancia:**

- En modo polling, la FIFO actúa como pequeño “amortiguador” entre hardware y software.
- En modo interrupción, el handler debe vaciar la FIFO rápidamente (leyendo todos los bytes disponibles) para evitar saturarla.
- En flujos rápidos, el uso de buffer circular externo y/o DMA se vuelve esencial.

---

## 3) Ejemplos Monitor Serial USB CDC

### 3.1 Configuración de USB-CDC

**USB‑CDC** hace que la Pico aparezca como **puerto COM/ttyACM**. 

Formato usual: **115200 8N1**.
**Saltos de línea:** usa `\r\n` o configura la terminal (CR/LF).

Antes de usar UART o USB-CDC, primero modificaremos el `CMakeLists.txt` :

```cmake
# Buscaremos el la linea:
pico_enable_stdio_usb(timer 1)#Por defecto 0 cambiaremos a uno 1 para activar

# Agregaremos tambien en la seccion de librerias harware_uart:
target_link_libraries(timer
        pico_stdlib
        hardware_uart
        )
```

### 3.2 Codigo Ejemplo echo



```c title="Echo consola"
#include "pico/stdlib.h"
#include <stdio.h>

int main() {
    stdio_init_all();
    sleep_ms(2000); // tiempo para enumeración USB

    printf("\n[Pico USB] Conexión lista. Escribe algo y Enter.\n");

    while (true) {
        int ch = getchar_timeout_us(0);
        if (ch != PICO_ERROR_TIMEOUT) {
            printf("Eco: %c\n", (char)ch);
        }
        sleep_ms(10);
    }
}
```

### 3.3 Probar con Monitor Serial en VS Code

1. Abre **Extensiones** (`Ctrl+Shift+X`) y busca **Serial Monitor** (Microsoft). Instala.
2. Conecta la Pico; espera a que aparezca el puerto **USB Serial Device COMn**.
3. **Paleta de comandos** (`Ctrl+Shift+P`) → `Serial Monitor: Select Serial Port` → elige el puerto.
4. `Serial Monitor: Start Monitoring` → selecciona **115200**.
5. Configura **CR/LF** según necesites; para  `printf("\r\n")` usa CR+LF.

---


## 4) Comunicación entre dos Pico (main/support) — UART0

### 4.1 Cableado de UART

**Conexión:**

| Pico Main | Pico Support |
| --------- | ------------ |
| GP0 (TX)  | GP1 (RX)     |
| GP1 (RX)  | GP0 (TX)     |
| GND       | GND          |



### 4.2 Glosario de funciones (SDK Pico)

| Función                                  | Descripción                           | Ejemplo                                        |
| ---------------------------------------- | ------------------------------------- | ---------------------------------------------- |
| `stdio_init_all()`                       | Habilita stdio (USB/UART) según CMake | `stdio_init_all();`                            |
| `printf()`                               | Imprime a stdio                       | `printf("Hola\n");`                            |
| `getchar_timeout_us(us)`                 | Lee char con timeout                  | `getchar_timeout_us(0);`                       |
| `uart_init(uart, baud)`                  | Inicializa UART a baudios             | `uart_init(uart0,115200);`                     |
| `uart_set_format(uart, db, sb, par)`     | Configura 8N1/E/O                     | `uart_set_format(uart0,8,1,UART_PARITY_NONE);` |
| `uart_puts/putc()`                       | Envía cadena/carácter                 | `uart_puts(uart0,"Hi\r\n");`                   |
| `uart_is_readable()`                     | ¿Hay datos RX?                        | `if(uart_is_readable(u0))`                     |
| `uart_getc()`                            | Lee 1 byte RX                         | `char c=uart_getc(u0);`                        |
| `gpio_set_function(pin, GPIO_FUNC_UART)` | Asigna pin a UART                     | `gpio_set_function(0,GPIO_FUNC_UART);`         |
| `irq_set_exclusive_handler(irq,h)`       | Registra ISR                          | `irq_set_exclusive_handler(UART0_IRQ,h);`      |
| `uart_set_irq_enables(u,rx,tx)`          | Enciende IRQ RX/TX                    | `uart_set_irq_enables(u0,true,false);`         |

---

### 4.3 Envio

```c title="Codigo Envio"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1

int main() {
    stdio_init_all();

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    uart_init(UART_ID, BAUD_RATE);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    printf("[MAIN] Enviando mensajes cada segundo...\n");

    int counter = 0;
    while (true) {
        char msg[64];
        sprintf(msg, "Mensaje %d desde MAIN\r\n", counter++);
        uart_puts(UART_ID, msg);
        sleep_ms(1000);
    }
}
```

**Polling (sondeo):** el `main()` revisa si hay datos (`uart_is_readable()`). Simple, pero consume CPU y puede perder datos si la CPU se ocupa demasiado tiempo (FIFO UART ≈ 16 bytes).


### 4.4 Recepcion

```c title="Codigo Recepcion"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1

int main() {
    stdio_init_all();

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    uart_init(uart0, BAUD_RATE);
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE);

    printf("[SUPPORT] Esperando mensajes (polling)...\n");

    while (true) {
        if (uart_is_readable(uart0)) {
            char c = uart_getc(uart0);
            printf("%c", c);
        }
    }
}
```

**Interrupciones:** el hardware llama a un **handler** cuando llega un byte; la ISR debe ser **rápida** y volcar el dato a un **buffer circular** en RAM para procesarlo luego. Eficiente y robusto a baudios altos.

```c title="Codigo Recepcion IRQ"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include <stdio.h>

#define UART_ID uart0
#define BAUD_RATE 115200
#define TX_PIN 0
#define RX_PIN 1

// Buffer circular simple
#define BUF_SIZE 256
volatile char buffer[BUF_SIZE];
volatile uint16_t head = 0, tail = 0;

static void on_uart_rx(void) {
    while (uart_is_readable(uart0)) {
        char c = uart_getc(uart0);
        buffer[head] = c;
        head = (head + 1) % BUF_SIZE; // sin control de overflow para simplicidad
    }
}

int main() {
    stdio_init_all();

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    uart_init(uart0, BAUD_RATE);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

    // Configurar IRQ de UART0
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false); // RX interrupt ON

    printf("[SUPPORT IRQ] Esperando datos con interrupciones...\n");

    while (true) {
        // Vaciar buffer a la consola USB
        while (tail != head) {
            char c = buffer[tail];
            tail = (tail + 1) % BUF_SIZE;
            printf("%c", c);
        }
        sleep_ms(10);
    }
}
```

---



## 5) Errores

### 5.1 Problemas de aplicacion comunes

* **No aparece el puerto USB:** cable solo de carga; habilita `pico_enable_stdio_usb(... 1)` y espera ~2 s tras `stdio_init_all()`.
* **Texto ilegible:** desajuste de baudios/formato; usa **115200 8N1**.
* **Sin eco por UART:** cruza **TX↔RX**, comparte **GND**, fija `GPIO_FUNC_UART`.
* **Pérdida de datos a baudios altos:** usa **IRQ + buffer circular** o **DMA**; aumenta `BUF_SIZE`.
* **CR/LF raros:** usa `\r\n` o ajusta la terminal a **CR+LF**.

### 5.2 Errores de Comunicacion UART

| Tipo de error        | Causa                                                                                       | Cómo detectarlo                                         | Efecto típico                                                   |
| -------------------- | ------------------------------------------------------------------------------------------- | ------------------------------------------------------- | --------------------------------------------------------------- |
| **Framing Error**    | El receptor no detecta correctamente el bit de *Stop* (por ruido o desajuste de baud rate). | Bit `FRAMING_ERR` en registro de estado (`UARTx_FRSR`). | Los datos recibidos se descartan o se interpretan erróneamente. |
| **Parity Error**     | Cuando se usa bit de paridad (even/odd) y no coincide con lo esperado.                      | Bit `PARITY_ERR`.                                       | Detección de corrupción de un bit individual.                   |
| **Overrun Error**    | El receptor recibió un nuevo byte antes de que el software leyera el anterior de la FIFO.   | Bit `OVERRUN_ERR`.                                      | Se pierde el byte más antiguo del buffer.                       |
| **Break Condition**  | La línea `TX` se mantiene en nivel bajo por más del tiempo de un byte.                      | Bit `BREAK_DETECT`.                                     | Puede indicar desconexión o error grave en hardware.            |
| **Noise / Glitches** | Ruido eléctrico o interferencia entre cables.                                               | Osciloscopio / analizador lógico.                       | Bytes espurios, framing errors aleatorios.                      |


### 5.3 Manejo de errores en codigo

| Registro                                                     | Función                                                     |
| ------------------------------------------------------------ | ----------------------------------------------------------- |
| **`UARTx_FR`** (Flag Register)                               | Muestra el estado de la FIFO y la línea (busy, empty, etc.) |
| **`UARTx_RSR` / `UARTx_ECR`** (Receive Status / Error Clear) | Contiene *flags* de error: `FE`, `PE`, `BE`, `OE`           |


| Bit | Nombre | Significado                                                 |
| --- | ------ | ----------------------------------------------------------- |
| 0   | **FE** | *Framing Error* — No se detectó bit de stop válido          |
| 1   | **PE** | *Parity Error* — Paridad incorrecta                         |
| 2   | **BE** | *Break Error* — La línea RX estuvo en bajo demasiado tiempo |
| 3   | **OE** | *Overrun Error* — Se perdió un dato por FIFO llena          |


```c title="Lectura de errores UART"
#include "hardware/uart.h"

if (uart_is_readable(uart0)) {
    int c = uart_getc(uart0); // Lee un carácter
    if (uart_get_hw(uart0)->rsr & (UART_UARTDR_FE_BITS | UART_UARTDR_PE_BITS | UART_UARTDR_OE_BITS)) {
        printf("⚠️ Error detectado en UART\n");
        uart_get_hw(uart0)->rsr = 0; // Limpia errores
    }
}
```