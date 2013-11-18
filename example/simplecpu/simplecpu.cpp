//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/exception.hpp>
#include <pcp-cpp/instance_domain.hpp>
#include <pcp-cpp/pmda.hpp>

class SimpleCPU : public pcp::pmda {
public:
    SimpleCPU()
    {
        cpuStates(0)
            (0, "user"),
            (1, "system"),
            (2, "idle"),
            (3, "iowait"),
            (4, "irq"),
            (5, "softirq"),
            (6, "steal"),
            (7, "guest"),
            (8, "guest_nice");
    }

    virtual pmda_domain_number_type default_pmda_domain_number()
    {
        return 129;
    }

protected:
    pcp::instance_domain cpuStates;

    virtual pcp::metrics_description get_supported_metrics() const
    {
        // simplecpu.cpu.ticks
        pcp::metrics_description metrics;
        metrics
            (0, "cpu");
                (0, "ticks", pcp::type<int64_t>(), &cpuStates, "count of blah blah");
        /*foreach (cpu) {
            metrics(index, "cpu%d")
                (0, "ticks", pcp::type<int64_t>(), &cpuStates, "count of blah blah on cpu %d");
        }*/
        return metrics;
    }

    virtual void begin_fetch_values()
    {
        // read from /proc/stat
        throw pcp::exception(PM_ERR_VALUE);
    }

    virtual pmAtomValue fetch_value(const pcp::metric_id &metric) const
    {
        throw pcp::exception(PM_ERR_NYI);
        return pcp::atom<int>(metric.type, 123);
    }
};

// DSO entry point.
void simplecpu_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<SimpleCPU>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<SimpleCPU>(argc, argv);
}
