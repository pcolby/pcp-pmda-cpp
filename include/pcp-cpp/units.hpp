//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_UNITS_HPP__
#define __PCP_CPP_UNITS_HPP__

#include "config.hpp"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

// Wrapper for PMDA_PMUNITS to avoid C++ "extended initializer lists" warnings.
template<typename Type>
inline pmUnits units(Type a, Type b, Type c, Type d, Type e, Type f)
{
    pmUnits units = PMDA_PMUNITS(a,b,c,d,e,f);
    return units;
}

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
