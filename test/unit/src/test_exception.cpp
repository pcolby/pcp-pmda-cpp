#include "gtest/gtest.h"

#include "pcp-cpp/exception.hpp"

#include <boost/lexical_cast.hpp>

TEST(exception, error_code) {
    const pcp::exception min(std::numeric_limits<int>::min());
    EXPECT_EQ(std::numeric_limits<int>::min(), min.error_code());

    const pcp::exception max(std::numeric_limits<int>::max());
    EXPECT_EQ(std::numeric_limits<int>::max(), max.error_code());
}

TEST(exception, default_message) {
    // Our fake ppmErrStr_r implementation (see fake_libpcp.cpp) always returns
    // the error code as string.

    const pcp::exception min(std::numeric_limits<int>::min());
    EXPECT_EQ(boost::lexical_cast<std::string>(min.error_code()), min.what());

    const pcp::exception max(std::numeric_limits<int>::max());
    EXPECT_EQ(boost::lexical_cast<std::string>(max.error_code()), max.what());
}

TEST(exception, explicit_message) {
    const pcp::exception test(-123, "test");
    EXPECT_STREQ("test", test.what());

    // Empty strings are treated the same as the default_message test above.

    const pcp::exception min(std::numeric_limits<int>::min(), "");
    EXPECT_EQ(boost::lexical_cast<std::string>(min.error_code()), min.what());

    pcp::exception max(std::numeric_limits<int>::max(), "");
    EXPECT_EQ(boost::lexical_cast<std::string>(max.error_code()), max.what());
}

TEST(exception, copy_constructor) {
    const pcp::exception original(456, "original");
    const pcp::exception copy(original);

    EXPECT_EQ(456, original.error_code());
    EXPECT_EQ(456, copy.error_code());

    EXPECT_STREQ("original", original.what());
    EXPECT_STREQ("original", copy.what());
}
