/*
    Copyright 2013 Paul Colby

    This file is part of pmda-cpp.

    Pmda-cpp is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Pmda-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
    more details.

    You should have received a copy of the GNU Lesser General Public License
    along with pmda-cpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pcp-cpp/exception.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/instance_domain.hpp> /// @todo remove include.

class SimpleCPU : public pcp::pmda {
public:
    SimpleCPU() {
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

    virtual int_fast16_t default_pmda_domain_number()
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
                <uint64_t>(0, "ticks", &cpuStates, "count of blah blah on cpu %d");
        }*/
        return metrics;
    }

    virtual void begin_fetch_values()
    {
        // read from /proc/stat
        throw pcp::exception(PM_ERR_VALUE);
    }

    /*template <typename Type>
    virtual Type fetch_metric(const pcp::metric_id &metric) const
    {
        throw pcp::pmda_exception(PM_ERR_VALUE, "msg");
        return pcp::atom(metric.typeinfo, 123);
    }*/
};

int main(int argc, char *argv[])
{
    return pcp::pmda::run<SimpleCPU>(argc, argv);
}
