//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Declares various types used throughout the PMDA++ library.
 */

#ifndef __PCP_CPP_TYPES_HPP__
#define __PCP_CPP_TYPES_HPP__

#include "config.hpp"

#include <stdint.h>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

typedef uint_fast8_t  atom_type_type;   // PM_TYPE_* (0 - 9)
typedef uint_fast16_t cluster_id_type;  // __pmID_int::cluster (12-bits)
typedef uint_fast16_t domain_id_type;   // __pmID_int::cluster (9-bits); pmdaIndom::it_indom
typedef unsigned int  instance_id_type; // https://github.com/pcolby/pcp-pmda-cpp/issues/11
typedef uint_fast16_t item_id_type;     // __pmID_int::item (10-bits)
typedef uint_fast8_t  semantic_type;    // PM_SEM_* (0 - 4)

template <typename Type> inline atom_type_type type();

// Integer types.
template <> inline atom_type_type type<int8_t>  () { return PM_TYPE_32; }
template <> inline atom_type_type type<int16_t> () { return PM_TYPE_32; }
template <> inline atom_type_type type<int32_t> () { return PM_TYPE_32; }
template <> inline atom_type_type type<int64_t> () { return PM_TYPE_64; }
template <> inline atom_type_type type<uint8_t> () { return PM_TYPE_U32; }
template <> inline atom_type_type type<uint16_t>() { return PM_TYPE_U32; }
template <> inline atom_type_type type<uint32_t>() { return PM_TYPE_U32; }
template <> inline atom_type_type type<uint64_t>() { return PM_TYPE_U64; }

// Floating point types.
template <> inline atom_type_type type<float> () { return PM_TYPE_FLOAT;  }
template <> inline atom_type_type type<double>() { return PM_TYPE_DOUBLE; }

// String types.
template <> inline atom_type_type type<char *>      () { return PM_TYPE_STRING; }
template <> inline atom_type_type type<std::string> () { return PM_TYPE_STRING; }

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
