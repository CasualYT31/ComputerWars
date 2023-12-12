/**
 * @file ClickableWidget.as
 * Defines the \c ClickableWidget class.
 */

/**
 * Represents a clickable widget in a GUI.
 */
shared class ClickableWidget : Widget {
    /**
     * Create the clickable widget.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c Button.
     */
    ClickableWidget() {
        super(ClickableWidgetEngineName);
    }

    /**
     * Create the clickable widget.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c Button.
     * @param widgetType The name of the type of widget to create.
     */
    ClickableWidget(const string&in widgetType) {
        super(widgetType);
    }
}
