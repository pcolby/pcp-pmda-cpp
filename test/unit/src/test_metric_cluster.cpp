//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/instance_domain.hpp"
#include "pcp-cpp/metric_description.hpp"
#include "pcp-cpp/units.hpp"

#include "gtest/gtest.h"

TEST(metric_cluster, constructor) {
    const pcp::metric_cluster min(std::numeric_limits<pcp::cluster_id_type>::min(), "min");
    EXPECT_EQ(std::numeric_limits<pcp::cluster_id_type>::min(), min.get_cluster_id());
    EXPECT_EQ("min", min.get_cluster_name());

    const pcp::metric_cluster max(std::numeric_limits<pcp::cluster_id_type>::max(), "max");
    EXPECT_EQ(std::numeric_limits<pcp::cluster_id_type>::max(), max.get_cluster_id());
    EXPECT_EQ("max", max.get_cluster_name());
}

TEST(metric_cluster, functor_1) {
    pcp::instance_domain domain;
    int opaque = 123;
    pcp::metric_cluster cluster(123, "cluster");
    cluster
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
         pcp::storable_metric, NULL, "short description", "verbose description", NULL)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
         static_cast<pcp::metric_flags>(0), &domain, "short", "verbose", &opaque);
    EXPECT_EQ(pcp::metric_cluster::size_type(2), cluster.size());

    EXPECT_EQ("one", cluster.at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, cluster.at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, cluster.at(1).semantic);
    EXPECT_EQ(1, cluster.at(1).units.dimSpace);
    EXPECT_EQ(2, cluster.at(1).units.dimTime);
    EXPECT_EQ(3, cluster.at(1).units.dimCount);
    EXPECT_EQ(4, cluster.at(1).units.scaleSpace);
    EXPECT_EQ(5, cluster.at(1).units.scaleTime);
    EXPECT_EQ(6, cluster.at(1).units.scaleCount);
    EXPECT_EQ(NULL, cluster.at(1).domain);
    EXPECT_EQ("short description", cluster.at(1).short_description);
    EXPECT_EQ("verbose description", cluster.at(1).verbose_description);
    EXPECT_EQ(NULL, cluster.at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, cluster.at(1).flags);

    EXPECT_EQ("two", cluster.at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, cluster.at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, cluster.at(2).semantic);
    EXPECT_EQ(-1, cluster.at(2).units.dimSpace);
    EXPECT_EQ(-2, cluster.at(2).units.dimTime);
    EXPECT_EQ(-3, cluster.at(2).units.dimCount);
    EXPECT_EQ(10, cluster.at(2).units.scaleSpace);
    EXPECT_EQ(11, cluster.at(2).units.scaleTime);
    EXPECT_EQ(-6, cluster.at(2).units.scaleCount);
    EXPECT_EQ(&domain, cluster.at(2).domain);
    EXPECT_EQ("short", cluster.at(2).short_description);
    EXPECT_EQ("verbose", cluster.at(2).verbose_description);
    EXPECT_EQ(&opaque, cluster.at(2).opaque);
    EXPECT_EQ(0, cluster.at(2).flags);
}

TEST(metric_cluster, functor_2) {
    pcp::instance_domain domain;
    int opaque = 123;
    pcp::metric_cluster cluster(123, "cluster");
    cluster
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
         NULL, pcp::storable_metric, "short description", "verbose description", NULL)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
         &domain, static_cast<pcp::metric_flags>(0), "short", "verbose", &opaque);
    EXPECT_EQ(pcp::metric_cluster::size_type(2), cluster.size());

    EXPECT_EQ("one", cluster.at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, cluster.at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, cluster.at(1).semantic);
    EXPECT_EQ(1, cluster.at(1).units.dimSpace);
    EXPECT_EQ(2, cluster.at(1).units.dimTime);
    EXPECT_EQ(3, cluster.at(1).units.dimCount);
    EXPECT_EQ(4, cluster.at(1).units.scaleSpace);
    EXPECT_EQ(5, cluster.at(1).units.scaleTime);
    EXPECT_EQ(6, cluster.at(1).units.scaleCount);
    EXPECT_EQ(NULL, cluster.at(1).domain);
    EXPECT_EQ("short description", cluster.at(1).short_description);
    EXPECT_EQ("verbose description", cluster.at(1).verbose_description);
    EXPECT_EQ(NULL, cluster.at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, cluster.at(1).flags);

    EXPECT_EQ("two", cluster.at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, cluster.at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, cluster.at(2).semantic);
    EXPECT_EQ(-1, cluster.at(2).units.dimSpace);
    EXPECT_EQ(-2, cluster.at(2).units.dimTime);
    EXPECT_EQ(-3, cluster.at(2).units.dimCount);
    EXPECT_EQ(10, cluster.at(2).units.scaleSpace);
    EXPECT_EQ(11, cluster.at(2).units.scaleTime);
    EXPECT_EQ(-6, cluster.at(2).units.scaleCount);
    EXPECT_EQ(&domain, cluster.at(2).domain);
    EXPECT_EQ("short", cluster.at(2).short_description);
    EXPECT_EQ("verbose", cluster.at(2).verbose_description);
    EXPECT_EQ(&opaque, cluster.at(2).opaque);
    EXPECT_EQ(0, cluster.at(2).flags);
}

TEST(metric_cluster, functor_3) {
    pcp::instance_domain domain;
    int opaque = 123;
    pcp::metric_cluster cluster(123, "cluster");
    cluster
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
         NULL, "short description", "verbose description", NULL, pcp::storable_metric)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
         &domain, "short", "verbose", &opaque);
    EXPECT_EQ(pcp::metric_cluster::size_type(2), cluster.size());

    EXPECT_EQ("one", cluster.at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, cluster.at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, cluster.at(1).semantic);
    EXPECT_EQ(1, cluster.at(1).units.dimSpace);
    EXPECT_EQ(2, cluster.at(1).units.dimTime);
    EXPECT_EQ(3, cluster.at(1).units.dimCount);
    EXPECT_EQ(4, cluster.at(1).units.scaleSpace);
    EXPECT_EQ(5, cluster.at(1).units.scaleTime);
    EXPECT_EQ(6, cluster.at(1).units.scaleCount);
    EXPECT_EQ(NULL, cluster.at(1).domain);
    EXPECT_EQ("short description", cluster.at(1).short_description);
    EXPECT_EQ("verbose description", cluster.at(1).verbose_description);
    EXPECT_EQ(NULL, cluster.at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, cluster.at(1).flags);

    EXPECT_EQ("two", cluster.at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, cluster.at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, cluster.at(2).semantic);
    EXPECT_EQ(-1, cluster.at(2).units.dimSpace);
    EXPECT_EQ(-2, cluster.at(2).units.dimTime);
    EXPECT_EQ(-3, cluster.at(2).units.dimCount);
    EXPECT_EQ(10, cluster.at(2).units.scaleSpace);
    EXPECT_EQ(11, cluster.at(2).units.scaleTime);
    EXPECT_EQ(-6, cluster.at(2).units.scaleCount);
    EXPECT_EQ(&domain, cluster.at(2).domain);
    EXPECT_EQ("short", cluster.at(2).short_description);
    EXPECT_EQ("verbose", cluster.at(2).verbose_description);
    EXPECT_EQ(&opaque, cluster.at(2).opaque);
    EXPECT_EQ(0, cluster.at(2).flags);
}
