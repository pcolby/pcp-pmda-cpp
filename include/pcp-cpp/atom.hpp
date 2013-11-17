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

#ifndef __PCP_CPP_ATOM_HPP__
#define __PCP_CPP_ATOM_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "types.hpp"

#include <pcp/pmapi.h>

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
