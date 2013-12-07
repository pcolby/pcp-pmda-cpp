# PMDA++ [![Build Status](https://travis-ci.org/pcolby/pcp-pmda-cpp.png?branch=master)](https://travis-ci.org/pcolby/pcp-pmda-cpp)

PMDA++ is a header-only library that allows developers to write Performance
Metrics Domain Agents (PMDAs) for [Performance Co-Pilot
(PCP)](http://oss.sgi.com/projects/pcp/) in C++.

### What is PCP?

> [Performance Co-Pilot (PCP)](http://oss.sgi.com/projects/pcp/) is an open
source infrastructure for monitoring, visualizing, recording, responding to,
and controlling the status, activity, and performance of networks, computers,
applications, and servers.
-- <cite>[Wikipedia](https://en.wikipedia.org/wiki/Performance_Co-Pilot)</cite>

### What is a PMDA?

PCP makes use of add-ons called Performance Metrics Domain Agents (PMDAs) to
fetch performance metrics for specific domains, such as database servers,
hardware, custom applications, etc.

For more information, see the [Performance Co-Pilot Programmer's
Guide](http://oss.sgi.com/projects/pcp/doc/pcp-programmers-guide.pdf).

### What is PMDA++?

PCP includes support for writing PMDAs in C, Perl and Python.  PMDA++ is a
header-only library that allows developers to write PMDAs in C++.  It is a
light C++ wrapper around PCP's C APIs.

### Writing a basic PMDA with PMDA++

Note, this section assumes that you are already familiar with basic PMDA
concepts. If you have not already done so, you should read the [Performance
Co-Pilot Programmer's
Guide](http://oss.sgi.com/projects/pcp/doc/pcp-programmers-guide.pdf).

To write a basic PMDA using PMDA++:

1. Create a new class derived from `pcp::pmda`.
```c++
#include <pcp-cpp/pmda.hpp>

class basic_pmda : public pcp::pmda {

}
```
2. Implement the pure-virtual `get_pmda_name` and `default_pmda_domain_number`
functions:
```c++
#include <pcp-cpp/pmda.hpp>

class basic_pmda : public pcp::pmda {
public:

    virtual std::string get_pmda_name() const
    {
        return "basic";
    }

    virtual int default_pmda_domain_number() const
    {
        return 456;
    }
}
```
3. Implement the pure-virtual `get_supported_metrics` function to declare the
metrics this PMDA makes available:
```c++
    virtual pcp::metrics_description get_supported_metrics()
    {
        // A single "basic.time" metric, with cluster ID 123 and item ID 456.
        return pcp::metrics_description()(123)
            (456, "time", pcp::type<uint32_t>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0));
    }
```
4. Implement the pure-virtual `fetch_value` function to fetch actual metric
value(s):
```c++
    virtual fetch_value_result fetch_value(const pcp::pmda::metric_id &metric)
    {
        // Return the current time.
        return pcp::atom(metric.type, time(NULL));
    }
```
5. Add either a DSO or daemon entry-point function, or both:
```c++
// DSO entry point.
extern "C" void basic_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<basic_pmda>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<basic_pmda>(argc, argv);
}
```

And that's it! There's a lot of other virtual functions that may be overridden
to customize / extend the PMDA's behaviour. See the [Reference](wiki#reference) section below.

For complete, albeit contrived, examples take a look at
[trivial.cpp](blob/master/example/trivial/trivial.cpp) and
[simple.cpp](blob/master/example/simple/simple.cpp).

## Find out more

Take a look at the project [wiki](https://github.com/pcolby/pcp-pmda-cpp/wiki).

## License

PMDA++ code is available under the open source [Boost Software
License](http://www.boost.org/users/license.html).

PMDA++ relies on [PCP](http://oss.sgi.com/projects/pcp/)
libraries (`libpcp` and `libpcp_pmda`), which are available under LGPL v2.1.
See [What licensing scheme does PCP
use?](http://oss.sgi.com/projects/pcp/faq.html#Q1b)
