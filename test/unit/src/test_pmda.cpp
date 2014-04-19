//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define protected public
#include "pcp-cpp/pmda.hpp"
#undef protected

#include "pcp-cpp/units.hpp"

#include "gtest/gtest.h"

class stub_pmda : public pcp::pmda {

public:
    pcp::metrics_description supported_metrics;

protected:
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

TEST(get_instance, null) {
    // Instance should be NULL, since we haven't initialised any DSO or daemon
    // interfaces yet.
    EXPECT_EQ(NULL, pcp::pmda::get_instance());
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
