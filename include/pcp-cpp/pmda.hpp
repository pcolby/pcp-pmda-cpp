//            Copyright Paul Colby 2013 - 2018.
//            Copyright Red Hat 2018
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 * @brief Defines the pcp::pmda class.
 */

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
#include <sstream>
#include <stack>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

/**
 * @brief Abstract base class for implementing PCP PMDAs.
 */
class pmda {

public:

    /**
     * @brief Initialize PCP's DSO interface for a PMDA.
     *
     * Example usage:
     * @code
     * class my_pmda : public pcp::pmda {
     *     ....
     * };
     *
     * extern "C" void trivial_init(pmdaInterface *interface)
     * {
     *     pcp::pmda::init_dso<my_pmda>(interface);
     * }
     * @endcode
     *
     * @param interface PMDA interface point provided by PCP as part of the DSO
     *                  initialisation process.
     */
    template <class Agent>
    static void init_dso(pmdaInterface * const interface)
    {
        try {
            set_instance(new Agent);
            get_instance()->initialize_dso(*interface);
        } catch (const std::exception &ex) {
            pmNotifyErr(LOG_ERR, "%s", ex.what());
        }
    }

    /**
     * @brief Run a PMDA in PCP's daemon mode.
     *
     * Example usage:
     * @code
     * class my_pmda : public pcp::pmda {
     *     ....
     * };
     *
     * int main(int argc, char *argv[])
     * {
     *     return pcp::pmda::run_daemon<my_pmda>(argc, argv);
     * }
     * @endcode
     *
     * @param argc Argument count.
     * @param argv Argument vector.
     *
     * @return EXIT_SUCCESS on success, EXIT_FAILURE on error.
     */
    template <class Agent>
    static int run_daemon(const int argc, char * const argv[])
    {
        try {
            set_instance(new Agent);
            get_instance()->run_daemon(argc, argv);
        } catch (const std::exception &ex) {
            pmNotifyErr(LOG_ERR, "%s", ex.what());
            delete set_instance(NULL);
            return EXIT_FAILURE;
        }
        delete set_instance(NULL);
        return EXIT_SUCCESS;
    }

protected:

    /// Description of all metrics supported by this PMDA. This is really just
    /// a cache of the value returned by get_supported_metrics during startup.
    metrics_description supported_metrics;

    /**
     * @brief  Struct returned by the fetch_value function.
     *
     * @see pcp::fetch_value
     */
    struct fetch_value_result {
        pmAtomValue atom; ///< Atom value.
        int code; ///< PMDA fetch code describing atom's memory allocation.

        /**
         * @brief Constructor.
         *
         * @param atom PCP atom value.
         * @param code Optional code to return to PCP. Should be one of the
         *             PMDA_FETCH_* constants.
         */
        fetch_value_result(const pmAtomValue &atom,
                           const int code = PMDA_FETCH_STATIC)
            : atom(atom), code(code) { }
    };

    /**
     * @brief Indentifies a metric to be fetched.
     *
     * @see pcp::fetch_value
     */
    struct metric_id {
        cluster_id_type cluster;   ///< Cluster ID.
        item_id_type item;         ///< Item ID.
        instance_id_type instance; ///< Instance ID.
        atom_type_type type;       ///< Expected atom type.
        void * opaque;             ///< Opaque pointer.
    };

    /// @brief  A simple vector of strings.
    typedef std::vector<std::string> string_vector;

    /**
     * @brief Destructor.
     */
    virtual ~pmda()
    {
        while (!free_on_destruction.empty()) {
            free(free_on_destruction.top());
            free_on_destruction.pop();;
        }
    }

    /**
     * @brief Get the single PMDA instance.
     *
     * @return A pointer to the single PMDA instance, if set, otherwise \c NULL.
     *
     * @see set_instance
     */
    static pmda * get_instance() {
        return instance;
    }

    /**
     * @brief Set the single PMDA instance.
     *
     * @note For performance reasons, this function is intentionally not thread
     *       safe. Nor does it need to be, given that the instance is set by
     *       either init_dso or run_daemon, long before any threads interested
     *       in this instance have been created.
     *
     * @param new_instance The new instance to set.
     *
     * @return A pointer to the previous instance, if there was one, otherwise
     *         \c NULL.
     *
     * @see get_instance
     */
    static pmda * set_instance(pmda * const new_instance) {
        pmda * const old_instance = instance;
        instance = new_instance;
        return old_instance;
    }

#ifndef PCP_CPP_NO_BOOST
    /**
     * @brief Get the default path to this PMDA's optional configuration file.
     *
     * Derived classes may override this function to provide a custom path. The
     * default is equivalent to $PCP_PMDAS_DIR/$PMDA_NAME/config.
     *
     * @note This function is only available if Boost support is enabled.
     *
     * @return The path to this PMDA's optional configuration file.
     */
    virtual std::string get_config_file_pathname() const
    {
        const std::string sep(1, pmPathSeparator());
        return pmGetConfig("PCP_PMDAS_DIR") + sep + get_pmda_name() + sep + "config";
    }
#endif

    /**
     * @brief Get the default path to this PMDA's optional help texts file.
     *
     * Derived classes may override this function to provide a custom path. The
     * default is equivalent to $PCP_PMDAS_DIR/$PMDA_NAME/help.
     *
     * @return The path to this PMDA's optional help texts file.
     */
    virtual std::string get_help_text_pathname() const
    {
        const std::string sep(1, pmPathSeparator());
        return pmGetConfig("PCP_PMDAS_DIR") + sep + get_pmda_name() + sep + "help";
    }

    /**
     * @brief Get the default path to this PMDA's log file.
     *
     * Derived classes may override this function to provide a custom path. The
     * default is equivalent to $PCP_PMCD_DIR/$PMDA_NAME.log.
     *
     * @return The path to this PMDA's log file.
     */
    virtual std::string get_log_file_pathname() const
    {
        return get_pmda_name() + ".log";
    }

