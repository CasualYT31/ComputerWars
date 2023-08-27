/**
 * @file ObjectDialog.as
 * Defines code that manages a \c ChildWindow that collects a group of
 * \c ObjectPanels.
 */

/**
 * Holds data on each \c ObjectPanel.
 */
class ObjectPanelSetUpData {
    /// Signature of the callback invoked when mapping an object button's short
    /// name to its object.
    funcdef ref@ MapNameToObject(const string&in);

    /**
     * Signature of the function that is invoked when a sprite array has to be
     * generated for a list of object buttons.
     * @param  const string&in Owner script name.
     * @return Array of sprite keys.
     */
    funcdef array<string>@ GetSpritesFunc(const string&in);

    /// The short name to give to the base \c Group widget.
    string group;

    /// The text to assign to this \c ObjectPanel's tab.
    string tabText;
    
    /// \c CurrentlySelectedObject class which this \c ObjectPanel interacts with.
    CurrentlySelectedObject@ currentlySelectedObject;

    /// The size of each object button.
    Vector2f buttonSize;

    /// \c TRUE if a Neutral option should be prepended to the owner \c ComboBox.
    /// \c FALSE if the first army in the turn order should be selected in
    /// \c setUp().
    bool neutralAvailable;

    /// The number of items to show in the owner \c ComboBox at one time.
    uint64 numberOfOwnerItems;

    /// Maps a name to an object.
    MapNameToObject@ mapNameToObject;

    /// Spritesheet to use with the object buttons.
    string spritesheet;

    /// Pointer to a function which is called when object sprites have to be
    /// refreshed.
    /// Can be one of \c @generateTileSpriteArray or \c @generateUnitSpriteArray.
    GetSpritesFunc@ generateSpritesArray;

    /// A list of script names to generate object buttons for.
    const array<string>@ scriptNames;

    /// Name of the owner icon \c Picture widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string ownerIcon;

    /// Name of the \c HorizontalWrap widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string wrap;

    /// Name of the \c ScrollablePanel widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string scrollablePanel;

    /// Name of the \c ComboBox widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string comboBox;

    /// Name of the base \c Group widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string groupFullname;

    /// Full name of the \c ComboBox widget.
    /// Set by the \c ObjectDialog class, so you can leave this blank.
    string comboBoxFullname;
}

/**
 * Holds data \c ObjectDialog uses to set itself up with.
 */
class ObjectDialogSetUpData {
    /// Name of the \c ChildWindow that represents the \c ObjectDialog.
    string window;

    /// The text/title to assign to the \c ChildWindow.
    string windowText;

    /// The \c ObjectPanel widgets to create in this \c ChildWindow.
    array<ObjectPanelSetUpData@> objectPanels;
}

/**
 * Allows the user to switch between different \c ObjectPanel widgets.
 */
class ObjectDialog {
    /**
     * Sets up the \c ChildWindow, \c Tabs, and \c ObjectPanel widgets.
     * @param data Data to initialise the \c ObjectDialog with.
     */
    void setUp(ObjectDialogSetUpData@ const data) {
        @initData = data;

        // Child window.
        addWidget(ChildWindow, data.window);
        setWidgetText(data.window, data.windowText);

        // Tabs.
        tabsName = data.window + ".Tabs";
        addWidget(Tabs, tabsName);
        connectSignalHandler(tabsName, SignalHandler(this.tabsSignalHandler));

        // Set up each ObjectPanel, and select the first tab if there is one.
        const auto len = data.objectPanels.length();
        for (uint i = 0; i < len; ++i)
            addPanel(data.window, data.objectPanels[i], i == 0);
    }

    /**
     * Opens and docks an \c ObjectDialog to one side of the screen.
     * Does nothing and logs an error if there is no tab selected.
     * @param onRight If \c TRUE, dock to the right of the screen. If \c FALSE,
     *                dock to the left.
     */
    void dock(const bool onRight = true, const float defaultColumnCount = 5.0f) {
        const auto data = getCurrentPanelData();
        if (data !is null) {
            const auto defaultWidth = formatFloat(
                data.buttonSize.x * defaultColumnCount +
                getScrollbarWidth(data.scrollablePanel) + 2.0f
            );
            if (onRight) {
                openChildWindow(initData.window, "100%-" + defaultWidth, "0");
                setWidgetSize(initData.window, defaultWidth, "100%");
            } else {
                openChildWindow(initData.window, "0", "0");
                setWidgetSize(initData.window, defaultWidth, "100%");
            }
        }
    }

