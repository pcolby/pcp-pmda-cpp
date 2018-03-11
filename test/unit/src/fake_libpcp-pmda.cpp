//               Copyright Paul Colby 2013 - 2018.
//               Copyright Red Hat 2018.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <pcp/pmapi.h> // Note, the order in which these are included matters
#include <pcp/impl.h>  // more for older versions of PCP, so don't reorder them
#include <pcp/pmda.h>  // without testing against older versions of PCP.

// PM_VERSION macro was not added until PCP 3.10.5.
#ifndef PM_VERSION
#define PM_VERSION(a,b,c) (((a)<<16)|((b)<<8)|(c))
#endif

// PCP 4.0.0 clean and promoted some functions, renaming them in the process.
#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < PM_VERSION(4,0,0)
#define pmInResult __pmInResult
#define pmProfile __pmProfile
#endif

extern "C" {

int pmdaAttribute(int /*context*/, int /*key*/, const char */*value*/, int /*length*/,
                  pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

int pmdaCacheLookup(pmInDom indom, int, char **, void **)
{
    return indom;
}

int pmdaCacheLookupName(pmInDom indom, const char *, int *, void **)
{
    return indom;
}

int pmdaCacheLookupKey(pmInDom indom, const char *, int, const void *, char **, int *, void **)
{
    return indom;
}

int pmdaCacheOp(pmInDom indom, int op)
{
    return ((int)indom < 0) ? indom : op;
}

int pmdaCachePurge(pmInDom indom, time_t recent)
{
    return ((int)indom < 0) ? indom : recent;
}

int pmdaCacheStore(pmInDom indom, int, const char *, void *)
{
    return indom;
}

int pmdaCacheStoreKey(pmInDom indom, int, const char *, int, const void *, void *)
{
    return indom;
}

int pmdaChildren(const char */*name*/, int /*traverse*/, char ***/*offspring*/,
                 int **/*status*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

void pmdaConnect(pmdaInterface */*dispatch*/)
{

}

// PCP 3.11.5 updated the pmdaDaemon signature to use const char pointers.
// Note, the PM_VERSION* macros themselves weren't added until PCP 3.10.5.
#if !defined PM_VERSION_CURRENT || PM_VERSION_CURRENT < 0x30B05 // 3.11.5
void pmdaDaemon(pmdaInterface */*dispatch*/, int /*interface*/, char */*name*/,
                int /*domain*/, char */*logfile*/, char */*helptext*/)
#else
void pmdaDaemon(pmdaInterface */*dispatch*/, int /*interface*/, const char */*name*/,
                int /*domain*/, const char */*logfile*/, const char */*helptext*/)
#endif
{

}

int pmdaDesc(pmID /*pmid*/, pmDesc */*desc*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

// Contrary to the man pages, pmdaDSO returns void, not int.
void pmdaDSO(pmdaInterface *, int , char *, char *)
{

}

int pmdaFetch(int /*numpmid*/, pmID */*pmidlist*/, pmResult **/*resp*/,
              pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

void pmdaInit(pmdaInterface *dispatch, pmdaIndom *indoms,
              int nindoms, pmdaMetric *metrics, int nmetrics)
{
    dispatch->version.two.ext = new pmdaExt;
    memset(dispatch->version.two.ext, 0, sizeof(pmdaExt));
    dispatch->version.two.ext->e_indoms = indoms;
    dispatch->version.two.ext->e_nindoms = nindoms;
    dispatch->version.two.ext->e_metrics = metrics;
    dispatch->version.two.ext->e_nmetrics = nmetrics;
}

int pmdaInstance(pmInDom /*indom*/, int /*inst*/, char */*name*/,
                 pmInResult **/*result*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

void pmdaOpenLog(pmdaInterface */*dispatch*/)
{

}

void pmdaMain(pmdaInterface */*dispatch*/)
{

}

int pmdaPMID(const char */*name*/, pmID */*pmid*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

int pmdaProfile(pmProfile */*prof*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

int pmdaName(pmID /*pmid*/, char ***/*nameset*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

void pmdaSetFetchCallBack(pmdaInterface *dispatch,
                          pmdaFetchCallBack callback)
{
    if (dispatch->version.two.ext != NULL) {
        dispatch->version.two.ext->e_fetchCallBack = callback;
    } else {
        dispatch->status = PM_ERR_GENERIC;
    }
}

int pmdaStore(pmResult */*result*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

int pmdaText(int /*ident*/, int /*type*/, char **/*buffer*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

} // extern "C"
