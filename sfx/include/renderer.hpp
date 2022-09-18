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

/**@file renderer.hpp
 * Classes used for rendering.
 * These classes were intended to accompany, and not replace or encapsulate, the
 * SFML dependency. The \c renderer class is an \c sf::RenderWindow that reads and
 * writes simple properties to a JSON script, such as size, caption, and style
 * flags. The client can also use the \c renderer_settings structure to apply
 * different properties at runtime, which can then be saved to a JSON script.\n
 * The \c animated_drawable class is intended to introduce a unified approach to
 * dealing with more complex drawables that change state in a pre-determined way.
 */

#pragma once

#include "safejson.hpp"
#include "SFML/Graphics.hpp"

namespace sfx {
	/**
	 * \c sf::Drawable that animates.
	 * This class was introduced for two main reasons:
	 * <ol><li>To introduce a common apporach to complex animated drawables.</li>
	 *     <li>To bundle a delta timer with each drawable.</li></ol>
	 * Here's how you'd use this class:
	 * <ol><li>Inherit from \c animated_drawable just like you would with
	 *         \c Drawable. Implement \c draw() as normal.</li>
	 *     <li>Implement \c animate(): these are the calculations performed to make
	 *         a drawable animate.</li>
	 *     <li>Within \c animate(), if a delta timer is required, make sure to add
	 *         the following line of code: <tt>float delta = calculateDelta();</tt>
	 *         If you need to accumulate delta overtime and reset the timer under
	 *         specific conditions, replace \c calculateDelta() with
	 *         \c accumulateDelta() and ensure to reset the accumulation using
	 *         \c resetDeltaAccumulation() at some point within the method.</li>
	 *     <li>When working with your \c animated_drawable object, \c animate()
	 *         should be called within the draw loop at some point before the
	 *         corresponding \c draw().</li></ol>
	 */
	class animated_drawable : public sf::Drawable {
	public:
		/**
		 * Polymorphic base classes should have virtual destructors.
		 */
		virtual ~animated_drawable() noexcept = default;

		/**
		 * Method which performs calculations on a drawable before drawing it.
		 * \c Drawable rightly prevents a subclass from changing its state within
		 * its \c draw() implementation. However, in some cases, drawables have a
		 * consistent animation which must be maintained throughout all instances.
		 * This method must be implemented by subclasses to make changes to the
		 * internal state, ready for drawing later. It is to accompany the
		 * \c draw() method, so it should be called within the draw loop, before
		 * it's drawn.
		 * @param  target  The render target which the drawable is to be later
		 *                 drawn to.
		 * @param  scaling Any scaling factor applied separately to \c target that
		 *                 this method should factor in when making calculations
		 *                 based off of the \c target's dimensions.
		 * @return This method can optionally return \c TRUE to signify that an
		 *         animation has completed, or \c FALSE if it has not.
		 *         Alternatively, subclasses can ignore this return value if it's
		 *         unimportant to them.
		 */
		virtual bool animate(const sf::RenderTarget& target,
			const double scaling = 1.0) noexcept = 0;
	protected:
		/**
		 * Calculates the time elapsed from the last call to this method.
		 * The \c animate() method should animate things independently from the
		 * render target's frame rate. In order to achieve this, the time since the
		 * last frame update is measured, and any calculations in transforms etc.
		 * can include this value to ensure animations play out in a consistent
		 * time frame. A local variable, preferably called \c delta, should be
		 * declared in \c animate(), and it should store the result of this
		 * method.\n
		 * There is an additional feature incorporated within the delta timer, and
		 * that is the delta timeout mechanic. After a certain length of time has
		 * elapsed since the last call to this method, or since the
		 * \c animated_drawable object was constructed, the delta timer will be
		 * reset upon the next call to this method, instead of this method
		 * returning the length of time that has elapsed since the last call or
		 * since object construction (returning \c 0 time instead). This feature
		 * was added so that the client could instantiate an \c animated_drawable
		 * whenever it wanted or needed to, and then animate and draw it later
		 * without any repercussions. It also removes after effects visible when
		 * the same \c animated_drawable is animated and drawn, then not drawn,
		 * then animated and drawn again some time later.
		 * @warning Do not forget to handle cases where the \c delta returned is
		 *          @c 0!
		 * @param   timeout In the case that your \c animated_drawable animates at
		 *                  a frame rate slower than the default timeout duration,
		 *                  you can provide a different duration here.
		 * @return  The time elapsed since the last call to \c calculateDelta(), in
		 *          seconds.
		 * @sa      sfx::animated_drawable::_timeout
		 */
		float calculateDelta(const sf::Time& timeout = sf::seconds(1.0f)) noexcept;

