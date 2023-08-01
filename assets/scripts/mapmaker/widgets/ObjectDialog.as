/**
 * @file ObjectDialog.as
 * Defines code that manages \c ObjectDialog windows.
 */

/**
 * Holds data \c ObjectDialog uses to set itself up with.
 */
class ObjectDialogSetUpData {
    /**
     * Signature of the function that is invoked when an sprite array has to be
     * generated for a list of object buttons.
     * @param  const string&in Owner script name.
     * @return Array of sprite keys.
     */
    funcdef array<string>@ GetSpritesFunc(const string&in);

    /// Name of the \c ChildWindow that represents the \c ObjectDialog.
    string window;

    /// The text/title to assign to the \c ChildWindow.
    string windowText;

    /// The size of each object button.
    Vector2f buttonSize;

    /// Name of the signal handler to assign to each object button.
    string buttonSignalHandler;

    /// Reference to the \c CurrentlySelectedObject widget to add.
    CurrentlySelectedObject@ csoWidget;

    /// \c TRUE if the \c ObjectDialog should dock to the right side of the
    /// screen, \c FALSE if it should dock to the left.
    bool dockOnRight;

    /// \c TRUE if a Neutral option should be prepended to the owner \c ComboBox.
    /// \c FALSE if the first army in the turn order should be selected in
    /// \c setUp().
    bool neutralAvailable;

    /// Spritesheet to use with the object buttons.
    string spritesheet;

    /// Pointer to a function which is called when object sprites have to be
    /// refreshed.
    /// Can be one of \c @generateTileSpriteArray or \c @generateUnitSpriteArray.
    GetSpritesFunc@ generateSpritesArray;

    /// A list of script names to generate object buttons for.
    const array<string>@ scriptNames;

    /// Defines the number of columns that should show in the Dialog when
    /// \c dock() is called.
    float defaultColumnCount;

    /// The number of items to show in the owner \c ComboBox at one time.
    uint64 numberOfOwnerItems;
}

/**
 * Represents a dialog box that contains a \c CurrentlySelectedObject widget, a
 * \c HorizontalWrap of \c BitmapButtons, each representing an object from a given
 * pool, and an owner combobox that allows the user to change the owner associated
 * with the currently selected object.
 */
class ObjectDialog {
    /**
     * The height of the owner group and \c CurrentlySelectedObject group.
     */
    string GROUP_HEIGHT = "35px";
    
    /**
     * The padding of the owner group and \c CurrentlySelectedObject group.
     */
    string GROUP_PADDING = "5px";

    /**
     * Cache of the \c ObjectDialogSetUpData given at \c setUp().
     */
    private const ObjectDialogSetUpData@ initData;

    /**
     * The name of the \c ScrollablePanel containing the \c HorizontalWrap.
     */
    private string ScrollablePanel;

    /**
     * The name of the \c HorizontalWrap containing the object buttons.
     */
    private string HorizontalWrap;

    /**
     * The name of the \c Picture containing the owner icon.
     */
    private string OwnerIconPicture;

    /**
     * The name of the \c ComboBox containing the owners.
     */
    private string OwnerComboBox;
    
