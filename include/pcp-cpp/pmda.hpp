//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_PMDA_HPP__
#define __PCP_CPP_PMDA_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "metric_description.hpp"

#include <pcp/impl.h> // For __pmNotifyErr.
#include <pcp/pmda.h>

#include <stdint.h>
#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class pmda {

public:

    template <class Agent>
    static void init_dso(pmdaInterface * const interface)
    {
        try {
            setInstance(new Agent);
            getInstance()->initialize_pmda(*interface);
        } catch (const pcp::exception &ex) {
            __pmNotifyErr(LOG_ERR, "%s", ex.what());
        }
    }

    template <class Agent>
    static int run_daemon(const int argc, char * const argv[])
    {
        try {
            setInstance(new Agent);
            getInstance()->run_daemon(argc, argv);
        } catch (const pcp::exception &ex) {
            __pmNotifyErr(LOG_ERR, "%s", ex.what());
            delete setInstance(NULL);
            return EXIT_FAILURE;
        }
        delete setInstance(NULL);
        return EXIT_SUCCESS;
    }

protected:

    typedef int_fast16_t pmda_domain_number_type; /// @see stdpmid

    typedef struct {
        cluster_id_type cluster;
        item_id_type item;
        instance_id_type instance;
        atom_type_type type;
    } metric_id;

    /// @brief  Virtual destructor for safe polymorphic destruction.
    virtual ~pmda() { }

    static pmda * getInstance() {
        return instance;
    }

    static pmda * setInstance(pmda * const new_instance) {
        pmda * const old_instance = instance;
        instance = new_instance;
        return old_instance;
    }

    virtual std::string get_help_text_pathname() const
    {
        const std::string sep(1, __pmPathSeparator());
        return pmGetConfig("PCP_PMDAS_DIR") + sep + get_pmda_name() + sep + "help";
    }

    virtual std::string get_pmda_name() const = 0;

    virtual pmda_domain_number_type default_pmda_domain_number() const = 0;

    pmda_domain_number_type get_pmda_domain_number() const
    {
        return pmda_domain_number;
    }

    void set_pmda_domain_number(const pmda_domain_number_type domain_number)
    {
        pmda_domain_number = domain_number;
    }

    virtual void run_daemon(const int argc, char * const argv[])
    {
        // Create some local strings.  We keep these as separate variables
        // (as opposed to using function call results directly) because PMAPI
        // functions will copy their pointers (not their contents), so the
        // pointers must remain valid for the life of this function.
        const std::string program_name = get_pmda_name();
        const std::string log_file_pathname = get_pmda_name() + ".log";
        const std::string help_text_pathname = get_help_text_pathname();

        // Set the program name.
        __pmSetProgname(program_name.c_str());

        // Initialize the PMDA to run as a daemon.
        pmdaInterface interface;
        pmdaDaemon(&interface, PCP_CPP_PMDA_INTERFACE_VERSION,
            pmProgname,       // Program name created above
            get_pmda_domain_number(),
            const_cast<char *>(log_file_pathname.c_str()),
            const_cast<char *>(help_text_pathname.c_str())
        );

        // Parse the command line options.
        if (!parse_command_line(argc, argv, interface)) {
            // The parse function already did whatever the command line asked
            // for (eg --help, or --export-pmns), so we're done now.
            return;
        }

        // Initialize the rest of the PMDA.
        initialize_pmda(interface);

        // Establish a connection between this daemon PMDA and PMCD.
        pmdaConnect(&interface);

        // Run the generic PDU processing loop.
        pmdaMain(&interface);
    }

    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface)
    {
        /// @todo Run, for example, pmdaGetOpt.
        return true;
    }

    virtual void initialize_pmda(pmdaInterface &interface)
    {
        // Setup the instance domain and metrics tables.
        /// @todo  For what lifespan do these need to remain valid?
        pmdaIndom * indomtab;   /// @todo
        pmdaMetric * metrictab; /// @todo

        // Assign our callback function pointers to the interface struct.
        set_callbacks(interface);

        // Initialize the PMDA interface.
        pmdaInit(&interface,
            indomtab, sizeof(indomtab)/sizeof(indomtab[0]),
            metrictab,sizeof(metrictab)/sizeof(metrictab[0])
        );
    }

    virtual pcp::metrics_description get_supported_metrics() const = 0;

    virtual void begin_fetch_values() const { }

    virtual pmAtomValue fetch_value(const metric_id &metric) const = 0;

    /* Virtual PMDA callback functions below here. You probably don't
     * want to override any of these, but you can if you want to. */

    /// Inform the agent about security aspects of a client connection,
    /// such as the authenticated userid.  Passed in a client identifier,
    /// numeric PCP_ATTR, pointer to the associated value, and the length
    /// of the value.
    virtual int on_attribute(int ctx, int attr, const char *value,
                                  int length, pmdaExt *pmda)
    {
        return 0;
    }

    /// If traverse == 0, return the names of all the descendent children
    ///     and their status, given a named metric in a dynamic subtree of
    /// the PMNS (this is the pmGetChildren or pmGetChildrenStatus variant).
    /// If traverse == 1, return the full names of all descendent metrics
    /// (this is the pmTraversePMNS variant, with the status added)
    virtual int on_children(const char *name, int traverse, char ***kids,
                                 int **sts, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Return the metric desciption.
    virtual int on_desc(pmID pmid, pmDesc *desc, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Resize the pmResult and call e_callback in the pmdaExt structure
    ///        for each metric instance required by the profile.
    virtual int on_fetch(int numpmid, pmID *pmidlist, pmResult **resp,
                              pmdaExt *pmda)
    {
        try {
            begin_fetch_values();
        } catch (const pcp::exception &ex) {
            __pmNotifyErr(LOG_ERR, "%s", ex.what());
            return ex.error_code();
        }
        return pmdaFetch(numpmid, pmidlist, resp, pmda);
    }

    /// @brief
    virtual int on_fetch_callback(pmdaMetric *mdesc, unsigned int inst,
                                        pmAtomValue *avp)
    {
        /// @todo return fetch_value(...);
        return 0;
    }

    /// @brief Return description of instances and instance domains.
    virtual int on_instance(pmInDom indom, int inst, char *name,
                                 __pmInResult **result, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief  Given a PMID, return the names of all matching metrics within a
    ///         dynamic subtree of the PMNS.
    virtual int on_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Return the PMID for a named metric within a dynamic subtree
    ///        of the PMNS.
    virtual int on_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Store the instance profile away for the next fetch.
    virtual int on_profile(__pmProfile *prof, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Store a value into a metric. This is a no-op.
    virtual int on_store(pmResult *result, pmdaExt *pmda)
    {
        return 0;
    }

    /// @brief Return the help text for the metric.
    virtual int on_text(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        return 0;
    }

    virtual void set_callbacks(pmdaInterface &interface)
    {
        interface.version.any.profile = &callback_profile;
        interface.version.any.fetch = &callback_fetch;
        interface.version.any.desc = &callback_desc;
        interface.version.any.instance = &callback_instance;
        interface.version.any.text = &callback_text;
        interface.version.any.store = &callback_store;
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 5
        interface.version.five.pmid = &callback_pmid;
        interface.version.five.name = &callback_name;
        interface.version.five.children = &callback_children;
        #endif
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
        interface.version.six.attribute = &callback_attribute;
        #endif

        //pmdaSetResultCallBack(&interface, ...);
        pmdaSetFetchCallBack(&interface, &callback_fetch_callback);
        //pmdaSetCheckCallBack(&interface, ...);
        //pmdaSetDoneCallBack(&interface, ...);
        //pmdaSetEndContextCallBack(&interface, ...);
    }

private:
    static pmda * instance;
    pmda_domain_number_type pmda_domain_number;

    bool export_pmns_data(/*dir*/) const
    {
        /// @todo
        return false;
    }

    /*
     * Static callback functions registered by the register_callbacks functions.
     * These all redirect thier non-static singleton counterparts above.
     */

    static int callback_attribute(int ctx, int attr, const char *value,int length, pmdaExt *pmda)
    {
        return getInstance()->on_attribute(ctx, attr, value, length, pmda);
    }

    static int callback_children(const char *name, int traverse, char ***kids, int **sts, pmdaExt *pmda)
    {
        return getInstance()->on_children(name, traverse, kids, sts, pmda);
    }

    static int callback_desc(pmID pmid, pmDesc *desc, pmdaExt *pmda)
    {
        return getInstance()->on_desc(pmid, desc, pmda);
    }

    static int callback_fetch(int numpmid, pmID *pmidlist, pmResult **resp, pmdaExt *pmda)
    {
        return getInstance()->on_fetch(numpmid, pmidlist, resp, pmda);
    }

    static int callback_fetch_callback(pmdaMetric *mdesc, unsigned int inst, pmAtomValue *avp)
    {
        return getInstance()->on_fetch_callback(mdesc, inst, avp);
    }

    static int callback_instance(pmInDom indom, int inst, char *name, __pmInResult **result, pmdaExt *pmda)
    {
        return getInstance()->on_instance(indom, inst, name, result, pmda);
    }

    static int callback_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return getInstance()->on_name(pmid, nameset, pmda);
    }

    static int callback_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return getInstance()->on_pmid(name, pmid, pmda);
    }

    static int callback_profile(__pmProfile *prof, pmdaExt *pmda)
    {
        return getInstance()->on_profile(prof, pmda);
    }

    static int callback_store(pmResult *result, pmdaExt *pmda)
    {
        return getInstance()->on_store(result, pmda);
    }

    static int callback_text(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        return getInstance()->on_text(ident, type, buffer, pmda);
    }

};

} // pcp namespace.

pcp::pmda * pcp::pmda::instance(NULL);

PCP_CPP_END_NAMESPACE

#endif