		/**
		 * Retrieves the delta accumulated overtime.
		 * This method calls \c calculateDelta() once.\n
		 * Upon every call to \c calculateDelta(), the return value is added to
		 * \c _delta internally. This is useful for drawables that don't act on
		 * delta directly, but overtime (i.e. can't reset the delta timer every
		 * single time they call \c calculateDelta()).
		 * @warning Do not forget to handle cases where the \c delta returned is
		 *          @c 0!
		 * @param   timeout See \c calculateDelta().
		 * @return  The delta accumulated since the object's construction, or since
		 *          the last call to \c resetDeltaAccumulation().
		 * @sa      \c calculateDelta()
		 * @sa      \c resetDeltaAccumulation()
		 */
		float accumulatedDelta(const sf::Time& timeout = sf::seconds(1.0f))
			noexcept;

		/**
		 * Resets delta accumulation.
		 * @param to The value to reset delta to. Defaults to \c 0.
		 * @sa    \c accumulatedDelta()
		 */
		void resetDeltaAccumulation(const float to = 0.0f) noexcept;

		/**
		 * Can be used to designate an animation finished.
		 * This method is paired with the \c isFinished() method to created
		 * \c animated_drawable classes that are intended to represent a single-use
		 * animation that does not loop. This feature is completely optional and
		 * can be ignored entirely by subclasses if so desired.
		 * @sa isFinished()
		 */
		void finish() noexcept;

		/**
		 * Finds out if this animation is finished.
		 * This method is paired with the \c finish() method. It allows subclasses
		 * to prevent further animation and/or drawing if the animation is
		 * finished. This method will always return \c FALSE if \c finish() has not
		 * been called.
		 * @return \c TRUE if the animation is finished, \c FALSE if not.
		 * @sa     finish()
		 */
		bool isFinished() const noexcept;

		/**
		 * Determines if the animated drawable hasn't been animated yet.
		 * Whilst setting up an \c animated_drawable should preferably be carried
		 * out in its constructor, this mechanic can be useful in the case where
		 * doing so is not as convenient. Instead, a subclass' \c animate() method
		 * can call this method in the first line to determine if further setup is
		 * required (for example, setup relying on the specific target provided).
		 * @return \c TRUE if \c calculateDelta() hasn't been called yet, \c FALSE
		 *         otherwise.
		 */
		bool firstTimeAnimated() const noexcept;

		/**
		 * Resets the animation so that it is unfinished and has not yet been
		 * animated.
		 * This method can be used with non-single-use \c animated_drawables to
		 * treat the drawable as if it hadn't yet been animated. This is useful
		 * when the drawable has been given different properties and so it must be
		 * reinitialised in \c animate() appropriately.
		 * @sa firstTimeAnimated()
		 */
		void resetAnimation() noexcept;
	private:
		/**
		 * Clock used to measure time between frames.
		 */
		sf::Clock _deltaTimer;

		/**
		 * Accumulates delta overtime.
		 */
		float _delta = 0.0f;

		/**
		 * Flag used to determine if the animation is finished or not.
		 */
		bool _finished = false;

		/**
		 * Flag used to determine if the animation hasn't been animated yet.
		 */
		bool _firsttime = true;
	};

