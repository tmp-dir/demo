#pragma once

#include "utils.hpp"

constexpr double SECONDS_IN_YEAR { 60*60*24*365.25 };

class InstrumentMeta
{
public:
    int expiry;
    std::string market;
    std::string feedcode;
    std::size_t hash;
    InstrumentMeta(int expiry_,
        std::string market_,
        std::string feedcode_): expiry{expiry_}, market{market_}, feedcode{feedcode_}
    { 
        auto instrument_blob{ market + ":" + feedcode + std::to_string(expiry) };
        hash = std::hash<std::string>{}(instrument_blob);
    }

    bool operator==(const InstrumentMeta& o) const
    {
        return o.hash == hash;
    }
};

struct InstrumentMetaHasher {
    std::size_t operator()(const InstrumentMeta& f) const {
        return f.hash;
    }
};

struct PriceUpdate
{
    InstrumentMeta meta;
    double price; // wow great stuff who knows what goes here.
};


struct Clock 
{
    // kinda shitty. changing the convention and return type
    virtual int now() const
    {
        const auto tp{ std::chrono::system_clock::now() };
        return to_unix_ts(tp);
    }
};

struct MockClock : Clock 
{
    int fixed;
    explicit MockClock(int fixed_): fixed{fixed_} {}

    int now() const override { return fixed; }
};

