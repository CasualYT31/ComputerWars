/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**@file pod.tpp
 * This header file was generated by the macgen.py tool.
 * It contains macros used to define POD types that are integrated with AngelScript.
 */

#include "maths.hpp"
#include "boost/call_traits.hpp"

namespace engine {
    /// Allows me to perform comparisons on any types T and U correctly without having to generalise closeTo() to let any type be passed to it.
    template<typename T, typename U>
    inline bool isEqual(typename boost::call_traits<T>::param_type t,
        typename boost::call_traits<U>::param_type u) {
        if constexpr (std::is_floating_point<T>::value && std::is_floating_point<U>::value) {
            return engine::closeTo(static_cast<const float>(t), static_cast<const float>(u));
        } else {
            return t == u;
        }
    }
}
