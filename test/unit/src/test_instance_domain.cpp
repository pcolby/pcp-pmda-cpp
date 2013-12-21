//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/instance_domain.hpp"

#include "gtest/gtest.h"

TEST(instance_domain, domain_id) {
    pcp::instance_domain indom;

    indom.set_domain_id(std::numeric_limits<pcp::domain_id_type>::min());
    EXPECT_EQ(std::numeric_limits<pcp::domain_id_type>::min(), indom.get_domain_id());

    indom.set_domain_id(std::numeric_limits<pcp::domain_id_type>::max());
    EXPECT_EQ(std::numeric_limits<pcp::domain_id_type>::max(), indom.get_domain_id());
}

TEST(instance_domain, pm_instance_domain) {
    pcp::instance_domain indom;

    indom.set_pm_instance_domain(std::numeric_limits<pmInDom>::min());
    EXPECT_EQ(std::numeric_limits<pmInDom>::min(), indom.get_pm_instance_domain());

    indom.set_pm_instance_domain(std::numeric_limits<pmInDom>::min());
    EXPECT_EQ(std::numeric_limits<pmInDom>::min(), indom.get_pm_instance_domain());
}

TEST(instance_domain, cast_to_pmInDom) {
    pcp::instance_domain indom;

    indom.set_pm_instance_domain(std::numeric_limits<pmInDom>::min());
    EXPECT_EQ(std::numeric_limits<pmInDom>::min(), static_cast<pmInDom>(indom));

    indom.set_pm_instance_domain(std::numeric_limits<pmInDom>::min());
    EXPECT_EQ(std::numeric_limits<pmInDom>::min(), static_cast<pmInDom>(indom));
}

TEST(instance_domain, functor_1) {
    pcp::instance_domain indom;
    EXPECT_EQ(pcp::instance_domain::size_type(0), indom.size());

    indom(std::numeric_limits<pcp::domain_id_type>::min());
    EXPECT_EQ(std::numeric_limits<pcp::domain_id_type>::min(), indom.get_domain_id());
    EXPECT_EQ(PM_INDOM_NULL, indom.get_pm_instance_domain());
    EXPECT_EQ(pcp::instance_domain::size_type(0), indom.size());

    indom(std::numeric_limits<pcp::domain_id_type>::max());
    EXPECT_EQ(std::numeric_limits<pcp::domain_id_type>::max(), indom.get_domain_id());
    EXPECT_EQ(PM_INDOM_NULL, indom.get_pm_instance_domain());
    EXPECT_EQ(pcp::instance_domain::size_type(0), indom.size());
}

TEST(instance_domain, functor_2) {
    pcp::instance_domain indom;
    EXPECT_EQ(pcp::instance_domain::size_type(0), indom.size());

    indom(std::numeric_limits<pcp::instance_id_type>::min(), "min");
    EXPECT_EQ(pcp::instance_domain::size_type(1), indom.size());
    EXPECT_EQ("min", indom.at(std::numeric_limits<pcp::instance_id_type>::min()));

    indom(std::numeric_limits<pcp::instance_id_type>::max(), "max");
    EXPECT_EQ(pcp::instance_domain::size_type(2), indom.size());
    EXPECT_EQ("max", indom.at(std::numeric_limits<pcp::instance_id_type>::max()));

    indom(123, "initial value");
    EXPECT_EQ(pcp::instance_domain::size_type(3), indom.size());
    EXPECT_EQ("initial value", indom.at(123));

    // Inserted a new value with an existing key has no effect (std::map behaviour).
    indom(123, "new value");
    EXPECT_EQ(pcp::instance_domain::size_type(3), indom.size());
    EXPECT_EQ("initial value", indom.at(123));
    EXPECT_EQ("min", indom.at(std::numeric_limits<pcp::instance_id_type>::min()));
    EXPECT_EQ("max", indom.at(std::numeric_limits<pcp::instance_id_type>::max()));
}
