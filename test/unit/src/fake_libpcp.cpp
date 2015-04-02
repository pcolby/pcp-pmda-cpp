//               Copyright Paul Colby 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "fake_libpcp.h"

// Boost headers that depend on boost/cstdint.hpp must be included before any
// PCP headers, because pcp/config.h sets a number of macros like ULONGLONG_MAX
// which fool Boost into thinking we're on an unknown, non-standard platform.
#include <boost/lexical_cast.hpp>
#include <pcp/pmapi.h>

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
    return (iter == fake_pm_config.end()) ? strdup(variable) : iter->second;
}

void __pmNotifyErr(int /*priority*/, const char */*message*/, ...)
{

}

int __pmParseDebug(const char *spec)
{
    // Allow our unit tests to invoke an error response.
    if (strcmp(spec, "invalid") == 0) {
        return PM_ERR_FAULT; // "QA fault injected"
    }

    // Lazily convert the spec to an int. Note, this is *not* the same logic as
    // the real __pmParseDebug function.
    const int result = (spec == NULL) ? 0 : atoi(spec);

    // The real __pmParseDebug does support "-1" as a synonym for "all".
    return (result == -1) ? std::numeric_limits<int>::max() : result;
}

int __pmPathSeparator()
{
    return '|';
}

void __pmSetProgname(const char */*program*/)
{

}

} // extern "C"
