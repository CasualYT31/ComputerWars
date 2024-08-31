/**
 * \file script/TestHelpers.hpp
 * \brief Lists the subclasses of ScriptEngine that are included within the tests.
 */

#include "gmock/gmock.h"

namespace cw {
/**
 * \brief The types of script engine to test.
 */
using ScriptEngineTypesToTest = testing::Types<cw::AngelScriptEngine>;

} // namespace cw
