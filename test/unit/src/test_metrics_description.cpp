//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/instance_domain.hpp"
#include "pcp-cpp/metric_description.hpp"
#include "pcp-cpp/units.hpp"

#include "gtest/gtest.h"

TEST(metrics_description, insert_parent_functor) {
    pcp::metrics_description desc;
    EXPECT_EQ(pcp::metrics_description::size_type(0), desc.size());
    desc
        (std::numeric_limits<pcp::cluster_id_type>::min(), "min")
        (std::numeric_limits<pcp::cluster_id_type>::max(), "max");
    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());
    EXPECT_EQ("min", desc.at(std::numeric_limits<pcp::cluster_id_type>::min()).get_cluster_name());
    EXPECT_EQ("max", desc.at(std::numeric_limits<pcp::cluster_id_type>::max()).get_cluster_name());
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(std::numeric_limits<pcp::cluster_id_type>::min()).size());
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(std::numeric_limits<pcp::cluster_id_type>::max()).size());

    // Check that the final functor call sets the iterator for the insert-child functors.
    desc(123, "metric", PM_TYPE_U64, PM_SEM_COUNTER, pcp::units(1,2,3, 4,5,6));
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(std::numeric_limits<pcp::cluster_id_type>::min()).size());
    EXPECT_EQ(pcp::metric_cluster::size_type(1), desc.at(std::numeric_limits<pcp::cluster_id_type>::max()).size());
    // We'll test that the functor inserted the correct values in the tests below.
}

TEST(metrics_description, insert_child_functor_1) {
    pcp::metrics_description desc;
    EXPECT_EQ(pcp::metrics_description::size_type(0), desc.size());

    // Insert-child functors should throw if the insert-parent functor has not been called yet.
    EXPECT_THROW(
        desc(123, "metric", PM_TYPE_U64, PM_SEM_COUNTER, pcp::units(1,2,3, 4,5,6)),
        pcp::exception
    );

    // Add some clusters.
    desc(123, "cluster 123")(456, "cluster 456");
    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());

    // Insert some metrics into the most recent cluster.
    pcp::instance_domain domain;
    int opaque = 123;
    desc
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6),
         pcp::storable_metric, NULL, "short description", "verbose description")
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
         static_cast<pcp::metric_flags>(0), &domain, "short", "verbose", &opaque);

    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());    // desc didn't increase is size.
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(123).size()); // first cluster remained empty.
    EXPECT_EQ(pcp::metric_cluster::size_type(2), desc.at(456).size()); // second cluster got two metrics.

    EXPECT_EQ("one", desc.at(456).at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, desc.at(456).at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, desc.at(456).at(1).semantic);
    EXPECT_EQ(1, desc.at(456).at(1).units.dimSpace);
    EXPECT_EQ(2, desc.at(456).at(1).units.dimTime);
    EXPECT_EQ(3, desc.at(456).at(1).units.dimCount);
    EXPECT_EQ(4, desc.at(456).at(1).units.scaleSpace);
    EXPECT_EQ(5, desc.at(456).at(1).units.scaleTime);
    EXPECT_EQ(6, desc.at(456).at(1).units.scaleCount);
    EXPECT_EQ(NULL, desc.at(456).at(1).domain);
    EXPECT_EQ("short description", desc.at(456).at(1).short_description);
    EXPECT_EQ("verbose description", desc.at(456).at(1).verbose_description);
    EXPECT_EQ(NULL, desc.at(456).at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, desc.at(456).at(1).flags);

    EXPECT_EQ("two", desc.at(456).at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, desc.at(456).at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, desc.at(456).at(2).semantic);
    EXPECT_EQ(-1, desc.at(456).at(2).units.dimSpace);
    EXPECT_EQ(-2, desc.at(456).at(2).units.dimTime);
    EXPECT_EQ(-3, desc.at(456).at(2).units.dimCount);
    EXPECT_EQ(10, desc.at(456).at(2).units.scaleSpace);
    EXPECT_EQ(11, desc.at(456).at(2).units.scaleTime);
    EXPECT_EQ(-6, desc.at(456).at(2).units.scaleCount);
    EXPECT_EQ(&domain, desc.at(456).at(2).domain);
    EXPECT_EQ("short", desc.at(456).at(2).short_description);
    EXPECT_EQ("verbose", desc.at(456).at(2).verbose_description);
    EXPECT_EQ(&opaque, desc.at(456).at(2).opaque);
    EXPECT_EQ(0, desc.at(456).at(2).flags);
}

