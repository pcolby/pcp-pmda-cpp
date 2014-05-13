//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <pcp/pmapi.h> // Note, the order in which these are included matters
#include <pcp/impl.h>  // more for older versions of PCP, so don't reorder them
#include <pcp/pmda.h>  // without testing against older versions of PCP.

extern "C" {

int pmdaAttribute(int /*context*/, int /*key*/, const char */*value*/, int /*length*/,
                  pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

int pmdaChildren(const char */*name*/, int /*traverse*/, char ***/*offspring*/,
                 int **/*status*/, pmdaExt */*pmda*/)
{
    return PM_ERR_NYI;
}

void pmdaConnect(pmdaInterface */*dispatch*/)
{

}

void pmdaDaemon(pmdaInterface */*dispatch*/, int /*interface*/, char */*name*/,
                int /*domain*/, char */*logfile*/, char */*helptext*/)
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
                 __pmInResult **/*result*/, pmdaExt */*pmda*/)
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

int pmdaProfile(__pmProfile */*prof*/, pmdaExt */*pmda*/)
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
