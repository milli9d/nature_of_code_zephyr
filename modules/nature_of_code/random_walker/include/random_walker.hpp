#pragma once

#include <cstdint>
#include <cstdbool>

#include <zephyr/kernel.h>

namespace noc {

class random_walker
{
  private:
    struct {
        uint32_t x;
        uint32_t y;
    } _pos;
};

} // namespace noc