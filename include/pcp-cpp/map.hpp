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
