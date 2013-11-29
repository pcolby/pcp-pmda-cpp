# PMDA++

PMDA++ is a header-only C++ library for writing Performance Metrics Domain Agents (PMDAs) for [Performance Co-Pilot (PCP)](http://oss.sgi.com/projects/pcp/).

PCP PMDAs can be written in a number of languages, including C, Perl and Python.This library adds C++ to that list.

## Find out more
Take a look at the project [wiki](https://github.com/pcolby/pcp-pmda-cpp/wiki).

## License

PMDA++ is copyright [Paul Colby](https://github.com/pcolby) 2013, and available under the very-flexibly [Boost Software License](http://www.boost.org/users/license.html).

Note, that the PMDA++ library is not much use without also linking to [PCP](http://oss.sgi.com/projects/pcp/)'s PMDA libraries (`libpcp` and `libpcp_pmda`) which are provided by PCP under LGPL v2.1. See [What licensing scheme does PCP use?](http://oss.sgi.com/projects/pcp/faq.html#Q1b)
