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

/**@file audio.h
 * Declares classes which utilise the SFML's audio module. The \c audio class
 * unifies both streamable audio and short sounds under one interface. A JSON
 * script is used to load specific sound files and assign each of them names. The
 * client then uses these names to interact with the audio files.
 */

#pragma once

#include "safejson.h"
#include "SFML/Audio.hpp"

namespace sfx {
	/**
	 * This structure is used to store information on a single sound object.
	 */
	struct sound {
		/**
		 * The SFML sound buffer object.
		 */
		sf::SoundBuffer buffer;

		/**
		 * The SFML sound object which is acted upon (play, pause, stop, etc.).
		 */
		sf::Sound sound;

		/**
		 * The path of the audio file which is loaded into the buffer object.
		 */
		std::string path;

		/**
		 * The offset to apply to this sound object's volume.
		 */
		float volumeOffset = 0.0;
	};

	/**
	 * This structure is used to store information on a single background audio
	 * object.
	 */
	struct music {
		/**
		 * The SFML music object which is acted upon (play, pause, stop, etc.).
		 */
		sf::Music music;

		/**
		 * The path of the audio file which is loaded into the music object.
		 */
		std::string path;

		/**
		 * The time stamp of the music, in milliseconds, to loop back to.
		 * By default, this value is 0, which means the beginning of the music.\n
		 * This and the \c loopWhen time stamp must be set to a negative value in
		 * order to cancel looping!
		 * @sa loopWhen
		 */
		sf::Int32 loopTo = 0;

		/**
		 * The time stamp of the music, in milliseconds, which causes the music to
		 * loop back to \c loopTo.
		 * By default, this value is -1. A negative value signifies the end of the
		 * music.\n
		 * If both this and \c loopTo are negative, looping is cancelled.
		 * @sa loopTo
		 */
		sf::Int32 loopWhen = -1;

		/**
		 * The offset to apply to this music object's volume.
		 */
		float volumeOffset = 0.0;
	};

	/**
	 * Class which represents a collection of audio files.
	 * Both background audio and sounds are unified under one interface.
	 * @remark You can emulate different sound groups with their separate volumes
	 *         by instantiating separate \c audio objects with their own volumes.
	 *         Using this technique, all music objects could go in one \c audio
	 *         object, and sounds can go in another \c audio object, each having
	 *         their own volume.
	 * @sa     _load()
	 */
	class audio : public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "audio."
		 * @sa    \c engine::logger
		 */
		audio(const std::string& name = "audio") noexcept;

		/**
		 * Retrieves the base volume of all audio objects.
		 * @return The volume, a value between \c 0.0 and \c 100.0.
		 */
		float getVolume() const noexcept;

		/**
		 * Updates the base volume of all audio objects.
		 * @param newvolume The new volume, a value between \c 0.0 and \c 100.0.
		 * @sa    _validateVolume()
		 */
		void setVolume(float newvolume) noexcept;

		/**
		 * Plays a specified audio object.
		 * If a sound is played, it will always start from the beginning unless it
		 * was paused, in which case playback will resume. If music is played, the
		 * currently playing or paused music (if any) will be stopped, and the new
		 * one will be played from the beginning or resumed if it was paused. If
		 * sound is played, it will play in conjunction with any other sound
		 * playing at the time of calling, as well as with the background music if
		 * it is being played. If an unregistered name was given, an error will be
		 * logged.
		 * @param name The name of the audio object to play. If a blank string, the
		 *             previously accessed \c music object will be substituted.
		 */
		void play(std::string name = "") noexcept;

		/**
		 * Stops a specified audio object.
		 * If a blank or invalid sound object name is given, the current music will
		 * stop.
		 * @param name The name of the sound object to stop.
		 */
		void stop(const std::string& name = "") noexcept;

		/**
		 * Pauses a specified audio object.
		 * If a blank or invalid sound object name is given, the current music will
		 * pause.
		 * @param name The name of the sound object to pause.
		 */
		void pause(const std::string& name = "") noexcept;

		/**
		 * Fades out the currently playing music until it is stopped.
		 * This method is to be called within the game loop continuously until it
		 * returns \c TRUE. It adjusts the specific music object's volume only and
		 * not the overall volume.\n
		 * Copies of the volume and granularity values are made on the first call
		 * to a fadeout, so that any undefined behaviour that could have occurred
		 * if either of these values were changed mid-fadeout is avoided. Any
		 * changes to the base volume will take affect once the fadeout is over.
		 * @param   length The amount of time that passes between music volume to
		 *                 zero volume.
		 * @return  \c TRUE if the fadeout is complete, \c FALSE if not.
		 */
		bool fadeout(sf::Time length = sf::seconds(1.0)) noexcept;

