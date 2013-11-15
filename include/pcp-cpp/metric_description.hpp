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

#ifndef __PCP_CPP_METRIC_DESCRIPTION_HPP__
#define __PCP_CPP_METRIC_DESCRIPTION_HPP__

#include "config.hpp"
#include "types.hpp"

#include <pcp/pmapi.h>

#include <assert.h>
#include <map>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

struct metric_description {
    std::string metric_name;
    instance_domain * const metric_domain;
    // dimensions
    std::string short_description;
    std::string verbose_description;
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

    template <typename Type>
    metric_cluster& operator()(const item_id_type item,
                               const std::string &metric_name,
                               const atom_type_type type,
                               const instance_domain * const domain = NULL,
                               // dimensions ...
                               const std::string &short_description = std::string(),
                               const std::string &verbose_description = std::string())
    {
        /// @todo
        return *this;
    }

private:
    const cluster_id_type cluster_id;
    const std::string cluster_name;
    void * const opaque;
};

class metrics_description : public std::map<cluster_id_type, metric_description> {
    public:
        metrics_description& operator()(const cluster_id_type cluster_id,
                                        const std::string &cluster_name,
                                        void * const opaque = NULL)
        {
            most_recent_cluster = insert(std::make_pair(cluster_id,
                metric_cluster(cluster_id, cluster_name, opaque))).first;
        }

        //template <typename Type>
        metrics_description& operator()(const item_id_type item_id,
                                        const std::string &item_name,
                                        const atom_type_type type,
                                        const instance_domain * const domain = NULL,
                                        const std::string &short_description = std::string(),
                                        const std::string &verbose_description = std::string())
        {
            assert(most_recent_cluster != end());
            most_recent_cluster->second(item_id, item_name, type, domain,
                                        short_description, verbose_description);
            return *this;
        }

    private:
        iterator most_recent_cluster;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
