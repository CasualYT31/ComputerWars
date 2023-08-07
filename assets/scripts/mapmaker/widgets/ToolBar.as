/**
 * @file ToolBar.as
 * Defines code responsible for manageing the Map Maker's ToolBar.
 */

/**
 * Defines the data required to setup a ToolBar button.
 */
class ToolBarButtonSetUpData {
    /// Initialises the class.
    ToolBarButtonSetUpData(const string&in sn, const string&in s) {
        shortName = sn;
        sprite = s;
    }

    /// The short name of the button.
    string shortName;

    /// The sprite to assign to the button.
    string sprite;
}

/**
 * Represents a ToolBar.
 */
class ToolBar {
    /**
     * Sets up the ToolBar.
     * Automatically selects the first button given.
     * @param name    The full name of the ToolBar's \c ChildWindow.
     * @param buttons The buttons to assign to this ToolBar.
     */
    void setUp(const string&in name,
        const array<const ToolBarButtonSetUpData@>@ const buttons) {
        // Create the ChildWindow.
        window = name;
        addWidget("ChildWindow", window);
        setWidgetText(window, "toolbar");
        setChildWindowTitleButtons(window, TitleButton::Close);
        setWidgetResizable(window, false);

        // Create each BitmapButton.
        buttonCount = formatUInt(buttons.length());
        for (uint i = 0, len = buttons.length(); i < len; ++i) {
            const auto data = buttons[i];
            const auto btn = window + "." + data.shortName;
            addWidget("BitmapButton", btn);
            connectSignalHandler(btn, SignalHandler(this.buttonHandler));
            setWidgetPosition(btn,
                PADDING + "+(" + BUTTON_WIDTH + "+" + PADDING + ")*" +
                    formatUInt(i),
                PADDING
            );
            setWidgetSprite(btn, "icon", data.sprite);
            // Automatically select the first tool.
            if (i == 0) buttonHandler(btn, "Pressed");
        }

        dock();
    }

    /**
     * Restores and docks the ToolBar to the top of the screen.
     */
    void dock() {
        setWidgetSize(window,
            PADDING + "+(" + BUTTON_WIDTH + "+" + PADDING + ")*" + buttonCount,
            PADDING + "*2+" + BUTTON_WIDTH + "+" +
                formatFloat(getTitleBarHeight(window))
        );
        setWidgetOrigin(window, 0.5f, 0.0f);
        openChildWindow(window, "50%", "0%");
    }
    
    /**
     * Updates the currently selected tool when a button is pressed.
     * @param widgetName The full name of the \c BitmapButton widget.
     * @param signalName The name of the signal emitted.
     */
    private void buttonHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed" && widgetName != previousButton) {
            buttonName = widgetName.substr(widgetName.findLast(".") + 1);
            setWidgetBackgroundColour(widgetName, Colour(200, 200, 200, 255));
            if (!previousButton.isEmpty()) {
                setWidgetBackgroundColour(previousButton,
                    Colour(245, 245, 245, 255));
            }
            previousButton = widgetName;
        }
    }

    /// The short name of the last pressed button.
    private string buttonName;
    string tool { get const { return buttonName; } }

    /// The name of the \c ChildWindow.
    private string window;

    /// The number of buttons.
    private string buttonCount = "0";

    /// Size of the padding around each \c BitmapButton.
    private string PADDING = "5";

    /// Width of each \c BitmapButton.
    private string BUTTON_WIDTH = "30";

    /// The previously selected button.
    private string previousButton;
}
