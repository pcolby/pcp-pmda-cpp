//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_METRIC_DESCRIPTION_HPP__
#define __PCP_CPP_METRIC_DESCRIPTION_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "types.hpp"

#include <assert.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class instance_domain;

enum metric_flags {
    storable_metric = 0x1 //< Metric supports pmstore operations.
};

inline metric_flags operator|(metric_flags a, metric_flags b)
{
    return static_cast<metric_flags>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

/**
 * @brief Individual metric description.
 */
struct metric_description {
    std::string metric_name;          ///< This metric's name.
    atom_type_type type;              ///< This metric's atom type.
    semantic_type semantic;           ///< This metric's PCP semantic.
    pmUnits units;                    ///< This metric's PCP units.
    instance_domain * domain;         ///< Optional instance domain for this metric.
    std::string short_description;    ///< This metric's short description.
    std::string verbose_description;  ///< This metric's verbose description.
    void * const opaque;              ///< Opaque value to track with this metric.
    metric_flags flags;               ///< Optional flags for this metric.

    /**
     * @brief Constructor
     *
     * @param metric_name         Metric name.
     * @param type                Atom type.
     * @param semantic            PCP semantic.
     * @param units               PCP units.
     * @param domain              Optional instance domain.
     * @param short_description   Short description.
     * @param verbose_description Verbose description.
     * @param opaque              Opaque value to track.
     * @param flags               Optional metric flags.
     */
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

    /**
     * @brief Operator for implicit cast to pmDesc.
     *
     * This operator allows this object to be used wherever PCP functions
     * require a pmDesc instance.
     *
     * @return A pmDesc struct representing this metric description.
     */
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

/**
 * @brief A cluster of metric descriptions.
 */
class metric_cluster : public std::map<item_id_type, metric_description> {

public:

    /**
     * @brief Constructor.
     *
     * @param id   ID for this cluster.
     * @param name Name for this cluster.
     */
    metric_cluster(const cluster_id_type id,
                   const std::string &name)
        : cluster_id(id),
          cluster_name(name)
    {

    }

    /**
     * @brief Get this cluster's ID.
     *
     * @return This cluster's ID.
     */
    cluster_id_type get_cluster_id() const
    {
        return cluster_id;
    }

    /**
     * @brief Get this cluster's name.
     *
     * @return This cluster's name.
     */
    std::string get_cluster_name() const
    {
        return cluster_name;
    }

    /**
     * @brief Metric insertion functor.
     *
     * This functor allows for chained insertion of metrics into this cluster.
     *
     * @param item_id             ID for the metric being inserted.
     * @param metric_name         Metric name.
     * @param type                Atom type.
     * @param semantic            PCP semantic.
     * @param units               PCP units.
     * @param flags               Optional metric flags.
     * @param domain              Optional instance domain.
     * @param short_description   Short description.
     * @param verbose_description Verbose description.
     * @param opaque              Opaque value to track.
     *
     * @return A reference to this metric cluster.
     */
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

    /**
     * @brief Metric insertion functor.
     *
     * This functor allows for chained insertion of metrics into this cluster.
     *
     * @param item_id             ID for the metric being inserted.
     * @param metric_name         Metric name.
     * @param type                Atom type.
     * @param semantic            PCP semantic.
     * @param units               PCP units.
     * @param domain              Optional instance domain.
     * @param flags               Optional metric flags.
     * @param short_description   Short description.
     * @param verbose_description Verbose description.
     * @param opaque              Opaque value to track.
     *
     * @return A reference to this metric cluster.
     */
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

    /**
     * @brief Metric insertion functor.
     *
     * This functor allows for chained insertion of metrics into this cluster.
     *
     * @param item_id             ID for the metric being inserted.
     * @param metric_name         Metric name.
     * @param type                Atom type.
     * @param semantic            PCP semantic.
     * @param units               PCP units.
     * @param domain              Optional instance domain.
     * @param short_description   Short description.
     * @param verbose_description Verbose description.
     * @param opaque              Opaque value to track.
     * @param flags               Optional metric flags.
     *
     * @return A reference to this metric cluster.
     */
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
    const cluster_id_type cluster_id; ///< The ID of this cluster.
    const std::string cluster_name;   ///< The name of this cluster.

};

/**
 * @brief Collection of clusters of metric descriptions.
 */
class metrics_description : public std::map<cluster_id_type, metric_cluster> {

public:

    /**
     * @brief Default constructor.
     *
     * Constructs an empty metrics_description class - ie one with no metric
     * clusters yet.
     */
    explicit metrics_description() :
        std::map<cluster_id_type, metric_cluster>(),
        most_recent_cluster(end())
    {

    }

    /**
     * @brief Cluster insertion functor.
     *
     * This functor inserts a new empty metric cluster into this object. It also
     * records the insertion iterator so that future calls to any of the metric
     * insertion functors add metrics to the new cluster inserted by this
     * function.
     *
     * @param cluster_id   ID of the new cluster to insert.
     * @param cluster_name Optional name of the cluster to insert.
     *
     * @return A reference to this metrics_description object.
     */
    metrics_description& operator()(const cluster_id_type cluster_id,
                                    const std::string &cluster_name = std::string())
    {
        most_recent_cluster = insert(value_type(cluster_id,
            metric_cluster(cluster_id, cluster_name))).first;
        return *this;
    }

    /**
     * @brief Metric description insertion functor.
     *
     * This functor inserts a metric description in the most recently inserted
     * cluster.
     *
     * The cluster insertion function must be called at least once prior to
     * calling this, or any of the other metric description insertion functors,
     * otherwise an exception will be throw.
     *
     * @param item_id             Metric ID.
     * @param metric_name         Metric name.
     * @param type                Metric atom type.
     * @param semantic            PCP metric semantic.
     * @param units               PCP metric units.
     * @param flags               Optional metric flags.
     * @param domain              Optional metric instance domain.
     * @param short_description   Short metric description.
     * @param verbose_description Verbose metric description.
     * @param opaque              Optional opaque pointer to track.
     *
     * @throw pcp::exception If no metric cluster has been inserted yet.
     *
     * @return A reference to this metrics_description object.
     */
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
        if (most_recent_cluster == end()) {
            throw pcp::exception(PM_ERR_GENERIC, "no cluster to add metric to");
        }
        most_recent_cluster->second(item_id, metric_name, type, semantic,
                                    units, domain, short_description,
                                    verbose_description, opaque, flags);
        return *this;
    }

    /**
     * @brief Metric description insertion functor.
     *
     * This functor inserts a metric description in the most recently inserted
     * cluster.
     *
     * The cluster insertion function must be called at least once prior to
     * calling this, or any of the other metric description insertion functors,
     * otherwise an exception will be throw.
     *
     * @param item_id             Metric ID.
     * @param metric_name         Metric name.
     * @param type                Metric atom type.
     * @param semantic            PCP metric semantic.
     * @param units               PCP metric units.
     * @param domain              Optional metric instance domain.
     * @param flags               Optional metric flags.
     * @param short_description   Short metric description.
     * @param verbose_description Verbose metric description.
     * @param opaque              Optional opaque pointer to track.
     *
     * @throw pcp::exception If no metric cluster has been inserted yet.
     *
     * @return A reference to this metrics_description object.
     */
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
        if (most_recent_cluster == end()) {
            throw pcp::exception(PM_ERR_GENERIC, "no cluster to add metric to");
        }
        most_recent_cluster->second(item_id, metric_name, type, semantic,
                                    units, domain, short_description,
                                    verbose_description, opaque, flags);
        return *this;
    }

    /**
     * @brief Metric description insertion functor.
     *
     * This functor inserts a metric description in the most recently inserted
     * cluster.
     *
     * The cluster insertion function must be called at least once prior to
     * calling this, or any of the other metric description insertion functors,
     * otherwise an exception will be throw.
     *
     * @param item_id             Metric ID.
     * @param metric_name         Metric name.
     * @param type                Metric atom type.
     * @param semantic            PCP metric semantic.
     * @param units               PCP metric units.
     * @param domain              Optional metric instance domain.
     * @param short_description   Short metric description.
     * @param verbose_description Verbose metric description.
     * @param opaque              Optional opaque pointer to track.
     * @param flags               Optional metric flags.
     *
     * @throw pcp::exception If no metric cluster has been inserted yet.
     *
     * @return A reference to this metrics_description object.
     */
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
        if (most_recent_cluster == end()) {
            throw pcp::exception(PM_ERR_GENERIC, "no cluster to add metric to");
        }
        most_recent_cluster->second(item_id, metric_name, type, semantic,
                                    units, domain, short_description,
                                    verbose_description, opaque, flags);
        return *this;
    }

private:
    iterator most_recent_cluster; ///< The most-recently inserted cluster.
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
