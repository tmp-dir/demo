#pragma once

#include <cassert>
#include "common.hpp"

template<typename ClockT>
class LinRegModel
{
public:
    using MapT = std::unordered_map<InstrumentMeta, std::pair<double, double>, InstrumentMetaHasher>;

    LinRegModel(ClockT clock_, double spot_, std::vector<PriceUpdate> instruments)
        : clock{ clock_ }, spot{spot_}
    {
        for (auto i : instruments)
            add_instrument(i);
    }

    void update_spot(double price) { spot = price; }

    void update_instrument(const PriceUpdate& update)
    {
        auto [old_log_price, old_T] = future_map[update.meta];
        auto diff = (update.meta.expiry - clock.now());
        auto T = diff / SECONDS_IN_YEAR;

        s_x += (T - old_T);
        s_xx += (T*T - old_T*old_T);

        auto old_inv_t = 1/old_T;
        auto new_inv_t = 1/T;
        auto new_log_price = std::log(update.price);

        f_i_t_i += (new_log_price*new_inv_t - old_log_price*old_inv_t);
        t_i += (new_inv_t - old_inv_t);
        f_i += (new_log_price - old_log_price);

        future_map[update.meta] = {new_log_price, T};
    }

    double get_rate(int expiry)
    {
        auto now = clock.now();
        if (expiry < now)
        {
            // arguable. result will be "sensical",
            // but almost certainly a bug in whatever is calling this.
            return std::numeric_limits<double>::quiet_NaN();
        }

        auto diff = expiry - now;
        auto T = diff / SECONDS_IN_YEAR;
        auto [intercept, slope] = get_coeff();

        return slope*T + intercept;
    }

    void add_instrument(const PriceUpdate& update)
    {
        auto diff = (update.meta.expiry - clock.now());
        constexpr double YEAR_IN_S = 60*60*24*365.25;

        auto T = diff / YEAR_IN_S;
        auto rate = std::log(update.price/spot)/T;

        ++n;
        s_x += T;
        s_xx += T*T;

        auto inv_t = 1/T;
        auto log_price = std::log(update.price);

        f_i_t_i += log_price*inv_t;
        t_i += inv_t;

        f_i += log_price;

        future_map[update.meta] = {log_price, T};
    }

    const MapT& get_map() { return future_map; }

    std::pair<double, double> get_coeff() const
    {
        double denom = n* s_xx - s_x*s_x;
        if ((n < 2) || doubles_equal(denom, 0.0))
            return { std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN() };

        // see README
        auto s_xy = n*std::log(spot) - f_i;
        auto s_y = std::log(spot) * t_i - f_i_t_i;

        double slope = (n * s_xy - s_x * s_y) / denom;
        double intercept = (s_y - slope * s_x) / n;

        // messed up the sign.
        return {-intercept, -slope};
    }

private:
    MapT future_map;
    ClockT clock;
    double spot, s_x, s_xx, n, f_i, t_i, f_i_t_i;

    void remove_instrument(const InstrumentMeta& meta)
    {
        auto [old_log_price, T] = future_map[meta];

        --n;
        s_x -= T;
        s_xx -= T*T;

        auto old_inv_t = 1/T;

        f_i_t_i -= old_log_price*old_inv_t;
        t_i -= old_inv_t;
        f_i -= old_log_price;

        future_map.erase(meta);
    }

};
