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

#include "logger.hpp"
#include <fstream>
#include <ctime>
#include "SystemProperties.hpp"
#include "SFML/Window/Joystick.hpp"

std::string engine::GetYear() {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_year + 1900);
}

std::string engine::GetDateTime() {
	time_t curtime = time(NULL);
	tm ltime;
	localtime_s(&ltime, &curtime);
	return std::to_string(ltime.tm_mday) + "-" + std::to_string(ltime.tm_mon + 1) +
		"-" + std::to_string(ltime.tm_year + 1900) + " " +
		std::to_string(ltime.tm_hour) + "-" + std::to_string(ltime.tm_min) + "-" +
		std::to_string(ltime.tm_sec);
}

engine::sink::sink(const std::string& name, const std::string& dev,
	std::string folder, const bool date,
	const std::shared_ptr<System::Properties>& hardwareDetails) {
	try {
		// First, attempt to write preliminary information to the log file.
		if (!folder.empty() && (folder.back() != '/' || folder.back() != '\\'))
			folder.push_back('/');
		std::string filename = folder + "Log" +
			((date) ? (" " + GetDateTime()) : ("")) + ".log";
		{
			std::ofstream logfile(filename);
			_fileCopy << name << " © " << GetYear() << " " << dev <<
				"\n---------------\n";
			if (hardwareDetails) {
				try {
					_fileCopy << "Hardware Specification:\n     CPU\t\t";
					_fileCopy << hardwareDetails->CPUModel() << "\n  Memory\t\t";
					_fileCopy << hardwareDetails->RAMTotal() << "\n     GPU\t\t";
					_fileCopy << hardwareDetails->GPUName() << "\n Storage\t\t";
					_fileCopy << hardwareDetails->StorageFree(System::Unit::MB) <<
						" out of " << hardwareDetails->StorageTotal() <<
						" is free\n";
					_fileCopy << "Platform\t\t" << hardwareDetails->OSName() <<
						" ~ " << hardwareDetails->OSVersion();
				} catch (const std::system_error& e) {
					// Exceptions thrown [likely] due to a System::Properties
					// failure should be reported in the log file (or, at least, an
					// attempt should be made to - don't try to deal with stream
					// exceptions).
					_fileCopy << "\nA failure occurred whilst trying to retrieve "
						"system properties: code " <<
						std::to_string(e.code().value()) << ", category: " <<
						e.code().category().name() << ", message: " <<
						e.code().message();
				}
				_fileCopy << "\n---------------\nGamepads\n";
				// I have avoided assuming that the SFML doesn't leave out IDs:
				// i.e. joystick 0 is connected, 1 is not connected, but 2 IS
				// connected.
				// I don't have 100% certainty regarding how the SFML deals with
				// IDs, so I'd prefer to be safe even if it's not 100% efficient.
				sf::Joystick::update();
				for (unsigned int i = 0; i < sf::Joystick::Count; ++i) {
					_fileCopy << "Gamepad #" << i << " is " <<
						((sf::Joystick::isConnected(i)) ? ("") : ("not ")) <<
						"connected\n";
				}
				_fileCopy << "---------------\n";
			}
			_fileCopy << "Event Log:\n";
			logfile << _fileCopy.str();
		}
		// Then, attempt to create the spdlog sinks.
		_sharedSink = std::make_shared<spdlog::sinks::dup_filter_sink_st>(
			std::chrono::seconds(5)
		);
		_sharedSink->add_sink(
			std::make_shared<spdlog::sinks::basic_file_sink_st>(filename)
		);
		_sharedSink->add_sink(
			std::make_shared<spdlog::sinks::ostream_sink_st>(_fileCopy)
		);
	} catch (const std::exception& e) {
		// If the above code throws an exception, we must guarantee that the
		// internal sink object is kept uninitialised to denote that it should not
		// be used.
		_sharedSink = nullptr;
		throw e;
	}
}

std::string engine::sink::getLog() {
	return _fileCopy.str();
}

std::size_t engine::logger::_objectCount = 0;

engine::logger::logger(const engine::logger::data& loggerData) {
	setData(loggerData);
}

engine::logger::logger(const engine::logger& logger) {
	*this = logger;
}

engine::logger::logger(engine::logger&& logger) noexcept :
	_logger(std::move(logger._logger)), _name(std::move(logger._name)),
	_data(std::move(logger._data)) {}

engine::logger::~logger() noexcept {
	_dropLogger();
}

engine::logger& engine::logger::operator=(const engine::logger& logger) {
	setData(logger);
	return *this;
}

void engine::logger::setData(const engine::logger::data& loggerData) {
	if (loggerData.sink) {
		assert(("The sink given to a logger must be constructed properly!",
			loggerData.sink->_sharedSink));
		_initialiseLogger(loggerData.name + "_" + std::to_string(_objectCount),
			{ loggerData.sink->_sharedSink }, loggerData);
	} else {
		_uninitialiseLogger(loggerData);
	}
}

void engine::logger::setData(const engine::logger& logger) {
	if (logger._logger) {
		if (logger._name.rfind('_') == std::string::npos) {
			_initialiseLogger(logger._name + "_" + std::to_string(_objectCount),
				logger._logger->sinks(), logger._data);
		} else {
			_initialiseLogger(logger._name.substr(0, logger._name.rfind('_') + 1) +
				std::to_string(_objectCount),
				logger._logger->sinks(), logger._data);
		}
	} else {
		_uninitialiseLogger(logger._data);
	}
}

std::size_t engine::logger::countCreated() noexcept {
	return _objectCount;
}

void engine::logger::_initialiseLogger(const std::string& name,
	const std::vector<spdlog::sink_ptr>& sinks,
	const engine::logger::data& data) {
	_logger = std::make_shared<spdlog::logger>(name, begin(sinks), end(sinks));
	// _logger has changed, but _name hasn't yet. So we can call _dropLogger() to
	// drop the old logger.
	_dropLogger();
	_name = name;
	_data = data;
	++_objectCount;
}

void engine::logger::_dropLogger() noexcept {
	if (_logger) {
		try {
			_logger->flush();
			spdlog::drop(_name);
		} catch (const std::exception& e) {
			try {
				boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
			} catch (const std::exception&) {
				// Swallow it. Can't let the destructor throw an exception.
			}
		}
	}
}

void engine::logger::_uninitialiseLogger(const engine::logger::data& loggerData) {
	if (_logger) {
		_dropLogger();
		_name = "";
		_data = loggerData;
		_logger = nullptr;
	}
}
