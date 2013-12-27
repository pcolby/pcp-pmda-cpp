#include <pcp/pmapi.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <string>

extern "C" {

int pmDebug;

// Drop-in pmErrStr[_r] replacement that always returns the \a code as a string
#ifdef PM_MAXERRMSGLEN
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
#else
const char *pmErrStr(int code)
{
    static char buf[128] = { '\0' };
    const std::string str = boost::lexical_cast<std::string>(code);
    strncpy(buf, str.c_str(), sizeof(buf));
    buf[sizeof(buf) - 1] = '\0';
    return buf;
}
#endif

char *pmGetConfig(const char *variable)
{
    return strdup(variable);
}

void __pmNotifyErr(int /*priority*/, const char */*message*/, ...)
{

}

int __pmParseDebug(const char */*spec*/)
{
    return PM_ERR_NYI;
}

int __pmPathSeparator()
{
    return '|';
}

void __pmSetProgname(const char */*program*/)
{

}

} // extern "C"
