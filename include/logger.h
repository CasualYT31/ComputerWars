/*Copyright 2020 CasualYouTuber31 <naysar@protonmail.com>

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

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "boxer/boxer.h"

namespace global {
	class sink {
	public:
		static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> Get(const std::string& name = "Application", const std::string& dev = "Developer", const std::string& folder = ".", const bool date = true) noexcept;
		static std::string ApplicationName() noexcept;
		static std::string DeveloperName() noexcept;
		static std::string GetYear() noexcept;
		static std::string GetDateTime() noexcept;
	protected:
		sink() noexcept;
	private:
		static std::shared_ptr<spdlog::sinks::basic_file_sink_mt> _sharedFileSink;
		static std::string _appName;
		static std::string _devName;
	};

	class logger {
	public:
		logger(const std::string& name) noexcept;
		~logger() noexcept;
		template<typename... Ts>
		void write(const std::string& line, Ts... values) noexcept;
		template<typename... Ts>
		void error(const std::string& line, Ts... values) noexcept;
	private:
		std::shared_ptr<spdlog::logger> _logger;
		static std::size_t _objectCount;
		std::string _name;
	};
}

template<typename... Ts>
void global::logger::write(const std::string& line, Ts... values) noexcept {
	try {
		_logger->info(line, values...);
	}
	catch (std::exception & e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

template<typename... Ts>
void global::logger::error(const std::string& line, Ts... values) noexcept {
	try {
		_logger->error(line, values...);
	}
	catch (std::exception & e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}