    /**
     * Sets up the \c ObjectDialog window.
     * DOCUMENT SIGNALS TO IMPLEMENT AND THEIR WIDGET NAMES!
     * @param data Data to initialise the \c ObjectDialog with.
     */
    void setUp(const ObjectDialogSetUpData@ data) {
        @initData = data;

        // Child window.
        const auto shortName = data.window.substr(data.window.findLast(".") + 1);
        addWidget("ChildWindow", data.window);
        setWidgetText(data.window, data.windowText);

        // CurrentlySelectObject group.
        const auto csoGroup = data.window + ".CurrentObjectGroup";
        addWidget("Group", csoGroup);
        setWidgetSize(csoGroup, "100%", GROUP_HEIGHT);
        setWidgetOrigin(csoGroup, 0.5f, 0.0f);
        setWidgetPosition(csoGroup, "50%", "0%");
        data.csoWidget.addWidget(csoGroup + ".CurSelObjectLayout");

        // Scrollable panel.
        ScrollablePanel = data.window + ".ScrollablePanel";
        addWidget("ScrollablePanel", ScrollablePanel);
        setWidgetSize(ScrollablePanel, "100%", "100%-" + GROUP_HEIGHT + "-" +
            GROUP_HEIGHT);
        setWidgetPosition(ScrollablePanel, "50%", "50%");
        setWidgetOrigin(ScrollablePanel, 0.5f, 0.5f);
        setVerticalScrollbarAmount(ScrollablePanel, 25);
        setHorizontalScrollbarPolicy(ScrollablePanel, ScrollbarPolicy::Never);

        // Object buttons.
        HorizontalWrap = ScrollablePanel + "." + shortName + "Wrap";
        addWidget("HorizontalWrap", HorizontalWrap);
        for (uint t = 0, len = data.scriptNames.length(); t < len; ++t) {
            const auto objName = data.scriptNames[t];
            const auto btn = HorizontalWrap + "." + objName;
            addWidget("BitmapButton", btn, data.buttonSignalHandler);
            // TODO-4: We should really find the tallest sprite and base the
            // button sizes on that.
            setWidgetSize(btn, formatFloat(data.buttonSize.x),
                formatFloat(data.buttonSize.y));
        }
        
        // SelectOwner widgets //
        const auto selectOwnerGroup = data.window + ".SelectOwnerGroup";
        addWidget("Group", selectOwnerGroup);
        setWidgetSize(selectOwnerGroup, "100%", GROUP_HEIGHT);
        setWidgetOrigin(selectOwnerGroup, 0.5f, 1.0f);
        setWidgetPosition(selectOwnerGroup, "50%", "100%");

        const auto ownerIconGroup = selectOwnerGroup + ".OwnerIconGroup";
        addWidget("Group", ownerIconGroup);
        setWidgetSize(ownerIconGroup, GROUP_HEIGHT, "100%");
        setGroupPadding(ownerIconGroup, "5px");

        OwnerIconPicture = ownerIconGroup + ".OwnerIcon";
        addWidget("Picture", OwnerIconPicture);
        setWidgetOrigin(OwnerIconPicture, 0.5f, 0.5f);
        setWidgetPosition(OwnerIconPicture, "50%", "50%");

        const auto ownerComboboxGroup = selectOwnerGroup + ".OwnerComboboxGroup";
        addWidget("Group", ownerComboboxGroup);
        setWidgetSize(ownerComboboxGroup, "100%-" + GROUP_HEIGHT, "100%");
        setWidgetPosition(ownerComboboxGroup, GROUP_HEIGHT, "0px");
        setGroupPadding(ownerComboboxGroup, "0px", "5px", "5px", "5px");

        OwnerComboBox = ownerComboboxGroup + "." + shortName + "OwnerCombobox";
        addWidget("ComboBox", OwnerComboBox);
        setWidgetSize(OwnerComboBox, "100%", "100%");
        if (data.neutralAvailable) addItem(OwnerComboBox, "neutral");
        const auto@ countryScriptNames = country.scriptNames;
        for (uint c = 0, len = countryScriptNames.length(); c < len; ++c)
            addItem(OwnerComboBox, country[countryScriptNames[c]].name);
        setSelectedItem(OwnerComboBox, 0);
        setItemsToDisplay(OwnerComboBox, data.numberOfOwnerItems);
        ownerSelected();

        dock();
        resize();
    }
    
    /**
     * Opens and docks a \c ObjectDialog to one side of the screen.
     */
    void dock() {
        const auto defaultWidth = formatFloat(
            initData.buttonSize.x * initData.defaultColumnCount +
            getScrollbarWidth(ScrollablePanel) + 2.0f
        );
        if (initData.dockOnRight) {
            openChildWindow(initData.window, "100%-" + defaultWidth, "0");
            setWidgetSize(initData.window, defaultWidth, "100%");
        } else {
            openChildWindow(initData.window, "0", "0");
            setWidgetSize(initData.window, defaultWidth, "100%");
        }
    }

    /**
     * Figures out how many rows of object buttons there are in the
     * \c HorizontalWrap and resizes the wrap to fit.
     */
    void resize() {
        const auto widthOfPanel = getWidgetFullSize(ScrollablePanel).x;
        auto columns = widthOfPanel / initData.buttonSize.x;
        if (columns < 1.0f) columns = 1.0f;
        const auto rows = ceil(getWidgetCount(HorizontalWrap) / floor(columns));
        setWidgetSize(HorizontalWrap,
            "100%-" + getScrollbarWidth(ScrollablePanel),
            formatFloat(rows * initData.buttonSize.y)
        );
    }

    /**
     * Handles the owner \c ComboBox's \c ItemSelected event.
     */
    void ownerSelected() {
        const auto itemID = getSelectedItem(OwnerComboBox);
        if (itemID < 0) return;
        string c;
        if (initData.neutralAvailable) {
            if (itemID == 0) {
                clearWidgetSprite(OwnerIconPicture);
                initData.csoWidget.owner = "";
            } else {
                c = country.scriptNames[itemID - 1];
                setWidgetSprite(OwnerIconPicture, "icon", country[c].iconName);
                initData.csoWidget.owner = c;
            }
        } else {
            c = country.scriptNames[itemID];
            setWidgetSprite(OwnerIconPicture, "icon", country[c].iconName);
            initData.csoWidget.owner = c;
        }
        applySpritesToWidgetsInContainer(HorizontalWrap, initData.spritesheet,
            initData.generateSpritesArray(c));
    }
}