		/**
		 * How finely the \c fadeout() method decreases the music volume.
		 * @return The fadeout granularity value.
		 */
		float getGranularity() const noexcept;

		/**
		 * Defines how finely the \c fadeout() method decreases the music volume.
		 * The higher the value, the finer the fadeout.\n
		 * Note that calling \c load() on this class will not reset the
		 * granularity.
		 * @param newval The new granularity value.
		 */
		void setGranularity(float newval) noexcept;

		/**
		 * Gets the name of the current music, whether playing or paused.
		 * If all music is in the stopped state, a blank string is returned. Note
		 * that this method does not return names of any sounds that may be
		 * playing.
		 * @return The name of the current music.
		 */
		std::string getCurrentMusic() const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * Within the root object, there is one special key, \c "volume", which
		 * stores the floating point base volume of each audio object.\n
		 * 
		 * All other keys are names of audio objects to store. Each of these keys
		 * must have an object value. Within these object values, there are several
		 * key-value pairs to include:
		 * 
		 * <ul><li>\c "path" contains the path of the audio file.</li>
		 *     <li>\c "offset" is the volume offset to apply to the base volume for
		 *         this audio object. Defaults to 0.0. It's helpful when your audio
		 *         assets are of different volumes and you're not easily able to
		 *         adjust them directly.</li>
		 *     <li>\c "type" must be either \c "sound" or \c "music" exactly. If an
		 *         invalid type is provided, \c "sound" will be assumed and a
		 *         warning will be logged.</li>
		 *     <li>\c "loopto" <em>Music type only</em> Used to apply
		 *         \c sfx::music::loopTo.</li>
		 *     <li>\c "loopwhen" <em>Music type only</em> Used to apply
		 *         \c sfx::music::loopWhen.</li></ul>
		 * 
		 * All other keys within these object values are ignored.
		 * @warning All calls to this method will clear the internal collections of
		 *          sound and music objects, even if \c FALSE is returned.
		 * @param   j The \c engine::json object representing the contents of the
		 *            loaded script which this method reads.
		 * @return  \c TRUE if all audio files could be loaded successfully,
		 *          \c FALSE if at least one couldn't be loaded.
		 * @sa      setVolume()
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON
		 * script that this method produces.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * Helper method called in \c _load() which reads all paths for each audio
		 * object and loads the files.
		 * This method also applies all the initial configurations, such as base
		 * and offset volumes, and looping properties. Any audio files that could
		 * not be loaded will be logged.
		 * @return \c TRUE if all audio objects could be loaded, \c FALSE if at
		 *         least one could not.
		 */
		bool _loadAudio() noexcept;

		/**
		 * Validates a given volume value.
		 * All volume values will be automatically adjusted if they are outside the
		 * range of 0.0-100.0 within this method. If below 0.0, it will be reset to
		 * 0.0. If above 100.0, it will be reset to 100.0.
		 * @param v The volume value to validate.
		 */
		void _validateVolume(float& v) noexcept;

		/**
		 * Applies an audio object's volume offset to the base volume and returns
		 * it.
		 * If the corresponding base volume is below 1.0, 0.0 will always be
		 * returned. The final volume value will never fall below 1.0 or rise above
		 * 100.0.
		 * @param  name       The name of the audio object to calculate the volume
		 *                    offset of.
		 * @param  baseVolume The base volume of all audio objects.
		 * @return The volume of the audio object after its offset has been
		 *         applied.
		 */
		float _volumeAfterOffset(const std::string& name, const float baseVolume)
			const noexcept;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;

		/**
		 * The collection of sound objects.
		 */
		std::unordered_map<std::string, sfx::sound> _sound;

		/**
		 * The collection of music objects.
		 */
		std::unordered_map<std::string, sfx::music> _music;

		/**
		 * The name of the current music.
		 * Blank represents the fact that no music is currently playing.
		 */
		std::string _currentMusic = "";

		/**
		 * The base volume.
		 */
		float _volume = 50.0f;

		/**
		 * Flag which keeps track of whether or not a piece of music is fading out
		 * or not.
		 */
		bool _fadingOut = false;

		/**
		 * Clock which is used to time fadeouts.
		 */
		sf::Clock _clock;

		/**
		 * The granularity of fadeouts.
		 */
		float _granularity = 100.0;

		/**
		 * Granularity used with the current fadeout.
		 */
		float _currentGranularity = _granularity;

		/**
		 * Volume used with the current fadeout.
		 */
		float _currentVolume = _volume;
	};
}