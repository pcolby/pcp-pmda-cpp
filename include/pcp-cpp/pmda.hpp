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

#ifndef __PCP_CPP_PMDA_HPP__
#define __PCP_CPP_PMDA_HPP__

#include "config.hpp"
#include "metric_description.hpp"

#include <pcp/pmapi.h>
#include <pcp/pmda.h>

#include <stdint.h>
#include <stdlib.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

void set_pmda_callbacks(pmdaInterface &interface);

class pmda {

public:
    typedef int_fast16_t pmda_domain_number_type; /// @see stdpmid

    static pmda * getInstance() {
        return NULL; /// @todo
    }

    /// @brief  Virtual destructor for safe polymorphic destruction.
    virtual ~pmda() { }

    std::string get_pmda_name() const
    {
        return pmda_name;
    }

    virtual pmda_domain_number_type default_pmda_domain_number() = 0;

    pmda_domain_number_type get_pmda_domain_number() const
    {
        return pmda_domain_number;
    }

    void set_pmda_domain_number(const pmda_domain_number_type domain_number)
    {
        pmda_domain_number = domain_number;
    }

    template <class Agent>
    static void init_dso(pmdaInterface * const interface)
    {
        try {
            //Agent::getInstance()->
            /// @todo register handlers
        } catch (...) {
            // set err flag?
        }
    }

    template <class Agent>
    static int run_daemon(const int argc, char * const argv[])
    {
        try {
            //Agent::getInstance(argc, argv);
            /// @todo
            /// parse command line options
            /// if done, return.
            /// initialise pmda.
            /// run pmda
            return EXIT_SUCCESS;
        } catch (...) {
            return EXIT_FAILURE;
        }
    }

protected:
    /// pcp::metric_desc -> description, including cluster id, etc.
    ///// supports one or more?
    /// pcp::metric_index -> cluster, item, instance, opaque.

    virtual void init(pmdaInterface &interface)
    {
        set_pmda_callbacks(interface);
    }

    virtual pcp::metrics_description get_supported_metrics() const = 0;

    virtual void begin_fetch_values() const { }

    virtual pmAtomValue fetch_value(const metric_id &metric) const = 0;

private:
    pmda_domain_number_type pmda_domain_number;
    std::string pmda_name;

    // parseCommandLine
    // ...?

    bool export_pmns_data(/*dir*/) const
    {
        /// @todo
        return false;
    }

public:
    virtual int on_pmda_desc(pmID pmid, pmDesc desc, pmdaExt pmda)
    {
        return 0;
    }

    virtual int on_pmda_fetch(int numpmid, pmID &pmidlist, pmResult *&resp,
                              pmdaExt &pmda)
    {
        return 0;
    }

    virtual int on_pmda_instance(pmInDom indom, int inst, char *name,
                                 __pmInResult *result, pmdaExt &pmda)
    {
        return 0;
    }

    virtual int on_pmda_profile(__pmProfile &prof, pmdaExt &pmda)
    {
        return 0;
    }

    virtual int on_pmda_store(pmResult &result, pmdaExt &pmda)
    {
        return 0;
    }

    virtual int on_pmda_text(int ident, int type, char *&buffer, pmdaExt &pmda)
    {
        return 0;
    }

};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#include "detail/interface.hpp"

#endif
