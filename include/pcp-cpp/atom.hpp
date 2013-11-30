//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

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
        case PM_TYPE_32:               atom.l   = value; break;
        case PM_TYPE_U32:              atom.ul  = value; break;
        case PM_TYPE_64:               atom.ll  = value; break;
        case PM_TYPE_U64:              atom.ull = value; break;
        case PM_TYPE_FLOAT:            atom.f   = value; break;
        case PM_TYPE_DOUBLE:           atom.d   = value; break;
      //case PM_TYPE_STRING:           atom.cp  = value; break;
      //case PM_TYPE_AGGREGATE:        atom.vbp = value; break;
      //case PM_TYPE_AGGREGATE_STATIC: atom.vbp = value; break;
      //case PM_TYPE_EVENT:            atom.vbp = value; break;
        default:
            throw pcp::exception(PM_ERR_TYPE);
    }
    return atom;
}

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