    /**
     * @brief Get this PMDA's name.
     *
     * Derived classes must override this function to return the name of the
     * PMDA.
     *
     * @return This PMDA's name.
     */
    virtual std::string get_pmda_name() const = 0;

    /**
     * @brief Get this PMDA's default performance metrics domain number.
     *
     * Derived classes must override this function to return the default domain
     * number for this PMDA.
     *
     * Unless the command line parsing functions are overridden, this class will
     * allow this default PMDA domain number to be overridden via the -d or
     * --domain (Boost only) command line options.
     *
     * @note The PCP project maintains a list of standard PMIDs. You should
     *       normally avoid clashing with any of those.
     *
     * @return This PMDA's default domain number.
     *
     * @see http://git.pcp.io/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmns/stdpmid.pcp
     */
    virtual int get_default_pmda_domain_number() const = 0;

    /**
     * @brief Get this PMDA's version string.
     *
     * Derived classes may override this function to return a custom version
     * string to be included in messages such as the outpt of the --version
     * command line option.
     *
     * This base implementation returns an empty string.
     *
     * @return This PMDA's version string.
     */
    virtual std::string get_pmda_version() const
    {
        return std::string();
    }

    /**
     * @brief Run this daemon.
     *
     * This function implements the main processing loop of the daemon-mode
     * PMDA. It performs various initalisations such as parsing the command
     * line options, then defers processing to the pmdaMain function.
     *
     * @param argc Argument count.
     * @param argv Argument vector.
     *
     * @throw pcp::exception On error.
     */
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
        pmSetProgname(program_name.c_str());

        // Initialize the PMDA to run as a daemon.
        pmdaInterface interface;
        // PCP 3.11.5 updated the pmdaDaemon signature to use const char pointers.
        // So for earlier versions only, we need to cast away some constness.
        // Note, the PM_VERSION* macros themselves weren't added until PCP 3.10.5.
        #if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < 0x30B05 // 3.11.5
        #define PCP_CPP_PMDA_CONST_CHAR(str) const_cast<char *>(str)
        #else
        #define PCP_CPP_PMDA_CONST_CHAR(str) str // Do nothing.
        #endif
        pmdaDaemon(&interface, PCP_CPP_PMDA_INTERFACE_VERSION,
            PCP_CPP_PMDA_CONST_CHAR(program_name.c_str()),
            get_default_pmda_domain_number(),
            PCP_CPP_PMDA_CONST_CHAR(log_file_pathname.c_str()),
            (help_text_pathname.empty()) ? NULL : PCP_CPP_PMDA_CONST_CHAR(help_text_pathname.c_str())
        );
        #undef PCP_CPP_PMDA_CONST_CHAR

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

        // Free the instance domains and metrics allocated in initialize_pmda.
        for (int index = 0; index < interface.version.two.ext->e_nindoms; ++index) {
            delete [] interface.version.two.ext->e_indoms[index].it_set;
        }
        delete[] interface.version.two.ext->e_indoms;
        delete[] interface.version.two.ext->e_metrics;
    }

#ifdef PCP_CPP_NO_BOOST
    /**
     * @brief Parse command line options.
     *
     * This non-Boost version uses PCP's pmdaGetOpt function (a wrapper for the
     * POSIX getopt function, with a number of built-in options) to parse the
     * command line options.
     *
     * @param argc      Argument count.
     * @param argv      Argumnet vector.
     * @param interface PMDA interface.
     *
     * @return \c true if the caller should continue to run the PMDA, \c false
     *         on error.
     */
    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface)
    {
        int c, error_count = 0;
        while ((c = pmdaGetOpt(argc, const_cast<char **>(argv), get_supported_options().c_str(), &interface, &error_count)) != EOF) {
            process_command_line_option(c);
        }
        if (error_count > 0) {
            return false;
        }
        return (error_count == 0);
    }

    /**
     * @brief Get a list of command line options supported by pmdaGetOpt.
     *
     * This function returns a getopt compatible list of command line options
     * that are known to be supported by PCP's pmdaGetOpt function.  Derived
     * classes may choose to build upon this list to add new command line
     * options in addition to those already built-in to PCP.
     *
     * @return A list of command line options supported by pmdaGetOpt.
     *
     * @see pmdaGetOpt
     * @see getopt
     */
    static std::string pcp_builtin_options()
    {
        return "d:D:h:i:l:pu:6:";
    }

    /**
     * @brief Get a list of command line options supported by this PMDA.
     *
     * This base implementation simply returns the PCP built-in command line
     * options, as returned by pcp_builtin_options.
     *
     * Derived classes may override this function to add to, or even replace,
     * the set of command line options supported by the derived PMDA.
     *
     * @return A list of command line options supported by this PMDA.
     */
    virtual std::string get_supported_options() const
    {
        return pcp_builtin_options();
    }

    /**
     * @brief Process as custom command line option.
     *
     * This function will be invoked by parse_command_line function for any
     * option not supported by pmdaGetOpt.  That is, if a derived class
     * overrides get_supported_options to include new custom command line
     * options, then that derived class should also override this function to
     * handle each of those command line options as they arise.
     *
     * This base implementation simply throws a generic pcp::exception.
     *
     * @param c The command line option character.
     *
     * @throw pcp::exception on error.
     */
    virtual void process_command_line_option(const int c)
    {
        throw pcp::exception(PM_ERR_GENERIC, (c == '?')
             ? std::string("unrecognised option '-") + (char)optopt + "')"
             : std::string("unimplemented option '-") + char(c) + "')");
    }
