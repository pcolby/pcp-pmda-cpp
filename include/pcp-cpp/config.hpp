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
#  include <boost/program_options.hpp>
// boost::program_options::typed_value::implicit_value was added in 1.35.0.
#  if BOOST_VERSION > 103500
#    define PCP_CPP_BOOST_PO_IMPLICIT_VALUE(...) ->implicit_value(__VA_ARGS__)
#  else
#    define PCP_CPP_BOOST_PO_IMPLICIT_VALUE(...)
#  endif
// boost::program_options::typed_value::value_name was added in 1.50.0.
#  if BOOST_VERSION > 105000
#    define PCP_CPP_BOOST_PO_VALUE_NAME(name)    ->value_name(name)
#  else
#    define PCP_CPP_BOOST_PO_VALUE_NAME(name)
#  endif
#endif

#include <pcp/pmapi.h> // Note, the order in which these are included matters
#include <pcp/impl.h>  // more for older versions of PCP, so don't reorder them
#include <pcp/pmda.h>  // without testing against older versions of PCP.

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
