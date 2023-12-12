/**
 * @file BoxLayout.as
 * Defines the \c BoxLayout class.
 */

/**
 * Represents a box layout container in a GUI.
 */
shared class BoxLayout : Group {
    /**
     * Create the box layout container.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c BoxLayoutRatios.
     */
    BoxLayout() {
        super(BoxLayoutEngineName);
    }

    /**
     * Create the box layout container.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c BoxLayoutRatios.
     * @param widgetType The name of the type of container to create.
     */
    BoxLayout(const string&in widgetType) {
        super(widgetType);
    }

    void setSpaceBetweenWidgets(const float s) {
        ::setSpaceBetweenWidgets(this, s);
    }
}
