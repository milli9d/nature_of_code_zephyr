#pragma once

#include <cstdint>
#include <cstdbool>

#include <zephyr/kernel.h>

namespace noc {

class random_walker
{
  public:
    using pos_t = struct {
        uint32_t x;
        uint32_t y;
    };

  public:
    pos_t pos; ///< Current position of the random walker
    const pos_t& next();

    random_walker(const pos_t& pos_in);
    random_walker(uint32_t x, uint32_t y);
    random_walker();
};

} // namespace noc