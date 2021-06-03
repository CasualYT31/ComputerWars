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

/**@file logger.h
 * Classes used for logging and debugging.
 * This file provides two classes: \c logger and \c sink. \c sink is a singleton
 * class which represents the log file which all instantiations of the \c logger
 * class output to. These classes act as simple wrappers for the \c spdlog backend.
 */

#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/dist_sink.h"
#include "boxer/boxer.h"
#include <sstream>

/**
 * The \c global namespace contains classes used for logging and debugging
 * purposes.
 */
namespace global {
	/**
	 * This class represents the log file which all loggers output to.
	 * This class uses the singleton design pattern. As such, no instantiation of
	 * this class can be made. Instead, the client must access the log file via
	 * \c sink.Get().\n
	 * <b>It is important to emphasise that the client should call \c Get() once at
	 * the start of the program to open and initalise the log file before any
	 * \c logger objects use it!</b>
	 */
	class sink {
	public:
		/**
		 * Retrieves the file sink.
		 * If called for the first time, the .log file is opened, and cleared of
		 * all contents if it already exists. A pointer to the file sink
		 * representing this log file is then returned.\n
		 * Subsequent calls are used to retrieve this pointer only: it will only
		 * attempt to create the necessary sinks if creating it in the first call
		 * failed. Subsequent calls will ignore all parameters given if sink
		 * creation was succesful.\n
		 * If creating the sink failed, the \c boxer library is used to produce an
		 * OS error dialog containing the error text.\n
		 * The log file has the following name: <tt>Log[ d-m-yyyy h-m-s].log</tt>.
		 * The components within brackets are optional and can be toggled off, see
		 * \c date below.
		 * @param  name   The name of the application to write in the first line of
		 *                the log file.
		 * @param  dev    The name of the application developer/s to write in the
		 *                first line of the log file.
		 * @param  folder The directory, relative or absolute, to write the log
		 *                file in.
		 * @param  date   If \c TRUE, a short form of the date and time will be
		 *                included in the file name.
		 * @return A pointer to the sink representing the log file.
		 */
		static std::shared_ptr<spdlog::sinks::dist_sink_mt> Get(
			const std::string& name = "Application",
			const std::string& dev = "Developer",
			const std::string& folder = ".",
			const bool date = true
		) noexcept;
		
		/**
		 * Retrieves a copy of the event log produced thus far.
		 * @return The event log produced thus far, including additional
		 *         information at the start of the log file.
		 */
		static std::string GetLog() noexcept;

		/**
		 * Retrieves the application name as defined in the first call to
		 * \c sink.Get().
		 * @return The application name.
		 */
		static std::string ApplicationName() noexcept;
		
		/**
		 * Retrieves the name of the application developer as defined in the first
		 * call to \c sink.Get().
		 * @return The name of the application developer.
		 */
		static std::string DeveloperName() noexcept;
		
		/**
		 * Retrieves the current year in string form.
		 * Used when writing the first line of the log file.
		 * @return The year in the format "yyyy".
		 */
		static std::string GetYear() noexcept;
		
		/**
		 * Retrieves the current date and time in the format "d-m-yyyy h-m-s".
		 * @return The date and time in string form.
		 */
		static std::string GetDateTime() noexcept;
	protected:
		/**
		 * This class cannot be instantiated by the client.
		 */
		sink() noexcept;
	private:
		/**
		 * The thread safe distribution sink which outputs to a file and an
		 * \c ostringstream.
		 */
		static std::shared_ptr<spdlog::sinks::dist_sink_mt> _sharedSink;

		/**
		 * The \c ostringstream used to store a copy of the event log of the log
		 * file.
		 */
		static std::ostringstream _fileCopy;
		
		/**
		 * Stores the name of the application.
		 */
		static std::string _appName;
		
		/**
		 * Stores the name of the application developer.
		 */
		static std::string _devName;
	};

