/**
 * @file MapProperties.as
 * Defines code that manages the map properties child window.
 */

namespace map_properties_internal {
    /**
     * Stores the name of the map properties \c ChildWindow.
     */
    string WINDOW;

    /**
     * Stores the name of the map name \c EditBox.
     */
    string MAP_NAME;

    /**
     * Stores the name of the day \c EditBox.
     */
    string DAY;

    /**
     * Stores the name of the width \c EditBox.
     */
    string WIDTH;

    /**
     * Stores the name of the height \c EditBox.
     */
    string HEIGHT;

    /**
     * Stores the name of the map resize confirmation \c ChildWindow.
     */
    string CONFIRM_RESIZE;

    /**
     * Stores the name of the map resize confirmation text label.
     */
    string RESIZE_TEXT;

    /**
     * Stores the name of the map resize confirmation current tile text label.
     */
    string RESIZE_TILE_TEXT;

    /**
     * Stores the name of the yes button in the resize confirmation window.
     */
    string RESIZE_YES_BUTTON;
    
    /**
     * Opens the resize confirmation window and updates its labels.
     * @warning Assumes a map is loaded!
     * @param   currentSize The current size of the map
     * @param   newSize     The new size of the map.
     */
    void openResizeConfirmation(const Vector2&in currentSize,
        const Vector2&in newSize) {
        setWidgetEnabled(WINDOW, false);
        if (currentSize.x > newSize.x || currentSize.y > newSize.y)
            setWidgetText(RESIZE_TEXT, "resizemapconfirmationsmaller");
        else
            setWidgetText(RESIZE_TEXT, "resizemapconfirmation");
        if (CurrentlySelectedTileType::Get() is null) {
            setWidgetText(RESIZE_TILE_TEXT, "resizemapconfirmationnotile");
            setWidgetEnabled(RESIZE_YES_BUTTON, false);
        } else {
            setWidgetText(RESIZE_TILE_TEXT, "resizemapconfirmationtile");
            setWidgetEnabled(RESIZE_YES_BUTTON, true);
        }
        openChildWindow(CONFIRM_RESIZE, "50px", "35px");
    }

    /**
     * Closes the resize confirmation window.
     */
    void closeResizeConfirmation() {
        setWidgetEnabled(WINDOW, true);
        closeChildWindow(CONFIRM_RESIZE);
    }
}

/**
 * Sets up the map properties child window.
 * @param parent The parent of the child window.
 */
