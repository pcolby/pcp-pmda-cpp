//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Declares and implements the pcp::units template function.
 */

#ifndef __PCP_CPP_UNITS_HPP__
#define __PCP_CPP_UNITS_HPP__

#include "config.hpp"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

/**
 * @brief Wrapper for PMDA_PMUNITS
 *
 * This allows C++11-style initialisation lists, while avoiding C++ "extended
 * initializer lists" warnings.
 *
 * @param a The first argument to pass to the PMDA_PMUNITS macro.
 * @param b The second argument to pass to the PMDA_PMUNITS macro.
 * @param d The third argument to pass to the PMDA_PMUNITS macro.
 * @param d The fourth argument to pass to the PMDA_PMUNITS macro.
 * @param e The fifth argument to pass to the PMDA_PMUNITS macro.
 * @param f The sixth argument to pass to the PMDA_PMUNITS macro.
 *
 * @return A pmUnits value constructed from the supplied arguments.
 */
template<typename Type>
inline pmUnits units(Type a, Type b, Type c, Type d, Type e, Type f)
{
    const pmUnits units = PMDA_PMUNITS(a,b,c,d,e,f);
    return units;
}

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
