/**
 * @file ButtonBase.as
 * Defines the \c ButtonBase class.
 */

/**
 * Represents any type of button widget in a GUI.
 */
shared class ButtonBase : ClickableWidget {
    /**
     * Create the button.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c Button.
     */
    ButtonBase() {
        super(ButtonBaseEngineName);
    }

    /**
     * Create the button.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c Button.
     * @param widgetType The name of the type of widget to create.
     */
    ButtonBase(const string&in widgetType) {
        super(widgetType);
    }

    void setText(const string&in caption) {
        setWidgetText(this, caption);
    }
}