#else
    /**
     * @brief Parse command line options.
     *
     * Override this overload if you want to customise the command line parsing
     * process, but don't want to access any of the parsed options explicitly.
     *
     * Note, it's pretty unusual to want to do this. This overload is really
     * here to maintain compatibility between the Boost and non-Boost
     * implementations of parse_command_line.
     *
     * @param argc      Argument count.
     * @param argv      Argumnet vector.
     * @param interface PMDA interface.
     *
     * @return \c true if the caller should continue to run the PMDA, else
     *         \c false.
     *
     * @throw pcp::exception On error.
     * @throw boost::program_options::error On parse error.
     */
    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface)
    {
        boost::program_options::variables_map options;
        return parse_command_line(argc, argv, interface, options);
    }

    /**
     * @brief Parse command line options.
     *
     * Override this function to gain access explicit to the parsed command line
     * options, if desired.
     *
     * @param argc      Argument count.
     * @param argv      Argumnet vector.
     * @param interface PMDA interface.
     * @param options   The parsed program options.
     *
     * @return \c true if the caller should continue to run the PMDA, else
     *         \c false.
     *
     * @throw pcp::exception On error.
     * @throw boost::program_options::error On parse error.
     */
    virtual bool parse_command_line(const int argc, const char * const argv[],
                                    pmdaInterface& interface,
                                    boost::program_options::variables_map &options)
    {
        using namespace boost::program_options;
        const options_description supported_options =
                get_supported_options().add(get_supported_hidden_options());
        store(command_line_parser(argc, argv).options(supported_options)
              .positional(get_supported_positional_options()).run(), options);

        const variable_value &config = options.at("config");
        std::ifstream config_stream(config.as<std::string>().c_str());
        if (config_stream) {
            store(parse_config_file(config_stream, supported_options), options);
        } else if (!config.defaulted()) {
            throw pcp::exception(PM_ERR_GENERIC, "Failed to open config file: " +
                                 config.as<std::string>());
        }

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
            display_help(
                ((argc > 0) && (argv[0] != NULL) && (argv[0][0] != '\0'))
                ? std::string(argv[0]) : get_pmda_name());
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
        if (options.count("export-" type) > 0) { \
            if (supported_metrics.empty()) { \
                supported_metrics = get_supported_metrics(); \
            } \
            const string_vector &filenames = options.at("export-" type).as<string_vector>(); \
            for (string_vector::const_iterator iter = filenames.begin(); iter != filenames.end(); ++iter) { \
                func(*iter); \
            } \
            exported = true; \
        }
        PCP_CPP_EXPORT("all",    export_support_files);
        PCP_CPP_EXPORT("domain", export_domain_header);
        PCP_CPP_EXPORT("help",   export_help_text);
        PCP_CPP_EXPORT("pmns",   export_pmns_data);
        PCP_CPP_EXPORT("root",   export_pmns_root);
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
                // PCP 3.12.2 changed the debugging control infrastructure.
                #if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(3,12,2)
                const int result = __pmParseDebug(iter->c_str());
                #else
                const int result = pmSetDebug(iter->c_str());
                #endif
                if (result < 0) {
                    throw pcp::exception(result,
                        "unrecognized debug flag specification '" + *iter + "'");
                }
                #if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(3,12,2)
                pmDebug |= result; // pmSetDebug does this internally; __pmParseDebug does not.
                #endif
            }
        }
        if (options.count("domain") > 0) {
            interface.domain = options.at("domain").as<int>();
        }
        if (options.count("help-file") > 0) {
            char * const help_file = strdup(options.at("help-file").as<std::string>().c_str());
            interface.version.two.ext->e_helptext = help_file;
                free_on_destruction.push(help_file);
        }
        if (options.count("inet") > 0) {
            interface.version.two.ext->e_io = pmdaInet;
            interface.version.two.ext->e_port = options.at("inet").as<int>();
        }
        if (options.count("log-file") > 0) {
            char * const log_file = strdup(options.at("log-file").as<std::string>().c_str());
            interface.version.two.ext->e_logfile = log_file;
            free_on_destruction.push(log_file);
        }
        if (options.count("pipe") > 0) {
            interface.version.two.ext->e_io = pmdaPipe;
        }
        if (options.count("unix") > 0) {
            interface.version.two.ext->e_io = pmdaUnix;
            free_on_destruction.push(interface.version.two.ext->e_sockname =
                strdup(options.at("unix").as<std::string>().c_str()));
        }
        if (options.count("inet6") > 0) {
            // The pmdaIPv6 value was added to PCP in version 3.8.1. There is
            // no reliable way to detect PCP's version at compile time, so we
            // use it's known value (4) here to prevent issues compiling against
            // earlier versions of PCP. Note, the default get_supported_options
            // implementation below does not include inet6 in the command line
            // options for earlier versions of PCP, as detected at runtime.
            interface.version.two.ext->e_io = static_cast<pmdaIoType>(4);
            interface.version.two.ext->e_port = options.at("inet6").as<int>();
        }
        return true;
    }

    /**
     * @brief Get a list of command line options built into the PCP libraries.
     *
     * @return A list of command line options built into the PCP libraries.
     */
    virtual boost::program_options::options_description pcp_builtin_options() const
    {
        using namespace boost::program_options;
        options_description options("Libpcp-pmda options");
        options.add_options()
            ("debug,D", value<string_vector>()
                PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "-1"), "-1")
                PCP_CPP_BOOST_PO_VALUE_NAME("spec"), "set debug specification")
            ("domain,d", value<int>()->default_value(get_default_pmda_domain_number())
                PCP_CPP_BOOST_PO_VALUE_NAME("n"), "domain number to use")
            ("help-file,h",
                value<std::string>()->default_value(get_help_text_pathname())
                PCP_CPP_BOOST_PO_VALUE_NAME("file"), "file containing help text")
            ("inet,i", value<int>() PCP_CPP_BOOST_PO_VALUE_NAME("port"),
                "use inet port for pmcd comms; conflicts with -p, -u and -6")
            ("log-file,l", value<std::string>()->default_value(get_log_file_pathname())
                PCP_CPP_BOOST_PO_VALUE_NAME("file"), "file to use for logging")
            ("pipe,p", "use stdin/stdout for pmcd comms; conflicts with -i, -u and -6")
            ("unix,u", value<std::string>() PCP_CPP_BOOST_PO_VALUE_NAME("socket"),
                "use named socket for pmcd comms; conflicts with -i, -p and -6");
        if (get_pcp_runtime_version<uint_fast32_t>() >= 0x30801) {
            options.add_options()
                ("inet6,6", value<int>() PCP_CPP_BOOST_PO_VALUE_NAME("port"),
                    "use IPv6 port for pmcd comms; conflicts with -i, -p and -u");
        }
        return options;
    }

    /**
     * @brief Get a list of command line options supported by this PMDA.
     *
     * This base implementation returns a set of options including options
     * supported by the PCP libraries, as well as a number of custom options
     * implemented by this pcp::pmda class (such as --help and --version).
     *
     * Derived classes may override this function to add to, or even replace,
     * the set of options supported by the derived PMDA.
     *
     * @return A list of command line options supported by this PMDA.
     */
    virtual boost::program_options::options_description get_supported_options() const
    {
        using namespace boost::program_options;
        options_description options("Extra options");
        options.add_options()
            ("config", value<std::string>()->default_value(get_config_file_pathname())
             PCP_CPP_BOOST_PO_VALUE_NAME("file"), "load options from config file")
            ("export-all", value<string_vector>()
             PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "."), ".")
             PCP_CPP_BOOST_PO_VALUE_NAME("dir"), "export domain, help, pmns and root then exit")
            ("export-domain", value<string_vector>()
             PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "-"), "-")
             PCP_CPP_BOOST_PO_VALUE_NAME("file"), "export domain header then exit")
            ("export-help", value<string_vector>()
             PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "-"), "-")
             PCP_CPP_BOOST_PO_VALUE_NAME("file"), "export help text then exit")
            ("export-pmns", value<string_vector>()
             PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "-"), "-")
             PCP_CPP_BOOST_PO_VALUE_NAME("file"), "export pmns text then exit")
            ("export-root", value<string_vector>()
             PCP_CPP_BOOST_PO_IMPLICIT_VALUE(string_vector(1, "-"), "-")
             PCP_CPP_BOOST_PO_VALUE_NAME("file"), "export pmns root then exit")
            ("help",    "display this message then exit")
            ("version", "display version info then exit");
        return pcp_builtin_options().add(options);
    }

    /**
     * @brief Get a list of hidden supported command line options.
     *
     * Hidden command line options behave just like their non-hidden veriety,
     * except that they are not included in the output of the --help option.
     *
     * This is allows derived classes to implement, for example, development
     * only options or other options that would be confusing to end users,
     * without necessary having to make the obvious.
     *
     * This, of course, should be used sparingly.
     *
     * This base implementation returns an empty list.
     *
     * @return A list of command line options to support, but not advertise.
     */
    virtual boost::program_options::options_description get_supported_hidden_options() const
    {
        return boost::program_options::options_description();
    }

    /**
     * @brief Get a list of supported positional options.
     *
     * Derived classes may override this function to support positional options.
     *
     * This base implementation returns an empty list.
     *
     * @return A list of supported positional options.
     */
    virtual boost::program_options::positional_options_description get_supported_positional_options()
    {
        return boost::program_options::positional_options_description();
    }

    /**
     * @brief Check for conflicting command line options.
     *
     * This convenience function can be used to check, in a standardised way,
     * that two mutually exclusive options are not both set.
     *
     * @param options_map Parsed command line options.
     * @param option1     The first of two options that are mutually exclusive.
     * @param option2     The second of two options that are mutually exclusive.
     *
     * @throw boost::program_options::error If both \a option1 and \a option2
     *                                      are set, and neither were defaulted.
     */
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

    /**
     * @brief Display a help message.
     *
     * This function is called if the --help command line option was given.
     *
     * Derived classes may override this function to customise the help text
     * output.
     *
     * @param program_name The program name to include in the help message.
     */
    virtual void display_help(const std::string &program_name) const
    {
        std::cout
            << std::endl << "Usage: " << get_usage(program_name) << std::endl
            << std::endl << get_supported_options() << std::endl;
    }

