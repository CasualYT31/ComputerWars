/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

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

#include "logger.hpp"
#include <fstream>
#include <ctime>
#include "SystemProperties.hpp"
#include "SFML/Window/Joystick.hpp"

std::shared_ptr<spdlog::sinks::dist_sink_st> engine::sink::_sharedSink = nullptr;
std::ostringstream engine::sink::_fileCopy = std::ostringstream();
std::string engine::sink::_appName = "";
std::string engine::sink::_devName = "";

engine::sink::sink() noexcept {}

std::shared_ptr<spdlog::sinks::dist_sink_st> engine::sink::Get(
	const std::string& name, const std::string& dev, const std::string& folder,
	const bool date, const bool hardwareDetails) noexcept {
	if (!_sharedSink) {
		try {
			_appName = name;
			_devName = dev;
			std::string filename = folder + "/Log" +
				(date ? " " + GetDateTime() : "") + ".log";

			_fileCopy << ApplicationName() << " © " << GetYear() << " " <<
				DeveloperName() << std::endl;
			_fileCopy << "---------------" << std::endl;
			if (hardwareDetails) {
				System::Properties sys;
				_fileCopy << "Hardware Specification:\n";
				_fileCopy << "CPU       " << sys.CPUModel() << std::endl;
				_fileCopy << "Memory    " << sys.RAMTotal() << std::endl;
				_fileCopy << "GPU       " << sys.GPUName() << std::endl;
				_fileCopy << "Storage   " << sys.StorageFree(System::Unit::MB) <<
					" out of " << sys.StorageTotal() << " is free" << std::endl;
				_fileCopy << "Platform  " << sys.OSName() << " ~ " <<
					sys.OSVersion() << std::endl;
				_fileCopy << "---------------" << std::endl;
				_fileCopy << "Gamepads" << std::endl;
				// I have avoided assuming that the SFML doesn't leave out IDs:
				// i.e. joystick 0 is connected, 1 is not connected, but 2 IS
				// connected.
				// I don't have 100% certainty regarding how the SFML deals with
				// IDs, so I'd prefer to be safe even if it's not 100% efficient.
				sf::Joystick::update();
				for (unsigned int i = 0; i < sf::Joystick::Count; i++) {
					_fileCopy << "Gamepad #" << i << " is " <<
						((sf::Joystick::isConnected(i)) ? ("") : ("not ")) <<
						"connected" << std::endl;
				}
				_fileCopy << "---------------" << std::endl;
			}
			_fileCopy << "Event Log:" << std::endl;

			std::ofstream logfile(filename);
			logfile << _fileCopy.str();
			logfile.close();

			_sharedSink = std::make_shared<spdlog::sinks::dist_sink_st>();
			_sharedSink->add_sink(
				std::make_shared<spdlog::sinks::basic_file_sink_st>(filename)
			);
			_sharedSink->add_sink(
				std::make_shared<spdlog::sinks::ostream_sink_st>(_fileCopy)
			);
		} catch (std::exception& e) {
			boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
		}
	}
	return _sharedSink;
}

std::string engine::sink::GetLog() noexcept {
	return _fileCopy.str();
}

std::string engine::sink::GetYear() noexcept {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_year + 1900);
}

std::string engine::sink::GetDateTime() noexcept {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_mday) + "-" + std::to_string(ltime.tm_mon + 1) +
		"-" + std::to_string(ltime.tm_year + 1900) + " " +
		std::to_string(ltime.tm_hour) + "-" + std::to_string(ltime.tm_min) + "-" +
		std::to_string(ltime.tm_sec);
}

std::string engine::sink::ApplicationName() noexcept {
	return _appName;
}

std::string engine::sink::DeveloperName() noexcept {
	return _devName;
}

std::size_t engine::logger::_objectCount = 0;

engine::logger::logger(const std::string& name) noexcept {
	try {
		_name = name + "_" + std::to_string(_objectCount);
		_logger = std::make_shared<spdlog::logger>(_name, engine::sink::Get());
		// Don't increment in to_string() so that if it throws, the object count
		// isn't incremented.
		_objectCount++;
	} catch (std::exception& e) { // Also catches spdlog errors, which might not be
		                          // the case if another logging backend is used!
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

engine::logger::~logger() noexcept {
	try {
		spdlog::drop(_name);
	} catch (std::exception& e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

std::size_t engine::logger::countCreated() noexcept {
	return _objectCount;
}
