//            Copyright Paul Colby 2013 - 2015.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Defines the pcp::units template function.
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
 * @param a The first argument to pass to the PMDA_PMUNITS macro (dimTime).
 * @param b The second argument to pass to the PMDA_PMUNITS macro (dimSpace).
 * @param c The third argument to pass to the PMDA_PMUNITS macro (dimCount).
 * @param d The fourth argument to pass to the PMDA_PMUNITS macro (scaleSpace).
 * @param e The fifth argument to pass to the PMDA_PMUNITS macro (scaleTime).
 * @param f The sixth argument to pass to the PMDA_PMUNITS macro (scaleCount).
 *
 * @return A pmUnits value constructed from the supplied arguments.
 */
inline pmUnits units(int a, int b, int c, unsigned d, unsigned e, int f)
{
    const pmUnits units = PMDA_PMUNITS(a,b,c,d,e,f);
    return units;
}

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
