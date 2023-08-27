/**
 * @file CommandWidget.as
 * Declares the command menu widget type.
 * This widget is a vertical layout of bitmap buttons. The widget will ensure that
 * all buttons remain the same height, by increasing the height of the layout each
 * time a button is added.
 */

/**
 * Used to create widgets that represent a command menu in-game.
 */
class CommandWidget {
	/**
	 * The full name of the layout containing all of the command buttons.
	 */
	string layout;

	/**
	 * The width of the layout.
	 */
	string layoutWidth;

	/**
	 * The height of each command button, in pixels.
	 */
	float commandHeight;

	/**
	 * Doesn't create any widgets.
	 */
	CommandWidget() {}

	/**
	 * Creates the vertical layout which will hold all of the command buttons.
	 * @param layoutName The full name of the layout which will contain all of the
	 *                   other widgets.
	 * @param width      The width to keep the layout at.
	 * @param height     The height to assign to each command button.
	 */
	CommandWidget(const string&in layoutName, const string&in width = "25%",
		const float height = 25.0) explicit {
		layout = layoutName;
		layoutWidth = width;
		commandHeight = height;
		addWidget(VerticalLayout, layout);
	}

	/**
	 * Adds a command button.
	 * If either \c spriteName or \c sheetName is empty, no sprite will be
	 * assigned.
	 * @param  widgetName The name to give to the bitmap button. Must \b not be a
	 *                    fullname!
	 * @param  widgetText The text to assign to the command button.
	 * @param  spriteName The key of the sprite to assign to this command button.
	 * @param  sheetName  The name of the sheet which contains the sprite.
     * @return The full name of the new command button widget.
	 */
	string addCommand(const string&in widgetName, const string&in widgetText,
		const string&in spriteName, const string&in sheetName = "icon") {
		const string fullWidgetName = layout + "." + widgetName;
		addWidget(BitmapButton, fullWidgetName);
		if (!spriteName.isEmpty() && !sheetName.isEmpty()) {
			setWidgetSprite(fullWidgetName, sheetName, spriteName);
		}
		setWidgetText(fullWidgetName, widgetText);
		_resizeLayout();
        _configureDirectionalFlow(fullWidgetName);
        return fullWidgetName;
	}

	/**
	 * Removes all command buttons from the layout.
	 */
	void removeAllCommands() {
		removeWidgetsFromContainer(layout);
        _bitmapButtons.resize(0);
		_resizeLayout();
	}

	/**
	 * Resizes the layout based on the number of command buttons present.
	 */
	private void _resizeLayout() {
		setWidgetSize(layout, layoutWidth,
			formatFloat(getWidgetCount(layout) * commandHeight) + "px");
	}

    /**
     * Reconfigures the directional flow of each BitmapButton.
     * @param newWidget Full name of the new widget being added.
     */
    private void _configureDirectionalFlow(const string&in newWidget) {
        // Better implementation would be to implement getWidgetDirectionalFlow()
        // in C++ and then just change the widgets I need to.
        _bitmapButtons.insertLast(newWidget);
        const auto len = _bitmapButtons.length();
        if (len == 1) {
            setWidgetDirectionalFlowStart(newWidget);
        } else {
            for (uint i = 0; i < len; ++i) {
                const auto up =
                    i == 0 ? _bitmapButtons[len - 1] : _bitmapButtons[i - 1];
                const auto down =
                    i == len - 1 ? _bitmapButtons[0] : _bitmapButtons[i + 1];
                setWidgetDirectionalFlow(_bitmapButtons[i], up, down, "", "");
            }
        }
    }

    /**
     * Cache of the BitmapButton names.
     */
    private array<string> _bitmapButtons;
}
