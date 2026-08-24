# GPIO Control: From SDK Functions to Hardware Registers

In the previous lesson, we used functions such as:

```c
gpio_put(LED, 1);
```

to control a GPIO pin.

In this lesson we will open that abstraction and answer a simple question:

> **What does `gpio_put()` actually do to the hardware?**

The goal is not to stop using the SDK. The goal is to understand what the SDK is doing for us.


## Learning objectives

By the end of this lesson, you should be able to:

- explain how GPIO peripherals are controlled through **memory-mapped registers**;
- represent GPIO states using binary, hexadecimal, and decimal values;
- create and use **bit masks**;
- use `AND`, `OR`, `XOR`, `NOT`, and shifts to manipulate selected bits;
- identify the main RP2350 **SIO** GPIO registers;
- relate Pico SDK GPIO functions to lower-level register operations;
- control several GPIO pins with a single mask.

---

## 1. From software to hardware

### Memory-mapped I/O

Microcontroller peripherals contain **registers**: small hardware storage locations used to configure or inspect hardware.

Peripheral registers are normally assigned addresses in the processor's **memory map**.

```text
CPU address space

├── Flash
├── RAM
└── Peripheral registers
      ├── GPIO
      ├── Timers
      ├── UART
      └── ...
```

Reading or writing one of these addresses communicates with the corresponding hardware peripheral.

A register is commonly 8, 16, or 32 bits wide. Individual **bits or bit fields** can represent hardware states such as:

- input or output direction;
- output HIGH or LOW;
- pull-up / pull-down configuration;
- interrupt flags;
- peripheral modes.

!!! tip "Key idea"
    At register level, controlling hardware means **reading and writing particular bits at particular addresses**.

### Examples across MCU families

The names change between microcontrollers, but the idea is similar.

**ATmega328P (Arduino Uno)**

- `DDRB` → data direction
- `PORTB` → output values
- `PINB` → input values

**RP2350 (Pico 2)**

- `sio_hw->gpio_oe_set` → enable selected outputs
- `sio_hw->gpio_set` → set selected outputs HIGH
- `sio_hw->gpio_clr` → set selected outputs LOW
- `sio_hw->gpio_in` → read GPIO states

---

## 2. GPIO from the inside

![GPIO internal structure](../images/GPIO.png){loading=lazy}

A GPIO pin usually contains several hardware blocks.

| Block | Purpose |
|---|---|
| **IOMUX / Function Select** | Chooses whether the pin acts as GPIO, UART, SPI, etc. |
| **DIR / OE** | Enables or disables the output driver |
| **OUT / DATA** | Selects the output logic level |
| **IN** | Reads the logic level present at the pin |
| **PULL-UP / PULL-DOWN** | Provides a default input level when nothing else drives the pin |

For a pin to behave as a normal digital output, we normally need to:

```text
Select GPIO/SIO function
        ↓
Enable output
        ↓
Write HIGH or LOW
```

The Pico SDK hides many of these details behind functions such as:

```c
gpio_init(pin);
gpio_set_dir(pin, GPIO_OUT);
gpio_put(pin, 1);
```

---

## 3. Bits and numeric representation

GPIO registers contain many bits. A convenient way to select bit `n` is:

```c
1u << n
```

For example:

| Expression | Binary | Hex | Decimal |
|---|---:|---:|---:|
| `1u << 0` | `0b00000001` | `0x01` | `1` |
| `1u << 2` | `0b00000100` | `0x04` | `4` |
| `1u << 5` | `0b00100000` | `0x20` | `32` |
| `1u << 7` | `0b10000000` | `0x80` | `128` |

All three representations describe the **same numeric value**.

For example:

```c
10
0x0A
0b1010
```

represent the same value.

!!! note "Why use shifts?"
    `1u << n` clearly expresses the intention:

    > **Create a value where bit `n` is 1.**

    This is usually easier to understand than manually writing a long binary or hexadecimal constant.

---

## 4. Masks

A **mask** is a bit pattern used to select particular bits.

For example, suppose we want to select GPIO2, GPIO4, and GPIO6:

```text
Bit:    7 6 5 4 3 2 1 0
        ───────────────
Mask:   0 1 0 1 0 1 0 0
          ↑   ↑   ↑
         GP6 GP4 GP2
```

The mask can be created with:

```c
const uint32_t MASK =
    (1u << 2) |
    (1u << 4) |
    (1u << 6);
```

The same mask can be written as:

```c
0b01010100
```

