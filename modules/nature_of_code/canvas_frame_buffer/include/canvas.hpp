#pragma once

#include <iostream>
#include <memory>

#include <frame_buffer.hpp>

namespace noc {

class canvas
{
    /**
     * @class canvas
     * @brief Represents a canvas for drawing operations with a frame buffer.
     *
     * This class provides functionality to manage a canvas with a specified configuration,
     * allowing for pixel manipulation and access to the underlying frame buffer.
     */
  public:
    /**
     * @struct canvas_cfg_t
     * @brief Configuration structure for the canvas.
     *
     * This structure holds the configuration parameters for the canvas, including
     * width, height, maximum x and y coordinates, and bits per pixel (bpp).
     */
    using canvas_cfg_t = struct {
        uint32_t w_pix; ///< Width of the canvas in pixels.
        uint32_t h_pix; ///< Height of the canvas in pixels.
        uint32_t x_max; ///< Maximum x coordinate.
        uint32_t y_max; ///< Maximum y coordinate.
        uint32_t bpp;   ///< Bits per pixel.
    };

  private:
    canvas_cfg_t                  _cfg{};         ///< Configuration of the canvas.
    std::shared_ptr<frame_buffer> _fb{ nullptr }; ///< Shared pointer to the frame buffer.

  public:
    /**
     * @brief Deleted default constructor.
     *
     * The default constructor is deleted to prevent instantiation without configuration.
     */
    canvas() = delete;

    /**
     * @brief Constructs a canvas with the specified configuration.
     * @param cfg The configuration parameters for the canvas.
     */
    canvas(const canvas_cfg_t& cfg);

    /**
     * @brief Retrieves a pointer to the raw frame buffer.
     * @return A pointer to the raw frame buffer data.
     */
    uint8_t* get_frame_buffer_raw(void);

    /**
     * @brief Retrieves a reference to the frame buffer.
     * @return A reference to the frame buffer object.
     */
    noc::frame_buffer& get_frame_buffer(void);

    /**
     * @brief Writes a pixel to the canvas at the specified coordinates.
     * @param x The x coordinate of the pixel.
     * @param y The y coordinate of the pixel.
     * @param color A pointer to the color data.
     * @param color_sz The size of the color data.
     */
    void write_pixel(uint32_t x, uint32_t y, uint8_t* color, uint8_t color_sz);
};

} // namespace noc