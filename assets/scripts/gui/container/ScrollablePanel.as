/**
 * @file ScrollablePanel.as
 * Defines the \c ScrollablePanel class.
 */

/**
 * Represents a scrollable panel container in a GUI.
 */
shared class ScrollablePanel : Panel {
    /**
     * Create the scrollable panel container.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c PanelListBox.
     */
    ScrollablePanel() {
        super(ScrollablePanelEngineName);
    }

    /**
     * Create the scrollable panel container.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c PanelListBox.
     * @param widgetType The name of the type of container to create.
     */
    ScrollablePanel(const string&in widgetType) {
        super(widgetType);
    }

    float getScrollbarWidth() const {
        return ::getScrollbarWidth(this);
    }

    void setVerticalScrollbarAmount(const uint a) {
        ::setVerticalScrollbarAmount(this, a);
    }
    
    bool isVerticalScrollbarVisible() const {
        return ::isVerticalScrollbarVisible(this);
    }

    void setHorizontalScrollbarAmount(const uint a) {
        ::setHorizontalScrollbarAmount(this, a);
    }

    void setHorizontalScrollbarPolicy(const ScrollbarPolicy p) {
        ::setHorizontalScrollbarPolicy(this, p);
    }
    
    bool isHorizontalScrollbarVisible() const {
        return ::isHorizontalScrollbarVisible(this);
    }
}
