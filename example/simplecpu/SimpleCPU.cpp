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

#include <pcp-cpp/pmda.hpp>

class SimpleCPU : public pcp::pmda {
protected:
    pcp::instance_domain cpuStates;

    virtual pcp::instance::domains fetchInstanceDomains() const
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
        return cpuStates;
    }

    virtual pcp::metric::desc fetchMetricsDescriptions() const
    {
        pcp::metric::desc(0, "cpu")
            <uint64_t>(0, "ticks", &cpuStates, "count of blah blah");
        foreach (cpu)
            pcp::metric::desc(index, "cpu%d")
                <uint64_t>(0, "ticks", &cpuStates, "count of blah blah on cpu %d");
        return desc;
    }

    virtual bool cacheValues()
    {
        // read from /proc/stat
        return true;
    }

    virtual pcp::metric_value fetchValue(const pcp::metric &metric) const
    {

    }
};

int main(int argc, char *argv[])
{
    return pcp::pmda::run<SimpleCPU>(argc, argv);
}
