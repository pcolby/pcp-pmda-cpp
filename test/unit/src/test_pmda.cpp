//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define protected public
#include "pcp-cpp/pmda.hpp"
#undef protected

#include "pcp-cpp/units.hpp"

#include "gtest/gtest.h"

/// @brief  Implements a bare-minimum concrete PMDA implementation.
class stub_pmda : public pcp::pmda {

public:
    pcp::metrics_description supported_metrics;

    virtual std::string get_pmda_name() const {
        return "stub";
    }

    virtual int get_default_pmda_domain_number() const {
        return -123;
    }

    virtual pcp::metrics_description get_supported_metrics() {
        return supported_metrics;
    }

    virtual fetch_value_result fetch_value(const metric_id &/*metric*/)
    {
        throw pcp::exception(PM_ERR_NYI);
    }

};

/// @brief  Overrides virtual pcp::pmda methods to verify thier virtualisation.
class override_pmda : public stub_pmda {
public:
    virtual std::string get_config_file_pathname() const
    {
        return "overridden config file pathname";
    }

    virtual std::string get_help_text_pathname() const
    {
        return "overridden help text pathname";
    }

    virtual std::string get_log_file_pathname() const
    {
        return "overridden log file pathname";
    }

    virtual std::string get_pmda_version() const {
        return "overridden version";
    }
};

TEST(pmda, get_instance) {
    // Instance should be NULL, since we haven't initialised any DSO or daemon
    // interfaces yet.
    EXPECT_EQ(NULL, pcp::pmda::get_instance());
}

TEST(pmda, get_config_file_pathname) {
    // The base implementation return a constructed pathname.
    stub_pmda pmda1;
    EXPECT_EQ("PCP_PMDAS_DIR|stub|config", pmda1.get_config_file_pathname());

    // The base implementation must be virtual, allowing us to override.
    override_pmda pmda2;
    EXPECT_EQ("overridden config file pathname", pmda2.get_config_file_pathname());
    EXPECT_EQ("overridden config file pathname", static_cast<pcp::pmda &>(pmda2).get_config_file_pathname());
    EXPECT_EQ(pmda2.get_config_file_pathname(), static_cast<pcp::pmda &>(pmda2).get_config_file_pathname());
}

TEST(pmda, get_help_text_pathname) {
    // The base implementation return a constructed pathname.
    stub_pmda pmda1;
    EXPECT_EQ("PCP_PMDAS_DIR|stub|help", pmda1.get_help_text_pathname());

    // The base implementation must be virtual, allowing us to override.
    override_pmda pmda2;
    EXPECT_EQ("overridden help text pathname", pmda2.get_help_text_pathname());
    EXPECT_EQ("overridden help text pathname", static_cast<pcp::pmda &>(pmda2).get_help_text_pathname());
    EXPECT_EQ(pmda2.get_help_text_pathname(), static_cast<pcp::pmda &>(pmda2).get_help_text_pathname());
}

TEST(pmda, get_log_file_pathname) {
    // The base implementation should return the PMDA name with ".log" extension.
    stub_pmda pmda1;
    EXPECT_EQ(pmda1.get_pmda_name() + ".log", pmda1.get_log_file_pathname());

    // The base implementation must be virtual, allowing us to override.
    override_pmda pmda2;
    EXPECT_EQ("overridden log file pathname", pmda2.get_log_file_pathname());
    EXPECT_EQ("overridden log file pathname", static_cast<pcp::pmda &>(pmda2).get_log_file_pathname());
    EXPECT_EQ(pmda2.get_log_file_pathname(), static_cast<pcp::pmda &>(pmda2).get_log_file_pathname());
}

TEST(pmda, get_pmda_version) {
    // The base implementation should return an empty PMDA version string.
    stub_pmda pmda1;
    EXPECT_EQ("", pmda1.get_pmda_version());

    // The base implementation must be virtual, allowing us to override.
    override_pmda pmda2;
    EXPECT_EQ("overridden version", pmda2.get_pmda_version());
    EXPECT_EQ("overridden version", static_cast<pcp::pmda &>(pmda2).get_pmda_version());
    EXPECT_EQ(pmda2.get_pmda_version(), static_cast<pcp::pmda &>(pmda2).get_pmda_version());
}

TEST(pmda, initialize_pmda) {
    stub_pmda pmda;
    pmda.supported_metrics(123, "cluster 123")(456, "cluster 456");
    pcp::instance_domain domain;
    int opaque = 123;
    pmda.supported_metrics
        (1, "one", PM_TYPE_U64, PM_SEM_INSTANT, pcp::units(1,2,3, 4,5,6), NULL,
         "short description", "verbose description", NULL, pcp::storable_metric)
        (2, "two", PM_TYPE_STRING, PM_SEM_COUNTER, pcp::units(-1,-2,-3, 10,11,-6),
        &domain, "short", "verbose", &opaque);

    pmdaInterface interface;
    memset(&interface, 0, sizeof(interface));
    pmda.initialize_pmda(interface);

    EXPECT_NE(static_cast<pmdaExt *>(NULL), interface.version.two.ext);
    EXPECT_NE(static_cast<pmdaIndom *>(NULL), interface.version.two.ext->e_indoms);
    EXPECT_NE(static_cast<pmdaMetric *>(NULL), interface.version.two.ext->e_metrics);

    EXPECT_EQ(1, interface.version.two.ext->e_nindoms);
    EXPECT_EQ(2, interface.version.two.ext->e_nmetrics);

    EXPECT_EQ(PM_TYPE_U64, interface.version.two.ext->e_metrics[0].m_desc.type);
    EXPECT_EQ(PM_SEM_INSTANT, interface.version.two.ext->e_metrics[0].m_desc.sem);
    EXPECT_EQ(1, interface.version.two.ext->e_metrics[0].m_desc.units.dimSpace);
    EXPECT_EQ(2, interface.version.two.ext->e_metrics[0].m_desc.units.dimTime);
    EXPECT_EQ(3, interface.version.two.ext->e_metrics[0].m_desc.units.dimCount);
    EXPECT_EQ(4u, interface.version.two.ext->e_metrics[0].m_desc.units.scaleSpace);
    EXPECT_EQ(5u, interface.version.two.ext->e_metrics[0].m_desc.units.scaleTime);
    EXPECT_EQ(6, interface.version.two.ext->e_metrics[0].m_desc.units.scaleCount);
    EXPECT_EQ(PM_INDOM_NULL, interface.version.two.ext->e_metrics[0].m_desc.indom);
    EXPECT_EQ(NULL, interface.version.two.ext->e_metrics[0].m_user);

    EXPECT_EQ(PM_TYPE_STRING, interface.version.two.ext->e_metrics[1].m_desc.type);
    EXPECT_EQ(PM_SEM_COUNTER, interface.version.two.ext->e_metrics[1].m_desc.sem);
    EXPECT_EQ(-1, interface.version.two.ext->e_metrics[1].m_desc.units.dimSpace);
    EXPECT_EQ(-2, interface.version.two.ext->e_metrics[1].m_desc.units.dimTime);
    EXPECT_EQ(-3, interface.version.two.ext->e_metrics[1].m_desc.units.dimCount);
    EXPECT_EQ(10u, interface.version.two.ext->e_metrics[1].m_desc.units.scaleSpace);
    EXPECT_EQ(11u, interface.version.two.ext->e_metrics[1].m_desc.units.scaleTime);
    EXPECT_EQ(-6, interface.version.two.ext->e_metrics[1].m_desc.units.scaleCount);
    EXPECT_EQ(&opaque, interface.version.two.ext->e_metrics[1].m_user);
}
