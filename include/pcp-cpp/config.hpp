//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_CONFIG_HPP__
#define __PCP_CPP_CONFIG_HPP__

// Boost headers that depend on boost/cstdint.hpp must be included before any
// PCP headers, because pcp/config.h sets a number of macros like ULONGLONG_MAX
// which fool Boost into thinking we're on an unknown, non-standard platform.
#ifndef PCP_CPP_NO_BOOST
#include <boost/program_options.hpp>
#endif

#include <pcp/pmapi.h>
#include <pcp/pmda.h>

// PMDA interface version to use; defaults to "latest".
#ifndef PCP_CPP_PMDA_INTERFACE_VERSION
#define PCP_CPP_PMDA_INTERFACE_VERSION PMDA_INTERFACE_LATEST
#endif

// Custom namespace wrapper macros.
#ifdef PCP_CPP_NAMESPACE
#  define PCP_CPP_BEGIN_NAMESPACE namespace PCP_CPP_NAMESPACE {
#  define PCP_CPP_END_NAMESPACE }
#else
#  define PCP_CPP_BEGIN_NAMESPACE
#  define PCP_CPP_END_NAMESPACE
#endif

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

template<typename Type>
inline Type get_pcp_runtime_version()
{
    const char * const str = pmGetConfig("PCP_VERSION");
    const char * const pos[2] = { strchr(str, '.'), strrchr(str, '.') };
    return (pos[0] == pos[1]) ? 0 :
        (strtoul(str, NULL, 10) << 16) +
        (strtoul(pos[0]+1, NULL, 10) << 8) +
         strtoul(pos[1]+1, NULL, 10);
}

template<>
inline char * get_pcp_runtime_version()
{
    return pmGetConfig("PCP_VERSION");
}

} // namespace pcp.

PCP_CPP_END_NAMESPACE

#endif
