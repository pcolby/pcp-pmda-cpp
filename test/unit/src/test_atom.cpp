#include "gtest/gtest.h"

#include "pcp-cpp/atom.hpp"

TEST(atom, pm_type_32) {
    // PM_TYPE_32 should support all signed integer types up to 32 bits.
    EXPECT_EQ(std::numeric_limits<int8_t>::min(),  pcp::atom(PM_TYPE_32, std::numeric_limits<int8_t>::min()).l);
    EXPECT_EQ(std::numeric_limits<int8_t>::max(),  pcp::atom(PM_TYPE_32, std::numeric_limits<int8_t>::max()).l);
    EXPECT_EQ(std::numeric_limits<int16_t>::min(), pcp::atom(PM_TYPE_32, std::numeric_limits<int16_t>::min()).l);
    EXPECT_EQ(std::numeric_limits<int16_t>::max(), pcp::atom(PM_TYPE_32, std::numeric_limits<int16_t>::max()).l);
    EXPECT_EQ(std::numeric_limits<int32_t>::min(), pcp::atom(PM_TYPE_32, std::numeric_limits<int32_t>::min()).l);
    EXPECT_EQ(std::numeric_limits<int32_t>::max(), pcp::atom(PM_TYPE_32, std::numeric_limits<int32_t>::max()).l);

    // PM_TYPE_32 should support floats.
    EXPECT_FLOAT_EQ(static_cast<float>(-1000.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(-1000.0)).l);
    EXPECT_FLOAT_EQ(static_cast<float>(   -1.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(   -1.0)).l);
    EXPECT_FLOAT_EQ(static_cast<float>(    0.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(    0.0)).l);
    EXPECT_FLOAT_EQ(static_cast<float>(    1.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(    1.0)).l);
    EXPECT_FLOAT_EQ(static_cast<float>( 1000.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>( 1000.0)).l);

    // PM_TYPE_32 should support doubles.
    EXPECT_DOUBLE_EQ(static_cast<double>(-1000.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(-1000.0)).l);
    EXPECT_DOUBLE_EQ(static_cast<double>(   -1.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(   -1.0)).l);
    EXPECT_DOUBLE_EQ(static_cast<double>(    0.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(    0.0)).l);
    EXPECT_DOUBLE_EQ(static_cast<double>(    1.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(    1.0)).l);
    EXPECT_DOUBLE_EQ(static_cast<double>( 1000.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>( 1000.0)).l);
}

TEST(atom, pm_type_u32) {
    // PM_TYPE_U32 should support all unsigned integer types up to 32 bits.
    EXPECT_EQ(std::numeric_limits<uint8_t>::min(),  pcp::atom(PM_TYPE_U32, std::numeric_limits<uint8_t>::min()).ul);
    EXPECT_EQ(std::numeric_limits<uint8_t>::max(),  pcp::atom(PM_TYPE_U32, std::numeric_limits<uint8_t>::max()).ul);
    EXPECT_EQ(std::numeric_limits<uint16_t>::min(), pcp::atom(PM_TYPE_U32, std::numeric_limits<uint16_t>::min()).ul);
    EXPECT_EQ(std::numeric_limits<uint16_t>::max(), pcp::atom(PM_TYPE_U32, std::numeric_limits<uint16_t>::max()).ul);
    EXPECT_EQ(std::numeric_limits<uint32_t>::min(), pcp::atom(PM_TYPE_U32, std::numeric_limits<uint32_t>::min()).ul);
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), pcp::atom(PM_TYPE_U32, std::numeric_limits<uint32_t>::max()).ul);

    // PM_TYPE_U32 should support positive floats.
    EXPECT_FLOAT_EQ(static_cast<float>(    0.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(    0.0)).ul);
    EXPECT_FLOAT_EQ(static_cast<float>(    1.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>(    1.0)).ul);
    EXPECT_FLOAT_EQ(static_cast<float>( 1000.0), pcp::atom<float>(PM_TYPE_32, static_cast<float>( 1000.0)).ul);

    // PM_TYPE_U32 should support positive doubles.
    EXPECT_DOUBLE_EQ(static_cast<double>(    0.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(    0.0)).ul);
    EXPECT_DOUBLE_EQ(static_cast<double>(    1.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>(    1.0)).ul);
    EXPECT_DOUBLE_EQ(static_cast<double>( 1000.0), pcp::atom<double>(PM_TYPE_32, static_cast<double>( 1000.0)).ul);
}

