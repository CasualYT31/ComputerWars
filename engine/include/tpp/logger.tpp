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

#pragma once

template<typename... Ts>
void engine::logger::write(const std::string& line, Ts... values) noexcept {
	if (_logger) _logger->info(line, values...);
}

template<typename... Ts>
void engine::logger::error(const std::string& line, Ts... values) noexcept {
	if (_logger) _logger->error(line, values...);
}

template<typename... Ts>
void engine::logger::warning(const std::string& line, Ts... values) noexcept {
	if (_logger) _logger->warn(line, values...);
}

template<typename... Ts>
void engine::logger::critical(const std::string& line, Ts... values) noexcept {
	if (_logger) _logger->critical(line, values...);
	try {
		// Produce dialog window.
		std::string result;
		fmt::format_to(std::back_inserter(result), line, values...);
		boxer::show(result.c_str(), "Critical Error!", boxer::Style::Error);
	} catch (const std::exception& e) {
		if (_logger)
			_logger->critical("Can't produce dialog box for above log: {}", e);
	}
}
