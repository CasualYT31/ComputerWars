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

/**@file logger.hpp
 * Classes used for logging and debugging.
 * This file provides two classes: \c logger and \c sink. A \c sink represents a
 * single log file, which multiple \c logger objects can write to.
 */

#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/dup_filter_sink.h"
#include "boxer/boxer.h"
#include "SFML/System/NonCopyable.hpp"
#include "SystemProperties.hpp"
#include <sstream>

namespace engine {
	/**
	 * Retrieves the current year in string form.
	 * @return The year in the format "yyyy".
	 * @safety Strong guarantee: see <a href=
	 *         "https://cplusplus.com/reference/string/string/string/"
	 *         target="_blank">string constructor documentation.</a>
	 */
	std::string GetYear();

	/**
	 * Retrieves the current date and time in the format "d-m-yyyy h-m-s".
	 * @return The date and time in string form.
	 * @safety Strong guarantee: see <a href=
	 *         "https://cplusplus.com/reference/string/string/string/"
	 *         target="_blank">string constructor documentation.</a>
	 */
	std::string GetDateTime();

	/**
	 * This class represents a log file which loggers can output to.
	 */
	class sink : sf::NonCopyable {
	public:
		/**
		 * Opens a log file.
		 * The log file has the following name: <tt>Log[ d-m-yyyy h-m-s].log</tt>.
		 * The components within brackets are optional and can be toggled off, see
		 * \c date below. If a file with the same path already exists, then it will
		 * be cleared of all its contents before it is opened.
		 * @param name            The name of the application to write in the first
		 *                        line of the log file.
		 * @param dev             The name of the application developer/s to write
		 *                        in the first line of the log file.
		 * @param folder          The directory, relative or absolute, to write the
		 *                        log file in.
		 * @param date            If \c TRUE, a short form of the date and time
		 *                        will be included in the file name.
		 * @param hardwareDetails If given, the hardware details of the machine
		 *                        running this game will be written to the
		 *                        beginning of the log.
		 */
		sink(const std::string& name = "Application",
			const std::string& dev = "Developer", std::string folder = "",
			const bool date = true,
			const std::shared_ptr<System::Properties>& hardwareDetails = nullptr);
		
		/**
		 * Retrieves a copy of the event log produced thus far.
		 * @return The event log produced thus far, including additional
		 *         information at the start of the log file.
		 * @safety Basic guarantee: all internal state is guaranteed to be
		 *         unaffected. However, the output string stream used to copy the
		 *         event log is only guaranteed to be left in a valid state should
		 *         its \c str() method throw.
		 */
		std::string getLog();
	private:
		/**
		 * The non-thread safe distribution sink which outputs to a file and an
		 * \c ostringstream.
		 */
		std::shared_ptr<spdlog::sinks::dup_filter_sink_st> _sharedSink;

		/**
		 * The \c ostringstream used to store a copy of the event log of the log
		 * file.
		 */
		std::ostringstream _fileCopy;

		/**
		 * \c logger was made a friend of \c sink so that it can access the
		 * internal sink object.
		 */
		friend class logger;
	};

	/**
	 * This class can write information to a \c sink object.
	 */
	class logger {
	public:
		/**
		 * Logger initialisation data.
		 */
		struct data {
			/**
			 * A pointer to the sink the logger will write to.
			 */
			std::shared_ptr<engine::sink> sink;

			/**
			 * The name used to identify the logger object in the sink's file.
			 */
			std::string name;
		};

		/**
		 * Creates a blank logger object that can be later initialised with
		 * \c setData() later.
		 */
		logger() = default;

		/**
		 * Creates a new logger object and adds it to the given sink.
		 * An internal object counter is used to keep track of the number of logger
		 * objects throughout the execution of the program: this is used only to
		 * ensure that all logger object names are unique as this is important to
		 * maintain for the logging backend. This counter is only incremented if
		 * object creation is successful.\n
		 * If \c nullptr is given as the sink, then the logger object will not
		 * write anything to a sink, and the object counter will not increment. If
		 * a critical log is written, it still won't be written to any file,
		 * however, the dialog will still pop up. In this state, the internal
		 * logger object won't be constructed, and this logger object will have a
		 * blank name.
		 * @warning Do not give a \c sink that couldn't be constructed! Doing this
		 *          will crash the game with an assertion.
		 * @param   data Data to initialise the logger with.
		 * @sa      \c setData(const engine::logger::data&)
		 */
		logger(const engine::logger::data& loggerData);

		/**
		 * Creates a new logger object based on the data of another logger object.
		 * The new, copied logger object will write to the same sink as the one
		 * given. It will also have the same name, but with a different number due
		 * to the workings of the internal object counter.\n
		 * If the given \c logger object isn't properly constructed, the newly
		 * constructed object won't be properly constructed either.\n
		 * If the given \c logger object wasn't given a sink on construction, then
		 * this logger object won't be assigned a sink, either.
		 * @param logger Reference to the logger object to copy from.
		 * @sa    \c logger(const engine::logger::data&)
		 * @sa    \c setData(const engine::logger&)
		 */
		logger(const engine::logger& logger);

