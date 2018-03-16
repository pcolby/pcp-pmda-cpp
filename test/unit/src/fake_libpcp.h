//               Copyright Paul Colby 2014 - 2018.
// Distributed under the Boost Software License, Version 1.0.
//       (See accompanying file LICENSE.md or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <string>

// PCP 4.0 removed the pmDebug global, but our mock PCP library still mimics it
// for some tests. This should probably we refactored sometime.
#if defined PM_VERSION_CURRENT && PM_VERSION_CURRENT >= 40000
extern "C" int pmDebug;
#endif

/// @brief  Test values to be returned by our mock pmGetConfig implementation.
extern std::map<std::string, char *> fake_pm_config;
