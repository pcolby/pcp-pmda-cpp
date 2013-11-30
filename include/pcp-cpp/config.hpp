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

// IPv6 support was added to PCP in version 3.8.1. Unfortunately, there's
// currently no reliable way to determine the PCP API version at the moment.
// Even the PCP_VERSION macro, which is not actually supplied by PCP headers,
// is set to an unusable format like "3.8.1" (which C/C++ preprocessor condition
// statements cannot evaluate meaningfully). However, in the same 3.8.1 release
// the PDU_FLAG_CREDS_REQD macro was added to impl.h, so here we default to
// enabling IPv6 support if this macro is present.
#if defined PDU_FLAG_CREDS_REQD && !defined PCP_CPP_ENABLE_IPV6_SUPPORT
#define PCP_CPP_ENABLE_IPV6_SUPPORT
#endif

#endif
