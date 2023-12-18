/**
 * @file TabContainer.as
 * Defines the \c TabContainer class.
 */

/**
 * Represents a tab container in a GUI.
 */
shared class TabContainer : Container {
    /**
     * Create the tab container.
     */
    TabContainer() {
        super(TabContainerEngineName);
    }

    WidgetID addTabAndPanel(const string&in t, array<any>@ const v = null) {
        const auto id = ::addTabAndPanel(this, t, v);
        if (id != NO_WIDGET) panelIDs.insertLast(id);
        return id;
    }

    bool removeTabAndPanel(const WidgetID id) {
        const auto result = ::removeTabAndPanel(id);
        if (result) panelIDs.removeAt(panelIDs.find(id));
        return result;
    }

    void setSelectedTab(const uint64 t) {
        ::setSelectedTab(this, t);
    }

    void deselectTab() {
        ::deselectTab(this);
    }

    int getSelectedTab() const {
        return ::getSelectedTab(this);
    }

    void setTabEnabled(const uint64 t, const bool e) {
        ::setTabEnabled(this, t, e);
    }

    bool getTabEnabled(const uint64 t) const {
        return ::getTabEnabled(this, t);
    }

    uint64 getTabCount() const {
        return ::getTabCount(this);
    }

    string getTabText(const uint64 t) const {
        return ::getTabText(this, t);
    }

    /**
     * Holds the IDs of all the panels in the tab container.
     * @warning We may have to account for Container::remove()/removeAll()?
     */
    private array<WidgetID> panelIDs;
    const array<WidgetID>@ panel {
        get const {
            return @panelIDs;
        }
    }
}