#endif

    /**
     * @brief Display a version message.
     *
     * This function is called if the --version command line option was given.
     *
     * Derived classes may override this function to customise the version text
     * output.
     *
     * @see get_pmda_version
     */
    virtual void display_version() const
    {
        const std::ostream::fmtflags cout_format_flags(std::cout.flags());

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

        std::cout.flags(cout_format_flags); // Restore cout to its entry state.
    }

    /**
     * @brief Get a usage syntax string.
     *
     * This function returns a string defining the command line syntax. It is
     * included, by default, in the text output of the --help command line
     * option.
     *
     * This base implementation returns a string like: `program [options]`. But,
     * for example, if a derived class made some options compulsory, and/or
     * allowed position arguments, then that class would want to override this
     * function to return something like:
     * `program --required-flag=arg [options] filename1 filename2`.
     *
     * @param program_name The name of the program to include in the syntax line.
     *
     * @return A usage syntax line.
     *
     * @see display_help
     */
    virtual std::string get_usage(const std::string &program_name) const
    {
        return program_name + " [options]";
    }

    /**
     * @brief Initialise a DSO interface with this PMDA.
     *
     * @param interface PMDA interface to initialise.
     *
     * @throws pcp::exception on error.
     */
    virtual void initialize_dso(pmdaInterface &interface) {
        const std::string help_text_pathname = get_help_text_pathname();

        // Contrary to the man pages, pmdaDSO returns void, not int.
        pmdaDSO(&interface, PCP_CPP_PMDA_INTERFACE_VERSION,
                const_cast<char *>(get_pmda_name().c_str()),
                (help_text_pathname.empty()) ? NULL : const_cast<char *>(help_text_pathname.c_str()));

        // Handle pmdaDSO errors (the man page is really lacking here).
        if (interface.comm.pmda_interface < PCP_CPP_PMDA_INTERFACE_VERSION) {
            std::ostringstream message;
            message << "This DSO uses protocol " << PCP_CPP_PMDA_INTERFACE_VERSION
                    << " but the caller only understands " << interface.comm.pmda_interface
                    << " or less";
            throw pcp::exception(PM_ERR_GENERIC, message.str());
        }
        if (interface.status < 0) {
            throw pcp::exception(interface.status, "Failed to initialize DSO via pmdaDSO");
        }

        // Initialise the rest of the PMDA.
        initialize_pmda(interface);
    }

    /**
     * @brief Initialise this PMDA.
     *
     * @param interface PMDA interface to initialise.
     */
    virtual void initialize_pmda(pmdaInterface &interface)
    {
        // Setup the instance domain and metrics tables. These will be
        // assigned to members of the interface struct (by pmdaInit), so they
        // must remain valid as long as the interface does.
        supported_metrics = get_supported_metrics();
        const std::pair<size_t,size_t> counts = count_metrics(supported_metrics);
        const size_t indom_count = counts.second;
        const size_t metric_count = counts.first;
        pmdaIndom * indom_table = (indom_count == 0) ? NULL : new pmdaIndom [indom_count];
        pmdaMetric * metric_table = new pmdaMetric [metric_count];

        std::map<const instance_domain *, pmInDom> instance_domain_ids;
        std::vector<instance_domain *> instance_domains;
        instance_domains.reserve(indom_count);
        size_t metric_index = 0;
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin();
             metrics_iter != supported_metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                 cluster_iter != cluster.end(); ++cluster_iter)
            {
                const metric_description &description = cluster_iter->second;
                assert(metric_index < metric_count);
                metric_table[metric_index].m_desc = description;
                metric_table[metric_index].m_desc.pmid =
                    PMDA_PMID(cluster.get_cluster_id(), cluster_iter->first);
                if (description.domain != NULL) {
                    const std::pair<std::map<const instance_domain *, pmInDom>::const_iterator, bool>
                        insert_result = instance_domain_ids.insert(
                            std::make_pair(cluster_iter->second.domain, instance_domain_ids.size()));
                    const pmInDom indom = insert_result.first->second;
                    assert(indom < indom_count);
                    if (insert_result.second) {
                        indom_table[indom] = allocate_pmda_indom(*cluster_iter->second.domain);
                        instance_domains.push_back(cluster_iter->second.domain);
                        assert(instance_domain_ids.size() == instance_domains.size());
                    }
                    metric_table[metric_index].m_desc.indom = indom;
                } else {
                    metric_table[metric_index].m_desc.indom = PM_INDOM_NULL;
                }
                metric_table[metric_index].m_user = description.opaque;
                metric_index++;
            }
        }
        assert(instance_domain_ids.size() == indom_count);
        assert(instance_domains.size() == indom_count);
        assert(metric_index == metric_count);

        // Assign our callback function pointers to the interface struct.
        set_callbacks(interface);

        // Initialize the PMDA interface.
        pmdaInit(&interface, indom_table, indom_count, metric_table, metric_count);

        // Record the pmdaIndom values as updated by pmdaInit.
        for (size_t indom_index = 0; indom_index < indom_count; ++indom_index) {
            instance_domain * const domain = instance_domains.at(indom_index);
            domain->set_pm_instance_domain(indom_table[indom_index].it_indom);
            this->instance_domains.insert(std::make_pair(domain->get_domain_id(), domain));
            this->instance_domains.insert(std::make_pair(domain->get_pm_instance_domain(), domain));
        }
    }

    /**
     * @brief Get descriptions of all of the metrics supported by this PMDA.
     *
     * Dervied classes must implement this function to define the metrics
     * supported by this PMDA.
     *
     * @return Descriptions of all of the metrics supported by this PMDA.
     */
    virtual pcp::metrics_description get_supported_metrics() = 0;

    /**
     * @brief Begin fetching values.
     *
     * Derived classes may override this function to perform any actions they
     * wish to perform at the start of each batch of fetch of metric values.
     *
     * This base implementation performs no actions.
     */
    virtual void begin_fetch_values() { }

    /**
     * @brief Fetch an individual metric value.
     *
     * Derived classes must implment this function to fetch individual metric
     * values.
     *
     * If the requested metric value is not found, or some other error occurs,
     * implementations should throw an appropriate pcp::exception, such as
     * `pcp::exception(PMDA_FETCH_NOVALUES)`.
     *
     * Otherwise, the a valid atom value should be returned, encapsulated in a
     * fetch_value_result struct.  Typically the \c code value of the returned
     * struct should be left as `PMDA_FETCH_STATIC`. However, advanced PMDAs may
     * use any of the `PMDA_FETCH_*` constants, such as `PMDA_FETCH_DYNAMIC`.
     *
     * Note, implementations should not return `PMDA_FETCH_NOVALUES` - in that
     * case, they should throw `pcp::exception(PMDA_FETCH_NOVALUES)` instead.
     *
     * @param metric The metric to fetch the value of.
     *
     * @throw pcp::exception on error, or if the requested metric is not
     *                       currently available.
     *
     * @return The value of the requested metric.
     */
    virtual fetch_value_result fetch_value(const metric_id &metric) = 0;

    /**
     * @brief Store an in situ value.
     *
     * Derived classes may override this function to allow PCP to request metric
     * values to be stored.
     *
     * This base implementation throws `pcp::exception(PM_ERR_PERMISSION)`.
     *
     * @note Unless `PCP_CPP_NO_ID_VALIDITY_CHECKS` has been defined, this
     *       function will not be called for any metric that did not include
     *       the `storable_metric` flag in the get_supported_metrics result.
     *
     * @param metric The metric to store.
     * @param value  The value to store.
     *
     * @throw pcp::exception on error.
     */
    virtual void store_value(const metric_id &metric, const int &value)
    {
        PCP_CPP_UNUSED(metric)
        PCP_CPP_UNUSED(value)
        throw pcp::exception(PM_ERR_PERMISSION);
    }

    /**
     * @brief Store an ex situ value.
     *
     * Derived classes may override this function to allow PCP to request metric
     * values to be stored.
     *
     * This base implementation throws `pcp::exception(PM_ERR_PERMISSION)`.
     *
     * @note Unless `PCP_CPP_NO_ID_VALIDITY_CHECKS` has been defined, this
     *       function will not be called for any metric that did not include
     *       the `storable_metric` flag in the get_supported_metrics result.
     *
     * @param metric The metric to store.
     * @param value  The value to store.
     *
     * @throw pcp::exception on error.
     */
    virtual void store_value(const metric_id &metric,
                             const pmValueBlock * const value)
    {
        PCP_CPP_UNUSED(metric)
        PCP_CPP_UNUSED(value)
        throw pcp::exception(PM_ERR_PERMISSION);
    }

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
            pmNotifyErr(LOG_ERR, "%s", ex.what());
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
            id.cluster = pmID_cluster(mdesc->m_desc.pmid);
            id.instance = inst;
            id.item = pmID_item(mdesc->m_desc.pmid);
            id.opaque = mdesc->m_user;

