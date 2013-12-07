//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_METRIC_DESCRIPTION_HPP__
#define __PCP_CPP_METRIC_DESCRIPTION_HPP__

#include "config.hpp"
#include "types.hpp"

#include <assert.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

enum metric_flags {
    storable_metric = 0x1
};

inline metric_flags operator|(metric_flags a, metric_flags b)
{
    return static_cast<metric_flags>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

struct metric_description {
    std::string metric_name;
    atom_type_type type;
    semantic_type semantic;
    pmUnits units;
    instance_domain * domain;
    std::string short_description;
    std::string verbose_description;
    void * const opaque;
    metric_flags flags;

    metric_description(const std::string &metric_name,
                       const atom_type_type type,
                       const semantic_type semantic,
                       const pmUnits units,
                       instance_domain * const domain = NULL,
                       const std::string &short_description = std::string(),
                       const std::string &verbose_description = std::string(),
                       void * const opaque = NULL,
                       const metric_flags flags = static_cast<metric_flags>(0))
        : metric_name(metric_name),
          type(type),
          semantic(semantic),
          units(units),
          domain(domain),
          short_description(short_description),
          verbose_description(verbose_description),
          opaque(opaque),
          flags(flags)
    {

    }

    inline operator pmDesc() const
    {
        pmDesc description;
        description.indom = PM_INDOM_NULL;
        description.pmid  = 0;
        description.sem   = semantic;
        description.type  = type;
        description.units = units;
        return description;
    }

};

class metric_cluster : public std::map<item_id_type, metric_description> {

public:
    metric_cluster(const cluster_id_type id,
                   const std::string &name)
        : cluster_id(id),
          cluster_name(name)
    {

    }

    cluster_id_type get_cluster_id() const
    {
        return cluster_id;
    }

    std::string get_cluster_name() const
    {
        return cluster_name;
    }

    metric_cluster& operator()(const item_id_type item_id,
                               const std::string &metric_name,
                               const atom_type_type type,
                               const semantic_type semantic,
                               const pmUnits units,
                               const metric_flags flags = static_cast<metric_flags>(0),
                               instance_domain * const domain = NULL,
                               const std::string &short_description = std::string(),
                               const std::string &verbose_description = std::string(),
                               void * const opaque = NULL)
    {
        insert(value_type(item_id, metric_description(metric_name, type, semantic,
            units, domain, short_description, verbose_description, opaque, flags)));
        return *this;
    }

    metric_cluster& operator()(const item_id_type item_id,
                               const std::string &metric_name,
                               const atom_type_type type,
                               const semantic_type semantic,
                               const pmUnits units,
                               instance_domain * const domain,
                               const metric_flags flags = static_cast<metric_flags>(0),
                               const std::string &short_description = std::string(),
                               const std::string &verbose_description = std::string(),
                               void * const opaque = NULL)
    {
        insert(value_type(item_id, metric_description(metric_name, type, semantic,
            units, domain, short_description, verbose_description, opaque, flags)));
        return *this;
    }

    metric_cluster& operator()(const item_id_type item_id,
                               const std::string &metric_name,
                               const atom_type_type type,
                               const semantic_type semantic,
                               const pmUnits units,
                               instance_domain * const domain = NULL,
                               const std::string &short_description = std::string(),
                               const std::string &verbose_description = std::string(),
                               void * const opaque = NULL,
                               const metric_flags flags = static_cast<metric_flags>(0))
    {
        insert(value_type(item_id, metric_description(metric_name, type, semantic,
            units, domain, short_description, verbose_description, opaque, flags)));
        return *this;
    }

private:
    const cluster_id_type cluster_id;
    const std::string cluster_name;

};

class metrics_description : public std::map<cluster_id_type, metric_cluster> {
    public:
        metrics_description& operator()(const cluster_id_type cluster_id,
                                        const std::string &cluster_name = std::string())
        {
            most_recent_cluster = insert(value_type(cluster_id,
                metric_cluster(cluster_id, cluster_name))).first;
            return *this;
        }

        metrics_description& operator()(const item_id_type item_id,
                                        const std::string &metric_name,
                                        const atom_type_type type,
                                        const semantic_type semantic,
                                        const pmUnits units,
                                        const metric_flags flags,
                                        instance_domain * const domain = NULL,
                                        const std::string &short_description = std::string(),
                                        const std::string &verbose_description = std::string(),
                                        void * const opaque = NULL)
        {
            assert(most_recent_cluster != end());
            most_recent_cluster->second(item_id, metric_name, type, semantic,
                                        units, domain, short_description,
                                        verbose_description, opaque, flags);
            return *this;
        }

        metrics_description& operator()(const item_id_type item_id,
                                        const std::string &metric_name,
                                        const atom_type_type type,
                                        const semantic_type semantic,
                                        const pmUnits units,
                                        instance_domain * const domain,
                                        const metric_flags flags,
                                        const std::string &short_description = std::string(),
                                        const std::string &verbose_description = std::string(),
                                        void * const opaque = NULL)
        {
            assert(most_recent_cluster != end());
            most_recent_cluster->second(item_id, metric_name, type, semantic,
                                        units, domain, short_description,
                                        verbose_description, opaque, flags);
            return *this;
        }

        metrics_description& operator()(const item_id_type item_id,
                                        const std::string &metric_name,
                                        const atom_type_type type,
                                        const semantic_type semantic,
                                        const pmUnits units,
                                        instance_domain * const domain = NULL,
                                        const std::string &short_description = std::string(),
                                        const std::string &verbose_description = std::string(),
                                        void * const opaque = NULL,
                                        const metric_flags flags = static_cast<metric_flags>(0))
        {
            assert(most_recent_cluster != end());
            most_recent_cluster->second(item_id, metric_name, type, semantic,
                                        units, domain, short_description,
                                        verbose_description, opaque, flags);
            return *this;
        }

    private:
        iterator most_recent_cluster;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
