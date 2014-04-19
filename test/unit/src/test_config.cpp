//               Copyright Paul Colby 2014.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fake_libpcp.h"

#include "pcp-cpp/config.hpp"

#include "gtest/gtest.h"

TEST(get_pcp_runtime_version, numeric) {
    fake_pm_config.clear();

    // Typical PCP version strings.
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("1.2.3");
    EXPECT_EQ(0x10203, pcp::get_pcp_runtime_version<int32_t>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("4.67.78");
    EXPECT_EQ(0x4434E, pcp::get_pcp_runtime_version<int32_t>());

    // Edge cases.
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("0.0.0");
    EXPECT_EQ(0, pcp::get_pcp_runtime_version<int32_t>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("255.255.255");
    EXPECT_EQ(0xFFFFFF, pcp::get_pcp_runtime_version<int32_t>());

    // Funky / bad / invalid versions.
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("not a version string");
    EXPECT_EQ(0, pcp::get_pcp_runtime_version<int32_t>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("1");
    EXPECT_EQ(0, pcp::get_pcp_runtime_version<int32_t>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("1.2");
    EXPECT_EQ(0, pcp::get_pcp_runtime_version<int32_t>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("1.2.4.5.6.7.8.9");
    EXPECT_EQ(0x10209, pcp::get_pcp_runtime_version<int32_t>());

    fake_pm_config.clear();
}

TEST(get_pcp_runtime_version, string) {
    fake_pm_config.clear();

    // The get_pcp_runtime_version<char *> specialisation simply returns the
    // pmGetValue value verbatim.
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("");
    EXPECT_STREQ("", pcp::get_pcp_runtime_version<char *>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("1.2.3");
    EXPECT_STREQ("1.2.3", pcp::get_pcp_runtime_version<char *>());
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("4.5.6");
    EXPECT_STREQ("4.5.6", pcp::get_pcp_runtime_version<char *>());

    fake_pm_config.clear();
}
