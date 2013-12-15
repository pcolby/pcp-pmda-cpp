#include "gtest/gtest.h"

#include "pcp-cpp/units.hpp"

TEST(units, independent_values) {
    const pmUnits new_units = pcp::units(1,2,3, 4,5,6);
    EXPECT_EQ(new_units.dimSpace,   1);
    EXPECT_EQ(new_units.dimTime,    2);
    EXPECT_EQ(new_units.dimCount,   3);
    EXPECT_EQ(new_units.scaleSpace, 4);
    EXPECT_EQ(new_units.scaleTime,  5);
    EXPECT_EQ(new_units.scaleCount, 6);
}

TEST(units, lower_bound) {
    const pmUnits new_units = pcp::units(-8,-8,-8, 0,0,-8);
    EXPECT_EQ(new_units.dimSpace,   -8);
    EXPECT_EQ(new_units.dimTime,    -8);
    EXPECT_EQ(new_units.dimCount,   -8);
    EXPECT_EQ(new_units.scaleSpace,  0);
    EXPECT_EQ(new_units.scaleTime,   0);
    EXPECT_EQ(new_units.scaleCount, -8);
}

TEST(units, upper_bound) {
    const pmUnits new_units = pcp::units(7,7,7, 15,15,7);
    EXPECT_EQ(new_units.dimSpace,   7);
    EXPECT_EQ(new_units.dimTime,    7);
    EXPECT_EQ(new_units.dimCount,   7);
    EXPECT_EQ(new_units.scaleSpace, 15);
    EXPECT_EQ(new_units.scaleTime,  15);
    EXPECT_EQ(new_units.scaleCount, 7);
}

TEST(units, underflow) {
    const pmUnits new_units = pcp::units(-9,-9,-9, -1,-1,-9);
    EXPECT_EQ(new_units.dimSpace,    7);
    EXPECT_EQ(new_units.dimTime,     7);
    EXPECT_EQ(new_units.dimCount,    7);
    EXPECT_EQ(new_units.scaleSpace, 15);
    EXPECT_EQ(new_units.scaleTime,  15);
    EXPECT_EQ(new_units.scaleCount,  7);
}

TEST(units, overflow) {
    const pmUnits new_units = pcp::units(8,8,8, 16,16,8);
    EXPECT_EQ(new_units.dimSpace,   -8);
    EXPECT_EQ(new_units.dimTime,    -8);
    EXPECT_EQ(new_units.dimCount,   -8);
    EXPECT_EQ(new_units.scaleSpace,  0);
    EXPECT_EQ(new_units.scaleTime,   0);
    EXPECT_EQ(new_units.scaleCount, -8);
}
