//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_INTERFACE_HPP__
#define __PCP_CPP_INTERFACE_HPP__

#include "../config.hpp"

#include <pcp/pmda.h>

extern "C" {

int pmda_attribute_callback(int ctx, int attr, const char *value, int length, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_attribute(ctx, attr, value, length, pmda);
}

int pmda_children_callback(const char *name, int traverse, char ***kids, int **sts, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_children(name, traverse, kids, sts, pmda);
}

int pmda_desc_callback(pmID pmid, pmDesc *desc, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_desc(pmid, *desc, *pmda);
}

int pmda_fetch_callback(int numpmid, pmID *pmidlist, pmResult **resp, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_fetch(numpmid, *pmidlist, *resp, *pmda);
}

int pmda_instance_callback(pmInDom indom, int inst, char *name, __pmInResult **result, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_instance(indom, inst, name, *result, *pmda);
}

int pmda_name_callback(pmID pmid, char ***nameset, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_name(pmid, nameset, pmda);
}

int pmda_pmid_callback(const char *name, pmID *pmid, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_pmid(name, pmid, pmda);
}

int pmda_profile_callback(__pmProfile *prof, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_profile(*prof, *pmda);
}

int pmda_store_callback(pmResult *result, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_store(*result, *pmda);
}

int pmda_text_callback(int ident, int type, char **buffer, pmdaExt *pmda)
{
    return pcp::pmda::getInstance()->on_pmda_text(ident, type, *buffer, *pmda);
}

} // extern "C"

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

void setPmdaCallbacks(pmdaInterface &interface)
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

    interface.version.any.ext;
}

} // namespace pcp

PCP_CPP_END_NAMESPACE

#endif
