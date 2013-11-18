//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_METRIC_DESCRIPTION_HPP__
#define __PCP_CPP_METRIC_DESCRIPTION_HPP__

#include "config.hpp"
#include "types.hpp"

#include <pcp/pmapi.h>

#include <assert.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

struct metric_description {
    std::string metric_name;
    atom_type_type type;
    semantic_type semantic;
    pmUnits units;
    const instance_domain * domain;
    std::string short_description;
    std::string verbose_description;

    metric_description(const std::string &metric_name,
                       const atom_type_type type,
                       const semantic_type semantic,
                       const pmUnits units,
                       const instance_domain * const domain = NULL,
                       const std::string &short_description = std::string(),
                       const std::string &verbose_description = std::string())
        : metric_name(metric_name),
          type(type),
          semantic(semantic),
          units(units),
          domain(domain),
          short_description(short_description),
          verbose_description(verbose_description)
    {

    }
};

class metric_cluster : public std::map<item_id_type, metric_description> {

public:
    metric_cluster(const cluster_id_type id,
                   const std::string &name,
                   void * const opaque = NULL)
        : cluster_id(id),
          cluster_name(name),
          opaque(opaque)
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

    void * get_opaque_data() const
    {
        return opaque;
    }

    metric_cluster& operator()(const item_id_type item_id,
                               const std::string &metric_name,
                               const atom_type_type type,
                               const semantic_type semantic,
                               const pmUnits units,
                               const instance_domain * const domain = NULL,
                               const std::string &short_description = std::string(),
                               const std::string &verbose_description = std::string())
    {
        insert(std::make_pair(item_id, metric_description(metric_name, type,
            semantic, units, domain, short_description, verbose_description)));
        return *this;
    }

private:
    const cluster_id_type cluster_id;
    const std::string cluster_name;
    void * const opaque;
};

class metrics_description : public std::map<cluster_id_type, metric_cluster> {
    public:
        metrics_description& operator()(const cluster_id_type cluster_id,
                                        const std::string &cluster_name,
                                        void * const opaque = NULL)
        {
            most_recent_cluster = insert(std::make_pair(cluster_id,
                metric_cluster(cluster_id, cluster_name, opaque))).first;
        }

        metrics_description& operator()(const item_id_type item_id,
                                        const std::string &metric_name,
                                        const atom_type_type type,
                                        const semantic_type semantic,
                                        const pmUnits units,
                                        const instance_domain * const domain = NULL,
                                        const std::string &short_description = std::string(),
                                        const std::string &verbose_description = std::string())
        {
            assert(most_recent_cluster != end());
            most_recent_cluster->second(item_id, metric_name, type, semantic, units,
                                        domain, short_description, verbose_description);
            return *this;
        }

    private:
        iterator most_recent_cluster;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
