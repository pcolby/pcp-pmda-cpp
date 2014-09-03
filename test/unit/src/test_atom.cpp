//               Copyright Paul Colby 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/atom.hpp"

#include "gtest/gtest.h"

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

    // PM_TYPE_32 does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_32, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_32, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
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

    // PM_TYPE_U32 does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_U32, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_U32, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
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

    // PM_TYPE_64 does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_64, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_64, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
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

    // PM_TYPE_U64 does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_U64, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_U64, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
}

TEST(atom, pm_type_float) {
    // PM_TYPE_FLOAT should support the full float range.
    EXPECT_EQ(std::numeric_limits<float>::min(), pcp::atom(PM_TYPE_FLOAT, std::numeric_limits<float>::min()).f);
    EXPECT_EQ(std::numeric_limits<float>::max(), pcp::atom(PM_TYPE_FLOAT, std::numeric_limits<float>::max()).f);

    // PM_TYPE_FLOAT does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_FLOAT, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_FLOAT, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
}

TEST(atom, pm_type_double) {
    // PM_TYPE_DOUBLE should support the full float and double ranges.
    EXPECT_EQ(std::numeric_limits<float>::min(),  pcp::atom(PM_TYPE_DOUBLE, std::numeric_limits<float>::min()).d);
    EXPECT_EQ(std::numeric_limits<float>::max(),  pcp::atom(PM_TYPE_DOUBLE, std::numeric_limits<float>::max()).d);
    EXPECT_EQ(std::numeric_limits<double>::min(), pcp::atom(PM_TYPE_DOUBLE, std::numeric_limits<double>::min()).d);
    EXPECT_EQ(std::numeric_limits<double>::max(), pcp::atom(PM_TYPE_DOUBLE, std::numeric_limits<double>::max()).d);

    // PM_TYPE_DOUBLE does not support string, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_DOUBLE, const_cast<char *>("not a number")), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_DOUBLE, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
}

TEST(atom, pm_type_string) {
    EXPECT_EQ("test", pcp::atom(PM_TYPE_STRING, const_cast<char *>("test")).cp);

    // PM_TYPE_STRING does not support number, aggregrate, event and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast<  int8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast< int16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast< int32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast< int64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast< uint8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast<uint16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast<uint32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast<uint64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast< float>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, static_cast<double>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_STRING, reinterpret_cast<pmValueBlock *>(NULL)), pcp::exception);
}

TEST(atom, pm_type_aggregate) {
    // PM_TYPE_AGGREGATE simply stores pmValueBlock pointers verbatim.
    pmValueBlock stack_value, * const null_pointer = NULL;
    EXPECT_EQ(&stack_value, pcp::atom(PM_TYPE_AGGREGATE, &stack_value).vbp);
    EXPECT_EQ(null_pointer, pcp::atom(PM_TYPE_AGGREGATE, null_pointer).vbp);

    // PM_TYPE_AGGREGATE does not support number, string, and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast<  int8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast< int16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast< int32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast< int64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast< uint8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast<uint16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast<uint32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast<uint64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast< float>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, static_cast<double>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE, const_cast<char *>("not an aggregate")), pcp::exception);
}

TEST(atom, pm_type_aggregate_static) {
    // PM_TYPE_AGGREGATE_STATIC simply stores pmValueBlock pointers verbatim.
    pmValueBlock stack_value, * const null_pointer = NULL;
    EXPECT_EQ(&stack_value, pcp::atom(PM_TYPE_AGGREGATE_STATIC, &stack_value).vbp);
    EXPECT_EQ(null_pointer, pcp::atom(PM_TYPE_AGGREGATE_STATIC, null_pointer).vbp);

    // PM_TYPE_AGGREGATE_STATIC does not support number, string, and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast<  int8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast< int16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast< int32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast< int64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast< uint8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast<uint16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast<uint32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast<uint64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast< float>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, static_cast<double>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_AGGREGATE_STATIC, const_cast<char *>("not an aggregate")), pcp::exception);
}

TEST(atom, pm_type_event) {
    // PM_TYPE_EVENT simply stores pmValueBlock pointers verbatim.
    pmValueBlock stack_value, * const null_pointer = NULL;
    EXPECT_EQ(&stack_value, pcp::atom(PM_TYPE_EVENT, &stack_value).vbp);
    EXPECT_EQ(null_pointer, pcp::atom(PM_TYPE_EVENT, null_pointer).vbp);

    // PM_TYPE_EVENT does not support number, string, and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast<  int8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast< int16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast< int32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast< int64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast< uint8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast<uint16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast<uint32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast<uint64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast< float>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, static_cast<double>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_EVENT, const_cast<char *>("not an event")), pcp::exception);
}

#ifdef PM_TYPE_HIGHRES_EVENT // PM_TYPE_HIGHRES_EVENT added in PCP 3.9.10.
TEST(atom, pm_type_highres_event) {
    // PM_TYPE_HIGHRES_EVENT simply stores pmValueBlock pointers verbatim.
    pmValueBlock stack_value, * const null_pointer = NULL;
    EXPECT_EQ(&stack_value, pcp::atom(PM_TYPE_HIGHRES_EVENT, &stack_value).vbp);
    EXPECT_EQ(null_pointer, pcp::atom(PM_TYPE_HIGHRES_EVENT, null_pointer).vbp);

    // PM_TYPE_HIGHRES_EVENT does not support number, string, and unknown types.
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast<  int8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast< int16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast< int32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast< int64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast< uint8_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast<uint16_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast<uint32_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast<uint64_t>(0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast< float>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, static_cast<double>(0.0)), pcp::exception);
    EXPECT_THROW(pcp::atom(PM_TYPE_HIGHRES_EVENT, const_cast<char *>("not an event")), pcp::exception);
}
#endif
