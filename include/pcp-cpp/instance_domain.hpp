//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_INSTANCE_DOMAIN_HPP__
#define __PCP_CPP_INSTANCE_DOMAIN_HPP__

#include "config.hpp"
#include "types.hpp"

#include <map>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain : public std::map<instance_id_type, std::string> {

public:
    domain_id_type get_domain_id() const
    {
        return domain_id;
    }

    pmInDom get_pm_instance_domain() const
    {
        return pm_instance_domain;
    }

    void set_domain_id(const domain_id_type id)
    {
        domain_id = id;
    }

    void set_pm_instance_domain(const pmInDom domain)
    {
        pm_instance_domain = domain;
    }

    instance_domain& operator()(const domain_id_type domain_id)
    {
        set_domain_id(domain_id);
        return *this;
    }

    instance_domain& operator()(const instance_id_type instance_id,
                                const std::string &instance_name)
    {
        insert(value_type(instance_id, instance_name));
        return *this;
    }

    inline operator pmInDom() const
    {
        return pm_instance_domain;
    }

private:
    domain_id_type domain_id;
    pmInDom pm_instance_domain;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
