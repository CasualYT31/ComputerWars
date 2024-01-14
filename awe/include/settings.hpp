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

/**
 * @file settings.hpp
 * Declares the class which brings together all the others to maintain the engine
 * as it executes.
 */

#pragma once

#include "safejson.hpp"
#include "map.hpp"
#include "audio.hpp"

namespace awe {
	/**
	 * Stores the game's global settings.
	 */
	class settings : public engine::json_script {
	public:
		/**
		 * Sets up the \c settings object with pointers to objects the settings
		 * will apply to.
		 * @param data       The data to initialise the logger object with.
		 * @param audios     The audio collection to update. Cannot be \c nullptr.
		 * @param dictionary The language dictionary to update. Cannot be
		 *                   \c nullptr.
		 */
		settings(const engine::logger::data& data,
			const std::shared_ptr<sfx::audios>& audios,
			const std::shared_ptr<engine::language_dictionary>& dictionary);

		/**
		 * Updates the selected animation preset.
		 * @param p The animation preset selected by the user.
		 */
		void setAnimationPreset(const awe::animation_preset p) noexcept;

		/**
		 * Retrieves the currently selected animation preset.
		 * @return The animation preset selected by the user.
		 */
		awe::animation_preset getAnimationPreset() const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * The following key-value pairs are expected:
		 * <ul><li>\c volumes - object of key-float pairs - keys refer to the audio
		 *     objects defined in the audios JSON script, and the floats store the
		 *     configured volume of that audio object. If an audio object is not
		 *     given a volume, it will default to 100.0.</li>
		 *     <li>\c language - string - The language key of the currently
		 *     selected language. If a key is not given, the first language in the
		 *     relevant JSON script will be selected.</li>
		 *     <li>\c preset - unsigned - The last selected map animation preset.
		 *     Will be to set to \c VisualA by default.</li></ul>
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return Always returns \c TRUE.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * The JSON save method for this class.
		 * Simply saves the values of this object's fields to the script.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j);

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * Pointer to the audio collection to update.
		 */
		std::shared_ptr<sfx::audios> _audios;

		/**
		 * Pointer to the language dictionary to update.
		 */
		std::shared_ptr<engine::language_dictionary> _dictionary;

		/**
		 * The current animation preset.
		 */
		awe::animation_preset _animationPreset = awe::animation_preset::Debug;
	};
}