    /**
     * Gets the index of the currently selected tab.
     * @return The index. <tt>< 0</tt> if no tab is selected.
     */
    int getSelectedTab() const {
        return ::getSelectedTab(tabsName);
    }

    /**
     * Selects an owner from the currently selected tab's \c ComboBox.
     * Logs an error if \c NO_ARMY is given for a \c ComboBox that does not
     * support neutral owners.
     * @param newOwner \c ArmyID of the new owner.
     */
    void setSelectedOwner(const ArmyID newOwner) {
        const auto data = getCurrentPanelData();
        if (data !is null) {
            if (newOwner == NO_ARMY && !data.neutralAvailable) {
                error("Attempted to set owner with ID " + formatArmyID(newOwner) +
                    "to ComboBox in group \"" + data.group + "\", which does not "
                    "allow a neutral owner.");
            } else if (data.neutralAvailable) {
                setSelectedItem(data.comboBoxFullname,
                    newOwner == NO_ARMY ? 0 : newOwner + 1);
            } else {
                setSelectedItem(data.comboBoxFullname, newOwner);
            }
        }
    }
    
    /**
     * The height of the owner group and \c CurrentlySelectedObject group.
     */
    private string GROUP_HEIGHT = "35px";
    
    /**
     * The padding of the owner group and \c CurrentlySelectedObject group.
     */
    private string GROUP_PADDING = "5px";

    /**
     * Handle to the data given to \c setUp().
     */
    private ObjectDialogSetUpData@ initData;

    /**
     * Name of the \c Tabs widget.
     */
    private string tabsName;

    /**
     * Adds an \c ObjectPanel to the \c ObjectDialog.
     * @param childWindow The name of the \c ChildWindow to add the
     *                    \c ObjectDialog to.
     * @param data        The data to set the \c ObjectPanel up with.
     * @param selectTab   If \c TRUE, select the newly created tab.
     */
    private void addPanel(const string&in childWindow,
        ObjectPanelSetUpData@ const data, const bool selectTab) {
        // Add a tab for the ObjectPanel.
        addTab(tabsName, data.tabText);
        const auto tabsHeight = formatFloat(getWidgetFullSize(tabsName).y);

        // Add base Group widget.
        data.groupFullname = childWindow + "." + data.group;
        addWidget(Group, data.groupFullname);
        setWidgetSize(data.groupFullname, "100%", "100%-" + tabsHeight);
        setWidgetOrigin(data.groupFullname, 0.5f, 1.0f);
        setWidgetPosition(data.groupFullname, "50%", "100%");
        setWidgetVisibility(data.groupFullname, false);

        // CurrentlySelectedObject group.
        const auto csoGroup = data.groupFullname + ".CurrentObjectGroup";
        addWidget(Group, csoGroup);
        setWidgetSize(csoGroup, "100%", GROUP_HEIGHT);
        setWidgetOrigin(csoGroup, 0.5f, 0.0f);
        setWidgetPosition(csoGroup, "50%", "0%");
        data.currentlySelectedObject.addWidget(csoGroup + ".CurSelObjectLayout");

        // Scrollable panel.
        data.scrollablePanel = data.groupFullname + ".ScrollablePanel";
        addWidget(ScrollablePanel, data.scrollablePanel);
        setWidgetSize(data.scrollablePanel, "100%", "100%-" + GROUP_HEIGHT + "-" +
            GROUP_HEIGHT);
        setWidgetPosition(data.scrollablePanel, "50%", "50%");
        setWidgetOrigin(data.scrollablePanel, 0.5f, 0.5f);
        setVerticalScrollbarAmount(data.scrollablePanel, 25);
        setHorizontalScrollbarPolicy(data.scrollablePanel,
            ScrollbarPolicy::Never);

        // Object buttons.
        data.wrap = data.scrollablePanel + "." + data.group + "Wrap";
        addWidget(HorizontalWrap, data.wrap);
        connectSignalHandler(data.wrap,
            SignalHandler(this.horizontalWrapSignalHandler));
        for (uint t = 0, len = data.scriptNames.length(); t < len; ++t) {
            const auto objName = data.scriptNames[t];
            const auto btn = data.wrap + "." + objName;
            addWidget(BitmapButton, btn);
            connectSignalHandler(btn,
                SignalHandler(this.objectButtonSignalHandler));
            // TODO-4: We should really find the tallest sprite and base the
            // button sizes on that.
            setWidgetSize(btn, formatFloat(data.buttonSize.x),
                formatFloat(data.buttonSize.y));
        }
        
        // SelectOwner widgets //
        const auto selectOwnerGroup = data.groupFullname + ".SelectOwnerGroup";
        addWidget(Group, selectOwnerGroup);
        setWidgetSize(selectOwnerGroup, "100%", GROUP_HEIGHT);
        setWidgetOrigin(selectOwnerGroup, 0.5f, 1.0f);
        setWidgetPosition(selectOwnerGroup, "50%", "100%");

        const auto ownerIconGroup = selectOwnerGroup + ".OwnerIconGroup";
        addWidget(Group, ownerIconGroup);
        setWidgetSize(ownerIconGroup, GROUP_HEIGHT, "100%");
        setGroupPadding(ownerIconGroup, "5px");

        data.ownerIcon = ownerIconGroup + ".OwnerIcon";
        addWidget(Picture, data.ownerIcon);
        setWidgetOrigin(data.ownerIcon, 0.5f, 0.5f);
        setWidgetPosition(data.ownerIcon, "50%", "50%");

        const auto ownerComboboxGroup = selectOwnerGroup + ".OwnerComboboxGroup";
        addWidget(Group, ownerComboboxGroup);
        setWidgetSize(ownerComboboxGroup, "100%-" + GROUP_HEIGHT, "100%");
        setWidgetPosition(ownerComboboxGroup, GROUP_HEIGHT, "0px");
        setGroupPadding(ownerComboboxGroup, "0px", "5px", "5px", "5px");

        if (selectTab) setSelectedTab(tabsName, getTabCount(tabsName) - 1);
        data.comboBox = data.group + "OwnerCombobox";
        data.comboBoxFullname = ownerComboboxGroup + "." + data.comboBox;
        addWidget(ComboBox, data.comboBoxFullname);
        connectSignalHandler(data.comboBoxFullname,
            SignalHandler(this.comboBoxSignalHandler));
        setWidgetSize(data.comboBoxFullname, "100%", "100%");
        awe::addCountriesToList(data.comboBoxFullname, data.neutralAvailable);
        setItemsToDisplay(data.comboBoxFullname, data.numberOfOwnerItems);
        setSelectedItem(data.comboBoxFullname, 0);
    }