or:

```c
0x54
```

or:

```c
84
```

Again, the processor does not care which notation we used. They represent the same value.

### Building masks

**One pin**

```c
uint32_t mask = 1u << PIN;
```

**Several pins**

```c
uint32_t mask =
    (1u << PIN_A) |
    (1u << PIN_B) |
    (1u << PIN_C);
```

!!! exercise "Quick check"
    Create a mask for **GPIO2, GPIO4, and GPIO6**.

    Write the result in:

    1. binary;
    2. hexadecimal;
    3. decimal.

---

## 5. Bitwise operations

Bitwise operators allow us to modify only selected bits of a register.

### The four patterns to remember

```c
reg |= mask;       // SET selected bits

reg &= ~mask;      // CLEAR selected bits

reg ^= mask;       // TOGGLE selected bits

value = reg & mask; // TEST / EXTRACT selected bits
```

### OR `|` — set bits

```text
Register:  00100000
Mask:      00000100
           --------
Result:    00100100
```

```c
reg |= (1u << 2);
```

The selected bit becomes `1` while the other bits are preserved.

### AND `&` — keep selected bits

```text
Register:  11001010
Mask:      11110000
           --------
Result:    11000000
```

```c
value = reg & mask;
```

Bits where the mask contains `0` are cleared in the result.

### AND + NOT — clear selected bits

To clear a particular bit, first create a mask containing `1` at that position and invert it:

```c
reg &= ~(1u << 2);
```

Conceptually, assuming an 8-bit value:

```text
Mask:      00000100
NOT mask:  11111011

Register:  00100100
AND:       11111011
           --------
Result:    00100000
```

!!! note
    `~` inverts all bits of the C integer type being used. The 8-bit diagram above is only a simplified visualization.

### XOR `^` — toggle bits

```text
Register:  00001111
Mask:      00000101
           --------
Result:    00001010
```

```c
reg ^= mask;
```

Selected bits switch:

```text
0 → 1
1 → 0
```

### Shifts `<<` and `>>`

Shifts move a bit pattern left or right.

```c
1u << 2
```

produces:

```text
00000001
   << 2
--------
00000100
```

This is especially useful when creating masks and moving patterns between GPIO pins.

!!! exercise "Predict the result"
    Assume an 8-bit register begins as:

    ```text
    reg = 00101000
    ```

    Determine the result of:

    ```c
    reg |=  0b00000101;
    reg &= ~0b00001000;
    reg ^=  0b00100001;
    ```

---

## 6. The RP2350 SIO block

The RP2350 provides **SIO (Single-Cycle I/O)** registers for direct GPIO access.

For the GPIO pins used in this lesson, each bit in the register corresponds to a GPIO number:

```text
bit 2 → GPIO2
bit 4 → GPIO4
bit 6 → GPIO6
```

### Main SIO registers

| Register | Purpose |
|---|---|
| `sio_hw->gpio_in` | Read GPIO states |
| `sio_hw->gpio_set` | Set selected outputs HIGH |
| `sio_hw->gpio_clr` | Set selected outputs LOW |
| `sio_hw->gpio_togl` | Toggle selected outputs |
| `sio_hw->gpio_oe_set` | Configure selected GPIOs as outputs |
| `sio_hw->gpio_oe_clr` | Configure selected GPIOs as inputs |

The SET, CLEAR, and TOGGLE registers allow selected bits to be changed without overwriting the state of unrelated GPIO pins.

---

## 7. Opening up the Blink program

Last class we used the Pico SDK to blink an LED.

### Starting point: SDK Blink

```c title="sdk_blink.c"
#include "pico/stdlib.h"

int main(void) {
    const uint LED = PICO_DEFAULT_LED_PIN;

    gpio_init(LED);
    gpio_set_dir(LED, GPIO_OUT);

    while (true) {
        gpio_put(LED, 1);
        sleep_ms(500);

        gpio_put(LED, 0);
        sleep_ms(500);
    }
}
```

The important question is:

> **What is `gpio_put()` changing underneath?**

### Step 1 — Create the bit mask

If the LED is connected to GPIO `LED`, its bit can be selected with:

```c
const uint32_t LED_MASK = 1u << LED;
```

### Step 2 — Replace the output operations

The SDK instruction:

```c
gpio_put(LED, 1);
```

can be represented at a lower level as:

```c
sio_hw->gpio_set = LED_MASK;
```

and:

```c
gpio_put(LED, 0);
```

as:

