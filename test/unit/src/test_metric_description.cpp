//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/instance_domain.hpp"
#include "pcp-cpp/metric_description.hpp"
#include "pcp-cpp/units.hpp"

#include "gtest/gtest.h"

TEST(metric_description, constructor) {
    const pcp::metric_description desc1(
        "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
        NULL, "short description", "verbose description", NULL, pcp::storable_metric
    );
    EXPECT_EQ("one", desc1.metric_name);
    EXPECT_EQ(PM_TYPE_U64, desc1.type);
    EXPECT_EQ(PM_SEM_INSTANT, desc1.semantic);
    EXPECT_EQ(1, desc1.units.dimSpace);
    EXPECT_EQ(2, desc1.units.dimTime);
    EXPECT_EQ(3, desc1.units.dimCount);
    EXPECT_EQ(4, desc1.units.scaleSpace);
    EXPECT_EQ(5, desc1.units.scaleTime);
    EXPECT_EQ(6, desc1.units.scaleCount);
    EXPECT_EQ(NULL, desc1.domain);
    EXPECT_EQ("short description", desc1.short_description);
    EXPECT_EQ("verbose description", desc1.verbose_description);
    EXPECT_EQ(NULL, desc1.opaque);
    EXPECT_EQ(pcp::storable_metric, desc1.flags);

    pcp::instance_domain domain;
    int opaque = 123;
    const pcp::metric_description desc2(
        "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
        &domain, "short", "verbose", &opaque
    );
    EXPECT_EQ("two", desc2.metric_name);
    EXPECT_EQ(PM_TYPE_STRING, desc2.type);
    EXPECT_EQ(PM_SEM_COUNTER, desc2.semantic);
    EXPECT_EQ(-1, desc2.units.dimSpace);
    EXPECT_EQ(-2, desc2.units.dimTime);
    EXPECT_EQ(-3, desc2.units.dimCount);
    EXPECT_EQ(10, desc2.units.scaleSpace);
    EXPECT_EQ(11, desc2.units.scaleTime);
    EXPECT_EQ(-6, desc2.units.scaleCount);
    EXPECT_EQ(&domain, desc2.domain);
    EXPECT_EQ("short", desc2.short_description);
    EXPECT_EQ("verbose", desc2.verbose_description);
    EXPECT_EQ(&opaque, desc2.opaque);
    EXPECT_EQ(0, desc2.flags);
}

TEST(metric_cluster, cast_to_pmDesc) {
    const pmDesc desc1 = pcp::metric_description(
        "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
        NULL, "short description", "verbose description", NULL, pcp::storable_metric
    );
    EXPECT_EQ(PM_INDOM_NULL, desc1.indom);
    EXPECT_EQ(0, desc1.pmid);
    EXPECT_EQ(PM_SEM_INSTANT, desc1.sem);
    EXPECT_EQ(PM_TYPE_U64, desc1.type);
    EXPECT_EQ(1, desc1.units.dimSpace);
    EXPECT_EQ(2, desc1.units.dimTime);
    EXPECT_EQ(3, desc1.units.dimCount);
    EXPECT_EQ(4, desc1.units.scaleSpace);
    EXPECT_EQ(5, desc1.units.scaleTime);
    EXPECT_EQ(6, desc1.units.scaleCount);

    pcp::instance_domain domain;
    int opaque = 123;
    const pmDesc desc2 = pcp::metric_description(
        "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
        &domain, "short", "verbose", &opaque
    );
    EXPECT_EQ(PM_INDOM_NULL, desc2.indom);
    EXPECT_EQ(0, desc2.pmid);
    EXPECT_EQ(PM_SEM_COUNTER, desc2.sem);
    EXPECT_EQ(PM_TYPE_STRING, desc2.type);
    EXPECT_EQ(-1, desc2.units.dimSpace);
    EXPECT_EQ(-2, desc2.units.dimTime);
    EXPECT_EQ(-3, desc2.units.dimCount);
    EXPECT_EQ(10, desc2.units.scaleSpace);
    EXPECT_EQ(11, desc2.units.scaleTime);
    EXPECT_EQ(-6, desc2.units.scaleCount);
}
