//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "pcp-cpp/pmda.hpp"

#include "gtest/gtest.h"

class stub_pmda : public pcp::pmda {
public:
    /// Expose the initialise_pmda method publicly.
    virtual void initialize_pmda(pmdaInterface &interface)
    {
        pcp::pmda::initialize_pmda(interface);
    }

protected:
    virtual std::string get_pmda_name() const {
        return "stub";
    }

    virtual int default_pmda_domain_number() const {
        return -123;
    }

    virtual pcp::metrics_description get_supported_metrics() {
        return supported_metrics;
    }

    virtual fetch_value_result fetch_value(const metric_id &metric)
    {
        throw pcp::exception(PM_ERR_NYI);
    }

private:
    pcp::metrics_description supported_metrics;

};

TEST(pmda, initialize_pmda) {
    stub_pmda pmda;
    pmdaInterface interface;
    pmda.initialize_pmda(interface);

    /// @todo  Test stuff here.
}

