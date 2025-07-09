#include "model.hpp"
#include "utils.hpp"
#include "common.hpp"
#include <iostream>
/* #include <cassert> */
/* #include <chrono> */
/* #include <string> */
/* #include <unordered_map> */
/* #include <iostream> */
/* #include <iomanip> */
/* #include <cmath> // std::log, std::exp */



int main()
{
    /* using Clock = std::chrono::system_clock; */
    /* auto now = Clock::now(); */

    std::vector<PriceUpdate> futs {
        {{construct_time(2025, 9,  26), "bybt", "btc-250926"}, 105000},
        {{construct_time(2025, 12, 30), "bybt", "btc-250926"}, 110000},
    };

    auto clock = Clock();
    LinRegModel<Clock> model {clock, 100000., futs};

    // c++ 20 onwards? + unix assumption? though i care only for relative so epoch is chill.
    // this is giving me a LOCAL TIME. not UNIX. YUCKY.
    const auto p1 = to_unix_ts(std::chrono::system_clock::now());

}

// fairly sure adding different markets without adjusting could be kinda weird.
// small tte is a big problem since it'll blow out the rate
// this also only deals with RAW rates. should throw some ewm on it
// or at least esure the price is at depth, particularly for the illiquid contracts.
// this also doesnt mess with USDC/T/E/ whatever. so could get funky around some points.

