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

#include "maths.hpp"
#include <chrono>

bool engine::closeTo(const float a, const float b, const float epsilon) noexcept {
	// Equal numbers and infinity will return immediately
	if (a == b) return true;
	// When very close to 0, we can use the absolute comparison
	const float diff = ::fabsf(a - b);
	if ((a == 0 || b == 0) && (diff < epsilon)) return true;
	// Otherwise we need to use relative comparison to account for precision
	return diff / (::fabs(a) + ::fabs(b)) < epsilon;
}

std::unique_ptr<std::mt19937> engine::RNGFactory() {
	// Credit: https://stackoverflow.com/a/13446015/6928376.
	std::random_device randomDevice;
	std::mt19937::result_type seed = randomDevice() ^ (
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()
		).count() +
		(std::mt19937::result_type)
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch()
		).count()
	);
	return std::make_unique<std::mt19937>(seed);
}
