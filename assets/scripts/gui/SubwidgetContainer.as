/**
 * @file SubwidgetContainer.as
 * Defines the \c SubwidgetContainer class.
 */

/**
 * Represents a widget in a GUI that is internal comprised of multiple children.
 */
shared abstract class SubwidgetContainer : Widget {
    /**
     * Create the widget.
     * @param widgetType The name of the type of subwidget container to create.
     */
    protected SubwidgetContainer(const string&in widgetType) {
        super(widgetType);
    }
}
