//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_MAP_HPP__
#define __PCP_CPP_MAP_HPP__

#include "config.hpp"

/// @todo

#if __cplusplus >= 201103L
#include <unordered_map>
typedef std::unordered_map pcp::map;
#else
#include <map>
typedef std::map pcp::map;
#endif

//} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
