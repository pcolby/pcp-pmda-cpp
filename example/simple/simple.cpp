//            Copyright Paul Colby 2013 - 2015.
//            Copyright Red Hat 2018
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/**
 * @file
 *
 * @brief Simple C++ PMDA
 *
 * Implements a C++ equivalent of PCP's Simple PMDA example.
 *
 * @see http://git.pcp.io/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmdas/simple/simple.c
 */

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/cache.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/units.hpp>

#include <sys/stat.h>

/*
 * internal routine from libpcp, defined in libpcp.h but not the
 * public headers
 */
extern "C" {
  int __pmProcessRunTimes(double *, double *);
}

static struct timeslice {
    int tm_field;
    int inst_id;
    const char *tm_name;
} timeslices[] = {
    { 0, 1, "sec" }, { 0, 60, "min" }, { 0, 3600, "hour" }
};

class simple : public pcp::pmda {

public:
    simple() : numfetch(0)
    {
        // Define the color and now instance domains.
        color_domain(0)(0, "red")(1, "green")(2, "blue");
        now_domain(1);

        rgb[0] = 0;
        rgb[1] = 100;
        rgb[2] = 200;
    }

    virtual std::string get_pmda_name() const
    {
        return "simple";
    }

    virtual int get_default_pmda_domain_number() const
    {
        return 253;
    }

protected:
    pcp::instance_domain color_domain;
    pcp::instance_domain now_domain;
    uint32_t numfetch;
    uint8_t rgb[3];

    virtual pcp::metrics_description get_supported_metrics()
    {
        // simple.numfetch    SIMPLE:0:0
        // simple.color       SIMPLE:0:1
        // simple.time.user   SIMPLE:1:2
        // simple.time.sys    SIMPLE:1:3
        // simple.now         SIMPLE:2:4
        return pcp::metrics_description()
        (0)
            (0, "numfetch", pcp::type<uint32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), pcp::storable_metric)
            (1, "color", pcp::type<int32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), &color_domain, pcp::storable_metric)
        (1, "time")
            (2, "user", pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
            (3, "sys", pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
        (2)
            (4, "now", pcp::type<uint32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), &now_domain);
    }

    virtual void begin_fetch_values()
    {
        numfetch++;
        timenow_check();
        timenow_refresh();
    }

    virtual fetch_value_result fetch_value(const metric_id &metric)
    {
        // simple.time.user   SIMPLE:1:2
        // simple.time.sys    SIMPLE:1:3
        if (metric.cluster == 1) {
            static uint32_t oldfetch(0);
            static double usr, sys;
            if (oldfetch < numfetch) {
                __pmProcessRunTimes(&usr, &sys);
                oldfetch = numfetch;
            }
            return pcp::atom(metric.type, metric.item == 3 ? usr : sys);
        }

        // simple.now         SIMPLE:2:4
        if (metric.cluster == 2) {
            const timeslice * const tsp =
                pcp::cache::lookup<timeslice *>(now_domain, metric.instance).opaque;
            return pcp::atom(metric.type, tsp->tm_field);
        }

        // simple.numfetch    SIMPLE:0:0
        if (metric.item == 0) {
            return pcp::atom(metric.type, numfetch);
        }

        // simple.color       SIMPLE:0:1
        rgb[metric.instance] = (rgb[metric.instance] + 1) % 256;
        return pcp::atom(metric.type, rgb[metric.instance]);
    }

    virtual void store_value(const metric_id &metric, const int &value)
    {
        // simple.numfetch    SIMPLE:0:0
        if (metric.item == 0) {
            numfetch = value;
            return;
        }

        // simple.color       SIMPLE:0:1
        if (value < 0) {
            throw pcp::exception(PM_ERR_SIGN);
        }
        if (value > 255) {
            throw pcp::exception(PM_ERR_CONV);
        }
        rgb[metric.instance] = value;
    }

