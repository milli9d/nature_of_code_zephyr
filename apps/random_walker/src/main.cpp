#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstdbool>

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/display/cfb.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/random/random.h>
#include <zephyr/input/input.h>
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
#define TFT_Y_OFF 0u
#define TFT_BPP   2u

#define TFT_LED_BRIGHTNESS 1u // 60% brightness

/* ========================================================================= */
/* INIT Section */
/* ========================================================================= */

/**
 * @brief init TFT LED light
 * @return
 */
static int init_tft_led()
{
    /* turn on PWM led0 at 80% brightness */
    const struct pwm_dt_spec tft_led = PWM_DT_SPEC_GET(DT_ALIAS(pwm_lcd0));
    if (!pwm_is_ready_dt(&tft_led)) {
        LOG_ERR("TFT LED PWM device not ready");
        return -ENODEV;
    }

    int rc = pwm_set_pulse_dt(&tft_led, (tft_led.period * TFT_LED_BRIGHTNESS) / 100u);
    if (rc != 0) {
        LOG_ERR("Failed to set TFT LED PWM pulse: %d", rc);
        return rc;
    }

    return 0;
}

SYS_INIT(init_tft_led, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

/* ========================================================================= */
/* Private */
/* ========================================================================= */

#ifdef CONFIG_INPUT
static const struct device* const touch_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_input));
K_SEM_DEFINE(ts_sync, 0, 1);

static struct {
    size_t x;
    size_t y;
    bool   pressed;
} touch_point, touch_point_drawn;

static void touch_event_callback(struct input_event* evt, void* user_data)
{
    if (evt->code == INPUT_ABS_X) {
        touch_point.x = evt->value;
    }
    if (evt->code == INPUT_ABS_Y) {
        touch_point.y = evt->value;
    }
    if (evt->code == INPUT_BTN_TOUCH) {
        touch_point.pressed = evt->value;
    }

    if (evt->sync) {
        LOG_DBG("Touch event: code=%d, value=%d, x=%zu, y=%zu, pressed=%d", evt->code, evt->value,
                touch_point.x, touch_point.y, touch_point.pressed);
        if (touch_point.pressed) {
            k_sem_give(&ts_sync);
        }
    }
}
INPUT_CALLBACK_DEFINE(touch_dev, touch_event_callback, NULL);
#endif

static int clear_screen(const struct device* dev)
{
    noc::canvas::canvas_cfg_t _cfg = { .w_pix = 1u,
                                       .h_pix = 1u,
                                       .x_max = TFT_W,
                                       .y_max = TFT_H,
                                       .bpp   = 2u };

    noc::canvas               _canvas = noc::canvas(_cfg);
    display_buffer_descriptor desc    = { .buf_size = _canvas.get_frame_buffer().get_size(),
                                          .width    = (uint16_t)_canvas.get_frame_buffer().get_width(),
                                          .height = (uint16_t)_canvas.get_frame_buffer().get_height(),
                                          .pitch  = (uint16_t)_canvas.get_frame_buffer().get_width(),
                                          .frame_incomplete = false };

    /* Fill the canvas with random colors */
    int rc = display_write(dev, TFT_X_OFF, TFT_Y_OFF, &desc, (void*)_canvas.get_frame_buffer_raw());
    if (rc != 0) {
        LOG_ERR("Failed to write to display: %d", rc);
        return rc;
    }
    return 0;
}

/* ========================================================================= */
/* Main Section */
/* ========================================================================= */

int main()
{
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

    rc = clear_screen(dev);
    if (rc != 0) {
        LOG_ERR("Failed to clear screen: %d", rc);
        return rc;
    }

    /* initialize the random walker */
    uint32_t                  count = 0;
    uint16_t                  color = sys_rand16_get() % UINT16_MAX;
    static noc::random_walker _rand_walk =
        noc::random_walker(sys_rand32_get() % TFT_W, sys_rand32_get() % TFT_H);

    while (1) {
#ifdef CONFIG_INPUT
        if (k_sem_take(&ts_sync, K_NO_WAIT) == 0) {
            rc = clear_screen(dev);
            if (rc != 0) {
                LOG_ERR("Failed to clear screen: %d", rc);
                return rc;
            }
            color            = sys_rand16_get() % UINT16_MAX;
            _rand_walk.pos.x = touch_point.x;
            _rand_walk.pos.y = touch_point.y;
            LOG_DBG("TOUCH %s X, Y: (%zu, %zu)", touch_point.pressed ? "PRESS" : "RELEASE",
                    touch_point.x, touch_point.y);
        }
#endif
        /* randomize color for artistic results */
        if (count % 2000u == 0) {
            color = sys_rand16_get() % UINT16_MAX;
        }

        /* draw current pixel */
        struct display_buffer_descriptor desc = { .buf_size         = sizeof(color),
                                                  .width            = 1u,
                                                  .height           = 1u,
                                                  .pitch            = 1u,
                                                  .frame_incomplete = false };
        rc = display_write(dev, TFT_X_OFF + _rand_walk.pos.x, TFT_Y_OFF + _rand_walk.pos.y, &desc,
                           &color);
        if (rc != 0) {
            LOG_ERR("Failed to write to display: %d", rc);
            return rc;
        }

        /* get next pixel location */
        (void)_rand_walk.next();
        count++;

        k_sleep(K_USEC(1u));
    }

    return 0;
}
