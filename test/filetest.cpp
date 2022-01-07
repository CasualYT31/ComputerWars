/*Copyright 2019-2022 CasualYouTuber31 <naysar@protonmail.com>

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

/**@file filetest.h
 * Tests the \c engine::binary_file class.
 */

#include "sharedfunctions.h"
#include "file.h"

/**
 * Tests \c engine::binary_file::convertNumber().
 * These tests should work out regardless of the byte ordering on the running
 * system.
 */
TEST(FileTest, ConvertNumber) {
	EXPECT_EQ(engine::binary_file::convertNumber<unsigned int>(255),
		(unsigned int)4278190080);
	EXPECT_EQ(engine::binary_file::convertNumber<long long>(255),
		(long long)-72057594037927936);
	EXPECT_NE(engine::binary_file::convertNumber<float>(1.0), (float)1.0);
	EXPECT_NE(engine::binary_file::convertNumber<double>(1.0), (double)1.0);
}

/**
 * Tests behaviour when a non-existent file is loaded by a \c binary_file object.
 */
TEST(FileTest, ReadNonExistentFile) {
	engine::binary_file file;
	EXPECT_ANY_THROW(file.open("badfile.bin", true));
}

/**
 * Tests behaviour when an existent file is read and acted upon via a
 * \c binary_file object.
 */
TEST(FileTest, ReadExistentFile) {
	engine::binary_file file;
	EXPECT_NO_THROW({
		file.open(getTestAssetPath("file/test.bin"), true);
		sf::Int32 number = file.readNumber<sf::Int32>();
		double decimal = file.readNumber<double>();
		std::string str = file.readString();
		bool flag = file.readBool();
		EXPECT_EQ(number, 13463);
		EXPECT_DOUBLE_EQ(decimal, -98.74);
		EXPECT_EQ(str, "Hello, World!");
		EXPECT_TRUE(flag);
		EXPECT_EQ(file.position(), 30);
		file.close();
	});
}

/**
 * Tests writing and reading a binary file via \c binary_file.
 */
TEST(FileTest, WriteAndRead) {
	engine::binary_file file;
	EXPECT_NO_THROW({
		file.open(getTestAssetPath("file/output.bin"), false);
		file.writeNumber<sf::Uint64>(7562);
		file.writeBool(false);
		file.writeBool(true);
		file.writeNumber<float>(45.1f);
		file.writeString("this is a\ntest");
		file.writeNumber<sf::Int8>(127);
		EXPECT_EQ(file.position(), 33);
		file.close();
		file.open(getTestAssetPath("file/output.bin"), true);
		EXPECT_EQ(file.position(), 0);
		EXPECT_EQ(file.readNumber<sf::Uint32>(), 7562);
		EXPECT_EQ(file.readNumber<sf::Uint32>(), 0);
		EXPECT_FALSE(file.readBool());
		EXPECT_EQ(file.position(), 9);
		EXPECT_TRUE(file.readBool());
		EXPECT_FLOAT_EQ(file.readNumber<float>(), 45.1f);
		EXPECT_EQ(file.position(), 14);
		EXPECT_EQ(file.readString(), "this is a\ntest");
		EXPECT_EQ(file.position(), 32);
		EXPECT_EQ(file.readNumber<sf::Int8>(), 127);
		file.close();
		EXPECT_EQ(file.position(), 33);
	});
}