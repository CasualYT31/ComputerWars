/**
 * @file Group.as
 * Defines the \c Group class.
 */

/**
 * Represents a group container in a GUI.
 */
shared class Group : Container {
    /**
     * Create the group container.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c Panel.
     */
    Group() {
        super(GroupEngineName);
    }

    /**
     * Create the group container.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c Panel.
     * @param widgetType The name of the type of container to create.
     */
    Group(const string&in widgetType) {
        super(widgetType);
    }
}