	/**
	 * This structure contains a collection of settings that can be applied to a
	 * \c renderer object.
	 */
	struct renderer_settings {
		/**
		 * Stores the width of the render window in pixels.
		 */
		unsigned int width = 1280;

		/**
		 * Stores the height of the render window in pixels.
		 */
		unsigned int height = 720;

		/**
		 * Stores the X position of the render window in pixels.
		 */
		int x = 0;

		/**
		 * Stores the Y position of the render window in pixels.
		 */
		int y = 0;

		/**
		 * Stores the frame rate limit of the render window.
		 */
		unsigned int framerate = 0;

		/**
		 * Stores the caption of the render window.
		 */
		std::string caption = "Application";

		/**
		 * Stores the path to the image file to use as the icon for this window.
		 * Blank represents the default icon.
		 */
		std::string iconPath = "";

		/**
		 * The OpenGL context settings the renderer is to use.
		 */
		sf::ContextSettings contextSettings;

#pragma pack(push, 1)
		/**
		 * This packed structure contains all the style flags associated with a
		 * render window object.
		 */
		struct style_flags {
			/**
			 * Corresponds to the \c sf::Style::Close style bit.
			 */
			bool close = false;

			/**
			 * Corresponds to the \c sf::Style::Default style bit.
			 */
			bool def = true;

			/**
			 * Corresponds to the \c sf::Style::Fullscreen style bit.
			 */
			bool fullscreen = false;

			/**
			 * Corresponds to the \c sf::Style::None style bit.
			 */
			bool none = false;

			/**
			 * Corresponds to the \c sf::Style::Resize style bit.
			 */
			bool resize = false;

			/**
			 * Corresponds to the \c sf::Style::Titlebar style bit.
			 */
			bool titlebar = false;

			/**
			 * Stores whether or not V-Sync is on.
			 */
			bool vsync = false;

			/**
			 * Stores whether or not the mouse cursor is visible within the render
			 * window.
			 */
			bool mouseVisible = true;

			/**
			 * Stores whether or not the mouse is kept within the render window
			 * whilst in set focus.
			 */
			bool mouseGrabbed = false;
		}
		/**
		 * This stores all the style flags associated with this
		 * \c renderer_settings instance.
		 */
		style;
#pragma pack(pop)
	};

	/**
	 * This class is a 'dynamically' configurable render window.
	 * It inherits from \c sf::RenderWindow publicly, so it should be treated like
	 * any other \c sf::RenderWindow. There are only two differences:
	 * <ol><li>The inheritance of \c engine::json_script to allow for the
	 *         configuration of the render window externally.</li>
	 *     <li>The [preferred] option to open the window with these internal
	 *         configurations.</li></ol>
	 */
	class renderer : public sf::RenderWindow, public engine::json_script {
	public:
		/**
		 * Initialises the internal logger object.
		 * @param name The name to give this particular instantiation within the
		 *             log file. Defaults to "renderer."
		 * @sa    \c engine::logger
		 */
		renderer(const std::string& name = "renderer") noexcept;

		/**
		 * Opens the render window using configurations.
		 * Since this class is an \c sf::RenderWindow, the \c create() method can
		 * be used. However, only the size and position of the window can be
		 * updated in the internal configurations if this is done: any change in
		 * any other property won't be saved.\n
		 * In addition to this, if the client uses \c create() to switch from
		 * windowed to fullscreen or vice versa, positional data may not be saved
		 * correctly.\n
		 * For these reasons it is discouraged to use \c create() with this class.
		 */
		void openWindow() noexcept;

		/**
		 * Used to acquire the current renderer settings.
		 * @return A reference to the current renderer settings.
		 * @sa     setSettings()
		 */
		const sfx::renderer_settings& getSettings() const noexcept;

		/**
		 * Used to update the renderer's settings.
		 * This is the preferred way of updating/reopening the renderer, as it
		 * ensures that all properties can be saved via \c save().\n
		 * Before this method returns, it will call \c openWindow() to apply all
		 * the changes given.
		 * @remark The client can use both the \c getSettings() and
		 *         \c setSettings() methods to provide their own overrides to
		 *         certain settings if they so desired.
		 * @param  newSettings The new settings to apply.
		 * @sa     getSettings()
		 * @sa     openWindow()
		 */
		void setSettings(const sfx::renderer_settings& newSettings) noexcept;

