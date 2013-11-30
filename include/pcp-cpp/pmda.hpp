//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_PMDA_HPP__
#define __PCP_CPP_PMDA_HPP__

#include "config.hpp"
#include "exception.hpp"
#include "instance_domain.hpp"
#include "metric_description.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

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

    typedef struct {
        cluster_id_type cluster;
        item_id_type item;
        instance_id_type instance;
        atom_type_type type;
        void * opaque;
    } metric_id;

    typedef std::vector<std::string> string_vector;

    /// @brief  Virtual destructor for safe polymorphic destruction.
    virtual ~pmda()
    {
        /// @todo  Free stuff.
    }

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

    virtual int default_pmda_domain_number() const = 0;

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
            const_cast<char *>(program_name.c_str()),
            default_pmda_domain_number(),
            const_cast<char *>(log_file_pathname.c_str()),
            const_cast<char *>(help_text_pathname.c_str())
        );

        // Parse the command line options.
        if (!parse_command_line(argc, argv, interface)) {
            // The parse function already did whatever the command line asked
            // for (eg --help, or --export-pmns), so we're done now.
            return;
        }

        // Redirect output the log.
        pmdaOpenLog(&interface);

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
        int c, error_count = 0;
        while ((c = pmdaGetOpt(argc, const_cast<char **>(argv), supported_options().c_str(), &interface, &error_count)) != EOF) {
            process_command_line_option(c);
        }
        if (error_count > 0) {
            return false;
        }
        return (error_count == 0);
    }

    static std::string pcp_builtin_options()
    {
        return "d:D:h:i:l:pu:6:";
    }

    virtual std::string supported_options() const
    {
        return pcp_builtin_options();
    }

    virtual void process_command_line_option(const int c)
    {
        throw pcp::exception(PM_ERR_GENERIC, (c == '?')
             ? std::string("unrecognised option '-") + (char)optopt + "')"
             : std::string("unimplemented option '-") + char(c) + "')");
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

        // Check if any of the export flags were given.
        bool exported = false;
        #define PCP_CPP_EXPORT(type, func) \
        if (options.count("export-"type) > 0) { \
            const string_vector &filenames = options.at("export-"type).as<string_vector>(); \
            for (string_vector::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter) { \
                func(*iter); \
            } \
            exported = true; \
        }
        PCP_CPP_EXPORT("domain", export_domain_header);
        PCP_CPP_EXPORT("help",   export_help_text);
        PCP_CPP_EXPORT("pmns",   export_pmns_data);
        #undef PCP_CPP_EXPORT
        if (exported) {
            return false;
        }

        check_conflicting_options(options, "inet", "pipe");
        check_conflicting_options(options, "inet", "unix");
        check_conflicting_options(options, "inet", "inet6");
        check_conflicting_options(options, "pipe", "unix");
        check_conflicting_options(options, "pipe", "inet6");
        check_conflicting_options(options, "unix", "inet6");

        // Apply all of the PCP built-in options the same as pmdaGetOpt would.
        if (options.count("debug") > 0) {
            const string_vector &values = options.at("debug").as<string_vector>();
            for (string_vector::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
                const int result = __pmParseDebug(iter->c_str());
                if (result < 0) {
                    throw pcp::exception(result,
                        "unrecognized debug flag specification '" + *iter + "'");
                }
                pmDebug |= result;
            }
        }
        if (options.count("domain") > 0) {
            interface.domain = options.at("domain").as<int>();
        }
        if (options.count("help-file") > 0) {
            interface.version.two.ext->e_helptext =
                strdup(options.at("help-file").as<std::string>().c_str());
        }
        if (options.count("inet") > 0) {
            interface.version.two.ext->e_io = pmdaInet;
            interface.version.two.ext->e_port = options.at("inet").as<int>();
        }
        if (options.count("log-file") > 0) {
            interface.version.two.ext->e_logfile =
                strdup(options.at("log-file").as<std::string>().c_str());
        }
        if (options.count("pipe") > 0) {
            interface.version.two.ext->e_io = pmdaPipe;
        }
        if (options.count("unix") > 0) {
            interface.version.two.ext->e_io = pmdaUnix;
            interface.version.two.ext->e_sockname =
                strdup(options.at("pipe").as<std::string>().c_str());
        }
        if (options.count("inet6") > 0) {
            // The pmdaIPv6 value was added to PCP in version 3.8.1. There is
            // no reliable way to detect PCP's version at compile time, so we
            // use it's known value (4) here to prevent issues compiling against
            // earlier versions of PCP. Note, the default supported_options
            // implementation below does not include inet6 in the command line
            // options for earlier versions of PCP, as detected at runtime.
            interface.version.two.ext->e_io = static_cast<pmdaIoType>(4);
            interface.version.two.ext->e_port = options.at("inet6").as<int>();
        }
        return true;
    }

    virtual boost::program_options::options_description pcp_builtin_options() const
    {
        using namespace boost::program_options;
        options_description options("Libpcp-pmda options");
        options.add_options()
            ("debug,D",
                value<string_vector>()->value_name("spec")->implicit_value(string_vector(1, "-1"), "-1"),
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
                "use named socket for pmcd comms; conflicts with -i, -p and -6");
        if (get_pcp_runtime_version<uint_fast32_t>() >= 0x30801) {
            options.add_options()
                ("inet6,6",
                    value<int>()->value_name("port"),
                    "use IPv6 port for pmcd comms; conflicts with -i, -p and -u");
        }
        return options;
    }

    virtual boost::program_options::options_description supported_options() const
    {
        using namespace boost::program_options;
        options_description options("Extra options");
        options.add_options()
            ("export-domain", value<string_vector>()->value_name("file")->implicit_value(string_vector(1, "-"), "-"),
             "export domain header then exit")
            ("export-help", value<string_vector>()->value_name("file")->implicit_value(string_vector(1, "-"), "-"),
             "export help text then exit")
            ("export-pmns", value<string_vector>()->value_name("file")->implicit_value(string_vector(1, "-"), "-1"),
             "export pmns text then exit")
            ("help",    "display this message then exit")
            ("version", "display version info then exit");
        return pcp_builtin_options().add(options);
    }

    virtual boost::program_options::positional_options_description supported_positional_options()
    {
        return boost::program_options::positional_options_description();
    }

    static void check_conflicting_options(const boost::program_options::variables_map &options_map,
                                          const std::string &option1, const std::string &option2)
    {
        // If both options are specified, and neither was defaulted...
        if ((options_map.count(option1)>0) && (!options_map[option1].defaulted()) &&
            (options_map.count(option2)>0) && (!options_map[option2].defaulted())) {
            throw boost::program_options::error(
                "conflicting options '" + option1 + "' and '" + option2 + "'.");
        }
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
        std::cout << std::endl << "PMDA interface version "
                  << PCP_CPP_PMDA_INTERFACE_VERSION
                  << std::endl << "PCP version "
                  << get_pcp_runtime_version<char *>() << " ("
                  << std::hex << get_pcp_runtime_version<uint_fast32_t>()
                  << ')' << std::endl << std::endl;
    }

    virtual std::string get_usage(const std::string &program_name) const
    {
        return program_name + " [options]";
    }

    virtual void initialize_pmda(pmdaInterface &interface)
    {
        // Setup the instance domain and metrics tables. These will be
        // assigned to members of the interface struct (by pmdaInit), so they
        // must remain valid as long as the interface does.
        supported_metrics = get_supported_metrics();
        const size_t indom_size = count_instance_domains(supported_metrics);
        const size_t metric_size = count_metrics(supported_metrics);
        pmdaIndom * indom_table = new pmdaIndom [indom_size];
        pmdaMetric * metric_table = new pmdaMetric [metric_size];

        std::map<const instance_domain *, pmInDom> instance_domain_ids;
        size_t metric_index = 0;
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin(); metrics_iter != supported_metrics.end(); ++metrics_iter) {
            const metric_cluster &cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                const metric_description &description = cluster_iter->second;
                assert(metric_index < metric_size);
                metric_table[metric_index].m_desc = description;
                metric_table[metric_index].m_desc.pmid = PMDA_PMID(cluster.get_cluster_id(), cluster_iter->first);
                if (description.domain != NULL) {
                    // Insert / find this instance domain.
                    const std::pair<std::map<const instance_domain *, pmInDom>::iterator, bool> insert_result =
                        instance_domain_ids.insert(std::make_pair(description.domain, instance_domain_ids.size()));

                    // If this is first time we've seen this instance domain, add it to the indom table.
                    if (insert_result.second == true) {
                        assert(instance_domain_ids.size() <= indom_size);
                        indom_table[instance_domain_ids.size()-1] = allocate_pmda_indom(*cluster_iter->second.domain);
                    }
                    metric_table[metric_index].m_desc.indom = insert_result.first->second;
                } else {
                    metric_table[metric_index].m_desc.indom = PM_INDOM_NULL;
                }
                metric_table[metric_index].m_user = description.opaque;
                metric_index++;
            }
        }

        // Assign our callback function pointers to the interface struct.
        set_callbacks(interface);

        // Initialize the PMDA interface.
        pmdaInit(&interface, indom_table, indom_size, metric_table, metric_size);
    }

    virtual pcp::metrics_description get_supported_metrics() const = 0;

    virtual void begin_fetch_values() const { }

    virtual pmAtomValue fetch_value(const metric_id &metric) const = 0;

    /* Virtual PMDA callback functions below here. You probably don't
     * want to override any of these, but you can if you want to. */

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
    /// Inform the agent about security aspects of a client connection,
    /// such as the authenticated userid.  Passed in a client identifier,
    /// numeric PCP_ATTR, pointer to the associated value, and the length
    /// of the value.
    virtual int on_attribute(int ctx, int attr, const char *value,
                             int length, pmdaExt *pmda)
    {
        return pmdaAttribute(ctx, attr, value, length, pmda);
    }
