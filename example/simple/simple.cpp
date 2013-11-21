//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 *
 * @brief Simple C++ PMDA
 *
 * Implements a C++ equivalent of PCP's Simple PMDA example.
 *
 * @see http://oss.sgi.com/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmdas/simple/simple.c
 */

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/exception.hpp>
#include <pcp-cpp/instance_domain.hpp>
#include <pcp-cpp/metric_description.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/units.hpp>

#include <pcp/impl.h> // For __pmProcessRunTimes.

class simple : public pcp::pmda {

public:
    simple() : numfetch(0)
    {
        // Define the color and now instance domains.
        color_domain(0)(0, "red")(1, "green"),(2, "blue");
        now_domain(1);
    }

    virtual std::string get_pmda_name() const
    {
        return "simple";
    }

    virtual pmda_domain_number_type default_pmda_domain_number() const
    {
        return 253;
    }

protected:
    pcp::instance_domain color_domain;
    pcp::instance_domain now_domain;
    uint32_t numfetch;

    virtual pcp::metrics_description get_supported_metrics() const
    {
        // simple.numfetch    SIMPLE:0:0
        // simple.color       SIMPLE:0:1
        // simple.time.user   SIMPLE:1:2
        // simple.time.sys    SIMPLE:1:3
        // simple.now         SIMPLE:2:4
        return pcp::metrics_description()
        (0)
            (0, "numfetch",pcp::type<uint32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0))
            (1, "color",pcp::type<int32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), &color_domain)
        (1, "time")
            (2, "user",pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
            (3, "sys",pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
        (2)
            (4, "now",pcp::type<uint32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), &now_domain);
    }

    virtual void begin_fetch_values()
    {
        numfetch++;
    }

    virtual pmAtomValue fetch_value(const pcp::pmda::metric_id &metric) const
    {
        if (metric.cluster == 0) {
            if (metric.item == 0) {
                return pcp::atom(metric.type, numfetch);
            } else if (metric.item == 1) {
                static int32_t rgb[] = { 0, 100, 200 };
                rgb[metric.instance] = (rgb[metric.instance] + 1) % 256;
                return pcp::atom(metric.type, rgb[metric.instance]);
            }
        } else if (metric.cluster == 1) {
            static uint32_t oldfetch(0);
            static double usr, sys;
            if (oldfetch < numfetch) {
                __pmProcessRunTimes(&usr, &sys);
                oldfetch = numfetch;
            }
            return pcp::atom(metric.type, metric.item == 3 ? usr : sys);
        }

        switch (metric.cluster) {
        case 0:
            switch (metric.item) {
            case 0:
                return pcp::atom(metric.type, numfetch);
            case 1:
                static int32_t rgb[] = { 0, 100, 200 };
                rgb[metric.instance] = (rgb[metric.instance] + 1) % 256;
                return pcp::atom(metric.type, rgb[metric.instance]);
            }
            break;
        case 1: {
            static uint32_t oldfetch(0);
            static double usr, sys;
            if (oldfetch < numfetch) {
                __pmProcessRunTimes(&usr, &sys);
                oldfetch = numfetch;
            }
            switch (metric.item) {
            case 2:
                return pcp::atom(metric.type, usr);
            case 3:
                return pcp::atom(metric.type, sys);
            }
        }
        throw pcp::exception(PM_ERR_NYI); // Not Yet Implemented.
        }
    }

};

// DSO entry point.
void trivial_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<simple>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<simple>(argc, argv);
}