void MapPropertiesSetUp(const string&in parent = "") {
    map_properties_internal::WINDOW =
        parent + (parent.isEmpty() ? "" : ".") + "SetMapProperties";
    addWidget("ChildWindow", map_properties_internal::WINDOW);
    setWidgetText(map_properties_internal::WINDOW, "mapprops");
    setWidgetSize(map_properties_internal::WINDOW, "250px", "175px");
    setWidgetResizable(map_properties_internal::WINDOW, false);
    setChildWindowTitleButtons(map_properties_internal::WINDOW,
        TitleButton::Close);

    const auto baseLayout = map_properties_internal::WINDOW + ".BaseLayout";
    addWidget("HorizontalLayout", baseLayout);
    setWidgetSize(baseLayout, "100%", "100%-30px");
    setGroupPadding(baseLayout, "5px");

    const auto labelLayout = baseLayout + ".LabelLayout";
    addWidget("VerticalLayout", labelLayout);
    setWidgetRatioInLayout(baseLayout, 0, 0.2f);

    const auto nameLabelGroup = labelLayout + ".MapNameLabelGroup";
    addWidget("Group", nameLabelGroup);

    const auto nameLabel = nameLabelGroup + ".MapNameLabel";
    addWidget("Label", nameLabel);
    setWidgetText(nameLabel, "name");
    setWidgetOrigin(nameLabel, 1.0f, 0.5f);
    setWidgetPosition(nameLabel, "100%", "50%");

    const auto dayLabelGroup = labelLayout + ".DayLabelGroup";
    addWidget("Group", dayLabelGroup);

    const auto dayLabel = dayLabelGroup + ".DayLabel";
    addWidget("Label", dayLabel);
    setWidgetText(dayLabel, "daylabel");
    setWidgetOrigin(dayLabel, 1.0f, 0.5f);
    setWidgetPosition(dayLabel, "100%", "50%");

    const auto sizeLabelGroup = labelLayout + ".SizeLabelGroup";
    addWidget("Group", sizeLabelGroup);

    const auto sizeLabel = sizeLabelGroup + ".SizeLabel";
    addWidget("Label", sizeLabel);
    setWidgetText(sizeLabel, "size");
    setWidgetOrigin(sizeLabel, 1.0f, 0.5f);
    setWidgetPosition(sizeLabel, "100%", "50%");

    const auto editLayout = baseLayout + ".EditLayout";
    addWidget("VerticalLayout", editLayout);

    const auto nameEditGroup = editLayout + ".MapNameEditGroup";
    addWidget("Group", nameEditGroup);
    setGroupPadding(nameEditGroup, "5px", "20%", "5px", "20%");

    map_properties_internal::MAP_NAME = nameEditGroup + ".MapNameEdit";
    addWidget("EditBox", map_properties_internal::MAP_NAME);
    setWidgetSize(map_properties_internal::MAP_NAME, "100%", "100%");

    const auto dayEditGroup = editLayout + ".DayEditGroup";
    addWidget("Group", dayEditGroup);
    setGroupPadding(dayEditGroup, "5px", "20%", "5px", "20%");

    map_properties_internal::DAY = dayEditGroup + ".DayEdit";
    addWidget("EditBox", map_properties_internal::DAY);
    setWidgetSize(map_properties_internal::DAY, "100%", "100%");
    onlyAcceptUIntsInEditBox(map_properties_internal::DAY);

    const auto sizeLayout = editLayout + ".SizeLayout";
    addWidget("HorizontalLayout", sizeLayout);

    const auto widthEditGroup = sizeLayout + ".WidthEditGroup";
    addWidget("Group", widthEditGroup);
    setGroupPadding(widthEditGroup, "5px", "20%", "5px", "20%");

    map_properties_internal::WIDTH = widthEditGroup + ".WidthEdit";
    addWidget("EditBox", map_properties_internal::WIDTH);
    setWidgetSize(map_properties_internal::WIDTH, "100%", "100%");
    onlyAcceptUIntsInEditBox(map_properties_internal::WIDTH);

    const auto xLabelGroup = sizeLayout + ".XGroup";
    addWidget("Group", xLabelGroup);
    setWidgetRatioInLayout(sizeLayout, 1, 0.2f);

    const auto xLabel = xLabelGroup + ".X";
    addWidget("Label", xLabel);
    setWidgetText(xLabel, "~x");
    setWidgetOrigin(xLabel, 0.5f, 0.5f);
    setWidgetPosition(xLabel, "50%", "50%");

    const auto heightEditGroup = sizeLayout + ".HeightEditGroup";
    addWidget("Group", heightEditGroup);
    setGroupPadding(heightEditGroup, "5px", "20%", "5px", "20%");

    map_properties_internal::HEIGHT = heightEditGroup + ".HeightEdit";
    addWidget("EditBox", map_properties_internal::HEIGHT);
    setWidgetSize(map_properties_internal::HEIGHT, "100%", "100%");
    onlyAcceptUIntsInEditBox(map_properties_internal::HEIGHT);

    awe::addButtonsToParent(map_properties_internal::WINDOW, {
        awe::ParentButton("MapPropertiesOK", "ok"),
        awe::ParentButton("MapPropertiesCancel", "cancel")
    }, 50, 25);

    map_properties_internal::CONFIRM_RESIZE =
        parent + (parent.isEmpty() ? "" : ".") + "MapPropertiesConfirmResize";
    addWidget("ChildWindow", map_properties_internal::CONFIRM_RESIZE);
    setWidgetText(map_properties_internal::CONFIRM_RESIZE, "alert");
    setWidgetSize(map_properties_internal::CONFIRM_RESIZE, "250px", "175px");
    setWidgetResizable(map_properties_internal::CONFIRM_RESIZE, false);
    setChildWindowTitleButtons(map_properties_internal::CONFIRM_RESIZE,
        TitleButton::None);
    
    const auto resizeBaseLayout = map_properties_internal::CONFIRM_RESIZE +
        ".BaseLayout";
    addWidget("VerticalLayout", resizeBaseLayout);
    setWidgetSize(resizeBaseLayout, "100%", "100%-30px");
    setGroupPadding(resizeBaseLayout, "5px");

    const auto resizeTextGroup = resizeBaseLayout + ".TextGroup";
    addWidget("Group", resizeTextGroup);

    map_properties_internal::RESIZE_TEXT = resizeTextGroup + ".Text";
    addWidget("Label", map_properties_internal::RESIZE_TEXT);
    setWidgetSize(map_properties_internal::RESIZE_TEXT, "100%", "100%");
    setWidgetTextMaximumWidth(map_properties_internal::RESIZE_TEXT,
        getWidgetFullSize(map_properties_internal::RESIZE_TEXT).x);

    const auto resizeTileLayout = resizeBaseLayout + ".TileLayout";
    addWidget("VerticalLayout", resizeTileLayout);

    const auto resizeTileTextGroup = resizeTileLayout + ".TextGroup";
    addWidget("Group", resizeTileTextGroup);

    map_properties_internal::RESIZE_TILE_TEXT = resizeTileTextGroup + ".Text";
    addWidget("Label", map_properties_internal::RESIZE_TILE_TEXT);

    const auto resizeTileTileGroup = resizeTileLayout + ".TileGroup";
    addWidget("Group", resizeTileTileGroup);

    CurrentlySelectedTileType::AddWidget(resizeTileTileGroup, function(){
        /// Make sure to enable the Yes button when a tile is selected.
        setWidgetEnabled(map_properties_internal::RESIZE_YES_BUTTON, true);
    });

    awe::addButtonsToParent(map_properties_internal::CONFIRM_RESIZE, {
        awe::ParentButton("ConfirmResizeYes", "yes"),
        awe::ParentButton("ConfirmResizeNo", "no")
    }, 50, 25);
    map_properties_internal::RESIZE_YES_BUTTON =
        map_properties_internal::CONFIRM_RESIZE + ".ConfirmResizeYes";

    CloseMapProperties();
}

