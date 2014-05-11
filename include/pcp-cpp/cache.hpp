//               Copyright Paul Colby 2014.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Defines PMDA cache helper functions.
 */

#ifndef __PCP_CPP_CACHE_HPP__
#define __PCP_CPP_CACHE_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "types.hpp"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {
namespace cache {

template <typename Type>
struct lookup_result_type {
    char * name;
    instance_id_type instance_id;
    Type opaque;
    int status;
};

/// @todo flag type instead of requireActive bool?

template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom,
                                const instance_id_type instance_id,
                                const bool requireActive=true)
{
    lookup_result_type<Type> result;
    void * opaque;
    result.status = pmdaCacheLookup(indom, instance_id, &result.name, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((requireActive) && (result.status != PMDA_CACHE_ACTIVE)) {
        throw pcp::exception(result.status, "not active"); ///< @todo Better message.
    }
    result.instance_id = instance_id;
    result.opaque = static_cast<Type>(opaque);
    return result;
}

template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom, const std::string &name,
                                const bool requireActive=true)
{
    lookup_result_type<Type> result;
    void * opaque;
    result.status = pmdaCacheLookupName(indom, name.c_str(), &result.instance_id, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((requireActive) && (result.status != PMDA_CACHE_ACTIVE)) {
        throw pcp::exception(result.status, "not active"); ///< @todo Better message.
    }
    result.name = NULL;
    result.opaque = static_cast<Type>(opaque);
    return result;
}

template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom, const std::string &name,
                                const std::string &key,
                                const bool requireActive=true)
{
    lookup_result_type<Type> result;
    void * opaque;
    result.status = pmdaCacheLookupKey(indom, name.c_str(), key.size(),
                                       key.c_str(), &result.name,
                                       &result.instance_id, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((requireActive) && (result.status != PMDA_CACHE_ACTIVE)) {
        throw pcp::exception(result.status, "not active"); ///< @todo Better message.
    }
    result.opaque = static_cast<Type>(opaque);
    return result;
}

size_t purge(const pmInDom indom, const time_t recent)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCachePurge(indom, recent);
    return result;
}

#ifndef PCP_CPP_NO_BOOST
size_t purge(const pmInDom indom, const boost::posix_time::time_duration &recent)
{
    return purge(indom, recent.seconds());
}
#endif

/// @todo  flags type
instance_id_type store(const pmInDom indom, const std::string &name, int flags = 0,
                       void ** opaque = NULL)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCacheStore(indom, flags, name.c_str(), opaque);
    return result;
}

/// @todo  flags type
instance_id_type store(const pmInDom indom, const std::string &name,
                       void ** opaque, int flags = 0)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCacheStore(indom, flags, name.c_str(), opaque);
    return result;
}

/// @todo  flags type
instance_id_type store(const pmInDom indom, const std::string &name,
                       const std::string &key, int flags = 0,
                       void ** opaque = NULL)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCacheStoreKey(indom, flags, name.c_str(), key.size(),
                                         key.c_str(), opaque);
    return result;
}

/// @todo  flags type
instance_id_type store(const pmInDom indom, const std::string &name,
                       const std::string &key, void ** opaque, int flags = 0)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCacheStoreKey(indom, flags, name.c_str(), key.size(),
                                         key.c_str(), opaque);
    return result;
}

/// @todo  Sensible name for this function.
int op(const pmInDom indom, int op)
{
    /// @todo  Error handling, of course.
    const int result = pmdaCacheOp(indom, op);
    return result;
}

} } // pcp::cache namespace.

PCP_CPP_END_NAMESPACE

#endif
