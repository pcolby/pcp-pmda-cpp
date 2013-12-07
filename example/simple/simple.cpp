//               Copyright Paul Colby 2013.
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
 * @see http://oss.sgi.com/cgi-bin/gitweb.cgi?p=pcp/pcp.git;a=blob;f=src/pmdas/simple/simple.c
 */

#include <pcp-cpp/atom.hpp>
#include <pcp-cpp/exception.hpp>
#include <pcp-cpp/instance_domain.hpp>
#include <pcp-cpp/metric_description.hpp>
#include <pcp-cpp/pmda.hpp>
#include <pcp-cpp/units.hpp>

#include <sys/stat.h>

static struct timeslice {
    int tm_field;
    int inst_id;
    const char *tm_name;
} timeslices[] = {
    { 0, 1, "sec" }, { 0, 60, "min" }, { 0, 3600, "hour" }
};

static int num_timeslices = sizeof(timeslices)/sizeof(timeslices[0]);

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

    virtual int default_pmda_domain_number() const
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
            (0, "numfetch",pcp::type<uint32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), pcp::storable_metric)
            (1, "color",pcp::type<int32_t>(), PM_SEM_INSTANT,
             pcp::units(0,0,0, 0,0,0), &color_domain, pcp::storable_metric)
        (1, "time")
            (2, "user",pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
            (3, "sys",pcp::type<double>(), PM_SEM_COUNTER,
             pcp::units(0,1,0, 0,PM_TIME_SEC,0))
        (2)
            (4, "now",pcp::type<uint32_t>(), PM_SEM_INSTANT,
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
            /// @todo Issue #4: pmdaCache encapsulation.
            void * tsp;
            int sts;
            if ((sts = pmdaCacheLookup(now_domain, metric.instance, NULL, &tsp)) != PMDA_CACHE_ACTIVE) {
                if (sts < 0)
                    __pmNotifyErr(LOG_ERR, "pmdaCacheLookup failed: inst=%d: %s", metric.instance, pmErrStr(sts));
                throw pcp::exception(PM_ERR_INST);
            }
            return pcp::atom(metric.type, static_cast<timeslice *>(tsp)->tm_field);
        }

        // simple.numfetch    SIMPLE:0:0
        if (metric.item == 0) {
            return pcp::atom(metric.type, numfetch);
        }

        // simple.color       SIMPLE:0:1
        rgb[metric.instance] = (rgb[metric.instance] + 1) % 256;
        return pcp::atom(metric.type, rgb[metric.instance]);

        if (metric.cluster == 0) {
            if (metric.item == 0) {
                // simple.numfetch    SIMPLE:0:0
                return pcp::atom(metric.type, numfetch);
            } else if (metric.item == 1) {
                // simple.color       SIMPLE:0:1
                static int32_t rgb[] = { 0, 100, 200 };
                rgb[metric.instance] = (rgb[metric.instance] + 1) % 256;
                return pcp::atom(metric.type, rgb[metric.instance]);
            }
        } else if (metric.cluster == 1) {
            // simple.time.user   SIMPLE:1:2
            // simple.time.sys    SIMPLE:1:3
            static uint32_t oldfetch(0);
            static double usr, sys;
            if (oldfetch < numfetch) {
                __pmProcessRunTimes(&usr, &sys);
                oldfetch = numfetch;
            }
            return pcp::atom(metric.type, metric.item == 3 ? usr : sys);
        } else if (metric.cluster == 2) {
            // simple.now         SIMPLE:2:4
            /// @todo Issue #4: pmdaCache encapsulation.
            void * tsp;
            int sts;
            if ((sts = pmdaCacheLookup(now_domain, metric.instance, NULL, &tsp)) != PMDA_CACHE_ACTIVE) {
                if (sts < 0)
                    __pmNotifyErr(LOG_ERR, "pmdaCacheLookup failed: inst=%d: %s", metric.instance, pmErrStr(sts));
                throw pcp::exception(PM_ERR_INST);
            }
            return pcp::atom(metric.type, static_cast<timeslice *>(tsp)->tm_field);
        }

        throw pcp::exception(PM_ERR_INST);
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
        struct stat     statbuf;
        static int      last_error = 0;
        int         sep = __pmPathSeparator();
        char mypath[MAXPATHLEN];
        static struct stat file_change; ///< Time of last configuration change.

        /* stat the file & check modification time has changed */
        snprintf(mypath, sizeof(mypath), "%s%c" "simple" "%c" "simple.conf",
                 pmGetConfig("PCP_PMDAS_DIR"), sep, sep);
        if (stat(const_cast<const char *>(mypath), &statbuf) == -1) {
            if (oserror() != last_error) {
                last_error = oserror();
                __pmNotifyErr(LOG_ERR, "stat failed on %s: %s\n",
                              mypath, pmErrStr(-last_error));
            }
            timenow_clear();
        } else {
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
        const int sts = pmdaCacheOp(now_domain, PMDA_CACHE_INACTIVE);
        if (sts < 0) {
            __pmNotifyErr(LOG_ERR, "pmdaCacheOp(INACTIVE) failed: indom=%s: %s",
                          pmInDomStr(now_domain), pmErrStr(sts));
        }
#ifdef DESPERATE
        __pmdaCacheDump(stderr, now_domain, 1);
#endif
    }

    void timenow_init()
    {
        int     i;
        int     sts;
        int     sep = __pmPathSeparator();
        FILE    *fp;
        char    *p, *q;
        char    buf[256];
        char mypath[MAXPATHLEN];

        snprintf(mypath, sizeof(mypath), "%s%c" "simple" "%c" "simple.conf",
                 pmGetConfig("PCP_PMDAS_DIR"), sep, sep);
        if ((fp = fopen(mypath, "r")) == NULL) {
            __pmNotifyErr(LOG_ERR, "fopen on %s failed: %s\n",
                          mypath, pmErrStr(-oserror()));
            return;
        }
        if ((p = fgets(&buf[0], sizeof(buf), fp)) == NULL) {
            __pmNotifyErr(LOG_ERR, "fgets on %s found no data\n", mypath);
            fclose(fp);
            return;
        }
        if ((q = strchr(p, '\n')) != NULL)
            *q = '\0';      /* remove eol character */

        q = strtok(p, ",");     /* and refresh using the updated file */
        while (q != NULL) {
            for (i = 0; i < num_timeslices; i++) {
                if (strcmp(timeslices[i].tm_name, q) == 0) {
                    sts = pmdaCacheStore(now_domain, PMDA_CACHE_ADD, q, &timeslices[i]);
                    if (sts < 0) {
                        __pmNotifyErr(LOG_ERR, "pmdaCacheStore failed: %s", pmErrStr(sts));
                        fclose(fp);
                        return;
                    }
                    now_domain(sts, q);
                    break;
                }
            }
            if (i == num_timeslices)
                __pmNotifyErr(LOG_WARNING, "ignoring \"%s\" in %s", q, mypath);
            q = strtok(NULL, ",");
        }
#ifdef DESPERATE
        __pmdaCacheDump(stderr, now_domain, 1);
#endif
        if (pmdaCacheOp(now_domain, PMDA_CACHE_SIZE_ACTIVE) < 1)
            __pmNotifyErr(LOG_WARNING, "\"timenow\" instance domain is empty");
        fclose(fp);
    }

    void timenow_refresh()
    {
        const time_t t = time(NULL);
        struct tm * const tptr = localtime(&t);
        timeslices[0].tm_field = tptr->tm_sec;
        timeslices[1].tm_field = tptr->tm_min;
        timeslices[2].tm_field = tptr->tm_hour;
    }

};

// DSO entry point.
extern "C" void trivial_init(pmdaInterface *interface)
{
    pcp::pmda::init_dso<simple>(interface);
}

// Daemon entry point.
int main(int argc, char *argv[])
{
    return pcp::pmda::run_daemon<simple>(argc, argv);
}
