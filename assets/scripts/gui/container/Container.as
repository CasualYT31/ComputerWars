/**
 * @file Container.as
 * Defines the \c Container class.
 */

/**
 * Represents a container in a GUI.
 */
shared abstract class Container : Widget {
    /**
     * Create the container.
     * @param widgetType The name of the type of container to create.
     */
    protected Container(const string&in widgetType) {
        super(widgetType);
    }

    void add(const Widget@ const child) {
        ::add(this, child);
    }

    void setIndexOfWidget(const uint64 o, const uint64 n) {
        setWidgetIndexInContainer(this, o, n);
    }
}