    /**
     * Gets a handle to the currently selected \c ObjectPanel's data.
     * If \c null is returned, an error will be logged.
     * @return Handle to the data, \c null if it could not be retrieved.
     */
    private ObjectPanelSetUpData@ getCurrentPanelData() {
        const auto tab = ::getSelectedTab(tabsName);
        if (tab < 0) {
            error("Unexpected selected tab value: " + formatInt(tab));
            return null;
        } else return initData.objectPanels[tab];
    }

    /**
     * Resizes the new \c HorizontalWrap when a new tab is selected.
     * @param widgetName The full name of the \c Tabs widget.
     * @param signalName The name of the signal emitted.
     */
    private void tabsSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TabSelected") {
            const auto data = getCurrentPanelData();
            if (data !is null) {
                for (int i = 0, len = initData.objectPanels.length(); i < len;
                    ++i) {
                    const auto groupName = initData.objectPanels[i].groupFullname;
                    if (groupName.length() == 0) continue;
                    setWidgetVisibility(groupName,
                        false);
                }
                setWidgetVisibility(data.groupFullname, true);
                horizontalWrapSignalHandler(data.wrap, "MouseEntered");
            }
        }
    }

    /**
     * Select an object when an object button is pressed.
     * @param widgetName The full name of the object button.
     * @param signalName The name of the signal emitted.
     */
    private void objectButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            const auto data = getCurrentPanelData();
            if (data !is null) {
                data.currentlySelectedObject.object =
                    data.mapNameToObject(
                        widgetName.substr(widgetName.findLast(".") + 1)
                    );
            }
        }
    }

    /**
     * Resize the \c HorizontalWrap if the mouse enters it.
     * @param widgetName The full name of the \c HorizontalWrap.
     * @param signalName The name of the signal emitted.
     */
    private void horizontalWrapSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "MouseEntered") {
            const auto data = getCurrentPanelData();
            if (data !is null) {
                const auto scrollablePanel = getParent(widgetName);
                const auto widthOfPanel = getWidgetFullSize(scrollablePanel).x;
                auto columns = widthOfPanel / data.buttonSize.x;
                if (columns < 1.0f) columns = 1.0f;
                const auto rows =
                    ceil(getWidgetCount(widgetName) / floor(columns));
                setWidgetSize(widgetName,
                    "100%-" + getScrollbarWidth(scrollablePanel),
                    formatFloat(rows * data.buttonSize.y)
                );
            }
        }
    }

    /**
     * Gets a handle to an \c ObjectPanel's data.
     * If \c null is returned, an error will be logged.
     * @param  comboBox The name of the \c ComboBox to use as a search criterion.
     * @return Handle to the data, \c null if it could not be retrieved.
     */
    private ObjectPanelSetUpData@ getPanelData(const string&in comboBox) {
        for (uint i = 0, len = initData.objectPanels.length(); i < len; ++i)
            if (initData.objectPanels[i].comboBox == comboBox)
                return initData.objectPanels[i];
        return null;
    }

    /**
     * Refreshes the object buttons when a new owner is selected.
     * @param widgetName The full name of the \c ComboBox.
     * @param signalName The name of the signal emitted.
     */
    private void comboBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ItemSelected") {
            const auto data =
                getPanelData(widgetName.substr(widgetName.findLast(".") + 1));
            if (data !is null) {
                const auto itemID = getSelectedItem(widgetName);
                if (itemID < 0) return;
                string c;
                if (data.neutralAvailable) {
                    if (itemID == 0) {
                        clearWidgetSprite(data.ownerIcon);
                        data.currentlySelectedObject.owner = "";
                    } else {
                        c = country.scriptNames[itemID - 1];
                        setWidgetSprite(data.ownerIcon, "icon",
                            country[c].iconName);
                        data.currentlySelectedObject.owner = c;
                    }
                } else {
                    c = country.scriptNames[itemID];
                    setWidgetSprite(data.ownerIcon, "icon", country[c].iconName);
                    data.currentlySelectedObject.owner = c;
                }
                applySpritesToWidgetsInContainer(data.wrap, data.spritesheet,
                    data.generateSpritesArray(c));
            }
        }
    }
}

