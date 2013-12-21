//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/types.hpp"

#include "gtest/gtest.h"

TEST(types, ranges) {
    // PM_TYPE_* (0 - 9)
    EXPECT_GE(pcp::atom_type_type(0), std::numeric_limits<pcp::atom_type_type>::min());
    EXPECT_LE(pcp::atom_type_type(9), std::numeric_limits<pcp::atom_type_type>::max());

    // __pmID_int::cluster (12-bits)
    EXPECT_GE(pcp::cluster_id_type(0),     std::numeric_limits<pcp::cluster_id_type>::min());
    EXPECT_LE(pcp::cluster_id_type(1<<12), std::numeric_limits<pcp::cluster_id_type>::max());

    // __pmID_int::cluster (9-bits); pmdaIndom::it_indom
    EXPECT_GE(pcp::domain_id_type(0),    std::numeric_limits<pcp::domain_id_type>::min());
    EXPECT_LE(pcp::domain_id_type(1<<9), std::numeric_limits<pcp::domain_id_type>::max());

    // pmdaInstid::i_inst (int)
    EXPECT_GE(std::numeric_limits<unsigned int>::min(), std::numeric_limits<pcp::instance_id_type>::min());
    EXPECT_LE(std::numeric_limits<unsigned int>::max(), std::numeric_limits<pcp::instance_id_type>::max());

    // __pmID_int::item (10-bits)
    EXPECT_GE(pcp::item_id_type(0),     std::numeric_limits<pcp::item_id_type>::min());
    EXPECT_LE(pcp::item_id_type(1<<10), std::numeric_limits<pcp::item_id_type>::max());

    // PM_SEM_* (0 - 4)
    EXPECT_GE(pcp::semantic_type(0), std::numeric_limits<pcp::semantic_type>::min());
    EXPECT_LE(pcp::semantic_type(4), std::numeric_limits<pcp::semantic_type>::max());
}

TEST(types, pm_types) {
    EXPECT_EQ(PM_TYPE_32,  pcp::type<int8_t>());
    EXPECT_EQ(PM_TYPE_32,  pcp::type<int16_t>());
    EXPECT_EQ(PM_TYPE_32,  pcp::type<int32_t>());
    EXPECT_EQ(PM_TYPE_64,  pcp::type<int64_t>());
    EXPECT_EQ(PM_TYPE_U32, pcp::type<uint8_t>());
    EXPECT_EQ(PM_TYPE_U32, pcp::type<uint16_t>());
    EXPECT_EQ(PM_TYPE_U32, pcp::type<uint32_t>());
    EXPECT_EQ(PM_TYPE_U64, pcp::type<uint64_t>());

    EXPECT_EQ(PM_TYPE_FLOAT,  pcp::type<float>());
    EXPECT_EQ(PM_TYPE_DOUBLE, pcp::type<double>());

    EXPECT_EQ(PM_TYPE_STRING, pcp::type<char *>());
    EXPECT_EQ(PM_TYPE_STRING, pcp::type<std::string>());
}
