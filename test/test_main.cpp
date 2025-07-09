#include "../src/model.hpp"
#include "../src/utils.hpp"
#include "../src/common.hpp"
#include "gtest/gtest.h"


TEST(ClockTests, TestTimeIsReal)
{
    // check timing, clocks outputting expected utc stamps
    auto time = construct_time(2025,12,30);
    auto utc_ts = 1767052800;
    ASSERT_EQ(time, utc_ts);

    // check mock clock mocks clock
    auto clock = MockClock(time);
    ASSERT_EQ(clock.now(), time);
}


TEST(ModelTests, TestConstruction)
{
    auto cur_time = construct_time(2025,7,1);
    auto clock = MockClock(cur_time);
    std::vector<PriceUpdate> futs {};

    LinRegModel<MockClock> model {clock, 100000., futs};

    auto time = construct_time(2025,8,1);
    
    EXPECT_TRUE(std::isnan(model.get_rate(time)));

    // now lets add an instrument and check that it correctly rips the rate
    auto i = InstrumentMeta{construct_time(2025, 9,  26), "bybt", "btc-250926"};
    model.add_instrument({i, 105000});

    auto m = model.get_map();
    ASSERT_TRUE(m.contains(i));
    auto [log_rate, T] = m[i];

    auto expected_rate = std::log(105000.);
    auto expected_T = (1758844800 - 1751328000)/SECONDS_IN_YEAR;
    std::cout << "exp rate: " << expected_rate << ", exp t: " << expected_T << "\n";

    EXPECT_DOUBLE_EQ(log_rate, expected_rate);
    EXPECT_DOUBLE_EQ(T, expected_T);

    // now lets add a second instrument. this should create a valid rate.
    model.add_instrument(
        {{construct_time(2025, 12, 30), "bybt", "btc-251230"}, 110000});

    auto [intercept, slope] = model.get_coeff();

    EXPECT_NEAR(intercept, 0.2172523130, 0.0000001);
    EXPECT_NEAR(slope, -0.052133116, 0.0000001);
    EXPECT_NEAR(model.get_rate(time), 0.212827599, 0.0000001);
}

class ModelTestFixture : public ::testing::Test
{
public:
    InstrumentMeta f1 {construct_time(2025, 9,  26), "bybt", "btc-250926"};
    InstrumentMeta f2 {construct_time(2025, 12,  30), "bybt", "btc-251230"};
    int frozen_time {construct_time(2025, 7, 1)};
    MockClock clock {frozen_time};
    
    std::vector<PriceUpdate> futs {{ f1, 105000.}, {f2, 110000.}};
    LinRegModel<MockClock> model {clock, 100000., futs};
};

TEST_F(ModelTestFixture, TestUpdate)
{
    auto time = construct_time(2025,8,1);
    EXPECT_NEAR(model.get_rate(time), 0.212827599, 0.0000001);

    auto update = PriceUpdate{f1, 106000.};

    model.update_instrument(update);

    EXPECT_NEAR(model.get_rate(time), 0.27607972, 0.000001);
}


TEST_F(ModelTestFixture, TestSomeOOB)
{
    auto before_now = construct_time(2025,6, 20);
    EXPECT_TRUE(std::isnan(model.get_rate(before_now)));
}

TEST_F(ModelTestFixture, ManyInstruments)
{
    for (int i = 1; i <= 10; i++)
        model.add_instrument({{construct_time(2026, i, 28), "bybt", "bleh"}, 110000. + i*1000.});

    auto time = construct_time(2025,8,1);
    EXPECT_NEAR(model.get_rate(time), 0.211477654, 0.0000001);

    auto update = PriceUpdate{f2, 90000.};
    model.update_instrument(update);

    EXPECT_NEAR(model.get_rate(time), 0.0839587931, 0.0000001);

    model.update_spot(80000.);
    EXPECT_NEAR(model.get_rate(time), 0.8360647245, 0.0000001);
}
