# [![PMDA++](https://s3.amazonaws.com/cloud.ohloh.net/attachments/71802/pcp-pmda-cpp_med.png)](#) PMDA++ [![Build Status](https://travis-ci.org/pcolby/pcp-pmda-cpp.png?branch=master)](https://travis-ci.org/pcolby/pcp-pmda-cpp)

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

Here is a complete, albeit very basic, PMDA written using PMDA++ that simply
returns the current time:

```c++
#include <pcp-cpp/atom.hpp>
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

    virtual pcp::metrics_description get_supported_metrics()
    {
        // trivial.time aka TRIVIAL:0:0.
        return pcp::metrics_description()(0)
            (0, "time",pcp::type<uint32_t>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0));
    }

    virtual fetch_value_result fetch_value(const metric_id &metric)
    {
        return pcp::atom(metric.type,time(NULL));
    }

};

int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<trivial>(argc, argv);
}
```

Compare that to PCP's official [trivial.c](http://oss.sgi.com/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmdas/trivial/trivial.c) example.

## Benchmarks

Some basic [benchmarks](../../wiki/Benchmarks) are availble on the [wiki](../../wiki/).

[![simple numfetch](https://f.cloud.github.com/assets/5195222/1700167/56531708-5ff7-11e3-8baf-80d1f3c72234.png)](../../wiki/Benchmarks)

## Requirements

PMDA++ requires little more than a modern C++ compiler and the PCP client libraries.

Additionaly, PMDA++ can make use of Boost libraries (though not required) to provide some enhanced functionaliy.

See the [requirements wiki page](https://github.com/pcolby/pcp-pmda-cpp/wiki/Requirements) for more details.

## Contributing

There are lots of way you can contribute, including (but not limited to):
* reviewing the code itself (its only 1K ~ 2K lines), looking for issues such as
bugs, portability issues, and the like.
* reviewing the API, suggesting improvments such as more intuitive naming and
future-proofing.
* indentifying violations of the project's desired [conventions](https://github.com/pcolby/pcp-pmda-cpp/wiki/conventions).
* writing your own PMDA using PMDA++ to test it in real world applications.
* improving the [wiki](https://github.com/pcolby/pcp-pmda-cpp/wiki) documentation.

Issues / suggestions can be reported via GitHub's [issue
tracker](https://github.com/pcolby/pcp-pmda-cpp/issues). Pull requests are very
welcome.

The [PMDA++ Google Group](http://groups.google.com/group/pcp-pmda-cpp/)
is used for general discussion, questions, comments, suggestions, announcements
etc.  Email pcp-pmda-cpp+subscribe@googlegroups.com to subscribe.

## License

PMDA++ code is available under the [OSI-approved](http://opensource.org/licenses/BSL-1.0)
[Boost Software License](http://www.boost.org/users/license.html).

PMDA++ relies on [PCP](http://oss.sgi.com/projects/pcp/)
libraries (`libpcp` and `libpcp_pmda`), which are [available under LGPL v2.1.](
http://oss.sgi.com/projects/pcp/faq.html#Q1b).
