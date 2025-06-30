#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <frame_buffer.hpp>
#include <pin_defines.hpp>

/* TODO: Get from device tree */
#define TFT_W     240u
#define TFT_H     280u
#define TFT_X_OFF 0u
#define TFT_Y_OFF 20u
#define TFT_BPP   2u

/* ========================================================================= */
/* INIT Section */
/* ========================================================================= */

/**
 * @brief init TFT LED light
 * @return
 */
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

static void tft_test()
{
    static noc::frame_buffer frame_buf(TFT_W, TFT_H, TFT_BPP);

    // uint16_t color = rand() % UINT16_MAX;
    // frame_buf.fill_buffer((uint8_t*)&color, sizeof(color));
    frame_buf.fill_buffer_rand();

    const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(dev)) {
        printk("Device %s not ready\n", dev->name);
        return;
    }

    if (display_set_pixel_format(dev, PIXEL_FORMAT_RGB_565) != 0) {
        printk("Failed to set required pixel format\n");
        return;
    }

    int rc = display_blanking_off(dev);
    if (rc != 0) {
        printk("Failed to turn on display blanking: %d\n", rc);
    }

    struct display_buffer_descriptor _desc = { .buf_size         = ((TFT_W * TFT_H) * TFT_BPP),
                                               .width            = TFT_W,
                                               .height           = TFT_H,
                                               .pitch            = TFT_W,
                                               .frame_incomplete = false };

    rc = display_write(dev, TFT_X_OFF, TFT_Y_OFF, &_desc, (void*)frame_buf.get_buffer());
    if (rc != 0) {
        printk("Failed to write to display: %d\n", rc);
    }
}

int main()
{
    while (1) {
        tft_test();
        LOG_DBG("Main is running");
        k_sleep(K_MSEC(30));
    }

    return 0;
}
