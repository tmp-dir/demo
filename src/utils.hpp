#pragma once
#include <cmath>
#include <chrono>

bool doubles_equal(double a, double b, double epsilon=1e-8)
{
    return std::fabs(a-b) < epsilon;
}

/// TIME UTILS

// is system clock correct here? should i be using my own impl?
int to_unix_ts(const std::chrono::system_clock::time_point& tp)
{
    // c++20 guarantees epoch. though i only use relative + one way translation here. so is fine.
    return std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count();
}

int construct_time(int y, int m, int d, int h=0)
{
    std::tm ft { .tm_hour=h, .tm_mday=d, .tm_mon = m -1, .tm_year= y-1900};
    /* ft.tm_isdst = -1; */
    auto ct = std::chrono::system_clock::from_time_t(timegm(&ft));
    return to_unix_ts(ct);
}
