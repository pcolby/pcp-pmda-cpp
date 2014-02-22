//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Defines the pcp::atom template class.
 */

#ifndef __PCP_CPP_ATOM_HPP__
#define __PCP_CPP_ATOM_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "types.hpp"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

template <typename ValueType>
pmAtomValue atom(const atom_type_type type, ValueType value)
{
    pmAtomValue atom;
    switch (type) {
        case PM_TYPE_32:               atom.l   = static_cast< int32_t>(value); break;
        case PM_TYPE_U32:              atom.ul  = static_cast<uint32_t>(value); break;
        case PM_TYPE_64:               atom.ll  = static_cast< int64_t>(value); break;
        case PM_TYPE_U64:              atom.ull = static_cast<uint64_t>(value); break;
        case PM_TYPE_FLOAT:            atom.f   = static_cast<float   >(value); break;
        case PM_TYPE_DOUBLE:           atom.d   = static_cast<double  >(value); break;
      //case PM_TYPE_STRING:           atom.cp  = value; break;
      //case PM_TYPE_AGGREGATE:        atom.vbp = value; break;
      //case PM_TYPE_AGGREGATE_STATIC: atom.vbp = value; break;
      //case PM_TYPE_EVENT:            atom.vbp = value; break;
        default:
            throw pcp::exception(PM_ERR_TYPE);
    }
    return atom;
}

// Note, we do not provide a <const char *> template instantiation, since that
// would require us to const_cast the value, which is something the caller
// ought to be very clear about / aware is happening. Hence, if you want to use
// a const char *, use: `pcp::atom(PM_TYPE_STRING, const_cast<char *>(value)`

template <>
pmAtomValue atom<char *>(const atom_type_type type, char * value)
{
    pmAtomValue atom;
    switch (type) {
      //case PM_TYPE_32:               atom.l   = value; break;
      //case PM_TYPE_U32:              atom.ul  = value; break;
      //case PM_TYPE_64:               atom.ll  = value; break;
      //case PM_TYPE_U64:              atom.ull = value; break;
      //case PM_TYPE_FLOAT:            atom.f   = value; break;
      //case PM_TYPE_DOUBLE:           atom.d   = value; break;
        case PM_TYPE_STRING:           atom.cp  = value; break;
      //case PM_TYPE_AGGREGATE:        atom.vbp = value; break;
      //case PM_TYPE_AGGREGATE_STATIC: atom.vbp = value; break;
      //case PM_TYPE_EVENT:            atom.vbp = value; break;
        default:
            throw pcp::exception(PM_ERR_TYPE);
    }
    return atom;
}

template <>
pmAtomValue atom<pmValueBlock *>(const atom_type_type type, pmValueBlock * value)
{
    pmAtomValue atom;
    switch (type) {
      //case PM_TYPE_32:               atom.l   = value; break;
      //case PM_TYPE_U32:              atom.ul  = value; break;
      //case PM_TYPE_64:               atom.ll  = value; break;
      //case PM_TYPE_U64:              atom.ull = value; break;
      //case PM_TYPE_FLOAT:            atom.f   = value; break;
      //case PM_TYPE_DOUBLE:           atom.d   = value; break;
      //case PM_TYPE_STRING:           atom.cp  = value; break;
        case PM_TYPE_AGGREGATE:        atom.vbp = value; break;
        case PM_TYPE_AGGREGATE_STATIC: atom.vbp = value; break;
        case PM_TYPE_EVENT:            atom.vbp = value; break;
        default:
            throw pcp::exception(PM_ERR_TYPE);
    }
    return atom;
}

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
