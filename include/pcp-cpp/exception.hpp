/*
    Copyright 2013 Paul Colby

    This file is part of pmda-cpp.

    Pmda-cpp is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    Pmda-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
    more details.

    You should have received a copy of the GNU Lesser General Public License
    along with pmda-cpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PCP_CPP_EXCEPTION_HPP__
#define __PCP_CPP_EXCEPTION_HPP__

#include "config.hpp"

#include <pcp/pmapi.h>

#include <string>

PCP_CPP_BEGIN_NAMESPACE

namespace pcp {

class exception : public std::exception {

public:
    exception(const int pm_error_code,
              const std::string &message = std::string())
        : pm_error_code(pm_error_code),
          message(message)
    {
        if (message.empty()) {
            char buffer[PM_MAXERRMSGLEN];
            pmErrStr_r(pm_error_code, buffer, sizeof(buffer));
            this->message.assign(buffer);
        }
    }

    exception(const pcp::exception &other)
        : pm_error_code(other.error_code()),
          message(other.what())
    {
    }

    virtual ~exception() throw() { };

    virtual int error_code() const
    {
        return pm_error_code;
    }

    virtual const char* what() const throw()
    {
        return message.c_str();
    }

protected:
    int pm_error_code;
    std::string message;
};

} // pcp namespace.

PCP_CPP_END_NAMESPACE

#endif
