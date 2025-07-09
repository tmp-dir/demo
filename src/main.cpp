#include "model.hpp"
#include "utils.hpp"
#include "common.hpp"
#include <iostream>


int main()
{
    std::vector<PriceUpdate> futs {
        {{construct_time(2025, 9,  26), "bybt", "btc-250926"}, 105000},
        {{construct_time(2025, 12, 30), "bybt", "btc-250926"}, 110000},
    };

    auto spot = 100000.;
    auto clock = Clock();
    LinRegModel<Clock> model {clock, spot, futs};

    auto target_time = construct_time(2025, 8, 15);
    auto T = (target_time - clock.now())/SECONDS_IN_YEAR;

    auto rate = model.get_rate(target_time);
    auto theo = spot*std::exp(rate*T);

    std::cout << "future expiry 2025-08-15 has theo price: " << theo << "\n";
}
