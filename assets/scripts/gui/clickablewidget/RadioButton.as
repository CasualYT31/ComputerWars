/**
 * @file RadioButton.as
 * Defines the \c RadioButton class.
 */

/**
 * Represents a radio button in a GUI.
 */
shared class RadioButton : ClickableWidget {
    /**
     * Create the radio button.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c CheckBox.
     */
    RadioButton() {
        super(RadioButtonEngineName);
    }

    /**
     * Create the radio button.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c CheckBox.
     * @param widgetType The name of the type of widget to create.
     */
    RadioButton(const string&in widgetType) {
        super(widgetType);
    }

    void setText(const string&in caption) {
        ::setWidgetText(this, caption);
    }

    void setChecked(const bool c) {
        ::setWidgetChecked(this, c);
    }

    bool getChecked() const {
        return ::isWidgetChecked(this);
    }
}
