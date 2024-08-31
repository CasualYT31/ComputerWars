/**
 * \file log/TestHelpers.hpp
 * \brief Declares macros that can be used in tests to figure out if e.g. certain text ended up being written in the log.
 */

#include "Log.hpp"

#include <gmock/gmock.h>

/**
 * \brief Tests that an expression produces log entries that match the given matcher.
 * \param expression The expression whose log output is to be tested.
 * \param matcher The matcher to use, typically testing::HasSubstr.
 */
#define EXPECT_LOG(expression, matcher)                                                                                     \
    {                                                                                                                       \
        const auto _INTERNAL_TEST_HELPER__currentLogSize = cw::Log::Get().size();                                           \
        expression;                                                                                                         \
        const auto _INTERNAL_TEST_HELPER__logSnippet = cw::Log::Get().substr(_INTERNAL_TEST_HELPER__currentLogSize);        \
        EXPECT_THAT(_INTERNAL_TEST_HELPER__logSnippet, matcher);                                                            \
    }
