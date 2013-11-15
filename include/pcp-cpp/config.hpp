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

#ifndef __PCP_CPP_CONFIG_HPP__
#define __PCP_CPP_CONFIG_HPP__

// Custom namespace wrapper macros.
#ifdef PCP_CPP_NAMESPACE
#  define PCP_CPP_BEGIN_NAMESPACE namespace PCP_CPP_NAMESPACE {
#  define PCP_CPP_END_NAMESPACE }
#else
#  define PCP_CPP_BEGIN_NAMESPACE
#  define PCP_CPP_END_NAMESPACE
#endif

#endif
