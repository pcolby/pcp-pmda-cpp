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

#include <sstream>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {
namespace cache {

/**
 * @brief Flags that may be applied to cache lookups.
 */
enum lookup_flags {
    require_active = 0x1 ///< Throw if the found cache entry is not active.
};

/**
 * @brief Pipe operator for combining lookup_flags values.
 *
 * This function performs a logical OR of two lookup_flags sets. This is a
 * convenience function, allowing lookup_flags enum values to be used both
 * standalone, and in combination.
 *
 * @param a First set of flags.
 * @param b Second set of flags.
 *
 * @return Combined set of both \a a and \a b flags.
 */
inline lookup_flags operator|(lookup_flags a, lookup_flags b)
{
    return static_cast<lookup_flags>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

/**
 * @brief Structure for return results from pcp::cache::lookup functions.
 *
 * @tparam Type Type to cast opaque pointers to.
 */
template <typename Type>
struct lookup_result_type {
    char * name;                  ///< Instance name.
    instance_id_type instance_id; ///< Instance identifier.
    Type opaque;                  ///< Opaque pointer, cast to type \a Type.
    int status;                   ///< Entry status, such as PMDA_CACHE_ACTIVE.
};

/**
 * @brief Lookup a cache entry, by instance ID.
 *
 * If \a flags includes \a require_active, and an inactive cache entry is found,
 * a pcp::exception will be thrown instead of the inactive entry returned.
 *
 * @tparam Type        Type to cast opaque pointers to.
 *
 * @param  indom       Instance domain to lookup.
 * @param  instance_id Instance ID to lookup.
 * @param  flags       Optional flags that alter the function's behaviour.
 *
 * @throw  pcp::exception  If no cache entry entry found.
 *
 * @return A struct containing the found cache entry details.
 *
 * @see pmdaCacheLookup
 */
template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom,
                                const instance_id_type instance_id,
                                const lookup_flags flags = require_active)
{
    lookup_result_type<Type> result;
    void * opaque;
    result.name = NULL;
    result.status = pmdaCacheLookup(indom, instance_id, &result.name, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((flags & require_active) && (result.status != PMDA_CACHE_ACTIVE)) {
        std::ostringstream message;
        message << "Cache entry " << indom << ':' << instance_id << " (";
        if (result.name == NULL) {
            message << "NULL";
        } else {
            message << '"' << result.name << '"';
        }
        message << ") inactive";
        throw pcp::exception(result.status, message.str());
    }
    result.instance_id = instance_id;
    result.opaque = static_cast<Type>(opaque);
    return result;
}

/**
 * @brief Lookup a cache entry, by instance name.
 *
 * If \a flags includes \a require_active, and an inactive cache entry is found,
 * a pcp::exception will be thrown instead of the inactive entry returned.
 *
 * @tparam Type        Type to cast opaque pointers to.
 *
 * @param  indom       Instance domain to lookup.
 * @param  name        Instance name to lookup.
 * @param  flags       Optional flags that alter the function's behaviour.
 *
 * @throw  pcp::exception  If no cache entry entry found.
 *
 * @return A struct containing the found cache entry details.
 *
 * @see pmdaCacheLookupName
 */
template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom, const std::string &name,
                                const lookup_flags flags = require_active)
{
    lookup_result_type<Type> result;
    void * opaque;
    int instance_id;
    result.status = pmdaCacheLookupName(indom, name.c_str(), &instance_id, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((flags & require_active) && (result.status != PMDA_CACHE_ACTIVE)) {
        std::ostringstream message;
        message << "Cache entry " << indom << ':' << result.instance_id
                << " (\"" << name << "\") inactive";
        throw pcp::exception(result.status, message.str());
    }
    result.name = NULL;
    result.instance_id = instance_id;
    result.opaque = static_cast<Type>(opaque);
    return result;
}

/**
 * @brief Lookup a cache entry, by instance name and key.
 *
 * If \a flags includes \a require_active, and an inactive cache entry is found,
 * a pcp::exception will be thrown instead of the inactive entry returned.
 *
 * @tparam Type        Type to cast opaque pointers to.
 *
 * @param  indom       Instance domain to lookup.
 * @param  name        Instance name to lookup.
 * @param  key         Instance key to lookup.
 * @param  flags       Optional flags that alter the function's behaviour.
 *
 * @throw  pcp::exception  If no cache entry entry found.
 *
 * @return A struct containing the found cache entry details.
 *
 * @see pmdaCacheLookupKey
 */
template <typename Type>
lookup_result_type<Type> lookup(const pmInDom indom, const std::string &name,
                                const std::string &key,
                                const lookup_flags flags = require_active)
{
    lookup_result_type<Type> result;
    void * opaque;
    int instance_id;
    result.status = pmdaCacheLookupKey(indom, name.c_str(), key.size(),
                                       key.c_str(), &result.name,
                                       &instance_id, &opaque);
    if (result.status < 0) {
        throw pcp::exception(result.status);
    }
    if ((flags & require_active) && (result.status != PMDA_CACHE_ACTIVE)) {
        std::ostringstream message;
        message << "Cache entry " << indom << ':' << result.instance_id
                << " (\"" << name << "\":\"" << key << "\") inactive";
        throw pcp::exception(result.status, message.str());
    }
    result.instance_id = instance_id;
    result.opaque = static_cast<Type>(opaque);
    return result;
}

/**
 * @brief Perform additional operations on the cache.
 *
 * This is a very simple wrapper for PCP's pmdaCacheOp function.
 *
 * @param  indom      Instance domain to operate on.
 * @param  operation  Operation to perform.  Should be one of the PMDA_CACHE_*
 *                    constants.
 *
 * @throw  pcp::exception  On errors.
 *
 * @return The result of the pmdaCacheOp function.  See pmdaCacheOp for details.
 *
 * @see pmdaCacheOp
 */
int perform(const pmInDom indom, const int operation)
{
    const int result = pmdaCacheOp(indom, operation);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

/**
 * @brief Purge cache entries that have not been active for some time.
 *
 * @param  indom   Instance domain to purge entries for.
 * @param  recent  All entries that have not been active within this many
 *                 seconds will be purged.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The number of items purged.
 *
 * @see pmdaCachePurge
 */
size_t purge(const pmInDom indom, const time_t recent)
{
    const int result = pmdaCachePurge(indom, recent);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

#ifndef PCP_CPP_NO_BOOST
/**
 * @brief Purge cache entries that have not been active for some time.
 *
 * @param  indom   Instance domain to purge entries for.
 * @param  recent  All entries that have not been active within this interval
 *                 will be purged.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The number of items purged.
 *
 * @see pmdaCachePurge
 */
size_t purge(const pmInDom indom, const boost::posix_time::time_duration &recent)
{
    return purge(indom, recent.total_seconds());
}
#endif

/**
 * @brief Add a item to the cache.
 *
 * @param  indom   Instance domain to add an entry for.
 * @param  name    Instance name to add to the cache.
 * @param  flags   Optional flags to be passed to pmdaCacheStore.
 * @param  opqaue  Optional opaque pointer to be include in the cache entry.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The instance ID of the stored cache entry.
 *
 * @see pmdaCacheStore
 */
instance_id_type store(const pmInDom indom, const std::string &name,
                       const int flags = PMDA_CACHE_ADD,
                       void * const opaque = NULL)
{
    const int result = pmdaCacheStore(indom, flags, name.c_str(), opaque);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

/**
 * @brief Add a item to the cache.
 *
 * @param  indom   Instance domain to add an entry for.
 * @param  name    Instance name to add to the cache.
 * @param  opqaue  Optional opaque pointer to be include in the cache entry.
 * @param  flags   Optional flags to be passed to pmdaCacheStore.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The instance ID of the stored cache entry.
 *
 * @see pmdaCacheStore
 */
instance_id_type store(const pmInDom indom, const std::string &name,
                       void * const opaque, const int flags = PMDA_CACHE_ADD)
{
    const int result = pmdaCacheStore(indom, flags, name.c_str(), opaque);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

/**
 * @brief Add a item to the cache.
 *
 * @param  indom   Instance domain to add an entry for.
 * @param  key     Hint to pass to pmdaCacheStoreKey.  See pmdaCacheStoreKey for
 *                 details.
 * @param  name    Instance name to add to the cache.
 * @param  flags   Optional flags to be passed to pmdaCacheStore.
 * @param  opaque  Optional opaque pointer to be include in the cache entry.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The instance ID of the stored cache entry.
 *
 * @see pmdaCacheStoreKey
 */
instance_id_type store(const pmInDom indom, const std::string &name,
                       const std::string &key, const int flags = 0,
                       void * const opaque = NULL)
{
    const int result = pmdaCacheStoreKey(indom, flags, name.c_str(), key.size(),
                                         key.c_str(), opaque);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

/**
 * @brief Add a item to the cache.
 *
 * @param  indom   Instance domain to add an entry for.
 * @param  key     Hint to pass to pmdaCacheStoreKey.  See pmdaCacheStoreKey for
 *                 details.
 * @param  name    Instance name to add to the cache.
 * @param  opaque  Optional opaque pointer to be include in the cache entry.
 * @param  flags   Optional flags to be passed to pmdaCacheStore.
 *
 * @throw  pcp::exception  On error.
 *
 * @return The instance ID of the stored cache entry.
 *
 * @see pmdaCacheStoreKey
 */
instance_id_type store(const pmInDom indom, const std::string &name,
                       const std::string &key, void * const opaque,
                       const int flags = 0)
{
    const int result = pmdaCacheStoreKey(indom, flags, name.c_str(), key.size(),
                                         key.c_str(), opaque);
    if (result < 0) {
        throw pcp::exception(result);
    }
    return result;
}

} } // pcp::cache namespace.

PCP_CPP_END_NAMESPACE

#endif
