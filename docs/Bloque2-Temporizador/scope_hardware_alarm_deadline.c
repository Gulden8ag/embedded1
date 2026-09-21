#include "pico/stdlib.h"
#include "pico/time.h"

#define TEST_PIN   15
#define TOGGLE_MS  250

static repeating_timer_t timer;

static bool timer_callback(repeating_timer_t *rt) {
    static bool state = false;

    state = !state;
    gpio_put(TEST_PIN, state);

    return true;
}

int main() {
    gpio_init(TEST_PIN);
    gpio_set_dir(TEST_PIN, GPIO_OUT);
    gpio_put(TEST_PIN, 0);

    add_repeating_timer_ms(-TOGGLE_MS, timer_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
}
