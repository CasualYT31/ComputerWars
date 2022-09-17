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

/**@file banktest.hpp
 * Tests the \c bank.h classes.
 */

#include "sharedfunctions.hpp"
#include "bank.hpp"

TEST(BankTest, OldTest) {
	awe::bank<awe::movement_type> move;
	move.load(getTestAssetPath("bank/move.json"));
	awe::bank<awe::unit_type> bank;
	bank.load(getTestAssetPath("bank/unit.json"));
	awe::updateAllMovementsAndLoadedUnits(bank, move);
	EXPECT_EQ(bank.size(), (std::size_t)11);
	EXPECT_TRUE(bank[0]);
	EXPECT_TRUE(bank[7]);
	EXPECT_TRUE(bank[7]->canLoad(bank[0]));
}