private:
    void timenow_check()
    {
        static const std::string sep(1, pmPathSeparator());
        static const std::string config_filename =
            pmGetConfig("PCP_PMDAS_DIR") + sep + "simple" + sep + "simple.conf";
        static int last_error = 0;

        /* stat the file & check modification time has changed */
        struct stat statbuf;
        if (stat(config_filename.c_str(), &statbuf) == -1) {
            if (oserror() != last_error) {
                last_error = oserror();
                pmNotifyErr(LOG_ERR, "stat failed on %s: %s\n",
                              config_filename.c_str(), pmErrStr(-last_error));
            }
            timenow_clear();
        } else {
            static struct stat file_change; ///< Time of last configuration change.
            last_error = 0;
#if defined(HAVE_ST_MTIME_WITH_E)
            if (statbuf.st_mtime != file_change.st_mtime) {
#elif defined(HAVE_ST_MTIME_WITH_SPEC)
            if (statbuf.st_mtimespec.tv_sec != file_change.st_mtimespec.tv_sec ||
                    statbuf.st_mtimespec.tv_nsec != file_change.st_mtimespec.tv_nsec) {
#else
            if (statbuf.st_mtim.tv_sec != file_change.st_mtim.tv_sec ||
                    statbuf.st_mtim.tv_nsec != file_change.st_mtim.tv_nsec) {
#endif
                timenow_clear();
                timenow_init();
                file_change = statbuf;
            }
        }
    }

    void timenow_clear()
    {
        pcp::cache::perform(now_domain, PMDA_CACHE_INACTIVE);
#ifdef DESPERATE
        __pmdaCacheDump(stderr, now_domain, 1);
#endif
    }

    void timenow_init()
    {
        static const std::string sep(1, pmPathSeparator());
        static const std::string config_filename =
            pmGetConfig("PCP_PMDAS_DIR") + sep + "simple" + sep + "simple.conf";

        std::ifstream file(config_filename.c_str());
        if (!file.is_open()) {
            pmNotifyErr(LOG_ERR, "failed to open %s", config_filename.c_str());
            return;
        }

        std::string line;
        if (!std::getline(file, line)) {
            pmNotifyErr(LOG_ERR, "failed to read from %s", config_filename.c_str());
            return;
        }

        for (std::string::size_type start = 0, end = line.find(',');
             start != std::string::npos;
             start = (end == std::string::npos) ? end : end+1,
             end = (end == std::string::npos) ? end : line.find(',', end+1))
        {
            const std::string name = line.substr(start, end - start);
            if (!name.empty()) {
                static size_t num_timeslices = sizeof(timeslices)/sizeof(timeslices[0]);
                size_t index;
                for (index = 0; index < num_timeslices; ++index) {
                    if (name == timeslices[index].tm_name) {
                        const pcp::instance_id_type instance_id =
                            pcp::cache::store(now_domain, name, &timeslices[index]);
                        now_domain(instance_id, name);
                        break;
                    }
                }
                if (index == num_timeslices) {
                    pmNotifyErr(LOG_WARNING, "ignoring \"%s\" in %s",
                                  name.c_str(), config_filename.c_str());
                }
            }
        }
#ifdef DESPERATE
        __pmdaCacheDump(stderr, now_domain, 1);
#endif
        if (pcp::cache::perform(now_domain, PMDA_CACHE_SIZE_ACTIVE) < 1) {
            pmNotifyErr(LOG_WARNING, "\"timenow\" instance domain is empty");
        }
    }

    void timenow_refresh()
    {
        const time_t t = time(NULL);
        struct tm ts;
        localtime_r(&t, &ts);
        timeslices[0].tm_field = ts.tm_sec;
        timeslices[1].tm_field = ts.tm_min;
        timeslices[2].tm_field = ts.tm_hour;
    }

};

// DSO entry point.
extern "C" void simple_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<simple>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<simple>(argc, argv);
}