#endif

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    /// If traverse == 0, return the names of all the descendent children
    ///     and their status, given a named metric in a dynamic subtree of
    /// the PMNS (this is the pmGetChildren or pmGetChildrenStatus variant).
    /// If traverse == 1, return the full names of all descendent metrics
    /// (this is the pmTraversePMNS variant, with the status added)
    virtual int on_children(const char *name, int traverse, char ***kids,
                            int **sts, pmdaExt *pmda)
    {
        return pmdaChildren(name, traverse, kids, sts, pmda);
    }
#endif

    /// @brief Return the metric desciption.
    virtual int on_desc(pmID pmid, pmDesc *desc, pmdaExt *pmda)
    {
        return pmdaDesc(pmid, desc, pmda);
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

    /// @brief Fetch the value of a single metric instance.
    virtual int on_fetch_callback(pmdaMetric *mdesc, unsigned int inst,
                                  pmAtomValue *avp)
    {
        try {
            // Setup the metric ID.
            metric_id id;
            id.cluster = pmid_cluster(mdesc->m_desc.pmid);
            id.instance = inst;
            id.item = pmid_item(mdesc->m_desc.pmid);
            id.opaque = mdesc->m_user;

#ifdef PCP_CPP_NO_ID_VALIDITY_CHECKS
            id.type = PM_TYPE_UNKNOWN;
#else
            const metric_description &description =
                supported_metrics.at(id.cluster).at(id.item);
            id.type = description.type;

            if (inst != PM_INDOM_NULL) {
                if (description.domain != NULL) {
                    // Instance provided, but non required.
                    throw pcp::exception(PM_ERR_INDOM);
                }
                if (description.domain->count(inst) <= 0) {
                    // Instance provided, but not one we've registered.
                    throw pcp::exception(PM_ERR_INST);
                }
            } else if (description.domain != NULL) {
                // Instance required, but none provided.
                throw pcp::exception(PM_ERR_INDOM);
            }
#endif

            // Fetch the metric value.
            *avp = fetch_value(id);
            return PMDA_FETCH_STATIC; ///< @todo Support PMDA_FETCH_DYNAMIC too.
        } catch (const pcp::exception &ex) {
            if (ex.error_code() != PMDA_FETCH_NOVALUES) {
                __pmNotifyErr(LOG_ERR, "%s", ex.what());
            }
            return ex.error_code();
        } catch (const std::out_of_range &ex) {
            __pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
            return PM_ERR_PMID; // Unknown or illegal metric identifier.
        } catch (const std::exception &ex) {
            __pmNotifyErr(LOG_ERR, "%s", ex.what());
            return PM_ERR_GENERIC;
        } catch (...) {
            __pmNotifyErr(LOG_ERR, "unknown exception in on_fetch_callback");
            return PM_ERR_GENERIC;
        }
    }

    /// @brief Return description of instances and instance domains.
    virtual int on_instance(pmInDom indom, int inst, char *name,
                            __pmInResult **result, pmdaExt *pmda)
    {
        return pmdaInstance(indom, inst, name, result, pmda);
    }

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    /// @brief  Given a PMID, return the names of all matching metrics within a
    ///         dynamic subtree of the PMNS.
    virtual int on_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return pmdaName(pmid, nameset, pmda);
    }

    /// @brief Return the PMID for a named metric within a dynamic subtree
    ///        of the PMNS.
    virtual int on_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return pmdaPMID(name, pmid, pmda);
    }
