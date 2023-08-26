/**
 * @file MapProperties.as
 * Defines code that manages the map properties child window.
 */

/**
 * Represents the Map Properties window.
 */
class MapPropertiesWindow {
    /**
     * Sets up \c MessageBox related data that can't be changed after
     * construction.
     * @param messageBoxName \c name parameter passed into \c OpenMessageBox()
     *                       when a \c MessageBox is required to be displayed.
     *                       Intended to be \c SIMPLE_MESSAGE_BOX.
     * @param disableThis    \c disableThis parameter pass into
     *                       \c OpenMessageBox() when a \c MessageBox is required
     *                       to be displayed. Intended to be \c BASE_GROUP.
     * @param enableThis     \c enableThis parameter pass into \c OpenMessageBox()
     *                       when a \c MessageBox is required to be displayed.
     *                       Intended to be \c MESSAGE_BOX_GROUP.
     */
    MapPropertiesWindow(const string&in messageBoxName,
        const string&in disableThis, const string&in enableThis) {
        mbName = messageBoxName;
        mbDisableThis = disableThis;
        mbEnableThis = enableThis;
    }

    /**
     * Sets up the map properties child window.
     * @param parent The parent of the child window.
     */
    void setUp(const string&in parent) {
        window = parent + ".SetMapProperties";
        addWidget("ChildWindow", window);
        setWidgetText(window, "mapprops");
        setWidgetSize(window, "250px", "175px");
        setWidgetResizable(window, false);
        setChildWindowTitleButtons(window, TitleButton::Close);

        const auto baseLayout = window + ".BaseLayout";
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

        mapName = nameEditGroup + ".MapNameEdit";
        addWidget("EditBox", mapName);
        setWidgetSize(mapName, "100%", "100%");

        const auto dayEditGroup = editLayout + ".DayEditGroup";
        addWidget("Group", dayEditGroup);
        setGroupPadding(dayEditGroup, "5px", "20%", "5px", "20%");

        day = dayEditGroup + ".DayEdit";
        addWidget("EditBox", day);
        setWidgetSize(day, "100%", "100%");
        onlyAcceptUIntsInEditBox(day);

        const auto sizeLayout = editLayout + ".SizeLayout";
        addWidget("HorizontalLayout", sizeLayout);

        const auto widthEditGroup = sizeLayout + ".WidthEditGroup";
        addWidget("Group", widthEditGroup);
        setGroupPadding(widthEditGroup, "5px", "20%", "5px", "20%");

        width = widthEditGroup + ".WidthEdit";
        addWidget("EditBox", width);
        setWidgetSize(width, "100%", "100%");
        onlyAcceptUIntsInEditBox(width);

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

        height = heightEditGroup + ".HeightEdit";
        addWidget("EditBox", height);
        setWidgetSize(height, "100%", "100%");
        onlyAcceptUIntsInEditBox(height);

        awe::addButtonsToParent(window, {
            awe::ParentButton("MapPropertiesOK", "ok",
                SignalHandler(this.okButtonSignalHandler)),
            awe::ParentButton("MapPropertiesCancel", "cancel",
                SignalHandler(this.cancelButtonSignalHandler))
        }, 50, 25);

        confirmResize.setUp(parent + ".ConfirmTileTypeForResizeWindow",
            "resizemapconfirmationnotile", "resizemapconfirmationtile",
            SignalHandler(this.yesButtonSignalHandler),
            SignalHandler(this.noButtonSignalHandler));

        close();
    }

    /// Ensures any windows are open and visible.
    void restore() {
        if (isChildWindowOpen(window)) {
            setWidgetPosition(window, "50px", "35px");
            setWidgetPosition(confirmResize.windowName, "50px", "35px");
        } else {
            openChildWindow(window, "50px", "35px");
            refresh();
        }
    }

    /// If the window is open, refresh it with up-to-date data.
    void refresh() {
        if (isChildWindowOpen(window)) {
            setWidgetText(mapName, edit.map.getMapName());
            setWidgetText(day, formatDay(edit.map.getDay()));
            const auto size = edit.map.getMapSize();
            setWidgetText(width, formatUInt(size.x));
            setWidgetText(height, formatUInt(size.y));
        }
    }

    /// Closes both the MapProperties window and the resize confirmation window.
    void close() {
        setWidgetEnabled(window, true);
        closeChildWindow(window);
        confirmResize.close();
    }

    /**
     * Gets the width and height currently in the \c EditBoxes.
     * @return The size.
     */
    private Vector2 _getSizeInEditBoxes() {
        return Vector2(parseUInt(getWidgetText(width)),
            parseUInt(getWidgetText(height)));
    }

    /**
     * Applies changes and closes the MapProperties windows.
     * @param resize \c TRUE if the map should be resized, \c FALSE if not.
     */
    private void _applyChangesAndClose(const bool resize) {
        edit.setMapProperties(getWidgetText(mapName),
            parseDay(getWidgetText(day)));
        if (resize) {
            if (CurrentlySelectedTileType.owner.isEmpty()) {
                edit.setMapSize(_getSizeInEditBoxes(),
                    cast<TileType>(CurrentlySelectedTileType.object).scriptName);
            } else {
                edit.setMapSize(_getSizeInEditBoxes(),
                    cast<TileType>(CurrentlySelectedTileType.object).scriptName,
                    country[CurrentlySelectedTileType.owner].turnOrder);
            }
        }
        close();
    }

    /**
     * Handles the OK \c Button widget's signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void okButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            if (edit is null) {
                error("MapPropertiesOK button was pressed when @edit was null!");
                return;
            }
            const auto currentSize = edit.map.getMapSize(),
                newSize = _getSizeInEditBoxes();
            if (newSize.x == 0 || newSize.y == 0) {
                awe::OpenMessageBox(mbName, "alert", "cannotresizeto0", null,
                    mbDisableThis, mbEnableThis);
                addMessageBoxButton(SIMPLE_MESSAGE_BOX, "ok");
            } else if (currentSize != newSize) {
                setWidgetEnabled(window, false);
                confirmResize.restore(
                    (currentSize.x > newSize.x || currentSize.y > newSize.y) ?
                        "resizemapconfirmationsmaller" : "resizemapconfirmation"
                );
            // A tile type may not have been selected yet so don't try to resize:
            } else _applyChangesAndClose(false);
        }
    }

    /**
     * Handles the cancel \c Button widget's signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void cancelButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") close();
    }

    /**
     * Handles the yes \c Button widget's signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void yesButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") _applyChangesAndClose(true);
    }

    /**
     * Handles the no \c Button widget's signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void noButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            setWidgetEnabled(window, true);
            confirmResize.close();
        }
    }

    /// Name of the \c MessageBox to create.
    private string mbName;

    /// Name of the widget to disable when opening a \c MessageBox.
    private string mbDisableThis;

    /// Name of the widget to enable when opening a \c MessageBox.
    private string mbEnableThis;

    /// Window used to allow the user to confirm a resize operation.
    private ConfirmTileTypeWindow confirmResize;

    /// The name of the \c ChildWindow.
    private string window;

    /// Name of the map name's \c EditBox.
    private string mapName;

    /// Name of the day's \c EditBox.
    private string day;

    /// Name of the width \c EditBox.
    private string width;

    /// Name of the height \c EditBox.
    private string height;
}