/**
 * Defines the \c TileDialog.
 */
ObjectDialog TileDialog;

/**
 * Returns the default setup data for the \c TileDialog.
 * @param  parent Parent widget of the \c TileDialog.
 * @return Can be passed directly into the \c setUp() method.
 */
ObjectDialogSetUpData@ DefaultTileDialogData(const string&in parent) {
    ObjectDialogSetUpData TileDialogData;
    TileDialogData.window = parent + ".TileDialog";
    TileDialogData.windowText = "tiledialog";
    TileDialogData.buttonSize.x = 35.0f;
    TileDialogData.buttonSize.y = 35.0f;
    TileDialogData.buttonSignalHandler = "TileDialogHandleObjectButtonSignal";
    @TileDialogData.csoWidget = CurrentlySelectedTileType;
    TileDialogData.dockOnRight = true;
    TileDialogData.neutralAvailable = true;
    TileDialogData.spritesheet = "tile.normal";
    @TileDialogData.generateSpritesArray = @generateTileSpriteArray;
    @TileDialogData.scriptNames = tiletype.scriptNames;
    TileDialogData.defaultColumnCount = 6.0f;
    TileDialogData.numberOfOwnerItems = 6;
    return TileDialogData;
}

/**
 * Trigger a \c resize() call when the mouse enters a \c TileDialog's wrap.
 */
void MapMakerMenu_TileDialogWrap_MouseEntered() {
    TileDialog.resize();
}

/**
 * Trigger an \c ownerSelected() call when the user selects an owner.
 */
void MapMakerMenu_TileDialogOwnerCombobox_ItemSelected() {
    TileDialog.ownerSelected();
}

/**
 * Signal handler for tile type object buttons.
 * @param widgetName Full name of the button.
 * @param signal     The signal that was emitted.
 */
void TileDialogHandleObjectButtonSignal(const string&in widgetName,
    const string&in signal) {
    if (signal == "Pressed") {
        CurrentlySelectedTileType.object =
            tiletype[widgetName.substr(widgetName.findLast(".") + 1)];
    }
}

/**
 * Defines the \c UnitDialog.
 */
ObjectDialog UnitDialog;

/**
 * Returns the default setup data for the \c UnitDialog.
 * @param  parent Parent widget of the \c UnitDialog.
 * @return Can be passed directly into the \c setUp() method.
 */
ObjectDialogSetUpData@ DefaultUnitDialogData(const string&in parent) {
    ObjectDialogSetUpData UnitDialogData;
    UnitDialogData.window = parent + ".UnitDialog";
    UnitDialogData.windowText = "unitdialog";
    UnitDialogData.buttonSize.x = 35.0f;
    UnitDialogData.buttonSize.y = 35.0f;
    UnitDialogData.buttonSignalHandler = "UnitDialogHandleObjectButtonSignal";
    @UnitDialogData.csoWidget = CurrentlySelectedUnitType;
    UnitDialogData.dockOnRight = false;
    UnitDialogData.neutralAvailable = false;
    UnitDialogData.spritesheet = "unit";
    @UnitDialogData.generateSpritesArray = @generateUnitSpriteArray;
    @UnitDialogData.scriptNames = unittype.scriptNames;
    UnitDialogData.defaultColumnCount = 3.0f;
    UnitDialogData.numberOfOwnerItems = 5;
    return UnitDialogData;
}

/**
 * Trigger a \c resize() call when the mouse enters a \c UnitDialog's wrap.
 */
void MapMakerMenu_UnitDialogWrap_MouseEntered() {
    UnitDialog.resize();
}

/**
 * Trigger an \c ownerSelected() call when the user selects an owner.
 */
void MapMakerMenu_UnitDialogOwnerCombobox_ItemSelected() {
    UnitDialog.ownerSelected();
}

/**
 * Signal handler for unit type object buttons.
 * @param widgetName Full name of the button.
 * @param signal     The signal that was emitted.
 */
void UnitDialogHandleObjectButtonSignal(const string&in widgetName,
    const string&in signal) {
    if (signal == "Pressed") {
        CurrentlySelectedUnitType.object =
            unittype[widgetName.substr(widgetName.findLast(".") + 1)];
    }
}
