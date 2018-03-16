//            Copyright Paul Colby 2013 - 2018.
//            Copyright Red Hat 2018.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Sets up common PMDA++ library macros.
 */

#ifndef __PCP_CPP_CONFIG_HPP__
#define __PCP_CPP_CONFIG_HPP__

/**
 * @def PCP_CPP_BOOST_PO_IMPLICIT_VALUE
 *
 * @brief Sets a command line option's implicit value, if supported by the
 *        version of Boost.Program_Options being built against.
 */

/**
 * @def PCP_CPP_BOOST_PO_VALUE_NAME
 *
 * @brief Sets a command line option's value name, if supported by the version
 *        of Boost.Program_Options being built against.
 *
 * @param name Option's value name
 */

// Boost headers that depend on boost/cstdint.hpp must be included before any
// PCP headers, because pcp/config.h sets a number of macros like ULONGLONG_MAX
// which fool Boost into thinking we're on an unknown, non-standard platform.
#ifndef PCP_CPP_NO_BOOST
#  include <boost/date_time/posix_time/posix_time_duration.hpp>
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

// Note, the order in which the PCP headers are included matters for older PCP
// versions; don't reorder them without testing against older versions of PCP.

#include <pcp/pmapi.h>

// PM_VERSION macro was not added until PCP 3.10.5.
#ifndef PM_VERSION
#define PM_VERSION(a,b,c) (((a)<<16)|((b)<<8)|(c))
#endif

// After PCP 4.0.0, we no longer need impl.h (indeed, its empty now anyway).
#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(4,0,0)
#include <pcp/impl.h>
#endif

#include <pcp/pmda.h>

#ifndef PM_TEXT_PMID
#define PM_TEXT_PMID	4
#define PM_TEXT_INDOM	8
#define PM_TEXT_DIRECT	16
#endif

// PCP 4.0.0 cleaned and promoted some functions, renaming them in the process.
#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(4,0,0)
#define pmID_cluster(pmid) pmid_cluster(pmid)
#define pmID_item(pmid) pmid_item(pmid)
#define pmInResult __pmInResult
#define pmNotifyErr __pmNotifyErr
#define pmPathSeparator __pmPathSeparator
#define pmProfile __pmProfile
#define pmSetProgname(program) __pmSetProgname(program)
#endif

/// PMDA interface version to use; defaults to "latest".
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

/**
 * @brief Get the PCP runtime version as a numeric value.
 * @returns The PCP runtime version as a numeric value.
 */
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

/**
 * @brief Get the PCP runtime version as a string.
 * @returns The PCP runtime version as a string.
 */
template<>
inline char * get_pcp_runtime_version()
{
    return pmGetConfig("PCP_VERSION");
}

} // namespace pcp.

PCP_CPP_END_NAMESPACE

/**
 * @brief Let the compiler know that a parameter is unsed.
 *
 * @param name The name of the unused parameter.
 */
#define PCP_CPP_UNUSED(name) (void)name;

#endif
