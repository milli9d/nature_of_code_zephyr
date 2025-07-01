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

const random_walker::pos_t& random_walker::next()
{
    do {
        pos.x = std::max(pos.x + ((sys_rand32_get() % 3u) - 1u), 0u);
        pos.x = std::min(pos.x, 240u - 1u);
    }
    while (pos.x >= 240u);

    do {
        pos.y = std::max(pos.y + ((sys_rand32_get() % 3u) - 1u), 0u);
        pos.y = std::min(pos.y, 280u - 1u);
    }
    while (pos.y >= 280u);

    LOG_DBG("Pos X %d Y %d", pos.x, pos.y);
    return pos;
}

/* ========================================================================= */
/* Constructors */
/* ========================================================================= */

random_walker::random_walker(const pos_t& pos_in) : pos(pos_in) {}

random_walker::random_walker()
{
    pos.x = 0u;
    pos.y = 0u;
}

} // namespace noc