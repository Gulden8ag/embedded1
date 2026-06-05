# I2C (Inter-Integrated Circuit)

## Introduccion

### ¿Que es I2C?

- Bus serial síncrono de dos líneas:
    - SCL (clock) y SDA (datos).
- Arquitectura multidispositivo sobre el mismo par de hilos.
- Comunicación máster ↔ esclavo (en la práctica: controller ↔ target).
- Cada target tiene una dirección (7 o 10 bits) en el bus.

### ¿Cuándo conviene usar I²C?

- Conectar muchos periféricos con pocos pines.
- Dispositivos típicos: sensores (temperatura, IMU), ADC/DAC, GPIO expanders, pantallas (OLED), EEPROM.
- Velocidades modestas: 100 kbps (Standard), 400 kbps (Fast), 1 Mbps (Fm+), 3.4 Mbps (Hs).
(Para flujos más altos o enlaces punto a punto, SPI suele ser mejor; para texto/terminal, UART).

## Hardware

![I2C Wiring](../../../images/i2c_wiring.png)

![I2C Internal](../../../images/open_i2c.png)

$$
R_p(min)= \frac{V_{cc}-V_{oL}(max)}{I_{oL}}
$$

Donde $V_{oL}(max)$ es el voltaje máximo que el dispositivo garantiza en LOW (normalmente 0.4 V) e $I_{oL}$ es la corriente que el dispositivo puede absorber en LOW (normalmente 3 mA).
$$
R_p(min)= \frac{3.3V-0.4V}{3mA}=966.67 \Omega
$$

![I2C senales](../../../images/i2c_behaviours.png)

$$
R_p(max)= \frac{t_r}{0.8473 \cdot C_b}
$$
Donde $t_r$ es el tiempo máximo de subida permitido (1000 ns para 100 kHz, 300 ns para 400 kHz, 120 ns para 1 MHz) y $C_b$ es la capacitancia total del bus (incluyendo cables, PCB y dispositivos).

Tipicamente usamos los siguientes valores:

- 100 kHz: 4.7 kΩ y hasta 10 kΩ puede funcionar.
- 400 kHz: 2.2–4.7 kΩ.
- 1 MHz (Fm+): 1–2.2 kΩ.

### Pines I2C en Raspberry Pi Pico 2

**I2C0**
- SDA pins: GP0, GP4, GP8, GP12, GP16, GP20
- SCL pins: GP1, GP5, GP9, GP13, GP17, GP21

**I2C1**
- SDA pins: GP2, GP6, GP10, GP14, GP18, GP26
- SCL pins: GP3, GP7, GP11, GP15, GP19, GP27

## Protocolo

### Formato de mensaje

![I2C Frame](../../../images/frame.avif)


- `START`: SDA cae (HIGH→LOW) mientras SCL está HIGH; inicio de transacción (maestro).
- `Address + R/W`: Byte con dirección (7 bits) + bit R/W; selecciona esclavo y sentido (maestro).
- `ACK/NACK`: Bit de acuse tras cada byte; lo da el receptor (esclavo en write, maestro en read).
- `DATA`: Bytes MSB→LSB; en write los envía el maestro, en read los envía el esclavo.
- `REPEATED START (Sr)`: Nuevo START sin STOP previo; encadena operaciones sin soltar el bus (maestro).
- `STOP`: SDA sube (LOW→HIGH) mientras SCL está HIGH; fin de transacción y libera bus (maestro).

### Secuencias comunes

![I2C Frame](../../../images/frames2.png)


## API de I2C en Raspberry Pi Pico SDK

- `i2c_init(i2c, baudrate)` inicializa el periférico I²C y fija la velocidad, donde:
    - `i2c` es el puntero a la instancia (`i2c0` o `i2c1`).
    - `baudrate` es la velocidad objetivo en Hz (p. ej. `100000`, `400000`).
    - Retorna: `uint` con la velocidad realmente aplicada.
- `i2c_deinit(i2c)` apaga/deshabilita el periférico I²C, donde:
    - `i2c` es la instancia (`i2c0` o `i2c1`).
    - Retorna: nada.
- `i2c_set_baudrate(i2c, baudrate)` cambia la velocidad en caliente, donde:
    - `i2c` es la instancia.
    - `baudrate` es la nueva velocidad en Hz.
    - Retorna: `uint` con la velocidad realmente aplicada.
- `i2c_write_blocking(i2c, addr, src, len, nostop)` envía bytes al esclavo (bloqueante), donde:
    - `i2c` es la instancia.
    - `addr` es la dirección de 7 bits del esclavo (ej. `0x8A`).
    - `src` es puntero a la memoria con los datos a transmitir.
    - `len` es el número de bytes a enviar desde `src`.
    - `nostop` si es `true` no emite **STOP** (prepara **REPEATED START**); si es `false` sí emite **STOP**.
    - Retorna: `int` con bytes escritos (debería ser `len`) o <0 en error (NACK/timeout).
- `i2c_read_blocking(i2c, addr, dst, len, nostop)` lee bytes del esclavo (bloqueante), donde:
    - `i2c` es la instancia.
    - `addr` es la dirección de 7 bits del esclavo.
    - `dst` es puntero a la memoria destino para almacenar los datos leídos.
    - `len` es el número de bytes a leer hacia `dst`.
    - `nostop` si es `true` no emite **STOP** al final; si es `false` emite **STOP** (el hardware NACKea el último byte).
    - Retorna: `int` con bytes leídos (debería ser `len`) o <0 en error.
