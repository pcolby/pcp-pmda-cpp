/*
    Copyright 2013 Paul Colby

    This file is part of pmda-cpp.

    Pmda-cpp is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Pmda-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
    more details.

    You should have received a copy of the GNU Lesser General Public License
    along with pmda-cpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PCP_CPP_TYPES_HPP__
#define __PCP_CPP_TYPES_HPP__

#include "config.hpp"

#include <pcp/pmapi.h>

#include <stdint.h>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

typedef uint_fast8_t atom_type_type;   // PM_TYPE_* (0 - 9)
typedef int_fast16_t cluster_id_type;  // __pmID_int::cluster (12-bits)
typedef int_fast16_t domain_id_type;   // __pmID_int::cluster (9-bits)
typedef int          instance_id_type; // pmdaInstid::i_inst
typedef int_fast16_t item_id_type;     // __pmID_int::item (10-bits)
typedef uint_fast8_t semantic_type;    // PM_SEM_* (0 - 4)


/// @todo include following info in docs.
//typedef pmInDom domain_id_type;         // pmdaIndom::it_indom

template <typename Type> uint_fast8_t type();
template <int8_t  > atom_type_type type() { return PM_TYPE_32; }
template <int16_t > atom_type_type type() { return PM_TYPE_32; }
template <int32_t > atom_type_type type() { return PM_TYPE_32; }
template <int64_t > atom_type_type type() { return PM_TYPE_64; }
template <uint8_t > atom_type_type type() { return PM_TYPE_U32; }
template <uint16_t> atom_type_type type() { return PM_TYPE_U32; }
template <uint32_t> atom_type_type type() { return PM_TYPE_U32; }
template <uint64_t> atom_type_type type() { return PM_TYPE_U64; }

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