```c
sio_hw->gpio_clr = LED_MASK;
```

### Register-level Blink

```c title="sio_blink.c"
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"

int main(void) {
    const uint LED = PICO_DEFAULT_LED_PIN;
    const uint32_t LED_MASK = 1u << LED;

    gpio_init(LED);

    // OE = Output Enable
    sio_hw->gpio_oe_set = LED_MASK;

    while (true) {
        sio_hw->gpio_set = LED_MASK;
        sleep_ms(500);

        sio_hw->gpio_clr = LED_MASK;
        sleep_ms(500);
    }
}
```

Both programs produce the same visible behavior.

The difference is the abstraction level:

```text
Pico SDK

gpio_put(LED, 1)
        ↓
      SDK
        ↓
SIO register operation
        ↓
GPIO hardware
```

!!! exercise "Open the abstraction"
    Starting from the SDK Blink from the previous lesson:

    1. keep `gpio_init()`;
    2. replace `gpio_set_dir()` with `sio_hw->gpio_oe_set`;
    3. replace each `gpio_put()` with the corresponding SIO SET/CLEAR operation;
    4. compile and test if hardware is available.

---

## 8. SDK vs register-level operations

| Intent | Pico SDK | SIO register |
|---|---|---|
| Configure output | `gpio_set_dir(pin, GPIO_OUT)` | `sio_hw->gpio_oe_set = mask` |
| Configure input | `gpio_set_dir(pin, GPIO_IN)` | `sio_hw->gpio_oe_clr = mask` |
| Set HIGH | `gpio_put(pin, 1)` | `sio_hw->gpio_set = mask` |
| Set LOW | `gpio_put(pin, 0)` | `sio_hw->gpio_clr = mask` |
| Toggle selected pins | `gpio_xor_mask(mask)` | `sio_hw->gpio_togl = mask` |
| Read a pin | `gpio_get(pin)` | `sio_hw->gpio_in & mask` |

!!! tip "Why use an SDK?"
    Register-level access helps us understand the hardware.

    The SDK usually gives us:

    - clearer code;
    - easier portability;
    - less dependence on register details;
    - useful safety and convenience functions.

    **The SDK is not magic; it is an abstraction over the hardware.**

---

## 9. Comparing abstraction layers

The same GPIO concept appears in many embedded environments.

### Pico SDK

```c
gpio_init(pin);
gpio_set_dir(pin, GPIO_OUT);
gpio_put(pin, 1);
gpio_put(pin, 0);
```

For several GPIO pins:

```c
gpio_set_mask(mask);
gpio_clr_mask(mask);
gpio_xor_mask(mask);
gpio_put_masked(mask, value);
```

### Arduino

```cpp
pinMode(pin, OUTPUT);
digitalWrite(pin, HIGH);
digitalWrite(pin, LOW);
```

### MicroPython / CircuitPython style

```python
from machine import Pin

p = Pin(pin, Pin.OUT)
p.on()
p.off()
```

Although the syntax and abstraction level change, all of them eventually configure and manipulate the MCU's GPIO hardware.

```text
Application code
      ↓
SDK / HAL / Runtime
      ↓
Peripheral registers
      ↓
GPIO hardware
```

---

## 10. Controlling several GPIOs with one mask

Suppose three LEDs are connected to GPIO2, GPIO4, and GPIO6.

```c
#define PIN_A 2
#define PIN_B 4
#define PIN_C 6

const uint32_t MASK =
    (1u << PIN_A) |
    (1u << PIN_B) |
    (1u << PIN_C);
```

Initialize them:

```c
gpio_init(PIN_A);
gpio_init(PIN_B);
gpio_init(PIN_C);
```

Configure all three as outputs with one operation:

```c
sio_hw->gpio_oe_set = MASK;
```

Then:

```c
sio_hw->gpio_set  = MASK;   // all HIGH
sio_hw->gpio_clr  = MASK;   // all LOW
sio_hw->gpio_togl = MASK;   // toggle all
```

The SDK provides equivalent operations:

```c
gpio_set_mask(MASK);
gpio_clr_mask(MASK);
gpio_xor_mask(MASK);
```

### Example

