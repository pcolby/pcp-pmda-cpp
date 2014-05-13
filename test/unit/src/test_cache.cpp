//               Copyright Paul Colby 2014.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/cache.hpp"

#include "gtest/gtest.h"

// PM_ERR_FAULT ("QA fault injected") was not added until PCP 3.6.0.
#ifndef PM_ERR_FAULT
#define PM_ERR_FAULT PM_ERR_GENERIC
#endif

TEST(cache, lookup_flags_pipe_operator) {
    EXPECT_EQ(pcp::cache::require_active, 0|pcp::cache::require_active);
}

TEST(cache, lookup_by_instance_id) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::lookup<void *>(PM_ERR_FAULT, 123), pcp::exception);

    // Active cache results should never result in exceptions.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, 123, pcp::cache::lookup_flags(0))
    );
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, 123, pcp::cache::require_active)
    );

    // Inactive cache results may or may not throw, depending on the require_active flag.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, 123, pcp::cache::lookup_flags(0))
    );
    EXPECT_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, 123, pcp::cache::require_active),
        pcp::exception
    );
}

TEST(cache, lookup_by_name) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::lookup<void *>(PM_ERR_FAULT, "foo"), pcp::exception);

    // Active cache results should never result in exceptions.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, "foo", pcp::cache::lookup_flags(0))
    );
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, "foo", pcp::cache::require_active)
    );

    // Inactive cache results may or may not throw, depending on the require_active flag.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, "foo", pcp::cache::lookup_flags(0))
    );
    EXPECT_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, "foo", pcp::cache::require_active),
        pcp::exception
    );
}

TEST(cache, lookup_by_name_and_key) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::lookup<void *>(PM_ERR_FAULT, "foo", "bar"), pcp::exception);

    // Active cache results should never result in exceptions.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, "foo", "bar", pcp::cache::lookup_flags(0))
    );
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_ACTIVE, "foo", "bar", pcp::cache::require_active)
    );

    // Inactive cache results may or may not throw, depending on the require_active flag.
    EXPECT_NO_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, "foo", "bar", pcp::cache::lookup_flags(0))
    );
    EXPECT_THROW(
        pcp::cache::lookup<void *>(PMDA_CACHE_INACTIVE, "foo", "bar", pcp::cache::require_active),
        pcp::exception
    );
}

TEST(cache, perform) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::perform(PM_ERR_FAULT, PMDA_CACHE_CHECK), pcp::exception);

    // All non-errors should return without exceptions.
    EXPECT_NO_THROW(pcp::cache::perform(123, 456));
}

TEST(cache, purge) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::purge(PM_ERR_FAULT, 123), pcp::exception);

    // All non-errors should return without exceptions.
    EXPECT_NO_THROW(pcp::cache::purge(123, 456));

    // Verify that the pcp::cache::purge wrapper passes the recent argument
    // (456 in this case) to pmdaCachePurge verbatim.
    EXPECT_EQ(456, pcp::cache::purge(123, 456));
    EXPECT_EQ(300, pcp::cache::purge(123, 300));
}

TEST(cache, purge_boost) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::purge(PM_ERR_FAULT, boost::posix_time::seconds(123)), pcp::exception);

    // All non-errors should return without exceptions.
    EXPECT_NO_THROW(pcp::cache::purge(123, boost::posix_time::seconds(123)));

    // Verify that the pcp::cache::purge boost overload converts to seconds.
    EXPECT_EQ(456, pcp::cache::purge(123, boost::posix_time::seconds(456)));
    EXPECT_EQ(300, pcp::cache::purge(123, boost::posix_time::minutes(5)));
}

TEST(cache, store_by_name) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::store(PM_ERR_FAULT, "foo", (void *)NULL), pcp::exception);
    EXPECT_THROW(pcp::cache::store(PM_ERR_FAULT, "foo", PMDA_CACHE_ADD), pcp::exception);

    // All non-errors should return without exceptions.
    EXPECT_NO_THROW(pcp::cache::store(123, "foo", (void *)NULL));
    EXPECT_NO_THROW(pcp::cache::store(123, "foo", PMDA_CACHE_ADD));
}

TEST(cache, store_by_name_and_key) {
    // Errors (such as PM_ERR_FAULT in this case) result in exceptions.
    EXPECT_THROW(pcp::cache::store(PM_ERR_FAULT, "foo", "bah", (void *)NULL), pcp::exception);
    EXPECT_THROW(pcp::cache::store(PM_ERR_FAULT, "foo", "bah", PMDA_CACHE_ADD), pcp::exception);

    // All non-errors should return without exceptions.
    EXPECT_NO_THROW(pcp::cache::store(123, "foo", "bah", (void *)NULL));
    EXPECT_NO_THROW(pcp::cache::store(123, "foo", "bah", PMDA_CACHE_ADD));
}
