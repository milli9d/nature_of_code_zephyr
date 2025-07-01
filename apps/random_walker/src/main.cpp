#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstdbool>

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

#include <canvas.hpp>
#include <random_walker.hpp>
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

int main()
{
    static noc::canvas::canvas_cfg_t _cfg = { .w_pix = 1u,
                                              .h_pix = 1u,
                                              .x_max = TFT_W,
                                              .y_max = TFT_H,
                                              .bpp   = 2u };

    static noc::canvas _canvas = noc::canvas(_cfg);

    const struct device* dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(dev)) {
        LOG_ERR("Device %s not ready", dev->name);
        return -ENODEV;
    }

    if (display_set_pixel_format(dev, PIXEL_FORMAT_RGB_565) != 0) {
        LOG_ERR("Failed to set required pixel format");
        return -ENOSYS;
    }

    int rc = display_blanking_off(dev);
    if (rc != 0) {
        LOG_ERR("Failed to turn on display blanking: %d", rc);
        return rc;
    }

    {
        struct display_buffer_descriptor desc = {
            .buf_size         = _canvas.get_frame_buffer().get_size(),
            .width            = (uint16_t)_canvas.get_frame_buffer().get_width(),
            .height           = (uint16_t)_canvas.get_frame_buffer().get_height(),
            .pitch            = (uint16_t)_canvas.get_frame_buffer().get_width(),
            .frame_incomplete = false
        };
        /* Fill the canvas with random colors */
        rc = display_write(dev, TFT_X_OFF, TFT_Y_OFF, &desc, (void*)_canvas.get_frame_buffer_raw());
        if (rc != 0) {
            LOG_ERR("Failed to write to display: %d", rc);
            return rc;
        }
    }

    uint16_t                  color      = sys_rand16_get() % UINT16_MAX;
    noc::random_walker::pos_t _pos       = { .x = sys_rand32_get() % TFT_W,
                                             .y = sys_rand32_get() % TFT_H };
    static noc::random_walker _rand_walk = noc::random_walker(_pos);

    uint32_t count = 0;
    while (1) {
        struct display_buffer_descriptor desc = { .buf_size         = 2u,
                                                  .width            = 1u,
                                                  .height           = 1u,
                                                  .pitch            = 1u,
                                                  .frame_incomplete = false };

        rc = display_write(dev, TFT_X_OFF + _pos.x, TFT_Y_OFF + _pos.y, &desc, &color);
        if (rc != 0) {
            LOG_ERR("Failed to write to display: %d", rc);
            return rc;
        }
        _pos = _rand_walk.next();
        count++;
        if (count % 2000u == 0) {
            color = sys_rand16_get() % UINT16_MAX;
        }

        k_sleep(K_USEC(10));
    }

    return 0;
}