		/**
		 * Animate an \c sfx::animated_drawable object.
		 * This method was introduced to provide an alternative way to animate
		 * \c animated_drawables that's more akin to SFML's \c draw() architecture.
		 * \c sfx::animated_drawable's \c animate() method remains public, so that
		 * it can remain compatible with \c sf::RenderWindow.
		 * @param drawable The animated drawable to animate.
		 * @param scaling  Any scaling factor that should be considered when
		 *                 animating
		 * @sa    sfx::animated_drawable::animate()
		 */
		bool animate(sfx::animated_drawable& drawable,
			const double scaling = 1.0) const noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * There are a variety of key-value pairs that are stored in the root
		 * object of the script. All other keys will be ignored.
		 * <table><tr><th>Key</th><th>Data Type</th><th>Meaning</th></tr>
		 *        <tr><td>width</td>
		 *        <td>unsigned integer</td>
		 *        <td>The width of the render window in pixels.</td></tr>
		 * 
		 *        <tr><td>height</td>
		 *        <td>unsigned integer</td>
		 *        <td>The height of the render window in pixels.</td></tr>
		 * 
		 *        <tr><td>x</td>
		 *        <td>integer</td>
		 *        <td>The X position of the render window in pixels.</td></tr>
		 * 
		 *        <tr><td>y</td>
		 *        <td>integer</td>
		 *        <td>The Y position of the render window in pixels.</td></tr>
		 * 
		 *        <tr><td>framerate</td>
		 *        <td>unsigned integer</td>
		 *        <td>The frame rate limit of the render window in frames per
		 *            second.</td></tr>
		 * 
		 *        <tr><td>caption</td>
		 *        <td>string</td>
		 *        <td>The caption of the render window.</td></tr>
		 * 
		 *        <tr><td>icon</td>
		 *        <td>string</td>
		 *        <td>The path of the image file to apply as the icon of the
		 *            window. Defaults to the default OS icon (blank string).
		 *            </td></tr>
		 * 
		 *        <tr><td>close</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window has a close button.</td></tr>
		 * 
		 *        <tr><td>def</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window has default styles (titlebar,
		 *            resize, close).</td></tr>
		 * 
		 *        <tr><td>fullscreen</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window is in full screen. In which
		 *            case, \c width and \c height both have to form a valid video
		 *            mode.</td></tr>
		 * 
		 *        <tr><td>none</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window has no styles.</td></tr>
		 * 
		 *        <tr><td>resize</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window can be resized.</td></tr>
		 * 
		 *        <tr><td>titlebar</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window has a titlebar.</td></tr>
		 * 
		 *        <tr><td>vsync</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window has vertical sync enabled.</td>
		 *             </tr>
		 * 
		 *        <tr><td>cursor</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window shows the OS mouse cursor.</td>
		 *             </tr>
		 * 
		 *        <tr><td>grabbedmouse</td>
		 *        <td>bool</td>
		 *        <td>\c TRUE if the render window keeps the mouse cursor within
		 *            the window if in set focus.</td></tr></table>
		 * @param  j The \c engine::json object representing the contents of the
		 *           loaded script which this method reads.
		 * @return Always returns \c TRUE.
		 */
		bool _load(engine::json& j) noexcept;

		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON
		 * script that this method produces. The width and height of the render
		 * window at the time of the client calling this method are stored. All
		 * other values are not changed from the last call to \c load().
		 * @param  j The \c nlohmann::ordered_json object representing the JSON
		 *           script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		bool _save(nlohmann::ordered_json& j) noexcept;

		/**
		 * The internal logger object.
		 */
		mutable engine::logger _logger;
		
		/**
		 * The settings of this renderer object.
		 */
		sfx::renderer_settings _settings;
	};
}