#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <frame_buffer.hpp>
#include <pin_defines.hpp>

/* TODO: Get from device tree */
#define TFT_WIDTH  240u
#define TFT_HEIGHT 240u
#define TFT_BPP    2u

/* ========================================================================= */
/* INIT Section */
/* ========================================================================= */

static int init_tft_led()
{
    /* turn on TFT LED */
    const struct gpio_dt_spec tft_led = TFT_LED_GPIO;
    gpio_pin_configure_dt(&tft_led, GPIO_OUTPUT_ACTIVE | GPIO_ACTIVE_HIGH);
    return 0;
}
SYS_INIT(init_tft_led, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/* ========================================================================= */
/* Main Section */
/* ========================================================================= */

int main()
{
    static noc::frame_buffer frame_buf(TFT_WIDTH, TFT_HEIGHT, TFT_BPP);

    while (1) {
        LOG_DBG("Main is running");
        k_sleep(K_SECONDS(10u));
    }

    return 0;
}