```c title="multi_gpio.c"
#include "pico/stdlib.h"
#include "hardware/structs/sio.h"

#define PIN_A 2
#define PIN_B 4
#define PIN_C 6

int main(void) {
    const uint32_t MASK =
        (1u << PIN_A) |
        (1u << PIN_B) |
        (1u << PIN_C);

    gpio_init(PIN_A);
    gpio_init(PIN_B);
    gpio_init(PIN_C);

    sio_hw->gpio_oe_set = MASK;

    while (true) {
        sio_hw->gpio_set = MASK;
        sleep_ms(300);

        sio_hw->gpio_clr = MASK;
        sleep_ms(300);

        sio_hw->gpio_togl = MASK;
        sleep_ms(300);
    }
}
```

!!! exercise "Mini challenge"
    Using GPIO2, GPIO4, and GPIO6:

    1. create one mask selecting all three pins;
    2. configure them as outputs with one masked operation;
    3. set all three HIGH;
    4. clear all three;
    5. toggle all three.

    Try implementing the same sequence once with **SIO registers** and once with the **Pico SDK**.

---

# Exercises

A practical arrangement is four LEDs connected to consecutive GPIOs:

```text
GPIO2 → LED0 → resistor → GND
GPIO3 → LED1 → resistor → GND
GPIO4 → LED2 → resistor → GND
GPIO5 → LED3 → resistor → GND
```

Use one current-limiting resistor for each LED.

---

## Exercise 1 — 4-bit binary counter

Use the four LEDs as a **4-bit binary number**.

The program must count:

```text
0 → 1 → 2 → ... → 15 → 0
```

For example:

```text
Decimal   Binary   LEDs
0         0000     ○ ○ ○ ○
1         0001     ○ ○ ○ ●
2         0010     ○ ○ ● ○
...
15        1111     ● ● ● ●
```

produce the same LED pattern.

!!! question "Think about it"
    Does the CPU know whether you wrote the value in decimal, hexadecimal, or binary?

---

## Exercise 2 — Bouncing light

Create a light that moves across the four LEDs and then returns:

```text
0001
0010
0100
1000
0100
0010
0001
...
```
---

## Exercise 3 — Fill and empty animation

Create an animation that progressively fills all four LEDs and then progressively empties them.

For example:

```text
○ ○ ○ ○
○ ○ ○ ●
○ ○ ● ●
○ ● ● ●
● ● ● ●
● ● ● ○
● ● ○ ○
● ○ ○ ○
○ ○ ○ ○
...
```

## Exercise 4 — Fill from the outside inward

Create an animation that progressively fills all four LEDs and then progressively empties them.

```text
○ ○ ○ ○
● ○ ○ ●
● ● ● ●
○ ● ● ○
○ ○ ○ ○
...
```

---

# Reference

## Pico 2 Pinout

![Pico 2 Pinout](../images/pico-2-r4-pinout.svg)

## Reset wiring

![Reset Wiring](../images/pico-reset-button-1.png)

## SIO quick reference

!!! note "Required include"
    ```c
    #include "hardware/structs/sio.h"
    ```

    Route each pin to SIO using `gpio_init(pin)` before using the examples below.

| Purpose | Register / Field | Operation |
|---|---|---|
| Read GPIOs | `sio_hw->gpio_in` | Read current input levels |
| SET outputs | `sio_hw->gpio_set = mask` | Selected GPIOs HIGH |
| CLEAR outputs | `sio_hw->gpio_clr = mask` | Selected GPIOs LOW |
| TOGGLE outputs | `sio_hw->gpio_togl = mask` | Toggle selected GPIOs |
| Direct output write | `sio_hw->gpio_out = value` | Replace the output register value |
| Direction OUT | `sio_hw->gpio_oe_set = mask` | Selected GPIOs become outputs |
| Direction IN | `sio_hw->gpio_oe_clr = mask` | Selected GPIOs become inputs |

## Pico SDK GPIO quick reference

!!! note "Required include"
    ```c
    #include "hardware/gpio.h"
    ```

| Purpose | Function |
|---|---|
| Initialize one GPIO | `gpio_init(pin)` |
| Initialize several GPIOs | `gpio_init_mask(mask)` |
| Set direction | `gpio_set_dir(pin, out)` |
| Several outputs | `gpio_set_dir_out_masked(mask)` |
| Several inputs | `gpio_set_dir_in_masked(mask)` |
| Write one GPIO | `gpio_put(pin, value)` |
| Read one GPIO | `gpio_get(pin)` |
| Set several HIGH | `gpio_set_mask(mask)` |
| Clear several LOW | `gpio_clr_mask(mask)` |
| Toggle several | `gpio_xor_mask(mask)` |
| Write masked pattern | `gpio_put_masked(mask, value)` |
