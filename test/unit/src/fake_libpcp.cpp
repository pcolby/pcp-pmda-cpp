#include <pcp/pmapi.h>

#include <boost/lexical_cast.hpp>

#include <string>

/**
 * Drop-in pmErrStr_r replacement that always returns the \a code itself as a
 * string. This is helpful for testing only.
 */
char *pmErrStr_r(int code, char *buf, int buflen)
{
    if (buflen < PM_MAXERRMSGLEN) {
        throw std::length_error(
            "pmErrStr_r: buflen (" +
            boost::lexical_cast<std::string>(buflen) +
            ") less than PM_MAXERRMSGLEN (" +
                    boost::lexical_cast<std::string>(buflen) + ')'
            );
    }
    const std::string str = boost::lexical_cast<std::string>(code);
    strncpy(buf, str.c_str(), buflen);
    buf[buflen - 1] = '\0';
    return buf;
}
