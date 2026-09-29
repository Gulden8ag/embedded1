#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"
#include "hardware/structs/timer.h"
#include "hardware/structs/sio.h"

#define TEST_PIN           15
#define ISR_MARK_PIN       14
#define ALARM_NUM          0
#define INTERVAL_US        10000u
#define WORK_US            2000u
#define CUMULATIVE_MODE    1

#define ALARM_IRQ timer_hardware_alarm_get_irq_num(timer_hw, ALARM_NUM)

static volatile uint32_t next_deadline;
static bool state = false;

static void on_alarm_irq(void) {
    // W1C: clear the pending alarm interrupt.
    timer_hw->intr = 1u << ALARM_NUM;

    gpio_put(ISR_MARK_PIN, 1);

    state = !state;
    gpio_put(TEST_PIN, state);

    // Intentional workload so timing differences are visible.
    busy_wait_us_32(WORK_US);

    gpio_put(ISR_MARK_PIN, 0);

#if CUMULATIVE_MODE
    // Stay aligned to the ideal timeline.
    next_deadline += INTERVAL_US;
#else
    // Schedule relative to the time at which the ISR finishes its work.
    next_deadline = timer_hw->timerawl + INTERVAL_US;
#endif

    timer_hw->alarm[ALARM_NUM] = next_deadline;
}

int main() {
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);
    gpio_put(TEST_PIN, 0);

    gpio_init(ISR_MARK_PIN);
    gpio_set_dir(ISR_MARK_PIN, GPIO_OUT);
    gpio_put(ISR_MARK_PIN, 0);

    hardware_alarm_claim(ALARM_NUM);
    irq_set_exclusive_handler(ALARM_IRQ, on_alarm_irq);

    timer_hw->intr = 1u << ALARM_NUM;
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_enabled(ALARM_IRQ, true);

    next_deadline = timer_hw->timerawl + INTERVAL_US;
    timer_hw->alarm[ALARM_NUM] = next_deadline;

    while (true) {
        tight_loop_contents();
    }
}
