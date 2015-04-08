//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define protected public
#include "pcp-cpp/pmda.hpp"
#undef protected

#include "pcp-cpp/units.hpp"

#include "fake_libpcp.h"

#include "gtest/gtest.h"

// PM_ERR_FAULT ("QA fault injected") was not added until PCP 3.6.0.
#ifndef PM_ERR_FAULT
#define PM_ERR_FAULT PM_ERR_GENERIC
#endif

/// @brief  Implements a bare-minimum concrete PMDA implementation.
class stub_pmda : public pcp::pmda {

public:
    pcp::metrics_description supported_metrics;

    // Since we use a const stub_pmda in at least one test, C++ requires us to
    // provide a user-declared constructor.
    stub_pmda() { }

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

/// @brief Overrides virtual pcp::pmda methods to test their exception handling.
class exceptional_pmda : public stub_pmda {
protected:
    virtual void run_daemon(const int, char * const []) {
        throw pcp::exception(PM_ERR_FAULT);
    }

    virtual void initialize_dso(pmdaInterface &) {
        throw pcp::exception(PM_ERR_FAULT);
    }
};

/// @brief Makes some virtual pcp::pmda methods public for testing.
class publicized_pmda : public stub_pmda {
public:
    typedef stub_pmda::metric_id metric_id;

    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface,
                                    boost::program_options::variables_map &options) {
        return stub_pmda::parse_command_line(argc, argv, interface, options);
    }

    virtual void store_value(const metric_id &metric, const int &value) {
        stub_pmda::store_value(metric, value);
    }

    virtual void store_value(const metric_id &metric,
                             const pmValueBlock * const value) {
        stub_pmda::store_value(metric, value);
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

TEST(pmda, init_dso_handles_exceptions) {
    EXPECT_NO_THROW(pcp::pmda::init_dso<exceptional_pmda>(NULL));
}

TEST(pmda, run_daemon_handles_exceptions) {
    EXPECT_EQ(EXIT_FAILURE, pcp::pmda::run_daemon<exceptional_pmda>(0, NULL));
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

TEST(pmda, parse_command_line_throws_on_invalid_config_option) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--config=/dev/null/invalid" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    EXPECT_THROW(pmda.parse_command_line(2, argv, interface, options), pcp::exception);
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_default_debug_option) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--debug" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    pmDebug = 0;
    EXPECT_TRUE(pmda.parse_command_line(2, argv, interface, options));
    EXPECT_EQ(std::numeric_limits<int>::max(), pmDebug);
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_throws_on_invalid_debug_options) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--debug=invalid" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    EXPECT_THROW(pmda.parse_command_line(2, argv, interface, options), pcp::exception);
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_pipe_option) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--pipe" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    EXPECT_TRUE(pmda.parse_command_line(2, argv, interface, options));
    EXPECT_EQ(pmdaPipe, interface.version.two.ext->e_io);
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_unix_option) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--unix=/dev/null" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    EXPECT_TRUE(pmda.parse_command_line(2, argv, interface, options));
    EXPECT_EQ(pmdaUnix, interface.version.two.ext->e_io);
    EXPECT_STREQ("/dev/null", interface.version.two.ext->e_sockname);
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_inet6_option) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--inet6=1234" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;

    // --inet6 not supported before PCP 3.8.1.
    fake_pm_config.clear();
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("3.8.0");
    EXPECT_EQ(0x30800, pcp::get_pcp_runtime_version<int32_t>());
    EXPECT_THROW(pmda.parse_command_line(2, argv, interface, options),
                 boost::program_options::unknown_option);

    // --inet6 supported from PCP 3.8.1 on.
    fake_pm_config.clear();
    fake_pm_config["PCP_VERSION"] = const_cast<char *>("3.8.1");
    EXPECT_EQ(0x30801, pcp::get_pcp_runtime_version<int32_t>());
    EXPECT_TRUE(pmda.parse_command_line(2, argv, interface, options));
    EXPECT_EQ(static_cast<pmdaIoType>(4), interface.version.two.ext->e_io);
    EXPECT_EQ(1234, interface.version.two.ext->e_port);

    fake_pm_config.clear();
    delete interface.version.two.ext;
}

TEST(pmda, parse_command_line_inet) {
    publicized_pmda pmda;
    const char * argv[] = { "pmda_name", "--inet=1234" };
    pmdaInterface interface;
    interface.version.two.ext = new pmdaExt;
    boost::program_options::variables_map options;
    EXPECT_TRUE(pmda.parse_command_line(2, argv, interface, options));
    EXPECT_EQ(pmdaInet, interface.version.two.ext->e_io);
    EXPECT_EQ(1234, interface.version.two.ext->e_port);
    delete interface.version.two.ext;
}

TEST(pmda, store_value_throws_by_default) {
    publicized_pmda pmda;
    publicized_pmda::metric_id metric_id;

    EXPECT_THROW(pmda.store_value(metric_id, 0), pcp::exception);

    const pmValueBlock * const value = NULL;
    EXPECT_THROW(pmda.store_value(metric_id, value), pcp::exception);
}

TEST(pmda, display_functions_do_not_leave_cout_flags_modified) {
    const std::ostream::fmtflags flags(std::cout.flags());
    const stub_pmda pmda;

    pmda.display_help(pmda.get_pmda_name());
    EXPECT_EQ(flags, std::cout.flags());

    pmda.display_version();
    EXPECT_EQ(flags, std::cout.flags());
}