/**
 * Opens the map properties dialog window.
 * If the map properties windows are already open, they will be repositioned to
 * the UL corner of the screen.
 */
void OpenMapProperties() {
    if (isChildWindowOpen(map_properties_internal::WINDOW)) {
        setWidgetPosition(map_properties_internal::WINDOW, "50px", "35px");
        setWidgetPosition(map_properties_internal::CONFIRM_RESIZE, "50px",
            "35px");
        return;
    }
    if (editmap is null) {
        awe::OpenMessageBox(SIMPLE_MESSAGE_BOX, "alert", "nomapisopen", null,
            BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
    } else {
        setWidgetText(map_properties_internal::MAP_NAME, editmap.getMapName());
        setWidgetText(map_properties_internal::DAY, formatDay(editmap.getDay()));
        const auto size = editmap.getMapSize();
        setWidgetText(map_properties_internal::WIDTH, formatUInt(size.x));
        setWidgetText(map_properties_internal::HEIGHT, formatUInt(size.y));
        openChildWindow(map_properties_internal::WINDOW, "50px", "35px");
    }
}

/**
 * Closes the map properties dialog window and its associated windows.
 */
void CloseMapProperties() {
    closeChildWindow(map_properties_internal::WINDOW);
    map_properties_internal::closeResizeConfirmation();
}

/**
 * Gets the width and height currently in the \c EditBoxes.
 * @return The size.
 */
Vector2 GetSizeInEditBoxes() {
    return Vector2(
        parseUInt(getWidgetText(map_properties_internal::WIDTH)),
        parseUInt(getWidgetText(map_properties_internal::HEIGHT))
    );
}

/**
 * Applies changes and closes the map properties windows.
 * @param resize \c TRUE if the map should be resized, \c FALSE if not.
 */
void ApplyChangesAndClose(const bool resize) {
    editmap.setMapName(getWidgetText(map_properties_internal::MAP_NAME));
    editmap.setDay(parseDay(getWidgetText(map_properties_internal::DAY)));
    if (resize) {
        const auto owner = CurrentlySelectedTileType::GetOwner();
        if (owner.isEmpty()) {
            editmap.setMapSize(GetSizeInEditBoxes(),
                CurrentlySelectedTileType::Get().scriptName);
        } else {
            editmap.setMapSize(GetSizeInEditBoxes(),
                CurrentlySelectedTileType::Get().scriptName,
                country[owner].turnOrder);
        }
    }
    CloseMapProperties();
}

/**
 * Closes the map properties dialog and applies changes.
 * If the map size is changing, open up the confirmation window instead.
 */
void MapMakerMenu_MapPropertiesOK_Pressed() {
    if (editmap is null) {
        error("MapPropertiesOK button was pressed when editmap was null!");
        return;
    }
    const auto currentSize = editmap.getMapSize(), newSize = GetSizeInEditBoxes();
    if (newSize.x == 0 && newSize.y == 0) {
        awe::OpenMessageBox(SIMPLE_MESSAGE_BOX, "alert", "cannotresizeto0", null,
            BASE_GROUP, MESSAGE_BOX_GROUP);
        addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
    } else if (currentSize != newSize)
        map_properties_internal::openResizeConfirmation(currentSize, newSize);
    else // A tile type may not have been selected yet so don't try to resize.
        ApplyChangesAndClose(false);
}

/**
 * Closes the map properties dialog without applying any changes.
 */
void MapMakerMenu_MapPropertiesCancel_Pressed() {
    CloseMapProperties();
}

/**
 * Closes the resize map confirmation window and applies changes.
 */
void MapMakerMenu_ConfirmResizeYes_Pressed() {
    ApplyChangesAndClose(true);
}

/**
 * Closes the resize map confirmation window without applying any changes.
 */
void MapMakerMenu_ConfirmResizeNo_Pressed() {
    map_properties_internal::closeResizeConfirmation();
}
