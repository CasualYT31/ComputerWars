/**
 * @file Panel.as
 * Defines the \c Panel class.
 */

/**
 * Represents a panel container in a GUI.
 */
shared class Panel : Group {
    /**
     * Create the panel container.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c ScrollablePanel.
     */
    Panel() {
        super(PanelEngineName);
    }

    /**
     * Create the panel container.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c ScrollablePanel.
     * @param widgetType The name of the type of container to create.
     */
    Panel(const string&in widgetType) {
        super(widgetType);
    }

    void setBackgroundColour(const Colour&in c) {
        ::setWidgetBackgroundColour(this, c);
    }

    void setBorderSize(const float s) {
        ::setWidgetBorderSize(this, s);
    }

    void setBorderSizes(const string&in l, const string&in t, const string&in r,
        const string&in b) {
        ::setWidgetBorderSizes(this, l, t, r, b);
    }

    void setBorderRadius(const float s) {
        ::setWidgetBorderRadius(this, s);
    }

    void setBorderColour(const Colour&in c) {
        ::setWidgetBorderColour(this, c);
    }
}