TEST(atom, pm_type_64) {
    // PM_TYPE_64 should support all signed integer types up to 64 bits.
    EXPECT_EQ(std::numeric_limits<int8_t>::min(),  pcp::atom(PM_TYPE_64, std::numeric_limits<int8_t>::min()).ll);
    EXPECT_EQ(std::numeric_limits<int8_t>::max(),  pcp::atom(PM_TYPE_64, std::numeric_limits<int8_t>::max()).ll);
    EXPECT_EQ(std::numeric_limits<int16_t>::min(), pcp::atom(PM_TYPE_64, std::numeric_limits<int16_t>::min()).ll);
    EXPECT_EQ(std::numeric_limits<int16_t>::max(), pcp::atom(PM_TYPE_64, std::numeric_limits<int16_t>::max()).ll);
    EXPECT_EQ(std::numeric_limits<int32_t>::min(), pcp::atom(PM_TYPE_64, std::numeric_limits<int32_t>::min()).ll);
    EXPECT_EQ(std::numeric_limits<int32_t>::max(), pcp::atom(PM_TYPE_64, std::numeric_limits<int32_t>::max()).ll);
    EXPECT_EQ(std::numeric_limits<int64_t>::min(), pcp::atom(PM_TYPE_64, std::numeric_limits<int64_t>::min()).ll);
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), pcp::atom(PM_TYPE_64, std::numeric_limits<int64_t>::max()).ll);

    // PM_TYPE_64 should support floats.
    EXPECT_FLOAT_EQ(static_cast<float>(-1000.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(-1000.0)).ll);
    EXPECT_FLOAT_EQ(static_cast<float>(   -1.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(   -1.0)).ll);
    EXPECT_FLOAT_EQ(static_cast<float>(    0.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(    0.0)).ll);
    EXPECT_FLOAT_EQ(static_cast<float>(    1.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(    1.0)).ll);
    EXPECT_FLOAT_EQ(static_cast<float>( 1000.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>( 1000.0)).ll);

    // PM_TYPE_64 should support doubles.
    EXPECT_DOUBLE_EQ(static_cast<double>(-1000.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(-1000.0)).ll);
    EXPECT_DOUBLE_EQ(static_cast<double>(   -1.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(   -1.0)).ll);
    EXPECT_DOUBLE_EQ(static_cast<double>(    0.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(    0.0)).ll);
    EXPECT_DOUBLE_EQ(static_cast<double>(    1.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(    1.0)).ll);
    EXPECT_DOUBLE_EQ(static_cast<double>( 1000.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>( 1000.0)).ll);
}

TEST(atom, pm_type_u64) {
    // PM_TYPE_U64 should support all unsigned integer types up to 64 bits.
    EXPECT_EQ(std::numeric_limits<uint8_t>::min(),  pcp::atom(PM_TYPE_U64, std::numeric_limits<uint8_t>::min()).ull);
    EXPECT_EQ(std::numeric_limits<uint8_t>::max(),  pcp::atom(PM_TYPE_U64, std::numeric_limits<uint8_t>::max()).ull);
    EXPECT_EQ(std::numeric_limits<uint16_t>::min(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint16_t>::min()).ull);
    EXPECT_EQ(std::numeric_limits<uint16_t>::max(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint16_t>::max()).ull);
    EXPECT_EQ(std::numeric_limits<uint32_t>::min(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint32_t>::min()).ull);
    EXPECT_EQ(std::numeric_limits<uint32_t>::max(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint32_t>::max()).ull);
    EXPECT_EQ(std::numeric_limits<uint64_t>::min(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint64_t>::min()).ull);
    EXPECT_EQ(std::numeric_limits<uint64_t>::max(), pcp::atom(PM_TYPE_U64, std::numeric_limits<uint64_t>::max()).ull);

    // PM_TYPE_U64 should support positive floats.
    EXPECT_FLOAT_EQ(static_cast<float>(    0.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(    0.0)).ull);
    EXPECT_FLOAT_EQ(static_cast<float>(    1.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>(    1.0)).ull);
    EXPECT_FLOAT_EQ(static_cast<float>( 1000.0), pcp::atom<float>(PM_TYPE_64, static_cast<float>( 1000.0)).ull);

    // PM_TYPE_U64 should support positive doubles.
    EXPECT_DOUBLE_EQ(static_cast<double>(    0.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(    0.0)).ull);
    EXPECT_DOUBLE_EQ(static_cast<double>(    1.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>(    1.0)).ull);
    EXPECT_DOUBLE_EQ(static_cast<double>( 1000.0), pcp::atom<double>(PM_TYPE_64, static_cast<double>( 1000.0)).ull);
}

TEST(atom, pm_type_float) {
    /// @todo
}

TEST(atom, pm_type_double) {
    /// @todo
}

TEST(atom, pm_type_string) {
    /// @todo
}

TEST(atom, pm_type_aggregate) {
    /// @todo
}

TEST(atom, pm_type_aggregate_static) {
    /// @todo
}

TEST(atom, pm_type_aggregate_event) {
    /// @todo
}
