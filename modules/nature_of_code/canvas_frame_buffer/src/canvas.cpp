#include <iostream>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(canvas);

#include <canvas.hpp>
#include <frame_buffer.hpp>

namespace noc {

uint8_t* canvas::get_frame_buffer_raw(void)
{
    if (!_fb) {
        LOG_ERR("Frame buffer is not initialized");
        throw std::runtime_error("Frame buffer is not initialized");
    }
    return _fb->get_buffer();
}

noc::frame_buffer& canvas::get_frame_buffer(void)
{
    if (!_fb) {
        LOG_ERR("Frame buffer is not initialized");
        throw std::runtime_error("Frame buffer is not initialized");
    }
    return *_fb;
}

void canvas::write_pixel(uint32_t x, uint32_t y, uint8_t* color, uint8_t color_sz)
{
    /* we want to write a square of w_pix * h_pix at x,y */
    if (x >= _cfg.x_max || y >= _cfg.y_max) {
        LOG_ERR("Coordinates out of bounds: x = %u, y = %u", x, y);
        return;
    }
    if (color_sz != _cfg.bpp) {
        LOG_ERR("Color size %u does not match BPP %u", color_sz, _cfg.bpp);
        return;
    }
    if (!_fb) {
        LOG_ERR("Frame buffer is not initialized");
        return;
    }
    uint32_t start_x = x * _cfg.w_pix;
    uint32_t start_y = y * _cfg.h_pix;
    for (uint32_t i = 0; i < _cfg.h_pix; ++i) {
        for (uint32_t j = 0; j < _cfg.w_pix; ++j) {
            uint8_t* pixel = (*_fb)(start_y + i, start_x + j);
            if (pixel) {
                memcpy(pixel, color, color_sz);
            } else {
                LOG_ERR("Failed to write pixel at (%u, %u)", start_x + j, start_y + i);
            }
        }
    }
}

canvas::canvas(const canvas_cfg_t& cfg) : _cfg(cfg)
{
    /* TODO : add invariant */

    /* allocate frame buffer */
    _fb =
        std::make_shared<frame_buffer>(_cfg.x_max * _cfg.w_pix, _cfg.y_max * _cfg.h_pix, _cfg.bpp);
    if (!_fb) {
        LOG_ERR("Could not allocate frame buffer");
    }

    LOG_DBG("Canvas created with config: w_pix = %u, h_pix = %u, x_max = %u, y_max = %u, bpp = %u",
            _cfg.w_pix, _cfg.h_pix, _cfg.x_max, _cfg.y_max, _cfg.bpp);
}

} // namespace noc