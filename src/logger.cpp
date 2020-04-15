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

#include "logger.h"
#include <fstream>
#include <ctime>

std::shared_ptr<spdlog::sinks::basic_file_sink_mt> global::sink::_sharedFileSink = nullptr;
std::string global::sink::_appName = "";
std::string global::sink::_devName = "";

global::sink::sink() noexcept {}

std::shared_ptr<spdlog::sinks::basic_file_sink_mt> global::sink::Get(const std::string& name, const std::string& dev, const std::string& folder, const bool date) noexcept {
	if (!_sharedFileSink) {
		try {
			_appName = name;
			_devName = dev;
			std::string filename = folder + "/Log" + (date ? " " + GetDateTime() : "") + ".log";

			std::ofstream logfile(filename);
			logfile << ApplicationName() << " © " << GetYear() << " " << DeveloperName() << std::endl;
			logfile << "---------------" << std::endl;
			/* logfile << "Hardware Specifications:" << std::endl;
			logfile << "CPU       " << std::endl;
			logfile << "Memory    " << std::endl;
			logfile << "GPU       " << std::endl;
			logfile << "Storage   " << std::endl;
			logfile << "Platform  " << std::endl;
			logfile << "Gamepads  " << std::endl;
			logfile << "---------------" << std::endl; */
			logfile << "Event Log:" << std::endl;
			logfile.close();

			_sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
		}
		catch (std::exception & e) {
			boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
		}
	}
	return _sharedFileSink;
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
	return std::to_string(ltime.tm_mday) + "-" + std::to_string(ltime.tm_mon + 1) + "-" + std::to_string(ltime.tm_year + 1900) + " " +
		std::to_string(ltime.tm_hour) + "-" + std::to_string(ltime.tm_min) + "-" + std::to_string(ltime.tm_sec);
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
		//don't increment in to_string() so that if it throws, the object count isn't incremented
		_objectCount++;
	}
	catch (std::exception & e) { //also catches spdlog errors, which might not be the case if another backend is used!
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

global::logger::~logger() noexcept {
	try {
		spdlog::drop(_name);
	}
	catch (std::exception & e) {
		//the logger object wasn't created successfully at construction, so ignore
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}