TEST(metrics_description, insert_child_functor_2) {
    pcp::metrics_description desc;
    EXPECT_EQ(pcp::metrics_description::size_type(0), desc.size());

    // Insert-child functors should throw if the insert-parent functor has not been called yet.
    EXPECT_THROW(
        desc(123, "metric", PM_TYPE_U64, PM_SEM_COUNTER, pcp::units(1,2,3, 4,5,6)),
        pcp::exception
    );

    // Add some clusters.
    desc(123, "cluster 123")(456, "cluster 456");
    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());

    // Insert some metrics into the most recent cluster.
    pcp::instance_domain domain;
    int opaque = 123;
    desc
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6), NULL,
         pcp::storable_metric, "short description", "verbose description", NULL)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
         &domain, static_cast<pcp::metric_flags>(0), "short", "verbose", &opaque);

    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());         // desc didn't increase is size.
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(123).size()); // first cluster remained empty.
    EXPECT_EQ(pcp::metric_cluster::size_type(2), desc.at(456).size()); // second cluster got two metrics.

    EXPECT_EQ("one", desc.at(456).at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, desc.at(456).at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, desc.at(456).at(1).semantic);
    EXPECT_EQ(1, desc.at(456).at(1).units.dimSpace);
    EXPECT_EQ(2, desc.at(456).at(1).units.dimTime);
    EXPECT_EQ(3, desc.at(456).at(1).units.dimCount);
    EXPECT_EQ(4, desc.at(456).at(1).units.scaleSpace);
    EXPECT_EQ(5, desc.at(456).at(1).units.scaleTime);
    EXPECT_EQ(6, desc.at(456).at(1).units.scaleCount);
    EXPECT_EQ(NULL, desc.at(456).at(1).domain);
    EXPECT_EQ("short description", desc.at(456).at(1).short_description);
    EXPECT_EQ("verbose description", desc.at(456).at(1).verbose_description);
    EXPECT_EQ(NULL, desc.at(456).at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, desc.at(456).at(1).flags);

    EXPECT_EQ("two", desc.at(456).at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, desc.at(456).at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, desc.at(456).at(2).semantic);
    EXPECT_EQ(-1, desc.at(456).at(2).units.dimSpace);
    EXPECT_EQ(-2, desc.at(456).at(2).units.dimTime);
    EXPECT_EQ(-3, desc.at(456).at(2).units.dimCount);
    EXPECT_EQ(10, desc.at(456).at(2).units.scaleSpace);
    EXPECT_EQ(11, desc.at(456).at(2).units.scaleTime);
    EXPECT_EQ(-6, desc.at(456).at(2).units.scaleCount);
    EXPECT_EQ(&domain, desc.at(456).at(2).domain);
    EXPECT_EQ("short", desc.at(456).at(2).short_description);
    EXPECT_EQ("verbose", desc.at(456).at(2).verbose_description);
    EXPECT_EQ(&opaque, desc.at(456).at(2).opaque);
    EXPECT_EQ(0, desc.at(456).at(2).flags);
}

TEST(metrics_description, insert_child_functor_3) {
    pcp::metrics_description desc;
    EXPECT_EQ(pcp::metrics_description::size_type(0), desc.size());

    // Insert-child functors should throw if the insert-parent functor has not been called yet.
    EXPECT_THROW(
        desc(123, "metric", PM_TYPE_U64, PM_SEM_COUNTER, pcp::units(1,2,3, 4,5,6)),
        pcp::exception
    );

    // Add some clusters.
    desc(123, "cluster 123")(456, "cluster 456");
    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());

    // Insert some metrics into the most recent cluster.
    pcp::instance_domain domain;
    int opaque = 123;
    desc
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6), NULL,
         "short description", "verbose description", NULL, pcp::storable_metric)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
        &domain, "short", "verbose", &opaque);

    EXPECT_EQ(pcp::metrics_description::size_type(2), desc.size());         // desc didn't increase is size.
    EXPECT_EQ(pcp::metric_cluster::size_type(0), desc.at(123).size()); // first cluster remained empty.
    EXPECT_EQ(pcp::metric_cluster::size_type(2), desc.at(456).size()); // second cluster got two metrics.

    EXPECT_EQ("one", desc.at(456).at(1).metric_name);
    EXPECT_EQ(PM_TYPE_U64, desc.at(456).at(1).type);
    EXPECT_EQ(PM_SEM_INSTANT, desc.at(456).at(1).semantic);
    EXPECT_EQ(1, desc.at(456).at(1).units.dimSpace);
    EXPECT_EQ(2, desc.at(456).at(1).units.dimTime);
    EXPECT_EQ(3, desc.at(456).at(1).units.dimCount);
    EXPECT_EQ(4, desc.at(456).at(1).units.scaleSpace);
    EXPECT_EQ(5, desc.at(456).at(1).units.scaleTime);
    EXPECT_EQ(6, desc.at(456).at(1).units.scaleCount);
    EXPECT_EQ(NULL, desc.at(456).at(1).domain);
    EXPECT_EQ("short description", desc.at(456).at(1).short_description);
    EXPECT_EQ("verbose description", desc.at(456).at(1).verbose_description);
    EXPECT_EQ(NULL, desc.at(456).at(1).opaque);
    EXPECT_EQ(pcp::storable_metric, desc.at(456).at(1).flags);

    EXPECT_EQ("two", desc.at(456).at(2).metric_name);
    EXPECT_EQ(PM_TYPE_STRING, desc.at(456).at(2).type);
    EXPECT_EQ(PM_SEM_COUNTER, desc.at(456).at(2).semantic);
    EXPECT_EQ(-1, desc.at(456).at(2).units.dimSpace);
    EXPECT_EQ(-2, desc.at(456).at(2).units.dimTime);
    EXPECT_EQ(-3, desc.at(456).at(2).units.dimCount);
    EXPECT_EQ(10, desc.at(456).at(2).units.scaleSpace);
    EXPECT_EQ(11, desc.at(456).at(2).units.scaleTime);
    EXPECT_EQ(-6, desc.at(456).at(2).units.scaleCount);
    EXPECT_EQ(&domain, desc.at(456).at(2).domain);
    EXPECT_EQ("short", desc.at(456).at(2).short_description);
    EXPECT_EQ("verbose", desc.at(456).at(2).verbose_description);
    EXPECT_EQ(&opaque, desc.at(456).at(2).opaque);
    EXPECT_EQ(0, desc.at(456).at(2).flags);
}