		/**
		 * Moves the given logger object.
		 * The new, moved logger object will write to the same sink as the one
		 * given. It will also have the same name, and same number.\n
		 * If the given \c logger object isn't properly constructed, the newly
		 * constructed object won't be properly constructed either.\n
		 * If the given \c logger object wasn't given a sink on construction, then
		 * this logger object won't be assigned a sink, either.
		 * @param logger Reference to the logger object to move.
		 */
		logger(engine::logger&& logger) noexcept;

		/**
		 * Drops the logger object from <tt>spdlog</tt>'s logger pool.
		 * If the call to \c spdlog::drop() failed, the \c boxer library is used to
		 * report an error string to the user via a message box. This will never
		 * happen if construction failed.
		 */
		~logger() noexcept;

		/**
		 * Initialises the internal logger object.
		 * This method will destroy any logger object that was previously
		 * allocated. If the \c sink field in the given data is \c nullptr, then
		 * any allocated logger object will be uninitialised and no new object will
		 * be constructed.
		 * @warning This method asserts that the internal sink in the given sink
		 *          object, if given, is not \c nullptr.
		 * @param   loggerData The data to construct the new logger object with.
		 * @safety  Basic guarantee.
		 */
		void setData(const engine::logger::data& loggerData);

		/**
		 * Constructs a new logger object using data found in another one.
		 * This method will destroy any logger object that was previously
		 * allocated. If the internal logger object in the given logger is
		 * \c nullptr, then any allocated logger object will be uninitialised and
		 * no new object will be constructed.
		 * @param  logger The logger to extract data from.
		 * @safety Basic guarantee.
		 */
		void setData(const engine::logger& logger);

		/**
		 * Retrieves a reference to the data used to initialise this logger object.
		 * @return The data given during construction.
		 * @safety Note that if the constructor throws during the storing of the
		 *         data, the data returned will be in an undefined state.
		 */
		inline const engine::logger::data& getData() const noexcept {
			return _data;
		}

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
		 * @safety <a href="https://github.com/gabime/spdlog/wiki/Error-handling"
		 *         target="_blank">Spdlog will not throw whilst logging.</a>
		 * @sa     logger.error()
		 * @sa     logger.warning()
		 * @sa     logger.critical()
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
		 * @safety <a href="https://github.com/gabime/spdlog/wiki/Error-handling"
		 *         target="_blank">Spdlog will not throw whilst logging.</a>
		 * @sa     logger.write()
		 * @sa     logger.warning()
		 * @sa     logger.critical()
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
		 * @safety <a href="https://github.com/gabime/spdlog/wiki/Error-handling"
		 *         target="_blank">Spdlog will not throw whilst logging.</a>
		 * @sa     logger.write()
		 * @sa     logger.error()
		 * @sa     logger.critical()
		 */
		template<typename... Ts>
		void warning(const std::string& line, Ts... values) noexcept;

		/**
		 * Outputs text to the log file as a critical error, and throws up a dialog
		 * window.
		 * A single line, which records a non-recoverable error, is output to the
		 * log file via a call to this method. Please see \c logger.write() for
		 * more information. Additionally, the line written to the log will be
		 * shown to the user via a dialog box.
		 * @tparam Ts     The types in the parameter pack.
		 * @param  line   The line of text.
		 * @param  values A parameter pack containing the values to insert into the
		 *                text.
		 * @safety <a href="https://github.com/gabime/spdlog/wiki/Error-handling"
		 *         target="_blank">Spdlog will not throw whilst logging.</a> If the
		 *         dialog box couldn't be created, then the reason why will be
		 *         logged at the critical level.
		 * @sa     logger.write()
		 * @sa     logger.error()
		 * @sa     logger.warning()
		 */
		template<typename... Ts>
		void critical(const std::string& line, Ts... values) noexcept;

		/**
		 * Retrieves the number of logger objects created thus far.
		 * @return The number of logger objects created thus far.
		 */
		static std::size_t countCreated() noexcept;
	private:
		/**
		 * Replaces the stored logger with a new one.
		 * @param  name  The name to give the logger object.
		 * @param  sinks The sinks to assign to the logger object.
		 * @param  data  The data that the caller used to construct the \c name and
		 *               \c sinks parameters.
		 * @safety Strong guarantee: if the new logger object could not be
		 *         constructed, then all of the old logger's information will be
		 *         retained.
		 */
		void _initialiseLogger(const std::string& name,
			const std::vector<spdlog::sink_ptr>& sinks,
			const engine::logger::data& data);

		/**
		 * Drops the logger object.
		 * This method has no effect if \c _logger is \c nullptr.
		 * @safety If the drop operation throws an exception, this method will
		 *         catch it and produce a message box containing details of what
		 *         occurred.
		 */
		void _dropLogger() noexcept;

		/**
		 * Drops the logger object and uninitialises the data stored in this
		 * object.
		 * This method has no effect if \c _logger is \c nullptr.
		 * @safety Basic guarantee: if this method throws, the object is left in a
		 *         valid state, but it might not have been fully uninitialised.
		 */
		void _uninitialiseLogger(const engine::logger::data& loggerData = {});

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

		/**
		 * Cache of the data used to initialise this logger object.
		 */
		engine::logger::data _data;
	};
}

#include "tpp/logger.tpp"