/**
 * Defines the \c ObjectDialog.
 */
ObjectDialog PaletteWindow;

/**
 * Index of the Tiles tab.
 */
const int TILE_DIALOG = 0;

/**
 * Index of the Units tab.
 */
const int UNIT_DIALOG = 1;

/**
 * Returns the default setup data for the \c ObjectDialog.
 * @param  parent Parent widget of the \c ObjectDialog.
 * @return Can be passed directly into the \c setUp() method.
 */
ObjectDialogSetUpData@ DefaultObjectDialogData(const string&in parent) {
    ObjectDialogSetUpData SetUpData;
    SetUpData.window = parent + ".ObjectDialog";
    SetUpData.windowText = "objectdialog";

    // Tile group.
    ObjectPanelSetUpData Tiles;
    Tiles.group = "TileDialog";
    Tiles.tabText = "tiledialog";
    @Tiles.currentlySelectedObject = CurrentlySelectedTileType;
    Tiles.buttonSize.x = 35.0f;
    Tiles.buttonSize.y = 35.0f;
    Tiles.neutralAvailable = true;
    Tiles.numberOfOwnerItems = 6;
    @Tiles.mapNameToObject = function(shortName){return tiletype[shortName];};
    Tiles.spritesheet = "tile.normal";
    @Tiles.generateSpritesArray = @generateTileSpriteArray;
    @Tiles.scriptNames = tiletype.scriptNames;
    SetUpData.objectPanels.insertLast(Tiles);

    // Unit group.
    ObjectPanelSetUpData Units;
    Units.group = "UnitDialog";
    Units.tabText = "unitdialog";
    @Units.currentlySelectedObject = CurrentlySelectedUnitType;
    Units.buttonSize.x = 35.0f;
    Units.buttonSize.y = 35.0f;
    Units.neutralAvailable = false;
    Units.numberOfOwnerItems = 5;
    @Units.mapNameToObject = function(shortName){return unittype[shortName];};
    Units.spritesheet = "unit";
    @Units.generateSpritesArray = @generateUnitSpriteArray;
    @Units.scriptNames = unittype.scriptNames;
    SetUpData.objectPanels.insertLast(Units);

    return SetUpData;
}
