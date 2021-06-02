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

/**
 * @file tests.h
 * Defines all the tests.
 */

#pragma once

#include "test.h"
#include "uuid.h"

/**
 * Macro that allows tests which rely on the logger dependency to test if their naming functionality works.
 * @param n The string literal containing the name of the object used in the log file.
 */
#define ASSERT_NAME_IN_LOG(n) ASSERT_NOT_EQUAL(global::sink::GetLog().find(n), std::string::npos);

// for documentation on the test namespace, please see test.h
namespace test {
	/**
	 * The entry point into the solution's tests.
	 * The test results are output to a variety of log files in the path <tt>./test/results/</tt>.\n
	 * When you write a new <tt>test_case</tt>-derived class, you need to add it to the list of test cases in this function.
	 * You can do this my pushing back a <tt>new</tt>d instance of your class to the \c testcases vector.
	 * @return The value ideally returned by \c main().
	 */
	int test();

	/**
	 * Tests logger.h classes.
	 * @todo Further tests required for better coverage include testing the date and time functionality,
	 *       and the logger object count functionality.
	 */
	class test_logger : public test::test_case {
	public:
		/**
		 * Assigns the name "logger_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_logger(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the creation of a log file via the \c sink class.
		 * @warning Contains tests that are dependent on the year of execution!
		 */
		void sink();

		/**
		 * Runs tests related to the \c logger class.
		 */
		void logger();
	};

	/**
	 * Tests safejson.h classes.
	 */
	class test_safejson : public test::test_case {
	public:
		/**
		 * Assigns the name "safejson_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_safejson(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c json class.
		 */
		void json();
	};

	/**
	 * Tests language.h classes.
	 */
	class test_language : public test::test_case {
	public:
		/**
		 * Assigns the name "language_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_language(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c expand_string class.
		 */
		void expand_string();

		/**
		 * Run a series of tests on \c expand_string with a given var char.
		 * @param var The var char to test with.
		 */
		void expand_string_(const std::string& var);

		/**
		 * Runs tests related to the \c language_dictionary class.
		 */
		void language_dictionary();

		/**
		 * Runs tests related to the JSON script component of the \c language_dictionary class.
		 */
		void language_dictionary_json();
	};

	/**
	 * Tests uuid.h classes.
	 */
	class test_uuid : public test::test_case {
	public:
		/**
		 * Assigns the name "uuid_test_case.log" to the output file.
		 * Also initialises \c ID with \c UUID_INIT.
		 * @param path The path in which to save the output file.
		 */
		test_uuid(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;

		/**
		 * The initial value of the UUID.
		 */
		static const awe::UUIDValue UUID_INIT = engine::uuid<test::test_uuid>::INVALID - 1;

		/**
		 * The \c uuid instantiation tested against.
		 */
		engine::uuid<test::test_uuid> ID;
	private:
		/**
		 * Runs tests related to the \c uuid class.
		 */
		void uuid();
	};

	/**
	 * Tests fonts.h classes.
	 */
	class test_fonts : public test::test_case {
	public:
		/**
		 * Assigns the name "fonts_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_fonts(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c fonts class.
		 */
		void fonts();
	};

	/**
	 * Tests audio.h classes.
	 * @warning I've straight up disabled these tests for now to speed up test development
	 *          elsewhere: don't forget to reenable these tests later!
	 */
	class test_audio : public test::test_case {
	public:
		/**
		 * Assigns the name "audio_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_audio(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c audio class.
		 */
		void audio();

		/**
		 * Wait 3 seconds.
		 * @param msg Message to send to \c std::cout along with a waiting message.
		 */
		void longWait(const std::string& msg) noexcept;

		/**
		 * Wait 1 second.
		 * @param msg Message to send to \c std::cout along with a waiting message.
		 */
		void shortWait(const std::string& msg) noexcept;
	};

	/**
	 * Tests renderer.h classes.
	 * @warning I've straight up disabled these tests for now to speed up test development
	 *          elsewhere: don't forget to reenable these tests later!
	 */
	class test_renderer : public test::test_case {
	public:
		/**
		 * Assigns the name "renderer_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_renderer(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c renderer class.
		 */
		void renderer();
	};

	/**
	 * Tests texture.h classes.
	 * @warning I've straight up disabled these tests for now to speed up test development
	 *          elsewhere: don't forget to reenable these tests later!
	 */
	class test_texture : public test::test_case {
	public:
		/**
		 * Assigns the name "texture_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_texture(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c animated_spritesheet and \c animated_sprite classes.
		 */
		void animation();
	};

	/**
	 * Tests userinput.h classes.
	 */
	class test_ui : public test::test_case {
	public:
		/**
		 * Assigns the name "ui_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_ui(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c userinput class.
		 */
		void ui();
	};

	/**
	 * Tests file.h classes.
	 */
	class test_file : public test::test_case {
	public:
		/**
		 * Assigns the name "file_test_case.log" to the output file.
		 * @param path The path in which to save the output file.
		 */
		test_file(const std::string& path) noexcept;

		/**
		 * Runs all the tests defined in the private methods of this class.
		 */
		void runTests() noexcept;
	private:
		/**
		 * Runs tests related to the \c binary_file class.
		 */
		void file();
	};
}