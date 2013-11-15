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

#ifndef __PCP_CPP_INSTANCE_DOMAIN_HPP__
#define __PCP_CPP_INSTANCE_DOMAIN_HPP__

#include "config.hpp"

#include <pcp/pmapi.h>

#include <map>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain : public std::map<instance_id_type, std::string> {

public:
    domain_id_type get_domain_id() const
    {
        return domain_id;
    }

    void set_domain_id(const domain_id_type id)
    {
        domain_id = id;
    }

    instance_domain& operator()(const domain_id_type domain_id)
    {
        set_domain_id(domain_id);
        return *this;
    }

    instance_domain& operator()(const instance_id_type instance_id,
                                const std::string &instance_name)
    {
        insert(std::make_pair(instance_id, instance_name));
        return *this;
    }

private:
    domain_id_type domain_id;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
