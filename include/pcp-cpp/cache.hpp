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
#include "types.hpp"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {
namespace cache {

struct result {
    std::string name;
    int flags;
    void * opaque;
};

// Note, due to a limitation in the PMDA API, this cannot return strings with
// NULL bytes.
std::string lookup(const pmInDom indom, const instance_id_type instance_id,
                   void ** opaque = NULL)
{
    char * name;
    /// @todo  Error handling, of course.
    pmdaCacheLookup(indom, instance_id, &name, opaque);
    return name;
}

instance_id_type lookup(const pmInDom indom, const std::string &instance_name,
                        void ** opaque = NULL)
{
    /// @todo  Error handling, of course.
    int instance_id;
    pmdaCacheLookupName(indom, instance_name.c_str(), &instance_id, opaque);
    return instance_id;
}

instance_id_type lookup(const pmInDom indom, const std::string &instance_name,
                        const std::string &key, char ** oname = NULL,
                        void ** opaque = NULL)
{
    /// @todo  Error handling, of course.
    int instance_id;
    pmdaCacheLookupKey(indom, instance_name.c_str(), key.size(), key.c_str(),
                       oname, &instance_id, opaque);
    return instance_id;
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

//store;

//op;

} } // pcp::cache namespace.

PCP_CPP_END_NAMESPACE

#endif
