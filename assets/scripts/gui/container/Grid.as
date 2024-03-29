/**
 * @file Grid.as
 * Defines the \c Grid class.
 */

/**
 * Represents a grid in a GUI.
 */
shared class Grid : Container {
    /**
     * Create the grid.
     */
    Grid() {
        super(GridEngineName);
    }

    void add(const Widget@ const child, const uint64 r, const uint64 c) {
        addWidgetToGrid(this, child, r, c);
    }

    uint64 getColumnCount() const {
        return getWidgetColumnCount(this);
    }

    uint64 getRowCount() const {
        return getWidgetRowCount(this);
    }

    void setAlignmentOfWidget(const uint64 r, const uint64 c,
        const WidgetAlignment a) {
        setWidgetAlignmentInGrid(this, r, c, a);
    }
}
