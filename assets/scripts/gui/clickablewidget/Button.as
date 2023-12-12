/**
 * @file Button.as
 * Defines the \c Button class.
 */

/**
 * Represents a button widget in a GUI.
 */
shared class Button : ButtonBase {
    /**
     * Create the button.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c BitmapButton.
     */
    Button() {
        super(ButtonEngineName);
    }

    /**
     * Create the button.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c Bitmaputton.
     * @param widgetType The name of the type of widget to create.
     */
    Button(const string&in widgetType) {
        super(widgetType);
    }
}
