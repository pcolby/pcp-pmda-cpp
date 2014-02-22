//               Copyright Paul Colby 2013.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef __PCP_CPP_EXCEPTION_HPP__
#define __PCP_CPP_EXCEPTION_HPP__

#include "config.hpp"

#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

/**
 * @brief Base class for all PMDA++ exceptions.
 */
class exception : public std::exception {

public:

    /**
     * @brief Constructor.
     *
     * If the optional \a message is not provided (or is empty), then either
     * pmErrStr_r (if available) or pmErrStr will be used to fetch PCP's message
     * for \a pm_error_code.
     *
     * @param pm_error_code PCP error code.
     * @param message       Error message.
     *
     * @see pmErrStr
     * @see pmErrStr_r
     */
    exception(const int pm_error_code,
              const std::string &message = std::string())
        : pm_error_code(pm_error_code),
          message(message)
    {
        if (message.empty()) {
#ifdef PM_MAXERRMSGLEN // pmErrStr_r and PM_MAXERRMSGLEN added in PCP 3.6.0.
            char buffer[PM_MAXERRMSGLEN];
            pmErrStr_r(pm_error_code, buffer, sizeof(buffer));
            this->message.assign(buffer);
#else
            this->message.assign(pmErrStr(pm_error_code));
#endif
        }
    }

    /**
     * @brief Copy constructor.
     *
     * @param other Other pcp::exception object to copy.
     */
    exception(const pcp::exception &other)
        : std::exception(other),
          pm_error_code(other.error_code()),
          message(other.what())
    {
    }

    /**
     * @brief Virtual destructor.
     *
     * For safe polymorphic destruction.
     */
    virtual ~exception() throw() { }

    /**
     * @brief Get this exception's error code.
     *
     * @return This exception's error code.
     */
    virtual int error_code() const
    {
        return pm_error_code;
    }

    /**
     * @brief Get this exception's error message.
     *
     * @return This excetion's error message.
     */
    virtual const char * what() const throw()
    {
        return message.c_str();
    }

protected:
    int pm_error_code;   ///< PCP error code.
    std::string message; ///< Error message.
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
