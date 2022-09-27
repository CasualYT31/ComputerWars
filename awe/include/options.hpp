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

/**@file options.hpp
 * Declares the class which represents options that \c awe::game will accept when
 * creating a new game.
 */

#pragma once

#include "script.hpp"
#include "typedef.hpp"
#include "bank.hpp"
#include <optional>

namespace awe {
	/**
	 * Holds options that are to be passed to \c awe::game when loading a new map.
	 */
	class game_options {
	public:
		using CommanderPair = std::pair<std::optional<awe::BankID>,
			std::optional<awe::BankID>>;

		/**
		 * Registers this struct with the script interface, if it hasn't been
		 * already.
		 */
		static void Register(asIScriptEngine* engine,
			const std::shared_ptr<DocumentationGenerator>& document) noexcept;
		/**
		 * Factory function for this reference type.
		 * @return Pointer to the new object.
		 */
		static awe::game_options* Create() noexcept;
		/**
		 * Copy reference to this object.
		 */
		void AddRef() const noexcept;
		/**
		 * Release reference to this object.
		 */
		void Release() const noexcept;

		/**
		 * Sets the current CO override for a given army.
		 * @param armyID The ID of the army whose current CO will be overridden.
		 * @param bankID The bank index of the commander to override with.
		 */
		void setCurrentCO(const awe::ArmyID armyID, const awe::BankID bankID)
			noexcept;

		/**
		 * Sets the tag CO override for a given army.
		 * @param armyID The ID of the army whose current CO will be overridden.
		 * @param bankID The bank index of the commander to override with.
		 */
		void setTagCO(const awe::ArmyID armyID, const awe::BankID bankID) noexcept;

		/**
		 * Allows the user to specify that they wish to override the tag CO for an
		 * army with a lack of a CO.
		 * @param armyID The ID of the army to override.
		 * @param tag    \c TRUE if there should be no tag CO, \c FALSE if the tag
		 *               CO should be overriden, if an override is given. \c FALSE
		 *               is the default.
		 */
		void setNoTagCO(const awe::ArmyID armyID, const bool tag) noexcept;

		/**
		 * Sets the team override for a given army.
		 * @param armyID The ID of the army whose team will be overridden.
		 * @param teamID The team ID to override with.
		 */
		void setTeam(const awe::ArmyID armyID, const awe::TeamID teamID) noexcept;

		/**
		 * Returns an override \c commander, if it has been configured.
		 * This method will throw if a bank isn't given, if the specified army
		 * wasn't configured with an override, or if the configured commander index
		 * was out of range.
		 * @param  armyID The ID of the army whose current CO override is being
		 *                queried.
		 * @param  coBank Pointer to the commander bank to pull the \c commander
		 *                from.
		 * @return Pointer to the \c commander, if there is an override. \c nullptr
		 *         if the current CO should be overridden with a lack of a CO.
		 * @throws range_error if no override should be applied. If there was an
		 *                     actual error, \c what() will return a non-empty
		 *                     string.
		 */
		std::shared_ptr<const awe::commander> getCurrentCO(
			const awe::ArmyID armyID,
			const std::shared_ptr<awe::bank<awe::commander>>& coBank) const;

		/**
		 * Returns an override \c commander, if it has been configured.
		 * This method will throw if a bank isn't given, if the specified army
		 * wasn't configured with an override, or if the configured commander index
		 * was out of range.
		 * @param  armyID The ID of the army whose tag CO override is being
		 *                queried.
		 * @param  coBank Pointer to the commander bank to pull the \c commander
		 *                from.
		 * @return Pointer to the \c commander, if there is an override. \c nullptr
		 *         if the tag CO should be overridden with a lack of a CO.
		 * @throws range_error if no override should be applied. If there was an
		 *                     actual error, \c what() will return a non-empty
		 *                     string.
		 */
		std::shared_ptr<const awe::commander> getTagCO(const awe::ArmyID armyID,
			const std::shared_ptr<awe::bank<awe::commander>>& coBank) const;

		/**
		 * Returns an override \c TeamID, if it has been configured.
		 * This method will throw if the specified army wasn't configured with an
		 * override.
		 * @param  armyID The ID of the army whose team override is being queried.
		 * @return The team ID override, if there is an override.
		 * @throws range_error if no override should be applied. If there was an
		 *                     actual error, \c what() will return a non-empty
		 *                     string.
		 */
		awe::TeamID getTeam(const awe::ArmyID armyID) const;
	private:
		/**
		 * Reference counter.
		 */
		mutable asUINT _refCount = 1;

		/**
		 * Stores CO overrides for armies.
		 */
		std::unordered_map<awe::ArmyID, CommanderPair> _commanderOverrides;

		/**
		 * Stores "no tag" configurations.
		 */
		std::unordered_map<awe::ArmyID, bool> _noTags;

		/**
		 * Stores team configurations.
		 */
		std::unordered_map<awe::ArmyID, awe::TeamID> _teamOverrides;
	};
}