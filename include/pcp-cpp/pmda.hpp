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
        } catch (const std::exception &ex) {
            __pmNotifyErr(LOG_ERR, "%s", ex.what());
        }
    }

    template <class Agent>
    static int run_daemon(const int argc, char * const argv[])
    {
        try {
            setInstance(new Agent);
            getInstance()->run_daemon(argc, argv);
        } catch (const std::exception &ex) {
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

    virtual std::string get_log_file_pathname() const
    {
        return get_pmda_name() + ".log";
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

    virtual std::string get_pmda_version() const
    {
        return std::string();
    }

    virtual void run_daemon(const int argc, char * const argv[])
    {
        // Create some local strings.  We keep these as separate variables
        // (as opposed to using function call results directly) because PMAPI
        // functions will copy their pointers (not their contents), so the
        // pointers must remain valid for the life of this function.
        const std::string program_name = get_pmda_name();
        const std::string help_text_pathname = get_help_text_pathname();
        const std::string log_file_pathname = get_log_file_pathname();

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

#ifdef PCP_CPP_NO_BOOST
    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface)
    {
        /// @todo Run, for example, pmdaGetOpt.
        return true;
    }

    static std::string pcp_builtin_options()
    {
        return "d:D:h:i:l:pu:6:";
    }

    virtual std::string supported_options() const
    {
        return pcp_builtin_options();
    }
#else
    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface)
    {
        using namespace boost::program_options;
        boost::program_options::variables_map options;
        store(command_line_parser(argc, argv).options(supported_options())
              .positional(supported_positional_options()).run(), options);

#ifdef PCP_CPP_DEBUG_COMMAND_LINE_OPTIONS
        for (variables_map::const_iterator iter = options.begin(); iter != options.end(); ++iter) {
            std::string value;
            try {
                if (iter->second.value().type() == typeid(int)) {
                    value = boost::lexical_cast<std::string>(iter->second.as<int>());
                } else {
                    value = iter->second.as<std::string>();
                }
            } catch (const boost::bad_any_cast &ex) {
                value = "(complex type \"" + std::string(iter->second.value().type().name()) + "\")";
            }
            std::cout << iter->first << ": " << value << std::endl;
        }
#endif

        //check_conflicting_options(...);

        // Check if the --help (or -h) flag was given.
        if (options.count("help") > 0) {
            display_help(((argc > 0) && (argv[0] != '\0')) ? std::string(argv[0]) : get_pmda_name());
            return false;
        }

        // Check if the --version (or -v) flag was given.
        if (options.count("version") > 0) {
            display_version();
            return false;
        }

        /// @todo Run, for example, pmdaGetOpt.
        return true;
    }

    virtual boost::program_options::options_description pcp_builtin_options() const
    {
        using namespace boost::program_options;
        options_description options("Libpcp-pmda options");
        options.add_options()
            ("debug,D",
                value<std::string>()->value_name("spec")->implicit_value("-1"),
                "set debug specification")
            ("domain,d",
                value<int>()->value_name("n")->default_value(default_pmda_domain_number()),
                "domain number to use")
            ("help-file,h",
                value<std::string>()->value_name("file")->default_value(get_help_text_pathname()),
                "file containing help text")
            ("inet,i",
                value<int>()->value_name("port"),
                "use inet port for pmcd comms; conflicts with -p, -u and -6")
            ("log-file,l",
                value<std::string>()->value_name("file")->default_value(get_log_file_pathname()),
                "file to use for logging")
            ("pipe,p", "use stdin/stdout for pmcd comms; conflicts with -i, -u and -6")
            ("unix,u",
                value<std::string>()->value_name("socket"),
                "use named socket for pmcd comms; conflicts with -i, -p and -6")
            ("inet6,6",
                value<int>()->value_name("port"),
                "use IPv6 port for pmcd comms; conflicts with -i, -p and -u");
        return options;
    }

    virtual boost::program_options::options_description supported_options() const
    {
        using namespace boost::program_options;
        options_description options("Extra options");
        options.add_options()
            ("export-domain", value<std::string>()->value_name("file")->implicit_value("-"),
             "export domain header then exit")
            ("export-help", value<std::string>()->value_name("file")->implicit_value("-"),
             "export help text then exit")
            ("export-pmns", value<std::string>()->value_name("file")->implicit_value("-"),
             "export pmns text then exit")
            ("help",    "display this message then exit")
            ("version", "display version info then exit");
        return pcp_builtin_options().add(options);
    }

    virtual boost::program_options::positional_options_description supported_positional_options()
    {
        return boost::program_options::positional_options_description();
    }

    virtual void display_help(const std::string &program_name) const
    {
        std::cout
            << std::endl << "Usage: " << get_usage(program_name) << std::endl
            << std::endl << supported_options() << std::endl;
    }

#endif

    virtual void display_version() const
    {
        std::cout << std::endl << get_pmda_name() << " PMDA";
        const std::string pmda_version = get_pmda_version();
        if (!pmda_version.empty()) {
            std::cout << " version " << pmda_version;
        }
        std::cout << ", using PMDA interface version "
                  << PCP_CPP_PMDA_INTERFACE_VERSION
                  << '.' << std::endl << std::endl;
    }

    virtual std::string get_usage(const std::string &program_name) const
    {
        return program_name + " [options]";
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
