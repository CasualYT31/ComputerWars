/*Copyright 2019-2021 CasualYouTuber31 <naysar@protonmail.com>

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

#include "logger.h"
#include <fstream>
#include <ctime>

std::shared_ptr<spdlog::sinks::dist_sink_mt> global::sink::_sharedSink = nullptr;
std::ostringstream global::sink::_fileCopy = std::ostringstream();
std::string global::sink::_appName = "";
std::string global::sink::_devName = "";

global::sink::sink() noexcept {}

std::shared_ptr<spdlog::sinks::dist_sink_mt> global::sink::Get(
	const std::string& name, const std::string& dev, const std::string& folder,
	const bool date) noexcept {
	if (!_sharedSink) {
		try {
			_appName = name;
			_devName = dev;
			std::string filename = folder + "/Log" +
				(date ? " " + GetDateTime() : "") + ".log";

			_fileCopy << ApplicationName() << " © " << GetYear() << " " <<
				DeveloperName() << std::endl;
			_fileCopy << "---------------" << std::endl;
			/* logfile << "Hardware Specifications:" << std::endl;
			_fileCopy << "CPU       " <<  << std::endl;
			_fileCopy << "Memory    " <<  << std::endl;
			_fileCopy << "GPU       " <<  << std::endl;
			_fileCopy << "Storage   " <<  << std::endl;
			_fileCopy << "Platform  " <<  << std::endl;
			_fileCopy << "Gamepads  " <<  << std::endl;
			_fileCopy << "---------------" << std::endl; */
			_fileCopy << "Event Log:" << std::endl;

			std::ofstream logfile(filename);
			logfile << _fileCopy.str();
			logfile.close();

			_sharedSink = std::make_shared<spdlog::sinks::dist_sink_mt>();
			_sharedSink->add_sink(
				std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename)
			);
			_sharedSink->add_sink(
				std::make_shared<spdlog::sinks::ostream_sink_mt>(_fileCopy)
			);
		} catch (std::exception& e) {
			boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
		}
	}
	return _sharedSink;
}

std::string global::sink::GetLog() noexcept {
	return _fileCopy.str();
}

std::string global::sink::GetYear() noexcept {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_year + 1900);
}

std::string global::sink::GetDateTime() noexcept {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_mday) + "-" + std::to_string(ltime.tm_mon + 1) +
		"-" + std::to_string(ltime.tm_year + 1900) + " " +
		std::to_string(ltime.tm_hour) + "-" + std::to_string(ltime.tm_min) + "-" +
		std::to_string(ltime.tm_sec);
}

std::string global::sink::ApplicationName() noexcept {
	return _appName;
}

std::string global::sink::DeveloperName() noexcept {
	return _devName;
}

std::size_t global::logger::_objectCount = 0;

global::logger::logger(const std::string& name) noexcept {
	try {
		_name = name + "_" + std::to_string(_objectCount);
		_logger = std::make_shared<spdlog::logger>(_name, global::sink::Get());
		// don't increment in to_string() so that if it throws, the object count
		// isn't incremented
		_objectCount++;
	} catch (std::exception& e) { // also catches spdlog errors, which might not be
		                          // the case if another backend is used!
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

global::logger::~logger() noexcept {
	try {
		spdlog::drop(_name);
	} catch (std::exception& e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}