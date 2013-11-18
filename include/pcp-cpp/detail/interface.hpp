//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_INTERFACE_HPP__
#define __PCP_CPP_INTERFACE_HPP__

#include "../config.hpp"

extern "C" {

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
    interface.version.any.ext;
    interface.version.any.profile = &pmda_profile_callback;
    interface.version.any.fetch;
    interface.version.any.desc;
    interface.version.any.instance;
    interface.version.any.text;
    interface.version.any.store;
    #if PCP_CPP_PMDA_INTERFACE_VERSION >= 5
    interface.version.five.pmid;
    interface.version.five.name;
    interface.version.five.children;
    #endif
    #if PCP_CPP_PMDA_INTERFACE_VERSION >= 6
    interface.version.six.attribute;
    #endif
}

} // namespace pcp

PCP_CPP_END_NAMESPACE

#endif
