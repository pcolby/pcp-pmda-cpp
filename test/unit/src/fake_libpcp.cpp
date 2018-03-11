//               Copyright Paul Colby 2013 - 2018.
//               Copyright Red Hat 2018.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fake_libpcp.h"

// Boost headers that depend on boost/cstdint.hpp must be included before any
// PCP headers, because pcp/config.h sets a number of macros like ULONGLONG_MAX
// which fool Boost into thinking we're on an unknown, non-standard platform.
#include <boost/lexical_cast.hpp>
#include <pcp/pmapi.h>

// PM_ERR_FAULT ("QA fault injected") was not added until PCP 3.6.0.
#ifndef PM_ERR_FAULT
#define PM_ERR_FAULT PM_ERR_GENERIC
#endif

// PM_VERSION macro was not added until PCP 3.10.5.
#ifndef PM_VERSION
#define PM_VERSION(a,b,c) (((a)<<16)|((b)<<8)|(c))
#endif

// PCP 4.0.0 cleaned and promoted some functions, renaming them in the process.
#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(4,0,0)
#define pmNotifyErr __pmNotifyErr
#define pmPathSeparator __pmPathSeparator
#define pmSetProgname(program) __pmSetProgname(program)
#endif

#include <stdexcept>
#include <string>

std::map<std::string, char *> fake_pm_config;

extern "C" {

int pmDebug;

// Drop-in pmErrStr[_r] replacement that always returns the \a code as a string
#ifdef PM_MAXERRMSGLEN
char *pmErrStr_r(int code, char *buf, int buflen)
{
    if (buflen < PM_MAXERRMSGLEN) {
        throw std::length_error(
            "pmErrStr_r: buflen (" +
            boost::lexical_cast<std::string>(buflen) +
            ") less than PM_MAXERRMSGLEN (" +
                    boost::lexical_cast<std::string>(buflen) + ')'
            );
    }
    const std::string str = boost::lexical_cast<std::string>(code);
    strncpy(buf, str.c_str(), buflen);
    buf[buflen - 1] = '\0';
    return buf;
}
#else
const char *pmErrStr(int code)
{
    static char buf[128] = { '\0' };
    const std::string str = boost::lexical_cast<std::string>(code);
    strncpy(buf, str.c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    return buf;
}
#endif

char *pmGetConfig(const char *variable)
{
    const std::map<std::string, char *>::const_iterator iter =
        fake_pm_config.find(variable);
    return (iter == fake_pm_config.end()) ? const_cast<char *>(variable) : iter->second;
}

void pmNotifyErr(int /*priority*/, const char */*message*/, ...)
{

}

#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT <= PM_VERSION(3,12,2)
int __pmParseDebug(const char *spec)
#else
int pmSetDebug(const char *spec)
#endif
{
    // The first thing the real __pmParseDebug does is dereference spec.
    if (spec == NULL) {
        throw "spec must not be NULL";
    }

    // Allow our unit tests to invoke an error response.
    if (strcmp(spec, "invalid") == 0) {
        return PM_ERR_FAULT; // "QA fault injected"
    }

    // Lazily convert the spec to an int. Note, this is *not* the same logic as
    // the real __pmParseDebug function.
    const int result = atoi(spec);

    // The real __pmParseDebug and pmSetDebug allow "-1" as a synonym for "all".
    if (result == -1) {
        return std::numeric_limits<int>::max();
    }

    // PCP 3.12.2 added support for string "all" too.
    #if defined PM_VERSION_CURRENT && PM_VERSION_CURRENT >= PM_VERSION(3,12,2)
    if (strcmp(spec, "all") == 0) {
        return std::numeric_limits<int>::max();
    }
    #endif

    // PCP 3.12.2 also sets the pmDebug global (ie pmSetDebug vs __pmParseDebug)
    pmDebug |= result;
    return result;
}

int pmPathSeparator()
{
    return '|';
}

void pmSetProgname(const char */*program*/)
{

}

} // extern "C"
