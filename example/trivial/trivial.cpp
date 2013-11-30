//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 *
 * @brief Trivial C++ PMDA
 *
 * Implements a C++ equivalent of PCP's Trivial PMDA example.
 *
 * @see http://oss.sgi.com/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmdas/trivial/trivial.c
 */

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/instance_domain.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/units.hpp>

class trivial : public pcp::pmda {

public:

    virtual std::string get_pmda_name() const
    {
        return "trivial";
    }

    virtual int default_pmda_domain_number() const
    {
        return 250;
    }

protected:

    virtual pcp::metrics_description get_supported_metrics() const
    {
        // trivial.time aka TRIVIAL:0:0.
        return pcp::metrics_description()(0)
            (0, "time",pcp::type<uint32_t>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0), NULL,
             "The time in seconds since 1 Jan 1970",
             "The time in seconds since the 1st of January, 1970.");
    }

    virtual pmAtomValue fetch_value(const pcp::pmda::metric_id &metric) const
    {
        return pcp::atom(metric.type,time(NULL));
    }

};

// DSO entry point.
extern "C" void trivial_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<trivial>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<trivial>(argc, argv);
}