#ifdef PCP_CPP_NO_ID_VALIDITY_CHECKS
            id.type = PM_TYPE_UNKNOWN;
#else
            const metric_description &description =
                supported_metrics.at(id.cluster).at(id.item);
            id.type = description.type;
            validate_instance(description, inst);
#endif

            // Fetch the metric value.
            const fetch_value_result result = fetch_value(id);
            *avp = result.atom;
#if PCP_CPP_PMDA_INTERFACE_VERSION <= 2
            return 0; // "No error" for PMDA interface 2.
#elif PCP_CPP_PMDA_INTERFACE_VERSION < 4
            return 1; // "Metric found" for PMDA interfaces 3 and 4.
#else // PCP_CPP_PMDA_INTERFACE_VERSION >= 5
            return result.code; // PMDA_FETCH_* values for inerfaces 5+
#endif
        } catch (const pcp::exception &ex) {
            if (ex.error_code() != PMDA_FETCH_NOVALUES) {
                pmNotifyErr(LOG_ERR, "%s", ex.what());
            }
            return ex.error_code();
        } catch (const std::out_of_range &ex) {
            pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
            return PM_ERR_PMID; // Unknown or illegal metric identifier.
        } catch (const std::exception &ex) {
            pmNotifyErr(LOG_ERR, "%s", ex.what());
            return PM_ERR_GENERIC;
        } catch (...) {
            pmNotifyErr(LOG_ERR, "unknown exception in on_fetch_callback");
            return PM_ERR_GENERIC;
        }
    }

    /// @brief Return description of instances and instance domains.
    virtual int on_instance(pmInDom indom, int inst, char *name,
                            pmInResult **result, pmdaExt *pmda)
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
    virtual int on_profile(pmProfile *prof, pmdaExt *pmda)
    {
        return pmdaProfile(prof, pmda);
    }

    /// @brief Store a value into a metric.
    virtual int on_store(pmResult *result, pmdaExt *pmda)
    {
        pmNotifyErr(LOG_INFO, "on store");
        try {
            bool any_stored = false;
            for (int value_set_index = 0; value_set_index < result->numpmid; ++value_set_index) {
                pmValueSet * const value_set = result->vset[value_set_index];

                // Setup the metric ID.
                metric_id id;
                id.cluster = pmID_cluster(value_set->pmid);
                id.item = pmID_item(value_set->pmid);
                id.opaque = NULL;
                id.type = PM_TYPE_UNKNOWN;

                for (int instance_index = 0; instance_index < value_set->numval; ++instance_index) {
                    id.instance = value_set->vlist[instance_index].inst;
#ifndef PCP_CPP_NO_ID_VALIDITY_CHECKS
                    const metric_description &description =
                        supported_metrics.at(id.cluster).at(id.item);
                    id.type = description.type;

                    validate_instance(description, id.instance);

                    if (!(description.flags & pcp::storable_metric)) {
                        // Metric does not support storing values.
                        throw pcp::exception(PM_ERR_PERMISSION);
                    }
#endif
                    if (value_set->valfmt == PM_VAL_INSITU) {
                        store_value(id, value_set->vlist[instance_index].value.lval);
                    } else {
                        store_value(id, value_set->vlist[instance_index].value.pval);
                    }
                    any_stored = true;
                }
            }
            if (any_stored) {
                return 0; // >= 0 implies success
            }
        } catch (const pcp::exception &ex) {
            if (ex.error_code() != PMDA_FETCH_NOVALUES) {
                pmNotifyErr(LOG_ERR, "%s", ex.what());
            }
            return ex.error_code();
        } catch (const std::out_of_range &ex) {
            pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
            return PM_ERR_PMID; // Unknown or illegal metric identifier.
        } catch (const std::exception &ex) {
            pmNotifyErr(LOG_ERR, "%s", ex.what());
            return PM_ERR_GENERIC;
        } catch (...) {
            pmNotifyErr(LOG_ERR, "unknown exception in on_fetch_callback");
            return PM_ERR_GENERIC;
        }
        return pmdaStore(result, pmda); // Just returns PM_ERR_PERMISSION.
    }

    /// @brief Return the help text for the metric.
    virtual int on_text(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        try {
            const bool get_one_line = ((type & PM_TEXT_ONELINE) == PM_TEXT_ONELINE);
            if ((type & PM_TEXT_PMID) == PM_TEXT_PMID) {
                const metric_description &description =
                    supported_metrics.at(pmID_cluster(ident)).at(pmID_item(ident));
                const std::string &text = get_one_line
                    ? description.short_description.empty()
                        ? description.verbose_description
                        : description.short_description
                    : description.verbose_description.empty()
                        ? description.short_description
                        : description.verbose_description;
                if (text.empty()) {
                    throw pcp::exception(PM_ERR_TEXT);
                }
                *buffer = strdup(text.c_str());
                return 0; // >= 0 implies success.
            } else if ((type & PM_TEXT_INDOM) == PM_TEXT_INDOM) {
                const pcp::instance_info &info =
                    instance_domains.at(pmInDom_domain(ident))->at(pmInDom_serial(ident));
                const std::string &text = get_one_line
                    ? info.short_description.empty()
                        ? info.verbose_description
                        : info.short_description
                    : info.verbose_description.empty()
                        ? info.short_description
                        : info.verbose_description;
                if (text.empty()) {
                    throw pcp::exception(PM_ERR_TEXT);
                }
                *buffer = strdup(text.c_str());
                return 0; // >= 0 implies success.
            } else {
                pmNotifyErr(LOG_NOTICE, "unknown text type 0x%x", type);
            }
        } catch (const pcp::exception &ex) {
            if (ex.error_code() != PM_ERR_TEXT) {
                pmNotifyErr(LOG_NOTICE, "%s", ex.what());
            } else {
                pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
            }
        } catch (const std::out_of_range &ex) {
            pmNotifyErr(LOG_DEBUG, "%s:%d:%s %s", __FILE__, __LINE__, __FUNCTION__, ex.what());
        } catch (const std::exception &ex) {
            pmNotifyErr(LOG_NOTICE, "%s", ex.what());
        } catch (...) {
            pmNotifyErr(LOG_ERR, "unknown exception in on_text");
            return PM_ERR_GENERIC;
        }
        return pmdaText(ident, type, buffer, pmda);
    }

    /**
     * @brief Set static callbacks on a PMDA interface.
     *
     * This function sets our static PMDA callback functions on the given
     * pmdaInterface struct.  These static callback functions then redirect all
     * calls to the current singleton PMDA instance.
     *
     * @param interface The interface to set our callbacks on.
     */
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
    std::stack<void *> free_on_destruction;
    std::map<pmInDom, instance_domain *> instance_domains;

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
            << "/* The " << get_pmda_name() << " PMDA's domain number. */" << std::endl
            << "#define " << upper_name << ' ' << get_default_pmda_domain_number() << std::endl;
    }

    void export_help_text(const std::string &filename) const
    {
        // Generated ordered maps of metric names and instance IDs to help texts.
        std::map<std::string, metric_description> metrics;
        std::map<domain_id_type, const instance_domain *> instances;
        const std::string pmda_name = get_pmda_name();
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin();
             metrics_iter != supported_metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                 cluster_iter != cluster.end(); ++cluster_iter)
            {
                const metric_description &metric = cluster_iter->second;
                std::string metric_name = pmda_name;
                if (!cluster_name.empty()) {
                    metric_name += '.' + cluster_name;
                }
                metric_name += '.' + metric.metric_name;
                metrics.insert(std::make_pair(metric_name, metric));
                if (metric.domain != NULL) {
                    instances.insert(std::make_pair(metric.domain->get_domain_id(), metric.domain));
                }
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
        for (std::map<std::string, metric_description>::const_iterator metric = metrics.begin();
             metric != metrics.end(); ++metric)
        {
            stream << "@ " << metric->first << ' ' << metric->second.short_description << std::endl;
            if (!metric->second.verbose_description.empty()) {
                stream << metric->second.verbose_description << std::endl;
            }
            stream << std::endl;
        }
        for (std::map<domain_id_type, const instance_domain *>::const_iterator indom = instances.begin();
             indom != instances.end(); ++indom)
        {
            for (instance_domain::const_iterator instance = indom->second->begin();
                 instance != indom->second->end(); ++instance)
            {
                stream << "@ " << indom->first << '.' << instance->first
                       << ' ' << instance->second.short_description << std::endl;
                if (!instance->second.verbose_description.empty()) {
                    stream << instance->second.verbose_description << std::endl;
                }
                stream << std::endl;
            }
        }
    }

    void export_pmns_data(const std::string &filename) const
    {
        // Some basic strings we'll use a couple of times.
        const std::string &pmda_name = get_pmda_name();
        std::string upper_name = get_pmda_name();
        std::transform(upper_name.begin(), upper_name.end(), upper_name.begin(), ::toupper);

        // Open the output file.
        std::ofstream file_stream;
        if (filename != "-") {
            file_stream.open(filename.c_str());
            if (!file_stream.is_open()) {
                throw pcp::exception(PM_ERR_GENERIC, "failed to open file for writing: " + filename);
            }
        }
        std::ostream &stream = (filename == "-") ? std::cout : file_stream;

        // Define the PMID, if not already.
        stream
            << std::endl
            << "#ifndef " << upper_name << std::endl
            << "#define " << upper_name << ' ' << get_default_pmda_domain_number() << std::endl
            << "#endif" << std::endl;

        // First pass to find the length of the longest metric name.
        std::string::size_type max_metric_name_size = 0;
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin();
             metrics_iter != supported_metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                 cluster_iter != cluster.end(); ++cluster_iter)
            {
                if (cluster_iter->second.metric_name.size() > max_metric_name_size) {
                    max_metric_name_size = cluster_iter->second.metric_name.size();
                }
            }
        }

        // Second pass to export the group names and ungrouped metrics.
        stream << std::endl << pmda_name << " {" << std::endl;
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin();
             metrics_iter != supported_metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            if (cluster_name.empty()) {
                for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                     cluster_iter != cluster.end(); ++cluster_iter)
                {
                    stream << "    " << cluster_iter->second.metric_name
                           << std::string(max_metric_name_size - cluster_iter->second.metric_name.size() + 4, ' ')
                           << upper_name << ':' << cluster.get_cluster_id() << ':'
                           << cluster_iter->first << std::endl;
                }
            } else {
                static std::string previous_cluster_name;
                if (cluster_name != previous_cluster_name) {
                    stream << "    " << cluster_name << std::endl;
                    previous_cluster_name = cluster_name;
                }
            }
        }
        stream << '}' << std::endl;

        // Third and final pass to export all metric groups.
        std::string previous_cluster_name;
        for (metrics_description::const_iterator metrics_iter = supported_metrics.begin();
             metrics_iter != supported_metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            const std::string cluster_name = cluster.get_cluster_name();
            if (!cluster_name.empty()) {
                if (cluster_name != previous_cluster_name) {
                    if (!previous_cluster_name.empty()) {
                        stream << "}" << std::endl;
                    }
                    stream << std::endl << pmda_name << '.' << cluster_name << " {" << std::endl;
                }
                for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                     cluster_iter != cluster.end(); ++cluster_iter)
                {
                    stream << "    " << cluster_iter->second.metric_name
                           << std::string(max_metric_name_size - cluster_iter->second.metric_name.size() + 4, ' ')
                           << upper_name << ':' << cluster.get_cluster_id() << ':'
                           << cluster_iter->first << std::endl;
                }
                previous_cluster_name = cluster_name;
            }
        }
        if (!previous_cluster_name.empty()) {
            stream << "}" << std::endl;
        }
        stream << std::endl;
    }

    void export_pmns_root(const std::string &filename) const
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
        stream
            << std::endl
            << "root { " << get_pmda_name() << " }" << std::endl << std::endl
            << "#include \"pmns\"" << std::endl << std::endl;
    }

    void export_support_files(const std::string &directory_name) const
    {
        const std::string sep(1, pmPathSeparator());
        export_domain_header(directory_name + sep + "domain.h");
        export_help_text(directory_name + sep + "help");
        export_pmns_data(directory_name + sep + "pmns");
        export_pmns_root(directory_name + sep + "root");
    }

    inline std::pair<size_t, size_t> count_metrics(const metrics_description &metrics) const
    {
        std::set<const instance_domain *> instance_domains;
        size_t metric_count = 0;
        for (metrics_description::const_iterator metrics_iter = metrics.begin();
             metrics_iter != metrics.end(); ++metrics_iter)
        {
            const metric_cluster &cluster = metrics_iter->second;
            for (metric_cluster::const_iterator cluster_iter = cluster.begin();
                 cluster_iter != cluster.end(); ++cluster_iter)
            {
                if (cluster_iter->second.domain != NULL) {
                    instance_domains.insert(cluster_iter->second.domain);
                }
                metric_count++;
            }
        }
        return std::pair<size_t, size_t>(metric_count, instance_domains.size());
    }

    static inline pmdaIndom allocate_pmda_indom(const instance_domain &domain)
    {
        pmdaIndom indom;
        indom.it_indom = domain.get_domain_id();
        indom.it_numinst = domain.size();
        indom.it_set = new pmdaInstid [domain.size()];
        size_t index = 0;
        for (instance_domain::const_iterator iter = domain.begin(); iter != domain.end(); ++iter, ++index) {
            indom.it_set[index].i_inst = iter->first;
            indom.it_set[index].i_name = const_cast<char *>(iter->second.instance_name.c_str());
        }
        return indom;
    }

    static inline void validate_instance(const metric_description &description,
                                         const unsigned int instance)
    {
#ifndef PCP_CPP_NO_ID_VALIDITY_CHECKS
        if (instance != PM_INDOM_NULL) {
            if (description.domain == NULL) {
                // Instance provided, but non required.
                throw pcp::exception(PM_ERR_INDOM);
            }
            if (description.domain->count(instance) <= 0) {
                // Instance provided, but not one we've registered.
                throw pcp::exception(PM_ERR_INST);
            }
        } else if (description.domain != NULL) {
            // Instance required, but none provided.
            throw pcp::exception(PM_ERR_INDOM);
        }
#endif
    }

    /*
     * Static callback functions registered by the register_callbacks functions.
     * These all redirect thier non-static singleton counterparts above.
     */

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
    static int callback_attribute(int ctx, int attr, const char *value, int length, pmdaExt *pmda)
    {
        return get_instance()->on_attribute(ctx, attr, value, length, pmda);
    }