	/**
	 * This class represents a single C++ object's interface with the log file.
	 * The multiple logger-single sink architecture was created to allow for
	 * different C++ objects to have their own logger object which could output to
	 * the same file. It also decouples C++ classes from the "global" file sink and
	 * makes implementing a multiple logger-multiple sink architecture easier.
	 */
	class logger {
	public:
		/**
		 * Creates a new logger object and adds it to the file sink.
		 * An internal object counter is used to keep track of the number of logger
		 * objects throughout the execution of the program: this is used only to
		 * ensure that all logger object names are unique as this is important to
		 * maintain for the logging backend. This counter is only incremented if
		 * object creation is successful. If the operation failed, the \c boxer
		 * library is used to report an error string to the user via a message box.
		 * @param name The name to give to the logger object.
		 */
		logger(const std::string& name) noexcept;

		/**
		 * Drops the logger object from <tt>spdlog</tt>'s logger pool.
		 * If the call to \c spdlog::drop() failed, the \c boxer library is used to
		 * report an error string to the user via a message box.
		 */
		~logger() noexcept;

		/**
		 * Outputs text to the log file.
		 * A single line of text is written to the log file via this method. This
		 * method is intended for reporting simple messages.\n
		 * Variables can be inserted into the message by inserting {} into the
		 * \c line string. Values are then appended to the parameter list and
		 * inserted on a FIFO basis, replacing the {}s in the process. Please see
		 * \c spdlog documentation for more information.
		 * @tparam Ts     The types in the parameter pack.
		 * @param  line   The line of text.
		 * @param  values A parameter pack containing the values to insert into the
		 *                text.
		 * @sa     logger.error()
		 * @sa     logger.warning()
		 */
		template<typename... Ts>
		void write(const std::string& line, Ts... values) noexcept;

		/**
		 * Outputs text to the log file as an error.
		 * A single line, which records an error, is output to the log file via a
		 * call to this method. Please see \c logger.write() for more information.
		 * @tparam Ts     The types in the parameter pack.
		 * @param  line   The line of text.
		 * @param  values A parameter pack containing the values to insert into the
		 *                text.
		 * @sa     logger.write()
		 * @sa     logger.warning()
		 */
		template<typename... Ts>
		void error(const std::string& line, Ts... values) noexcept;

		/**
		 * Outputs text to the log file as a warning.
		 * A single line, which records a non-fatal error, is output to the log
		 * file via a call to this method. Please see \c logger.write() for more
		 * information.
		 * @tparam Ts     The types in the parameter pack.
		 * @param  line   The line of text.
		 * @param  values A parameter pack containing the values to insert into the
		 *                text.
		 * @sa     logger.write()
		 * @sa     logger.error()
		 */
		template<typename... Ts>
		void warning(const std::string& line, Ts... values) noexcept;
	private:
		/**
		 * A pointer to the logger object.
		 */
		std::shared_ptr<spdlog::logger> _logger;

		/**
		 * A static object count used to ensure that each logger object name is
		 * unique.
		 */
		static std::size_t _objectCount;

		/**
		 * The name of the logger object which is used to identify it within the
		 * log file.
		 * The name is stored so that the \c spdlog::drop() function can be called
		 * for the internal logger object later.
		 */
		std::string _name;
	};
}

template<typename... Ts>
void global::logger::write(const std::string& line, Ts... values) noexcept {
	try {
		_logger->info(line, values...);
	} catch (std::exception & e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

template<typename... Ts>
void global::logger::error(const std::string& line, Ts... values) noexcept {
	try {
		_logger->error(line, values...);
	} catch (std::exception & e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}

template<typename... Ts>
void global::logger::warning(const std::string& line, Ts... values) noexcept {
	try {
		_logger->warn(line, values...);
	} catch (std::exception& e) {
		boxer::show(e.what(), "Fatal Error!", boxer::Style::Error);
	}
}