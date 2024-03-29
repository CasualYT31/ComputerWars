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

/**@file audio.hpp
 * Declares classes which utilise the SFML's audio module. The \c audio class
 * unifies both streamable audio and short sounds under one interface. A JSON
 * script is used to load specific sound files and assign each of them names. The
 * client then uses these names to interact with the audio files.
 */

#pragma once

#include "safejson.hpp"
#include "resourcepool.hpp"
#include "renderer.hpp"
#include "SFML/Audio.hpp"
#include <queue>

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

		/**
		 * If this flag is \c TRUE, this music will instead be treated like a
		 * sound.
		 * This means that they will play and stop immediately, instead of being
		 * pushed to the queue, and if they need to be stopped, their name needs to
		 * be explicitly given to the stop method (they cannot fade out).\n
		 * This flag was added (instead of allowing sounds to be loopable) so that
		 * sounds can have custom loop points just like music.
		 */
		bool soundMode = false;
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
	class audio : public engine::json_script, public sfx::delta_timer {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		audio(const engine::logger::data& data);

		/**
		 * Processes the music queue.
		 */
		void process();

		/**
		 * Retrieves the base volume of all audio objects.
		 * @return The volume, a value between \c 0.0 and \c 100.0.
		 */
		float getVolume() const noexcept;

		/**
		 * Updates the base volume of all audio objects.
		 * @param  newvolume The new volume, a value between \c 0.0 and \c 100.0.
		 * @safety No guarantee. The new volume is always guaranteed to be assigned
		 *         to \c _volume.
		 * @sa     _validateVolume()
		 */
		void setVolume(float newvolume);

		/**
		 * Plays a specified audio object.
		 * If a sound is played, it will be played immediately without being
		 * queued.\n
		 * If music is played, a \c stop() action will be queued first, and it will
		 * be given the \c length parameter. Then, the given music's play action
		 * will be queued.\n
		 * If an unregistered name was given, an error will be logged.\n
		 * If an empty name is given, this call will have no effect.
		 * @param  name The name of the audio object to play.
		 * @safety No guarantee.
		 */
		void play(const std::string& name,
			const sf::Time& length = sf::seconds(1.0));

		/**
		 * Stops the currently playing music at the time the queued stop is
		 * actioned, either immediately or with a fade out.
		 * @warning Avoid changing any volume value during the fadeout: undefined
		 *          behaviour will ensue.
		 * @param   length The duration of the fade out. \c sf::Time::Zero should
		 *                 be given if you wish to stop the music immediately.
		 */
		void stop(const sf::Time& length = sf::seconds(1.0));

		/**
		 * Immediately stops a sound, or a piece of music if it is in sound mode.
		 * @param name The name of the sound or piece of music to stop. Cannot stop
		 *             music that is not in sound mode, for that use the other
		 *             \c stop() method.
		 */
		void stop(const std::string& name);

		/**
		 * Gets the name of the current music, whether playing or paused.
		 * If all music is in the stopped state, a blank string is returned. Note
		 * that this method does not return names of any sounds that may be
		 * playing.
		 * @return The name of the current music.
		 * @safety Strong guarantee.
		 */
		inline std::string getCurrentMusic() const {
			return _currentMusic;
		}
	private:
		/**
		 * The JSON load method for this class.
		 * All keys are names of audio objects to store. Each of these keys must
		 * have an object value. Within these object values, there are several
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
		 *         \c sfx::music::loopTo. <b>If both loop properties are given to a
		 *         sound type, \c sfx::music::soundMode will be set to \c TRUE and
		 *         the sound will become music.</b></li>
		 *     <li>\c "loopwhen" <em>Music type only</em> Used to apply
		 *         \c sfx::music::loopWhen. <b>If both loop properties are given to
		 *         a sound type, \c sfx::music::soundMode will be set to \c TRUE
		 *         and the sound will become music.</b></li></ul>
		 * 
		 * All other keys within these object values are ignored.
		 * @warning All calls to this method will clear the internal collections of
		 *          sound and music objects, even if \c FALSE is returned.
		 * @param   j The \c engine::json object representing the contents of the
		 *            loaded script which this method reads.
		 * @return  \c TRUE if all audio files could be loaded successfully,
		 *          \c FALSE if at least one couldn't be loaded.
		 * @safety  No guarantee.
		 * @sa      setVolume()
		 */
		bool _load(engine::json& j);

		/**
		 * Helper method called in \c _load() which reads all paths for each audio
		 * object and loads the files.
		 * This method also applies all the initial configurations, such as base
		 * and offset volumes, and looping properties. Any audio files that could
		 * not be loaded will be logged.
		 * @return \c TRUE if all audio objects could be loaded, \c FALSE if at
		 *         least one could not.
		 * @safety No guarantee.
		 */
		bool _loadAudio();

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
		 * @return The volume of the audio object after its offset has been
		 *         applied.
		 */
		float _volumeAfterOffset(const std::string& name) const;

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
		 * The granularity of fadeouts, which is 100.0.
		 * Setting this to very small values (< 10.0) doesn't sound very nice, and
		 * anything higher sounds similar to \c 100.0 anyway, so I saw little point
		 * in letting the client choose the granularity of fadeouts.
		 */
		static constexpr float _granularity = 100.0f;

		/**
		 * The music queue.
		 * When play and stop requests are made, they are pushed to this queue, and
		 * carried out in sequence. The only reason why the queue should
		 * temporarily block is if a fade out is on-going.
		 */
		std::queue<std::function<bool(void)>> _queue;
	};

	/**
	 * A collection of \c audio objects.
	 */
	class audios : public engine::resource_pool<sfx::audio> {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param data The data to initialise the logger object with.
		 * @sa    \c engine::logger
		 */
		audios(const engine::logger::data& data);
	private:
		/**
		 * The JSON load method for this class.
		 * Within the root object, there are simply a list of key-string pairs,
		 * with the keys defining the names of the audio objects, and the string
		 * values containing the paths to the JSON scripts to pass to the \c load()
		 * method of each \c audio.
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return \c TRUE if every audio file could be loaded successfully,
		 *         \c FALSE if even one of the \c load() methods returned \c FALSE.
		 * @safety No guarantee.
		 */
		bool _load(engine::json& j);

		/**
		 * The JSON save method for this class.
		 * Simply rewrites the audio list.
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j);
	};
}
