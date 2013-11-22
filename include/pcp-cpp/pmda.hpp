//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_PMDA_HPP__
#define __PCP_CPP_PMDA_HPP__

#include "config.hpp"
#include "metric_description.hpp"

#include <pcp/pmda.h>

#include <stdint.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

void set_pmda_callbacks(pmdaInterface &interface);

class pmda {

public:
    typedef int_fast16_t pmda_domain_number_type; /// @see stdpmid

    typedef struct {
        cluster_id_type cluster;
        item_id_type item;
        instance_id_type instance;
        atom_type_type type;
    } metric_id;

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
        register_callbacks(interface);
    }

    virtual pcp::metrics_description get_supported_metrics() const = 0;

    virtual void begin_fetch_values() const { }

    virtual pmAtomValue fetch_value(const metric_id &metric) const = 0;

    /* Virtual PMDA callback functions below here. You probably don't
     * want to override any of these, but you can if you want to. */

    virtual int on_pmda_attribute(int ctx, int attr, const char *value,
                                  int length, pmdaExt *pmda)
    {
        return 0;
    }

    virtual int on_pmda_children(const char *name, int traverse, char ***kids,
                                 int **sts, pmdaExt *pmda)
    {
        return 0;
    }

    virtual int on_pmda_desc(pmID pmid, pmDesc desc, pmdaExt pmda)
    {
        return 0;
    }

    virtual int on_pmda_fetch(int numpmid, pmID &pmidlist, pmResult *&resp,
                              pmdaExt &pmda)
    {
        begin_fetch_values();
        return 0;
    }

    virtual int on_pmda_instance(pmInDom indom, int inst, char *name,
                                 __pmInResult *result, pmdaExt &pmda)
    {
        return 0;
    }

    virtual int on_pmda_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return 0;
    }

    virtual int on_pmda_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
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

    virtual void register_callbacks(pmdaInterface &interface)
    {
        interface.version.any.profile = &pmda_profile_callback;
        interface.version.any.fetch = &pmda_fetch_callback;
        interface.version.any.desc = &pmda_desc_callback;
        interface.version.any.instance = &pmda_instance_callback;
        interface.version.any.text = &pmda_text_callback;
        interface.version.any.store = &pmda_store_callback;
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 5
        interface.version.five.pmid = &pmda_pmid_callback;
        interface.version.five.name = &pmda_name_callback;
        interface.version.five.children = &pmda_children_callback;
        #endif
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
        interface.version.six.attribute = &pmda_attribute_callback;
        #endif

        interface.version.any.ext->e_resultCallBack; /// @todo
        interface.version.any.ext->e_fetchCallBack; /// @todo
        interface.version.any.ext->e_checkCallBack; /// @todo
        interface.version.any.ext->e_doneCallBack; /// @todo
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 5
        interface.version.any.ext->e_endCallBack; /// @todo
        #endif

        //pmdaSetFetchCallBack(interface, &pmda_fetch_???_callback);
    }

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

    /*
     * Static callback functions registered by the register_callbacks functions.
     * These all redirect thier non-static singleton counterparts above.
     */

    static int pmda_attribute_callback(int ctx, int attr, const char *value,int length, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_attribute(ctx, attr, value, length, pmda);
    }

    static int pmda_children_callback(const char *name, int traverse, char ***kids, int **sts, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_children(name, traverse, kids, sts, pmda);
    }

    static int pmda_desc_callback(pmID pmid, pmDesc *desc, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_desc(pmid, *desc, *pmda);
    }

    static int pmda_fetch_callback(int numpmid, pmID *pmidlist, pmResult **resp, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_fetch(numpmid, *pmidlist, *resp, *pmda);
    }

    static int pmda_fetch_callback_callback(pmdaMetric *mdesc, unsigned int inst, pmAtomValue *avp)
    {
        return 0;//getInstance()->on_pmda_...;
    }

    static int pmda_instance_callback(pmInDom indom, int inst, char *name, __pmInResult **result, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_instance(indom, inst, name, *result, *pmda);
    }

    static int pmda_name_callback(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_name(pmid, nameset, pmda);
    }

    static int pmda_pmid_callback(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_pmid(name, pmid, pmda);
    }

    static int pmda_profile_callback(__pmProfile *prof, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_profile(*prof, *pmda);
    }

    static int pmda_store_callback(pmResult *result, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_store(*result, *pmda);
    }

    static int pmda_text_callback(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        return getInstance()->on_pmda_text(ident, type, *buffer, *pmda);
    }

};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
