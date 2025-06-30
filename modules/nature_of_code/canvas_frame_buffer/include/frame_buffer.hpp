#pragma once

#include <iostream>

#include <cstdint>
#include <cstdbool>
#include <cstdlib>

namespace noc {

#define FRAME_BOUND_X 400u
#define FRAME_BOUND_Y 400u
#define FRAME_BPP_MAX 8u

class frame_buffer
{
  private:
    /* ===================================================================== */
    /* Private data */
    /* ===================================================================== */

    /* deep private */
    struct {
        bool stk_alloc;
    } _pvt;

    /* normal private */
    uint8_t* _data{ nullptr }; // heap or stack allocated through constructor
    uint32_t _width{ 0u };
    uint32_t _height{ 0u };
    uint32_t _size{ 0u };
    uint32_t _bpp{ 0u };

    /* ===================================================================== */
    /* Private API */
    /* ===================================================================== */

  public:
    /* ===================================================================== */
    /* Public API */
    /* ===================================================================== */

    /**
     * @brief Overloaded operator() to access pixel data in the frame buffer.
     *
     * This operator allows for accessing the pixel data at a specific row and column
     * in the frame buffer. It returns a pointer to the pixel data represented as
     * an array of uint8_t values.
     *
     * @param row The row index of the pixel in the frame buffer.
     * @param col The column index of the pixel in the frame buffer.
     * @return A pointer to the pixel data at the specified (row, col).
     */
    uint8_t* operator()(uint32_t row, uint32_t col);

    /**
     * @brief get the bytes per pixel value
     * @return
     */
    constexpr uint32_t get_bpp()
    {
        return _bpp;
    }

    /**
     * @brief get the total size of the buffer in bytes
     * @return
     */
    constexpr uint32_t get_size()
    {
        return _size;
    }

    /**
     * @brief get the width of the framebuffer in pixels
     * @return
     */
    constexpr uint32_t get_width()
    {
        return _width;
    }

    /**
     * @brief get the height of the framebuffer in pixels
     * @return
     */
    constexpr uint32_t get_height()
    {
        return _height;
    }

    /**
     * @brief get the raw data buffer
     * @return
     */
    uint8_t* get_buffer();

    /* ===================================================================== */
    /* Constructors */
    /* ===================================================================== */

    /* delete default constructor */
    frame_buffer() = delete;

    void fill_buffer(uint8_t* val, uint8_t val_size);

    void fill_buffer_rand();

    /**
     * @brief Constructs a frame buffer with the specified dimensions and bits per pixel (BPP).
     *
     * This constructor initializes a frame buffer object, allocating memory for the pixel data
     * based on the provided width, height, and bits per pixel (BPP). It checks for excessive
     * dimensions and BPP values to prevent allocation failures.
     *
     * @param width The width of the frame buffer in pixels.
     * @param height The height of the frame buffer in pixels.
     * @param bpp The number of bits per pixel (must not exceed FRAME_BPP_MAX).
     *
     * @note If the product of width and height exceeds FRAME_BOUND_X * FRAME_BOUND_Y,
     *       or if BPP exceeds FRAME_BPP_MAX, an error will be logged and the constructor
     *       will return without allocating the frame buffer.
     */
    frame_buffer(uint32_t width, uint32_t height, uint32_t bpp);

    /**
     * @brief Destructor for the frame_buffer class.
     *
     * This destructor is responsible for freeing the allocated memory for the
     * frame buffer data. It checks if the data was heap allocated and only
     * frees it if that is the case. If the data was not heap allocated, it
     * does nothing.
     *
     * @note The destructor sets the pointer _data to nullptr after freeing
     *       the memory to avoid dangling pointers.
     */
    ~frame_buffer();
};

} // namespace noc