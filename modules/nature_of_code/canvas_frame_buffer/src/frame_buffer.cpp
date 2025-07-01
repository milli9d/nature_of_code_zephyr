#include <iostream>

#include <cstdint>
#include <cstdbool>
#include <cstdlib>

#include <frame_buffer.hpp>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(frame_buf);

/**
 * @brief
 */
namespace noc {

/* ========================================================================= */
/* Private API */
/* ========================================================================= */

/* ========================================================================= */
/* Public API */
/* ========================================================================= */

/**
 * @brief get the raw data buffer
 * @return pointer to data buffer [uint8_t*]
 */
uint8_t* frame_buffer::get_buffer()
{
    return _data;
}

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
uint8_t* frame_buffer::operator()(uint32_t row, uint32_t col)
{
    if (row >= _height || col >= _width) {
        LOG_ERR("Index out of bounds\n");
        return nullptr;
    }

    uint32_t out_idx = (_width * row + col) * _bpp;
    if (out_idx >= _size) {
        LOG_ERR("Index out of bounds for size %u\n", _size);
        return nullptr;
    }

    LOG_DBG("_data(%u, %u) = index %u", row, col, out_idx);
    return &_data[out_idx];
}

void frame_buffer::fill_buffer(uint8_t* val, uint8_t val_size)
{
    /* S.C : check if value matches the size expected */
    if (val_size != _bpp) {
        LOG_ERR("Value size %u does not match BPP %u", val_size, _bpp);
        return;
    }

    for (uint32_t i = 0u; i < _size; i++) {
        memcpy(&_data[i], val, val_size);
    }
}

void frame_buffer::fill_buffer_rand()
{
    uint16_t* ptr = (uint16_t*)_data;
    for (uint32_t i = 0u; i < _size; i++) {
        ptr[i] = rand() % UINT16_MAX;
    }
}

/* ========================================================================= */
/* Constructors */
/* ========================================================================= */

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
frame_buffer::frame_buffer(uint32_t width, uint32_t height, uint32_t bpp)
{
    /* S.C : very large buffer */
    if (width * height > (FRAME_BOUND_X * FRAME_BOUND_Y)) {
        LOG_ERR("Product of Height and Width is too large %u %u\n", width, height);
        return;
    }

    /* S.C : very large BPP */
    if (bpp > FRAME_BPP_MAX) {
        LOG_ERR("BPP is crazy large %u", bpp);
        return;
    }

    /* init data members */
    _width  = width;
    _height = height;
    _bpp    = bpp;
    _size   = _width * _height * _bpp;
    _data   = (uint8_t*)calloc(sizeof(uint8_t), _size);

    _pvt.stk_alloc = false;

    if (_data) {
        LOG_DBG("Frame Buffer allocated on Heap (0x%p) : w = %u h = %u bpp = %u sz = %u",
                (void*)_data, _width, _height, _bpp, _size);
    }
}

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
frame_buffer::~frame_buffer()
{
    /* free only if heap allocated */
    if (_data && !_pvt.stk_alloc) {
        free(_data);
        _data = nullptr;
    }
}

} // namespace noc