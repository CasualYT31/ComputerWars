/**
 * @file BoxLayoutRatios.as
 * Defines the \c BoxLayoutRatios class.
 */

/**
 * Represents a box layout ratios container in a GUI.
 */
shared class BoxLayoutRatios : BoxLayout {
    /**
     * Create the box layout ratios container.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c HorizontalLayout.
     */
    BoxLayoutRatios() {
        super(BoxLayoutRatiosEngineName);
    }

    /**
     * Create the box layout ratios container.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c HorizontalLayout.
     * @param widgetType The name of the type of container to create.
     */
    BoxLayoutRatios(const string&in widgetType) {
        super(widgetType);
    }

    void setRatioOfWidget(const uint64 i, const float r) {
        setWidgetRatioInLayout(this, i, r);
    }
}