#endif

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    static int callback_children(const char *name, int traverse, char ***kids, int **sts, pmdaExt *pmda)
    {
        return get_instance()->on_children(name, traverse, kids, sts, pmda);
    }
#endif

    static int callback_desc(pmID pmid, pmDesc *desc, pmdaExt *pmda)
    {
        return get_instance()->on_desc(pmid, desc, pmda);
    }

    static int callback_fetch(int numpmid, pmID *pmidlist, pmResult **resp, pmdaExt *pmda)
    {
        return get_instance()->on_fetch(numpmid, pmidlist, resp, pmda);
    }

    static int callback_fetch_callback(pmdaMetric *mdesc, unsigned int inst, pmAtomValue *avp)
    {
        return get_instance()->on_fetch_callback(mdesc, inst, avp);
    }

    static int callback_instance(pmInDom indom, int inst, char *name, pmInResult **result, pmdaExt *pmda)
    {
        return get_instance()->on_instance(indom, inst, name, result, pmda);
    }

#if PCP_CPP_PMDA_INTERFACE_VERSION >= 4
    static int callback_name(pmID pmid, char ***nameset, pmdaExt *pmda)
    {
        return get_instance()->on_name(pmid, nameset, pmda);
    }

    static int callback_pmid(const char *name, pmID *pmid, pmdaExt *pmda)
    {
        return get_instance()->on_pmid(name, pmid, pmda);
    }
#endif

    static int callback_profile(pmProfile *prof, pmdaExt *pmda)
    {
        return get_instance()->on_profile(prof, pmda);
    }

    static int callback_store(pmResult *result, pmdaExt *pmda)
    {
        return get_instance()->on_store(result, pmda);
    }

    static int callback_text(int ident, int type, char **buffer, pmdaExt *pmda)
    {
        return get_instance()->on_text(ident, type, buffer, pmda);
    }

};

} // pcp namespace.

#ifndef PCP_CPP_SKIP_PCP_PMDA_INSTANCE_DEFINITION
pcp::pmda * pcp::pmda::instance(NULL);
#endif

PCP_CPP_END_NAMESPACE

#endif
