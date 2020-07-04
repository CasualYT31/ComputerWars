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

/**@file renderer.h
 * Classes used for rendering.
 * These classes were intended to accompany, and not replace or encapsulate, the SFML dependency.
 * The \c renderer class is an \c sf::RenderWindow that reads and writes simple properties to a JSON script,
 * such as size, caption, and style flags.
 * The \c animated_drawable class is intended to introduce a unified approach to dealing with more
 * complex drawables that change state in a pre-determined way.
 * @todo Add support for icons.
 * @todo Allow all settings to be overriden in-code.
 */

#pragma once

#include "safejson.h"
#include "sfml/Graphics.hpp"

/**
 * The \c sfx namespace contains components which build upon existing SFML classes.
 */
namespace sfx {
	/**
	 * This class is a 'dynamically' configurable render window.
	 * It inherits from \c sf::RenderWindow publicly, so it should be treated like any other \c sf::RenderWindow.
	 * There are only two differences:
	 * <ol><li>The inheritance of \c safe::json_script to allow for the configuration of the render window externally.</li>
	 *     <li>The option to open the window with these internal configurations.</li></ol>
	 */
	class renderer : public sf::RenderWindow, public safe::json_script {
	public:
		/**
		 * Opens the render window using internal configurations.
		 * These internal configurations can only be changed via reading from and writing to a JSON script.
		 * Since this class is an \c sf::RenderWindow, the \c create() method can be used. However, only the size
		 * of the window can be updated in the internal configurations if this done: any change in any other property
		 * won't be saved.
		 * @param settings The context settings to pass to the \c create() method.
		 */
		void openWindow(const sf::ContextSettings& settings = sf::ContextSettings()) noexcept;
	private:
		/**
		 * The JSON load method for this class.
		 * There are a variety of key-value pairs that are stored in the root object of the script:
		 * <table><tr>Key<th></th><th>Data Type</th><th>Meaning</th></tr>
		 * <tr><td>width</td><td>unsigned integer</td><td>The width of the render window in pixels.</td></tr>
		 * <tr><td>height</td><td>unsigned integer</td><td>The height of the render window in pixels.</td></tr>
		 * <tr><td>x</td><td>integer</td><td>The X position of the render window in pixels.</td></tr>
		 * <tr><td>y</td><td>integer</td><td>The Y position of the render window in pixels.</td></tr>
		 * <tr><td>framerate</td><td>unsigned integer</td><td>The frame rate limit of the render window in frames per second.</td></tr>
		 * <tr><td>caption</td><td>string</td><td>The caption of the render window.</td></tr>
		 * <tr><td>close</td><td>bool</td><td>\c TRUE if the render window has a close button.</td></tr>
		 * <tr><td>def</td><td>bool</td><td>\c TRUE if the render window has default styles (titlebar, resize, close).</td></tr>
		 * <tr><td>fullscreen</td><td>bool</td><td>\c TRUE if the render window is in full screen. In which case, \c width and \c height both have to form a valid video mode.</td></tr>
		 * <tr><td>none</td><td>bool</td><td>\c TRUE if the render window has no styles.</td></tr>
		 * <tr><td>resize</td><td>bool</td><td>\c TRUE if the render window can be resized.</td></tr>
		 * <tr><td>titlebar</td><td>bool</td><td>\c TRUE if the render window has a titlebar.</td></tr></table>
		 * <tr><td>vsync</td><td>bool</td><td>\c TRUE if the render window has vertical sync enabled.</td></tr></table>
		 * <tr><td>cursor</td><td>bool</td><td>\c TRUE if the render window shows the OS mouse cursor.</td></tr></table>
		 * <tr><td>grabbedmouse</td><td>bool</td><td>\c TRUE if the render window keeps the mouse cursor within the window if in set focus.</td></tr></table>
		 * All other keys will be ignored.
		 * @param  j The \c safe::json object representing the contents of the loaded script which this method reads.
		 * @return Always returns \c TRUE.
		 */
		virtual bool _load(safe::json& j) noexcept;
		/**
		 * The JSON save method for this class.
		 * Please see \c _load() for a detailed summary of the format of JSON script that this method produces.
		 * The width and height of the render window at the time of the client calling this method are stored.
		 * All other values are not changed from the last call to \c load().
		 * @param  j The \c nlohmann::json object representing the JSON script which this method writes to.
		 * @return Always returns \c TRUE.
		 */
		virtual bool _save(nlohmann::json& j) noexcept;

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
#pragma pack(push, 1)
		/**
		 * This packed structure contains all the style flags associated with a render window object.
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
			 * Stores whether or not the mouse cursor is visible within the render window.
			 */
			bool mouseVisible = true;
			/**
			 * Stores whether or not the mouse is kept within the render window whilst in set focus.
			 */
			bool mouseGrabbed = false;
		} style; /*!< This stores all the style flags associated with this render window instance. */
#pragma pack(pop)
	};

	/**
	 * \c sf::Drawable that animates.
	 * This class was introduced for two main reasons:
	 * <ol><li>To introduce a common apporach to complex animated drawables.</li>
	 *     <li>To bundle a delta timer with each drawable.</li></ol>
	 * Here's how you'd use this class:
	 * <ol><li>Inherit from \c animated_drawable just like you would with \c Drawable. Implement \c draw() as normal.</li>
	 *     <li>Implement \c animate(): these are the calculations performed to make a drawable animate.</li>
	 *     <li>Within \c animate(), if a delta timer is required, make sure to add the following line of code: <tt>float delta = calculateDelta();</tt></li>
	 *     <li>When working with your \c animated_drawable object, \c animate() should be called within the draw loop at some point before the corresponding \c draw().</li></ol>
	 */
	class animated_drawable : public sf::Drawable {
	public:
		/**
		 * Method which performs calculations on a drawable before drawing it.
		 * \c Drawable rightly prevents a subclass from changing its state within its \c draw() implementation.
		 * However, in some cases, drawables have a consistent animation which must be maintained throughout all instances.
		 * This method must be implemented by subclasses to make changes to the internal state, ready for drawing later.
		 * It is to accompany the \c draw() method, so it should be called within the draw loop, before it's drawn.
		 * @param  target The render target which the drawable is to be later drawn to.
		 * @return This method can optionally return \c TRUE to signify that an animation has completed, or \c FALSE if it has not.
		 *         Alternatively, subclasses can ignore this return value if this is unimportant to them.
		 */
		virtual bool animate(sf::RenderTarget& target) noexcept = 0;
	protected:
		/**
		 * Calculates the time elapsed from the last call to this method.
		 * The \c animate() method should animate things independent from the render target's frame rate.
		 * In order to achieve this, the time since the last frame update is measured, and any calculations in
		 * transforms etc. can include this value to ensure animations remain smooth.
		 * A local variable, preferably called \c delta, should be declared in \c animate(), and it should store the
		 * result of this method.
		 * The unit used with this method is \b seconds.
		 * @return The time elapsed since the last call \c calculateDelta().
		 */
		float calculateDelta() noexcept;
	private:
		/**
		 * Clock used to measure time between frames.
		 */
		sf::Clock _deltaTimer;
	};
}