- `i2c_write_timeout_us(i2c, addr, src, len, nostop, timeout_us)` igual que `i2c_write_blocking` pero con timeout, donde:
    - `timeout_us` es el tiempo máximo en microsegundos.
    - Retorna: bytes escritos, `PICO_ERROR_TIMEOUT` si expira, o <0 en otros errores.
- `i2c_read_timeout_us(i2c, addr, dst, len, nostop, timeout_us)` igual que `i2c_read_blocking` pero con timeout, donde:
    - `timeout_us` es el tiempo máximo en microsegundos.
    - Retorna: bytes leídos, `PICO_ERROR_TIMEOUT` si expira, o <0 en otros errores.

### CMakeLists.txt


Para poder ocupar I2C es necesario agregar la libreria de hardware_i2c en el CMakeLists.txt de nuestro proyecto:
```cmake

target_link_libraries(i2c_demo
    pico_stdlib
    hardware_i2c          # <-- AÑADIR esta línea
)

```

### Ejemplo básico de uso Escritura

Siguiendo la siguiente transaccion:

1. `START` inicio de la comunicacion
1. `Address + W → ACK (esclavo)` indico con quien me voy a comunicar
1. `Data (registro) → ACK (esclavo)` indico donde quiero escribir el valor
1. `Data (valor) → ACK (esclavo)` indico que valor quiero escribir
1. `STOP` fin de la comunicacion

```c title="i2c_escribe"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT    i2c0
#define SDA_PIN     4
#define SCL_PIN     5
#define ADDR 0x8A //Direccion del dispositivo esclavo de 7 bits

int main(void) {
    stdio_init_all();
    //Configura el inicio de i2c con una velocidad de 100 KHz
    i2c_init(I2C_PORT, 100000);
    //Configura los pines para trabajar como i2c
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    //Habilita pullup interno para pines
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(500);
    uint8_t reg = 0x00; //Registro a escribir     
    uint8_t value = 0x67; //Valor a escribir  
    uint8_t value2 = 0x42; //Valor a escribir   
    uint8_t memoria[3];
    while (true) {
        memoria[0] = reg;
        memoria[1] = value;
        memoria[2] = value2;

        /*OPCION 1 TODO JUNTO*/
        // START + addr|W + WRITE(reg) + ACK  
        int w = i2c_write_blocking(I2C_PORT, ADDR, memoria, sizeof(memoria), /*nostop=*/false);
        if (w < 0) {
        printf("I2C error (ret=%d)\n", w);
        } else if (w != (int)sizeof(memoria)) {
            printf("Escritura parcial: %d/%d bytes\n", w, (int)sizeof(memoria));
        } else {
            printf("Escrito correctamente\n");
        }
        sleep_ms(1000);
    }
    return 0;
}
```
### Ejemplo básico de uso Lectura



1. `START` inicio de la comunicacion
1. `Address + W → ACK (esclavo)` indico con quien me voy a comunicar en escritura
1. `Data (registro) → ACK (esclavo)` Escribo de donde quiero leer
1. `REPEATED START` Reinicio la comunicacion
1. `Address + R → ACK (esclavo)` indico con quien me voy a comunicar pero en lectura
1. `Data byte(s) → ACK (maestro)` Leo el/los byte(s) solicitados
1. `Último Data → NACK (maestro)` Indico que es el último byte que voy a leer
1. `STOP`

```c title="i2c_leer"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT    i2c0
#define SDA_PIN     4
#define SCL_PIN     5
#define ADDR 0x2A //Direccion del dispositivo esclavo de 7 bits

int main(void) {
    stdio_init_all();
    //Configura el inicio de i2c con una velocidad de 100 KHz
    i2c_init(I2C_PORT, 100000);
    //Configura los pines para trabajar como i2c
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    //Habilita pullup interno para pines
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    sleep_ms(500);
    uint8_t reg = 0x00; //Registro a Leer       
    uint8_t memoria[3]; //Leeremos 3 bytes
    while (true) {
        // START + addr|W + WRITE(reg) + ACK  + NoStop
        int w = i2c_write_blocking(I2C_PORT, ADDR, &reg, 1, /*nostop=*/true);
        if (w < 0) {
            printf("I2C error (ret=%d)\n", w);
            goto next;
        } else if (w != 1) {
            printf("Escritura parcial: %d/1 bytes\n", w);
            goto next;
        } else {
            printf("Escrito correctamente\n");
        }
        // REPEATED START + addr|R + READ(data) + NACK + STOP
        int r = i2c_read_blocking(I2C_PORT, ADDR, memoria, sizeof(memoria), /*nostop=*/false);
        if (r < 0) {
            printf("I2C error (ret=%d)\n", r);
        } else if (r != (int)sizeof(memoria)) {
            printf("Lectura parcial: %d/%d bytes\n", r, (int)sizeof(memoria));
            printf("Datos leidos: ");
            for (int i = 0; i < r; i++) {
                printf("0x%02X ", memoria[i]);
            }
        } else {
            printf("Leido correctamente\n");
            printf("Datos leidos: \n");
            for (int i = 0; i < (int)sizeof(memoria); i++) {
                printf("0x%02X \n", memoria[i]);
            }
        } 
        next:
            sleep_ms(1000);
    }
    return 0;
}
```
