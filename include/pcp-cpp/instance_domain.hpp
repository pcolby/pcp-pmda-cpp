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

/**
 * @brief Basic instance domain information.
 */
struct instance_info {
    std::string instance_name;       ///< Instance domain name.
    std::string short_description;   ///< Instance domain short description.
    std::string verbose_description; ///< Instance domain verbose description.
};

/**
 * @brief Performance metric instance domain.
 */
class instance_domain : public std::map<instance_id_type, const instance_info> {

public:

    /**
     * @brief  Get this instance domain's user-defined ID.
     *
     * @return This instance domain's user-defined ID.
     *
     * @see set_domain_id
     */
    domain_id_type get_domain_id() const
    {
        return domain_id;
    }

    /**
     * @brief  Get this instance domain's PCP-modifed ID.
     *
     * This function is used to track the instance ID as modifed by PCP's
     * pmdaInit function.  This is essentially the same as the ID returned by
     * get_domain_id, with a number of higher bits set to PCP-internal values.
     *
     * This can be necessary for passing this instance domain's ID to some other
     * PCP built-in functions.
     *
     * @return This instance domain's PCP-modifed ID.
     *
     * @see set_pm_instance_domain
     * @see pmdaInit
     */
    pmInDom get_pm_instance_domain() const
    {
        return pm_instance_domain;
    }

    /**
     * @brief Set this instance domain's user-defined ID.
     *
     * @param id The domiain ID to set.
     */
    void set_domain_id(const domain_id_type id)
    {
        domain_id = id;
    }

    /**
     * @brief Set this instance domain's PCP-modified ID.
     *
     * This normally used by the pcp::pmda class to update the domain ID after
     * invoking pmdaInit.
     *
     * @param domain The domiain ID to set.
     */
    void set_pm_instance_domain(const pmInDom domain)
    {
        pm_instance_domain = domain;
    }

    /**
     * @brief Functor for setting this instance's domain ID.
     *
     * This functor is equivalent to the set_domain_id function, but allows for
     * convenient functor chaining.
     *
     * @param domain_id ID for the instance domain.
     *
     * @return A reference to this instance domain.
     */
    instance_domain& operator()(const domain_id_type domain_id)
    {
        set_domain_id(domain_id);
        set_pm_instance_domain(PM_INDOM_NULL);
        return *this;
    }

    /**
     * @brief Instance insertion functor.
     *
     * This functor allows for convenient insertion of instances into this
     * domain.
     *
     * @param instance_id ID for the instance to insert.
     * @param info        Basic instance information.
     *
     * @return A reference to this instance domain.
     */
    instance_domain& operator()(const instance_id_type instance_id,
                                const instance_info &info)
    {
        insert(value_type(instance_id, info));
        return *this;
    }

    /**
     * @brief Instance insertion functor.
     *
     * This functor allows for convenient insertion of instances into this
     * domain.
     *
     * @param instance_id         ID for the instance to insert.
     * @param instance_name       Name of the instance to insert.
     * @param short_description   Short description of the instance to insert.
     * @param verbose_description Verbose description of the instance to insert.
     *
     * @return A reference to this instance domain.
     */
    instance_domain& operator()(const instance_id_type instance_id,
                                const std::string &instance_name,
                                const std::string &short_description = std::string(),
                                const std::string &verbose_description = std::string())
    {
        instance_info info;
        info.instance_name = instance_name;
        info.short_description = short_description;
        info.verbose_description = verbose_description;
        insert(value_type(instance_id, info));
        return *this;
    }

    /**
     * @brief Operator for implicit cast to pmInDom.
     *
     * This operator allows this object to be used wherever PCP functions
     * require a pmInDom instance.  It is equivalent to calling the
     * get_pm_instance_domain function.
     *
     * @return This instance's PCP-modified ID.
     */
    inline operator pmInDom() const
    {
        return pm_instance_domain;
    }

private:
    domain_id_type domain_id;   ///< User-defined ID for this instance.
    pmInDom pm_instance_domain; ///< PCP modified ID for this instance.
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