#endif

    /// @brief Store the instance profile away for the next fetch.
    virtual int on_profile(__pmProfile *prof, pmdaExt *pmda)
    {
        return pmdaProfile(prof, pmda);
    }

    /// @brief Store a value into a metric. This is a no-op.
    virtual int on_store(pmResult *result, pmdaExt *pmda)
    {
        return pmdaStore(result, pmda);
    }

    /// @brief Return the help text for the metric.
    virtual int on_text(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        try {
            if ((type & PM_TEXT_PMID) == PM_TEXT_PMID) {
                const metric_description &description =
                    supported_metrics.at(pmid_cluster(ident)).at(pmid_item(ident));
                const std::string &text =
                    ((type & PM_TEXT_ONELINE) == PM_TEXT_ONELINE)
                        ? description.short_description
                        : description.verbose_description;
                if (text.empty()) {
                    throw pcp::exception(PM_ERR_TEXT);
                }
                *buffer = strdup(text.c_str());
                return 0; // >= 0 implies success.
            } else if ((type & PM_TEXT_INDOM) == PM_TEXT_INDOM) {
                /// @todo  Handle PM_TEXT_INDOM too.
                __pmNotifyErr(LOG_DEBUG, "PM_TEXT_INDOM not implemented yet;"
                                         " falling back to pmdaText instead");
            } else {
                __pmNotifyErr(LOG_NOTICE, "unknown text type 0x%x", type);
            }
        } catch (const pcp::exception &ex) {
            if (ex.error_code() != PM_ERR_TEXT) {
                __pmNotifyErr(LOG_NOTICE, "%s", ex.what());
            }
        } catch (const std::out_of_range &ex) {
            __pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
        } catch (const std::exception &ex) {
            __pmNotifyErr(LOG_NOTICE, "%s", ex.what());
        } catch (...) {
            __pmNotifyErr(LOG_ERR, "unknown exception in on_text");
            return PM_ERR_GENERIC;
        }
        return pmdaText(ident, type, buffer, pmda);
    }

    virtual void set_callbacks(pmdaInterface &interface)
    {
        interface.version.two.profile   = &callback_profile;
        interface.version.two.fetch     = &callback_fetch;
        interface.version.two.desc      = &callback_desc;
        interface.version.two.instance  = &callback_instance;
        interface.version.two.text      = &callback_text;
        interface.version.two.store     = &callback_store;
        #if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
        interface.version.four.pmid     = &callback_pmid;
        interface.version.four.name     = &callback_name;
        interface.version.four.children = &callback_children;
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
    metrics_description supported_metrics;

    void export_domain_header(const std::string &filename) const
    {
        // Open the output file.
        std::ofstream file_stream;
        if (filename != "-") {
            file_stream.open(filename.c_str());
            if (!file_stream.is_open()) {
                throw pcp::exception(PM_ERR_GENERIC, "failed to open file for writing: " + filename);
            }
        }
        std::ostream &stream = (filename == "-") ? std::cout : file_stream;

        // Export the header file content.
        std::string upper_name = get_pmda_name();
        std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);
        stream
            << "// The " << get_pmda_name() << " PMDA's domain number." << std::endl
            << "#define " << upper_name << ' ' << default_pmda_domain_number() << std::endl;
    }

    void export_help_text(const std::string &filename) const
    {
        // Generate a map of metric names to help texts for sorting.
        std::map<std::string, metric_description> map;
        const std::string pmda_name = get_pmda_name();
        const metrics_description metrics = get_supported_metrics();
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                std::string metric_name = pmda_name;
                if (!cluster_name.empty()) {
                    metric_name += '.' + cluster_name;
                }
                metric_name += '.' + cluster_iter->second.metric_name;
                map.insert(std::make_pair(metric_name, cluster_iter->second));
            }
        }

        // Open the output file.
        std::ofstream file_stream;
        if (filename != "-") {
            file_stream.open(filename.c_str());
            if (!file_stream.is_open()) {
                throw pcp::exception(PM_ERR_GENERIC, "failed to open file for writing: " + filename);
            }
        }
        std::ostream &stream = (filename == "-") ? std::cout : file_stream;

        // Export the help text.
        stream << std::endl;
        for (std::map<std::string, metric_description>::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
            stream << "@ " << iter->first << ' ' << iter->second.short_description << std::endl;
            if (!iter->second.verbose_description.empty()) {
                stream << iter->second.verbose_description << std::endl;
            }
            stream << std::endl;
        }
    }

    void export_pmns_data(const std::string &filename) const
    {
        // Open the output file.
        std::ofstream file_stream;
        if (filename != "-") {
            file_stream.open(filename.c_str());
            if (!file_stream.is_open()) {
                throw pcp::exception(PM_ERR_GENERIC, "failed to open file for writing: " + filename);
            }
        }
        std::ostream &stream = (filename == "-") ? std::cout : file_stream;

        const metrics_description metrics = get_supported_metrics();

        // First pass to find the length of the longest metric name.
        std::string::size_type max_metric_name_size = 0;
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            const metric_cluster &cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                if (cluster_iter->second.metric_name.size() > max_metric_name_size) {
                    max_metric_name_size = cluster_iter->second.metric_name.size();
                }
            }
        }

        // Some basic strings we'll use a couple of times.
        const std::string &pmda_name = get_pmda_name();
        std::string upper_name = get_pmda_name();
        std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

        // Second pass to export the group names and ungrouped metrics.
        stream << std::endl << pmda_name << " {" << std::endl;
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            if (cluster_name.empty()) {
                for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                    stream << "    " << cluster_iter->second.metric_name
                           << std::string(max_metric_name_size - cluster_iter->second.metric_name.size() + 4, ' ')
                           << upper_name << ':' << cluster.get_cluster_id() << ':'
                           << cluster_iter->first << std::endl;
                }
            } else {
                stream << "    " << cluster_name << std::endl;
            }
        }
        stream << '}' << std::endl;

        // Third and final pass to export all metric groups.
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            if (!cluster_name.empty()) {
                stream << std::endl << pmda_name << '.' << cluster_name << " {" << std::endl;
                for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                    stream << "    " << cluster_iter->second.metric_name
                           << std::string(max_metric_name_size - cluster_iter->second.metric_name.size() + 4, ' ')
                           << upper_name << ':' << cluster.get_cluster_id() << ':'
                           << cluster_iter->first << std::endl;
                }
                stream << "}" << std::endl;
            }
        }
        stream << std::endl;
    }

    static inline size_t count_instance_domains(const metrics_description &metrics)
    {
        std::set<const instance_domain *> instance_domains;
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            const metric_cluster cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin(); cluster_iter != cluster.end(); ++cluster_iter) {
                instance_domains.insert(cluster_iter->second.domain);
            }
        }
        return instance_domains.size();
    }

    static inline size_t count_metrics(const metrics_description &metrics)
    {
        size_t count = 0;
        for (metrics_description::const_iterator metrics_iter = metrics.begin(); metrics_iter != metrics.end(); ++metrics_iter) {
            count += metrics_iter->second.size();
        }
        return count;
    }

    static inline pmdaIndom allocate_pmda_indom(const instance_domain &domain)
    {
        pmdaIndom indom;
        indom.it_indom = domain;
        indom.it_numinst = domain.size();
        indom.it_set = new pmdaInstid [domain.size()];
        size_t index = 0;
        for (instance_domain::const_iterator iter = domain.begin(); iter != domain.end(); ++iter, ++index) {
            indom.it_set[index].i_inst = iter->first;
            indom.it_set[index].i_name = const_cast<char *>(iter->second.c_str());
        }
        return indom;
    }

    /*
     * Static callback functions registered by the register_callbacks functions.
     * These all redirect thier non-static singleton counterparts above.
     */

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
    static int callback_attribute(int ctx, int attr, const char *value,int length, pmdaExt *pmda)
    {
        return getInstance()->on_attribute(ctx, attr, value, length, pmda);
    }
#endif

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    static int callback_children(const char *name, int traverse, char ***kids, int **sts, pmdaExt *pmda)
    {
        return getInstance()->on_children(name, traverse, kids, sts, pmda);
    }
#endif

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

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    static int callback_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return getInstance()->on_name(pmid, nameset, pmda);
    }

    static int callback_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return getInstance()->on_pmid(name, pmid, pmda);
    }
#endif

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
