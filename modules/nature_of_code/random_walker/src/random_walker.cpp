#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <cstdbool>
#include <iostream>

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(rand_walk);

#include <random_walker.hpp>

namespace noc {

/* ========================================================================= */
/* Public API */
/* ========================================================================= */

static int rand_arr[] = { -1, 0, 0, 1, -1, 1, -1, -1, 1, 1 };

const random_walker::pos_t& random_walker::next()
{
    size_t rand_arr_sz = sizeof(rand_arr) / sizeof(rand_arr[0u]);
    do {
        int off = rand_arr[sys_rand32_get() % rand_arr_sz];
        pos.x   = std::max(pos.x + off, 0u);
        pos.x   = std::min(pos.x, 240u);
    }
    while (pos.x >= 240u);

    do {
        int off = rand_arr[sys_rand32_get() % rand_arr_sz];
        pos.y   = std::max(pos.y + off, 0u);
        pos.y   = std::min(pos.y, 280u);
    }
    while (pos.y >= 280u);

    LOG_DBG("Pos X %d Y %d", pos.x, pos.y);
    return pos;
}

/* ========================================================================= */
/* Constructors */
/* ========================================================================= */

random_walker::random_walker(const pos_t& pos_in) : pos(pos_in) {}

random_walker::random_walker(uint32_t x, uint32_t y)
{
    pos.x = x;
    pos.y = y;
}

random_walker::random_walker()
{
    pos.x = 0u;
    pos.y = 0u;
}

} // namespace noc