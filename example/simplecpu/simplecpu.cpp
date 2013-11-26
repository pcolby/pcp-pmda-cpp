//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/exception.hpp>
#include <pcp-cpp/instance_domain.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/units.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class simple_cpu : public pcp::pmda {
public:
    simple_cpu()
    {
        cpu_states(0)
            (0, "user")
            (1, "system")
            (2, "idle")
            (3, "iowait")
            (4, "irq")
            (5, "softirq")
            (6, "steal")
            (7, "guest")
            (8, "guest_nice");
        load_cpu_ticks();
    }

    virtual std::string get_pmda_name() const
    {
        return "simplecpu";
    }

    virtual pmda_domain_number_type default_pmda_domain_number() const
    {
        return 129;
    }

protected:
    pcp::instance_domain cpu_states;
    typedef struct {
        std::string label;
        std::vector<uint64_t> ticks;
    } cpu_info_type;
    std::vector<cpu_info_type> cpu_info;

    virtual pcp::metrics_description get_supported_metrics() const
    {
        // This declaresa set of "ticks" metrics for each available
        // CPU, in each of the cpu_states instance domains, within
        // metric cluster 0; ie:
        //
        // simplecpu.ticks.total[cpu_status] => SIMPLECPU:0:0
        // simplecpu.ticks.cpu0[cpu_status]  => SIMPLECPU:0:1
        // ...
        // simplecpu.ticks.cpuN[cpu_status]  => SIMPLECPU:0:N+1
        //
        pcp::metrics_description metrics;
        metrics(0, "ticks");
        for (std::vector<cpu_info_type>::size_type index = 0;
             index < cpu_info.size(); ++index)
        {
            metrics
                (index, (index == 0) ? std::string("total") : cpu_info.at(index).label,
                 pcp::type<uint64_t>(), PM_SEM_COUNTER, pcp::units(0,0,1, 0,0,PM_COUNT_ONE),
                 &cpu_states, "The amount of time spent in various states");
        }
        return metrics;
    }

    virtual void begin_fetch_values()
    {
        load_cpu_ticks();
    }

    virtual pmAtomValue fetch_value(const pcp::pmda::metric_id &metric) const
    {

        return pcp::atom<uint64_t>(
            metric.type,cpu_info.at(metric.item).ticks.at(metric.instance));
    }

    void load_cpu_ticks()
    {
        cpu_info.clear();
        std::ifstream file("/proc/stat");
        if (!file.is_open()) {
            throw pcp::exception(PM_ERR_NODATA);
        }
        for (std::string line; std::getline(file, line); line.clear()) {
            if (line.compare(0, 3, "cpu") == 0) {
                const std::string::size_type pos = line.find(' ');
                if (pos != std::string::npos) {
                    cpu_info_type info = { line.substr(0, pos) };
                    std::istringstream stream(line.substr(pos));
                    for (uint64_t tick_count; stream >> tick_count;) {
                        info.ticks.push_back(tick_count);
                    }
                    cpu_info.push_back(info);
                }
            }
        }
        if (cpu_info.empty()) {
            throw pcp::exception(PM_ERR_NODATA);
        }
    }

};

// DSO entry point.
extern "C" void simplecpu_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<simple_cpu>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<simple_cpu>(argc, argv);
}
