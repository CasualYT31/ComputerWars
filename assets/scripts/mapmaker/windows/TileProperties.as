/**
 * @file TileProperties.as
 * Defines the GUI used to observe and amend tiles and their units.
 */

/**
 * Represents a window that displays editable tile properties.
 */
class TilePropertiesWindow {
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
    TilePropertiesWindow(const string&in messageBoxName,
        const string&in disableThis, const string&in enableThis) {
        mbName = messageBoxName;
        mbDisableThis = disableThis;
        mbEnableThis = enableThis;
    }

    /**
     * Sets up the window.
     * @param parent The parent of the child window.
     */
    void setUp(const string&in parent) {
        // Child window.
        window = parent + ".TileProperties";
        addWidget(ChildWindow, window);
        setWidgetText(window, "tileprops");

        // Scrollable panel.
        const auto panel = window + ".ScrollablePanel";
        addWidget(ScrollablePanel, panel);
        setGroupPadding(panel, "5px");
        setVerticalScrollbarAmount(panel, 80);
        scrollBarWidth = formatFloat(getScrollbarWidth(panel));

        // Message displayed when no tile is selected.
        const auto errorMessageGroup = panel + ".ErrorMessageGroup";
        addWidget(Group, errorMessageGroup);
        setWidgetSize(errorMessageGroup, "100%-" + scrollBarWidth, "100%");

        errorMessageLabel = errorMessageGroup + ".ErrorMessage";
        addWidget(Label, errorMessageLabel);
        setWidgetOrigin(errorMessageLabel, 0.5f, 0.0f);
        setWidgetPosition(errorMessageLabel, "50%", "0px");
        setWidgetSize(errorMessageLabel, "100%", "100px");
        setWidgetText(errorMessageLabel, "pleaseselecttile");
        setWidgetTextAlignment(errorMessageLabel, HorizontalAlignment::Centre,
            VerticalAlignment::Top);

        // Vertical layout.
        layout = panel + ".VerticalLayout";
        addWidget(VerticalLayout, layout);
        setSpaceBetweenWidgets(layout, 20.f);
        
        // Tiles vertical layout.
        const auto tilesLayout = layout + ".TilesLayout";
        addWidget(VerticalLayout, tilesLayout);

        // Tile picture.
        const auto tilePictureGroup = tilesLayout + ".TilePictureGroup";
        addWidget(Group, tilePictureGroup);
        setWidgetRatioInLayout(tilesLayout, 0, 2.5f);

        tilePicture = tilePictureGroup + ".TilePicture";
        addWidget(Picture, tilePicture);
        setWidgetOrigin(tilePicture, 0.5f, 0.0f);
        setWidgetPosition(tilePicture, "50%", "0%");

        // Tile type long name.
        const auto tileTypeLongNameGroup = tilesLayout + ".TileTypeLongNameGroup";
        addWidget(Group, tileTypeLongNameGroup);
        setWidgetRatioInLayout(tilesLayout, 1, 0.4f);

        tileTypeLabel = tileTypeLongNameGroup + ".TileTypeLongName";
        addWidget(Label, tileTypeLabel);
        setWidgetOrigin(tileTypeLabel, 0.5f, 0.0f);
        setWidgetPosition(tileTypeLabel, "50%", "0%");
        setWidgetTextSize(tileTypeLabel, 12);
        setWidgetTextColour(tileTypeLabel, Colour(125, 125, 125, 255));
        setWidgetTextStyles(tileTypeLabel, "Italic");

        // Terrain long name.
        const auto terrainLongNameGroup = tilesLayout + ".TerrainLongNameGroup";
        addWidget(Group, terrainLongNameGroup);

        terrainTypeLabel = terrainLongNameGroup + ".TerrainLongName";
        addWidget(Label, terrainTypeLabel);
        setWidgetOrigin(terrainTypeLabel, 0.5f, 0.0f);
        setWidgetPosition(terrainTypeLabel, "50%", "0%");
        setWidgetTextSize(terrainTypeLabel, 24);
        setWidgetTextStyles(terrainTypeLabel, "Bold");

        // Location.
        const auto locationLayout = tilesLayout + ".LocationLayout";
        addWidget(HorizontalLayout, locationLayout);

        const auto xLabelGroup = locationLayout + ".XLabelGroup";
        addWidget(Group, xLabelGroup);

        const auto xLabel = xLabelGroup + ".XLabel";
        addWidget(Label, xLabel);
        setWidgetText(xLabel, "~X");
        setWidgetOrigin(xLabel, 1.0f, 0.5f);
        setWidgetPosition(xLabel, "100%", "50%");

        const auto xGroup = locationLayout + ".XGroup";
        addWidget(Group, xGroup);

        tileXLabel = xGroup + ".X";
        addWidget(Label, tileXLabel);
        setWidgetOrigin(tileXLabel, 0.0f, 0.5f);
        setWidgetPosition(tileXLabel, "0%", "50%");

        const auto yLabelGroup = locationLayout + ".YLabelGroup";
        addWidget(Group, yLabelGroup);

        const auto yLabel = yLabelGroup + ".YLabel";
        addWidget(Label, yLabel);
        setWidgetText(yLabel, "~Y");
        setWidgetOrigin(yLabel, 1.0f, 0.5f);
        setWidgetPosition(yLabel, "100%", "50%");

        const auto yGroup = locationLayout + ".YGroup";
        addWidget(Group, yGroup);

        tileYLabel = yGroup + ".Y";
        addWidget(Label, tileYLabel);
        setWidgetOrigin(tileYLabel, 0.0f, 0.5f);
        setWidgetPosition(tileYLabel, "0%", "50%");

        // Defence.
        const auto defenceLayout = tilesLayout + ".DefenceLayout";
        addWidget(HorizontalLayout, defenceLayout);

        const auto defenceLabelGroup = defenceLayout + ".DefenceLabelGroup";
        addWidget(Group, defenceLabelGroup);

        const auto defenceLabel = defenceLabelGroup + ".DefenceLabel";
        addWidget(Label, defenceLabel);
        setWidgetText(defenceLabel, "def");
        setWidgetOrigin(defenceLabel, 1.0f, 0.5f);
        setWidgetPosition(defenceLabel, "100%", "50%");

        const auto defenceRatingGroup = defenceLayout + ".DefenceRatingGroup";
        addWidget(Group, defenceRatingGroup);

        tileDefenceLabel = defenceRatingGroup + ".DefenceRating";
        addWidget(Label, tileDefenceLabel);
        setWidgetOrigin(tileDefenceLabel, 0.0f, 0.5f);
        setWidgetPosition(tileDefenceLabel, "0%", "50%");

        // HP.
        const auto hpLayout = tilesLayout + ".HPLayout";
        addWidget(HorizontalLayout, hpLayout);

        const auto hpLabelGroup = hpLayout + ".HPLableGroup";
        addWidget(Group, hpLabelGroup);

        const auto hpLabel = hpLabelGroup + ".HPLabel";
        addWidget(Label, hpLabel);
        setWidgetText(hpLabel, "hp");
        setWidgetOrigin(hpLabel, 1.0f, 0.5f);
        setWidgetPosition(hpLabel, "100%", "50%");

        const auto hpGroup = hpLayout + ".HPGroup";
        addWidget(Group, hpGroup);

        tileHPEditBox = hpGroup + ".HP";
        addWidget(EditBox, tileHPEditBox);
        setWidgetSize(tileHPEditBox, "100%", "30px");
        setWidgetOrigin(tileHPEditBox, 0.5f, 0.5f);
        setWidgetPosition(tileHPEditBox, "50%", "50%");
        onlyAcceptUIntsInEditBox(tileHPEditBox);

        const auto slashLabelGroup = hpLayout + ".SlashLabelGroup";
        addWidget(Group, slashLabelGroup);

        const auto slashLabel = slashLabelGroup + ".SlashLabel";
        addWidget(Label, slashLabel);
        setWidgetText(slashLabel, "~/");
        setWidgetOrigin(slashLabel, 0.5f, 0.5f);
        setWidgetPosition(slashLabel, "50%", "50%");

        const auto maxHpGroup = hpLayout + ".MaxHPGroup";
        addWidget(Group, maxHpGroup);

        tileMaxHPLabel = maxHpGroup + ".MaxHP";
        addWidget(Label, tileMaxHPLabel);
        setWidgetOrigin(tileMaxHPLabel, 0.0f, 0.5f);
        setWidgetPosition(tileMaxHPLabel, "0%", "50%");

        // Owner.
        const auto ownerLayout = tilesLayout + ".OwnerLayout";
        addWidget(HorizontalLayout, ownerLayout);

        const auto ownerLabelGroup = ownerLayout + ".OwnerLabelGroup";
        addWidget(Group, ownerLabelGroup);

        const auto ownerLabel = ownerLabelGroup + ".OwnerLabel";
        addWidget(Label, ownerLabel);
        setWidgetText(ownerLabel, "owner");
        setWidgetOrigin(ownerLabel, 1.0f, 0.5f);
        setWidgetPosition(ownerLabel, "100%", "50%");

        const auto ownerListGroup = ownerLayout + ".OwnerListGroup";
        addWidget(Group, ownerListGroup);

        tileOwnerComboBox = ownerListGroup + ".OwnerList";
        addWidget(ComboBox, tileOwnerComboBox);
        setWidgetSize(tileOwnerComboBox, "100%", "30px");
        setWidgetOrigin(tileOwnerComboBox, 0.5f, 0.5f);
        setWidgetPosition(tileOwnerComboBox, "50%", "50%");
        awe::addCountriesToList(tileOwnerComboBox, true);
        setItemsToDisplay(tileOwnerComboBox, 6);

        // Units header.
        const auto unitsHeaderGroup = tilesLayout + ".UnitsHeaderGroup";
        addWidget(Group, unitsHeaderGroup);

        const auto unitsHeader = unitsHeaderGroup + ".UnitsHeader";
        addWidget(Label, unitsHeader);
        setWidgetText(unitsHeader, "unitsheader");
        setWidgetOrigin(unitsHeader, 0.5f, 0.5f);
        setWidgetPosition(unitsHeader, "50%", "50%");
        setWidgetTextSize(unitsHeader, 24);
        setWidgetTextStyles(unitsHeader, "Bold | Italic");

        // Units TreeView.
        const auto unitsTreeViewGroup = tilesLayout + ".UnitsTreeViewGroup";
        addWidget(Group, unitsTreeViewGroup);
        setWidgetRatioInLayout(tilesLayout, 8, 2.0f);

        unitTreeView = unitsTreeViewGroup + ".UnitsTreeView";
        addWidget(TreeView, unitTreeView);
        setWidgetSize(unitTreeView, "100%", "100%");

        // UNITS //
        
        // Units vertical layout.
        unitLayout = layout + ".UnitsLayout";
        addWidget(VerticalLayout, unitLayout);

        // Unit picture.
        const auto unitPictureGroup = unitLayout + ".UnitPictureGroup";
        addWidget(Group, unitPictureGroup);
        setWidgetRatioInLayout(unitLayout, 0, 1.5f);

        unitPicture = unitPictureGroup + ".UnitPicture";
        addWidget(Picture, unitPicture);
        setWidgetOrigin(unitPicture, 0.5f, 0.0f);
        setWidgetPosition(unitPicture, "50%", "0%");

        // Unit type long name.
        const auto unitTypeLongNameGroup = unitLayout + ".UnitTypeLongNameGroup";
        addWidget(Group, unitTypeLongNameGroup);

        unitTypeLabel = unitTypeLongNameGroup + ".UnitTypeLongName";
        addWidget(Label, unitTypeLabel);
        setWidgetOrigin(unitTypeLabel, 0.5f, 0.5f);
        setWidgetPosition(unitTypeLabel, "50%", "50%");
        setWidgetTextSize(unitTypeLabel, 24);
        setWidgetTextStyles(unitTypeLabel, "Bold");

        // Movement type.
        const auto movementLayout = unitLayout + ".MovementLayout";
        addWidget(HorizontalLayout, movementLayout);

        const auto movementLabelGroup = movementLayout + ".MovementLabelGroup";
        addWidget(Group, movementLabelGroup);

        const auto movementLabel = movementLabelGroup + ".MovementLabel";
        addWidget(Label, movementLabel);
        setWidgetText(movementLabel, "movement");
        setWidgetOrigin(movementLabel, 1.0f, 0.5f);
        setWidgetPosition(movementLabel, "100%", "50%");

        const auto movementTypeGroup = movementLayout + ".MovementTypeGroup";
        addWidget(Group, movementTypeGroup);

        unitMovementType = movementTypeGroup + ".MovementType";
        addWidget(Label, unitMovementType);
        setWidgetOrigin(unitMovementType, 0.0f, 0.5f);
        setWidgetPosition(unitMovementType, "0%", "50%");

        // Price.
        const auto priceLayout = unitLayout + ".PriceLayout";
        addWidget(HorizontalLayout, priceLayout);

        const auto priceLabelGroup = priceLayout + ".PriceLabelGroup";
        addWidget(Group, priceLabelGroup);

        const auto priceLabel = priceLabelGroup + ".PriceLabel";
        addWidget(Label, priceLabel);
        setWidgetText(priceLabel, "pricelabel");
        setWidgetOrigin(priceLabel, 1.0f, 0.5f);
        setWidgetPosition(priceLabel, "100%", "50%");

        const auto priceGroup = priceLayout + ".PriceGroup";
        addWidget(Group, priceGroup);

        unitPrice = priceGroup + ".Price";
        addWidget(Label, unitPrice);
        setWidgetOrigin(unitPrice, 0.0f, 0.5f);
        setWidgetPosition(unitPrice, "0%", "50%");

        // HP.
        const auto unitHPLayout = unitLayout + ".UnitHPLayout";
        addWidget(HorizontalLayout, unitHPLayout);

        const auto unitHPLabelGroup = unitHPLayout + ".UnitHPLableGroup";
        addWidget(Group, unitHPLabelGroup);

        const auto unitHPLabel = unitHPLabelGroup + ".UnitHPLabel";
        addWidget(Label, unitHPLabel);
        setWidgetText(unitHPLabel, "hp");
        setWidgetOrigin(unitHPLabel, 1.0f, 0.5f);
        setWidgetPosition(unitHPLabel, "100%", "50%");

        const auto unitHPGroup = unitHPLayout + ".UnitHPGroup";
        addWidget(Group, unitHPGroup);

        unitHPEditBox = unitHPGroup + ".UnitHP";
        addWidget(EditBox, unitHPEditBox);
        setWidgetSize(unitHPEditBox, "100%", "30px");
        setWidgetOrigin(unitHPEditBox, 0.5f, 0.5f);
        setWidgetPosition(unitHPEditBox, "50%", "50%");
        onlyAcceptUIntsInEditBox(unitHPEditBox);

        const auto slashLabelGroup2 = unitHPLayout + ".SlashLabelGroup";
        addWidget(Group, slashLabelGroup2);

        const auto slashLabel2 = slashLabelGroup2 + ".SlashLabel";
        addWidget(Label, slashLabel2);
        setWidgetText(slashLabel2, "~/");
        setWidgetOrigin(slashLabel2, 0.5f, 0.5f);
        setWidgetPosition(slashLabel2, "50%", "50%");

        const auto maxUnitHpGroup = unitHPLayout + ".MaxUnitHPGroup";
        addWidget(Group, maxUnitHpGroup);

        unitMaxHPLabel = maxUnitHpGroup + ".MaxUnitHP";
        addWidget(Label, unitMaxHPLabel);
        setWidgetOrigin(unitMaxHPLabel, 0.0f, 0.5f);
        setWidgetPosition(unitMaxHPLabel, "0%", "50%");

        // Fuel.
        const auto unitFuelLayout = unitLayout + ".UnitFuelLayout";
        addWidget(HorizontalLayout, unitFuelLayout);

        const auto unitFuelLabelGroup = unitFuelLayout + ".UnitFuelLableGroup";
        addWidget(Group, unitFuelLabelGroup);

        const auto unitFuelLabel = unitFuelLabelGroup + ".UnitFuelLabel";
        addWidget(Label, unitFuelLabel);
        setWidgetText(unitFuelLabel, "fuel");
        setWidgetOrigin(unitFuelLabel, 1.0f, 0.5f);
        setWidgetPosition(unitFuelLabel, "100%", "50%");

        const auto unitFuelGroup = unitFuelLayout + ".UnitFuelGroup";
        addWidget(Group, unitFuelGroup);

        unitFuelEditBox = unitFuelGroup + ".UnitFuel";
        addWidget(EditBox, unitFuelEditBox);
        setWidgetSize(unitFuelEditBox, "100%", "30px");
        setWidgetOrigin(unitFuelEditBox, 0.5f, 0.5f);
        setWidgetPosition(unitFuelEditBox, "50%", "50%");
        onlyAcceptUIntsInEditBox(unitFuelEditBox);

        const auto slashLabelGroup3 = unitFuelLayout + ".SlashLabelGroup";
        addWidget(Group, slashLabelGroup3);

        const auto slashLabel3 = slashLabelGroup3 + ".SlashLabel";
        addWidget(Label, slashLabel3);
        setWidgetText(slashLabel3, "~/");
        setWidgetOrigin(slashLabel3, 0.5f, 0.5f);
        setWidgetPosition(slashLabel3, "50%", "50%");

        const auto maxUnitFuelGroup = unitFuelLayout + ".MaxUnitFuelGroup";
        addWidget(Group, maxUnitFuelGroup);

        unitMaxFuelLabel = maxUnitFuelGroup + ".MaxUnitFuel";
        addWidget(Label, unitMaxFuelLabel);
        setWidgetOrigin(unitMaxFuelLabel, 0.0f, 0.5f);
        setWidgetPosition(unitMaxFuelLabel, "0%", "50%");

        // MP.
        const auto mpLayout = unitLayout + ".MPLayout";
        addWidget(HorizontalLayout, mpLayout);

        const auto mpLabelGroup = mpLayout + ".MPLabelGroup";
        addWidget(Group, mpLabelGroup);

        const auto mpLabel = mpLabelGroup + ".MPLabel";
        addWidget(Label, mpLabel);
        setWidgetText(mpLabel, "mp");
        setWidgetOrigin(mpLabel, 1.0f, 0.5f);
        setWidgetPosition(mpLabel, "100%", "50%");

        const auto mpGroup = mpLayout + ".MPGroup";
        addWidget(Group, mpGroup);

        unitMPLabel = mpGroup + ".MP";
        addWidget(Label, unitMPLabel);
        setWidgetOrigin(unitMPLabel, 0.0f, 0.5f);
        setWidgetPosition(unitMPLabel, "0%", "50%");

        // Vision.
        const auto visionLayout = unitLayout + ".VisionLayout";
        addWidget(HorizontalLayout, visionLayout);

        const auto visionLabelGroup = visionLayout + ".VisionLabelGroup";
        addWidget(Group, visionLabelGroup);

        const auto visionLabel = visionLabelGroup + ".VisionLabel";
        addWidget(Label, visionLabel);
        setWidgetText(visionLabel, "vision");
        setWidgetOrigin(visionLabel, 1.0f, 0.5f);
        setWidgetPosition(visionLabel, "100%", "50%");

        const auto visionGroup = visionLayout + ".VisionGroup";
        addWidget(Group, visionGroup);

        unitVisionLabel = visionGroup + ".Vision";
        addWidget(Label, unitVisionLabel);
        setWidgetOrigin(unitVisionLabel, 0.0f, 0.5f);
        setWidgetPosition(unitVisionLabel, "0%", "50%");

        // Load limit.
        const auto loadLimitLayout = unitLayout + ".LoadLimitLayout";
        addWidget(HorizontalLayout, loadLimitLayout);

        const auto loadLimitLabelGroup = loadLimitLayout + ".LoadLimitLabelGroup";
        addWidget(Group, loadLimitLabelGroup);

        const auto loadLimitLabel = loadLimitLabelGroup + ".LoadLimitLabel";
        addWidget(Label, loadLimitLabel);
        setWidgetText(loadLimitLabel, "loadlimit");
        setWidgetOrigin(loadLimitLabel, 1.0f, 0.5f);
        setWidgetPosition(loadLimitLabel, "100%", "50%");

        const auto loadLimitGroup = loadLimitLayout + ".LoadLimitGroup";
        addWidget(Group, loadLimitGroup);

        unitLoadLimitLabel = loadLimitGroup + ".LoadLimit";
        addWidget(Label, unitLoadLimitLabel);
        setWidgetOrigin(unitLoadLimitLabel, 0.0f, 0.5f);
        setWidgetPosition(unitLoadLimitLabel, "0%", "50%");

        // Waiting?
        const auto waitingLayout = unitLayout + ".WaitingLayout";
        addWidget(HorizontalLayout, waitingLayout);

        const auto waitingLabelGroup = waitingLayout + ".WaitingLabelGroup";
        addWidget(Group, waitingLabelGroup);

        const auto waitingLabel = waitingLabelGroup + ".WaitingLabel";
        addWidget(Label, waitingLabel);
        setWidgetText(waitingLabel, "waiting");
        setWidgetOrigin(waitingLabel, 1.0f, 0.5f);
        setWidgetPosition(waitingLabel, "100%", "50%");

        const auto waitingGroup = waitingLayout + ".WaitingGroup";
        addWidget(Group, waitingGroup);

        unitWaitingCheckBox = waitingGroup + ".Waiting";
        addWidget(CheckBox, unitWaitingCheckBox);
        setWidgetOrigin(unitWaitingCheckBox, 0.0f, 0.5f);
        setWidgetPosition(unitWaitingCheckBox, "0%", "50%");

        // Capturing?
        const auto capturingLayout = unitLayout + ".CapturingLayout";
        addWidget(HorizontalLayout, capturingLayout);

        const auto capturingLabelGroup = capturingLayout + ".CapturingLabelGroup";
        addWidget(Group, capturingLabelGroup);

        const auto capturingLabel = capturingLabelGroup + ".CapturingLabel";
        addWidget(Label, capturingLabel);
        setWidgetText(capturingLabel, "capturing");
        setWidgetOrigin(capturingLabel, 1.0f, 0.5f);
        setWidgetPosition(capturingLabel, "100%", "50%");

        const auto capturingGroup = capturingLayout + ".CapturingGroup";
        addWidget(Group, capturingGroup);

        unitCapturingCheckBox = capturingGroup + ".Capturing";
        addWidget(CheckBox, unitCapturingCheckBox);
        setWidgetOrigin(unitCapturingCheckBox, 0.0f, 0.5f);
        setWidgetPosition(unitCapturingCheckBox, "0%", "50%");

        // Hiding?
        const auto hidingLayout = unitLayout + ".HidingLayout";
        addWidget(HorizontalLayout, hidingLayout);

        const auto hidingLabelGroup = hidingLayout + ".HidingLabelGroup";
        addWidget(Group, hidingLabelGroup);

        const auto hidingLabel = hidingLabelGroup + ".HidingLabel";
        addWidget(Label, hidingLabel);
        setWidgetText(hidingLabel, "hiding");
        setWidgetOrigin(hidingLabel, 1.0f, 0.5f);
        setWidgetPosition(hidingLabel, "100%", "50%");

        const auto hidingGroup = hidingLayout + ".HidingGroup";
        addWidget(Group, hidingGroup);

        unitHidingCheckBox = hidingGroup + ".Hiding";
        addWidget(CheckBox, unitHidingCheckBox);
        setWidgetOrigin(unitHidingCheckBox, 0.0f, 0.5f);
        setWidgetPosition(unitHidingCheckBox, "0%", "50%");

        // Army.
        const auto armyLayout = unitLayout + ".ArmyLayout";
        addWidget(HorizontalLayout, armyLayout);

        const auto armyLabelGroup = armyLayout + ".ArmyLabelGroup";
        addWidget(Group, armyLabelGroup);

        const auto armyLabel = armyLabelGroup + ".ArmyLabel";
        addWidget(Label, armyLabel);
        setWidgetText(armyLabel, "army");
        setWidgetOrigin(armyLabel, 1.0f, 0.5f);
        setWidgetPosition(armyLabel, "100%", "50%");

        const auto armyListGroup = armyLayout + ".ArmyListGroup";
        addWidget(Group, armyListGroup);

        unitArmyLabel = armyListGroup + ".ArmyList";
        addWidget(Label, unitArmyLabel);
        setWidgetOrigin(unitArmyLabel, 0.0f, 0.5f);
        setWidgetPosition(unitArmyLabel, "0%", "50%");

        // Load and delete buttons.
        const auto unitButtonsLayout = unitLayout + ".UnitButtonsLayout";
        addWidget(HorizontalLayout, unitButtonsLayout);

        const auto unitCreateAndLoadButtonGroup = unitButtonsLayout +
            ".UnitCreateAndLoadButtonGroup";
        addWidget(Group, unitCreateAndLoadButtonGroup);

        unitCreateAndLoadButton =
            unitCreateAndLoadButtonGroup + ".UnitCreateAndLoadButton";
        addWidget(BitmapButton, unitCreateAndLoadButton);
        setWidgetText(unitCreateAndLoadButton, "createandload");
        setWidgetSize(unitCreateAndLoadButton, "100%", "100%");
        setWidgetSprite(unitCreateAndLoadButton, "icon", "loadicon");
        connectSignalHandler(unitCreateAndLoadButton,
            SignalHandler(this.unitCreateAndLoadButtonSignalHandler));

        const auto unitDeleteButtonGroup = unitButtonsLayout +
            ".UnitDeleteButtonGroup";
        addWidget(Group, unitDeleteButtonGroup);

        const auto unitDeleteButton = unitDeleteButtonGroup + ".UnitDeleteButton";
        addWidget(BitmapButton, unitDeleteButton);
        setWidgetText(unitDeleteButton, "deleteunit");
        setWidgetSize(unitDeleteButton, "100%", "100%");
        setWidgetSprite(unitDeleteButton, "icon", "deleteicon");
        connectSignalHandler(unitDeleteButton,
            SignalHandler(this.unitDeleteButtonSignalHandler));

        deselect();
    }

    /**
     * Opens and docks a \c TilePropertiesWindow to one side of the screen.
     * @param onRight If \c TRUE, dock to the right of the screen. If \c FALSE,
     *                dock to the left.
     */
    void dock(const bool onRight = false) {
        const auto defaultWidth = "200px";
        if (onRight) {
            openChildWindow(window, "100%-" + defaultWidth, "0");
            setWidgetSize(window, defaultWidth, "100%");
        } else {
            openChildWindow(window, "0", "0");
            setWidgetSize(window, defaultWidth, "100%");
        }
    }

    /**
     * Refreshes a \c TilePropertiesWindow with a message that instructs the user
     * to select a tile.
     * Also closes the window.
     */
    void deselect() {
        currentUnit = 0;
        setWidgetVisibility(layout, false);
        setWidgetVisibility(errorMessageLabel, true);
        setWidgetSize(layout, "100%-" + scrollBarWidth, "0px");
        closeChildWindow(window);
    }

    /**
     * Refreshes a \c TilePropertiesWindow with a given tile's information.
     * Refreshes both the tile and unit layouts.
     * @param tile The location of the tile. If the tile is out-of-bounds,
     *             \c deselect() will be invoked and the call will return.
     */
    void refresh(const Vector2&in tile) {
        // If the tile is out-of-bounds, display a message in the window by hiding
        // everything else.
        if (edit is null || edit.map.isOutOfBounds(tile)) {
            deselect();
            return;
        }
        disconnectTileLayoutSignalHandlers();

        // If the window is currently closed, dock it.
        // If it is minimised, restore it.
        if (!isChildWindowOpen(window))
            dock();
        else
            restoreChildWindow(window);

        const auto tileType = edit.map.getTileType(tile);
        const auto tileOwner = edit.map.getTileOwner(tile);
        const auto terrain = tileType.type;

        setWidgetSprite(tilePicture, "tilePicture.normal",
            tileOwner == NO_ARMY ? terrain.iconName : terrain.picture(tileOwner));
        setWidgetText(tileTypeLabel, "~" + tileType.scriptName);
        setWidgetText(terrainTypeLabel, terrain.name);
        setWidgetText(tileXLabel, "~" + formatUInt(tile.x));
        setWidgetText(tileYLabel, "~" + formatUInt(tile.y));
        setWidgetText(tileHPEditBox, formatHP(edit.map.getTileHP(tile)));
        setWidgetEnabled(tileHPEditBox, terrain.maxHP != 0);
        setWidgetText(tileMaxHPLabel, "~" + formatUInt(terrain.maxHP));
        setSelectedItem(tileOwnerComboBox,
            tileOwner == NO_ARMY ? 0 : tileOwner + 1);
        setWidgetText(tileDefenceLabel, "~" + formatUInt(terrain.defence));

        tilesUnit = currentUnit = edit.map.getUnitOnTile(tile);
        populateUnitTreeView(currentUnit);

        if (currentUnit == 0) {
            setWidgetRatioInLayout(layout, 0, 1.f);
            setWidgetRatioInLayout(layout, 1, 0.f);
            setWidgetSize(layout, "100%-" + scrollBarWidth, "450px");
        } else {
            setWidgetRatioInLayout(layout, 0, 1.f);
            setWidgetRatioInLayout(layout, 1, 1.5f);
            // We increase the layout's height for each weapon a unit has so that
            // if a unit has a lot of weapons, every widget in the unit layout can
            // still be presented nicely.
            setWidgetSize(layout, "100%-" + scrollBarWidth, "1017+39*" +
                formatUInt(edit.map.getUnitType(currentUnit).weaponCount));
            refreshUnit(currentUnit);
        }

        setWidgetVisibility(layout, true);
        setWidgetVisibility(errorMessageLabel, false);
        connectTileLayoutSignalHandlers();
    }

    /// Connects all tile layout signal handlers.
    private void connectTileLayoutSignalHandlers() {
        connectSignalHandler(tileHPEditBox, function(widgetName, signalName){
            if (signalName == "TextChanged") {
                const auto text = getWidgetText(widgetName);
                const auto hp = parseHP(text);
                const auto actualHP = edit.setSelectedTileHP(hp);
                if (hp != actualHP || text.isEmpty())
                    setWidgetText(widgetName, formatHP(actualHP));
            }
        });
        connectSignalHandler(tileOwnerComboBox,
            SignalHandler(this.tileOwnerComboBoxSignalHandler));
        connectSignalHandler(unitTreeView,
            SignalHandler(this.unitTreeViewSignalHandler));
    }

    /// Disconnects all tile layout signal handlers.
    /// Used to prevent \c refresh() from applying changes to the map that already
    /// exist.
    private void disconnectTileLayoutSignalHandlers() {
        ::disconnectSignalHandlers(
            { tileHPEditBox, tileOwnerComboBox, unitTreeView }
        );
    }

    /**
     * Refreshes the unit layout with information on the given unit.
     * @param tileUnit The ID of the unit to display information on.
     */
    void refreshUnit(const UnitID tileUnit) {
        disconnectUnitLayoutSignalHandlers();
        const auto unitType = edit.map.getUnitType(tileUnit);
        const auto unitArmy = edit.map.getArmyOfUnit(tileUnit);

        setWidgetSprite(unitPicture, "unitPicture",
            unitType.pictureSprite(unitArmy));
        setWidgetText(unitTypeLabel, unitType.name);
        setWidgetText(unitMovementType, unitType.movementType.name);
        setWidgetText(unitPrice, "price", {any(unitType.cost)});
        setWidgetText(unitHPEditBox, formatHP(edit.map.getUnitHP(tileUnit)));
        setWidgetText(unitMaxHPLabel, "~" + formatUInt(unitType.maxHP));
        if (unitType.hasInfiniteFuel) {
            setWidgetEnabled(unitFuelEditBox, false);
            setWidgetText(unitFuelEditBox, "");
            setWidgetText(unitMaxFuelLabel, "inf");
        } else {
            setWidgetEnabled(unitFuelEditBox, true);
            setWidgetText(unitFuelEditBox,
                formatFuel(edit.map.getUnitFuel(tileUnit)));
            setWidgetText(unitMaxFuelLabel,
                "~" + formatInt(unitType.maxFuel));
        }
        setWidgetText(unitMPLabel, "~" + formatUInt(unitType.movementPoints));
        setWidgetText(unitVisionLabel, "~" + formatUInt(unitType.vision));
        setWidgetText(unitLoadLimitLabel,
            "~" + formatUInt(unitType.loadLimit));
        
        for (uint i = 0, len = unitAmmoLayouts.length(); i < len; ++i)
            removeWidget(unitAmmoLayouts[i]);
        unitAmmoLayouts = {};
        for (uint64 i = 0, len = unitType.weaponCount; i < len; ++i) {
            const auto weapon = unitType.weapon(i);

            const string unitAmmoLayout = unitLayout + ".UnitAmmoLayout" +
                formatUInt(i);
            addWidget(HorizontalLayout, unitAmmoLayout);
            setWidgetIndexInContainer(unitLayout,
                getWidgetCount(unitLayout) - 1, 9 + i);
            unitAmmoLayouts.insertLast(unitAmmoLayout);

            const auto unitAmmoLabelGroup =
                unitAmmoLayout + ".UnitAmmoLableGroup";
            addWidget(Group, unitAmmoLabelGroup);

            const auto unitAmmoLabel = unitAmmoLabelGroup + ".UnitAmmoLabel";
            addWidget(Label, unitAmmoLabel);
            setWidgetText(unitAmmoLabel, weapon.name);
            setWidgetOrigin(unitAmmoLabel, 1.0f, 0.5f);
            setWidgetPosition(unitAmmoLabel, "100%", "50%");

            const auto unitAmmoGroup = unitAmmoLayout + ".UnitAmmoGroup";
            addWidget(Group, unitAmmoGroup);

            const auto unitAmmo = unitAmmoGroup + ".UnitAmmo" +
                formatUInt(i);
            addWidget(EditBox, unitAmmo);
            setWidgetSize(unitAmmo, "100%", "30px");
            setWidgetOrigin(unitAmmo, 0.5f, 0.5f);
            setWidgetPosition(unitAmmo, "50%", "50%");
            onlyAcceptUIntsInEditBox(unitAmmo);
            if (weapon.hasInfiniteAmmo) {
                setWidgetEnabled(unitAmmo, false);
            } else {
                setWidgetText(unitAmmo, formatAmmo(
                    edit.map.getUnitAmmo(tileUnit, weapon.scriptName)));
            }
            connectSignalHandler(unitAmmo,
                SignalHandler(this.unitAmmoEditBoxSignalHandler));

            const auto slashLabelGroup = unitAmmoLayout + ".SlashLabelGroup";
            addWidget(Group, slashLabelGroup);

            const auto slashLabel = slashLabelGroup + ".SlashLabel";
            addWidget(Label, slashLabel);
            setWidgetText(slashLabel, "~/");
            setWidgetOrigin(slashLabel, 0.5f, 0.5f);
            setWidgetPosition(slashLabel, "50%", "50%");

            const auto maxUnitAmmoGroup =
                unitAmmoLayout + ".MaxUnitAmmoGroup";
            addWidget(Group, maxUnitAmmoGroup);

            const auto maxUnitAmmo = maxUnitAmmoGroup + ".MaxUnitAmmo";
            addWidget(Label, maxUnitAmmo);
            setWidgetOrigin(maxUnitAmmo, 0.0f, 0.5f);
            setWidgetPosition(maxUnitAmmo, "0%", "50%");
            if (weapon.hasInfiniteAmmo)
                setWidgetText(maxUnitAmmo, "inf");
            else
                setWidgetText(maxUnitAmmo, "~" + formatInt(weapon.maxAmmo));
        }

        setWidgetChecked(unitWaitingCheckBox,
            edit.map.isUnitWaiting(tileUnit));
        setWidgetChecked(unitCapturingCheckBox,
            edit.map.isUnitCapturing(tileUnit));
        setWidgetChecked(unitHidingCheckBox, edit.map.isUnitHiding(tileUnit));
        setWidgetText(unitArmyLabel,
            country[country.scriptNames[unitArmy]].name);
        setWidgetEnabled(unitCreateAndLoadButton,
            edit.map.getLoadedUnits(tileUnit).length() < unitType.loadLimit);

        connectUnitLayoutSignalHandlers();
    }

    /// Connects all unit layout signal handlers (except \c Button handlers).
    private void connectUnitLayoutSignalHandlers() {
        connectSignalHandler(unitHPEditBox,
            SignalHandler(this.unitHPEditBoxSignalHandler));
        connectSignalHandler(unitFuelEditBox,
            SignalHandler(this.unitFuelEditBoxSignalHandler));
        connectSignalHandler(unitWaitingCheckBox,
            SignalHandler(this.unitWaitingCheckBoxSignalHandler));
        connectSignalHandler(unitCapturingCheckBox,
            SignalHandler(this.unitCapturingCheckBoxSignalHandler));
        connectSignalHandler(unitHidingCheckBox,
            SignalHandler(this.unitHidingCheckBoxSignalHandler));
    }

    /// Disconnects all unit layout signal handlers (except \c Button handlers).
    /// Used to prevent \c refreshUnit() from applying changes to the map that
    /// already exist.
    private void disconnectUnitLayoutSignalHandlers() {
        ::disconnectSignalHandlers(
            { unitHPEditBox, unitFuelEditBox, unitWaitingCheckBox,
                unitCapturingCheckBox, unitHidingCheckBox }
        );
    }

    /**
     * Clears the unit \c TreeView and populates it with a given unit.
     * @param unit      The ID of the unit to display in the \c TreeView.
     * @param hierarchy Recursive parameter, leave to \c null.
     */
    private void populateUnitTreeView(const UnitID unit,
        array<string>@ hierarchy = null) {
        if (hierarchy is null) {
            clearItems(unitTreeView);
            if (unit == 0) return;
            @hierarchy = array<string>();
        }

        hierarchy.insertLast("#" + formatUnitID(unit) + " " +
            translate(edit.map.getUnitType(unit).shortName) + " " +
            formatHP(edit.map.getUnitDisplayedHP(unit)) + " " + translate("hp") +
            " " + formatFuel(edit.map.getUnitFuel(unit)) + " " +
            translate("fuel") + (edit.map.isUnitHiding(unit) ? " " +
                translate("hidden") : ""));
        addTreeViewItem(unitTreeView, hierarchy);

        const auto loaded = edit.map.getLoadedUnits(unit);
        for (uint i = 0, len = loaded.length(); i < len; ++i)
            populateUnitTreeView(loaded[i], hierarchy);

        hierarchy.removeLast();
    }

    /**
     * Handles the tile's owner \c ComboBox signals.
     * @param widgetName The full name of the \c ComboBox.
     * @param signalName The name of the signal emitted.
     */
    private void tileOwnerComboBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ItemSelected") {
            const auto id = getSelectedItem(widgetName);
            if (id == 0) {
                edit.setSelectedTileOwner(NO_ARMY);
            } else if (id > 0) {
                edit.setSelectedTileOwner(ArmyID(id - 1));
            }
            // If a unit is capturing a property, it will be set to not capture if
            // the owner of the tile is changed. So refresh the unit layout just
            // in case.
            if (currentUnit != 0) refreshUnit(currentUnit);
        }
    }

    /**
     * Handles the unit \c TreeView signals.
     * @param widgetName The full name of the \c TreeView.
     * @param signalName The name of the signal emitted.
     */
    private void unitTreeViewSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ItemSelected") {
            // Find ID of the unit and refresh the unit layout.
            const auto selectedItem = getSelectedItemTextHierarchy(widgetName);
            // This signal can also be emitted when an item is deselected, in
            // which case the above array will be empty. We don't want to do
            // anything special, just prevent refreshing the unit widgets/throwing
            // a runtime error.
            if (selectedItem.isEmpty()) return;
            const auto unitStr = selectedItem[selectedItem.length() - 1];
            const auto hash = unitStr.findFirst("#");
            const auto space = unitStr.findFirst(" ", hash);
            currentUnit = parseUnitID(unitStr.substr(hash + 1, space - hash - 1));
            refreshUnit(currentUnit);
        }
    }

    /**
     * Handles the unit's HP \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitHPEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto text = getWidgetText(widgetName);
            const auto hp = parseHP(text);
            const auto actualHP = edit.setUnitHP(currentUnit, hp);
            if (hp != actualHP || text.isEmpty())
                setWidgetText(widgetName, formatHP(actualHP));
            populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's fuel \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitFuelEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto text = getWidgetText(widgetName);
            const auto fuel = parseFuel(text);
            const auto actualFuel = edit.setUnitFuel(currentUnit, fuel);
            if (fuel != actualFuel || text.isEmpty())
                setWidgetText(widgetName, formatFuel(actualFuel));
            populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's ammo \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitAmmoEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto weaponIndex = parseUInt(
                widgetName.substr(widgetName.findLastOf("0123456789") + 1));
            const auto text = getWidgetText(widgetName);
            const auto ammo = parseAmmo(text);
            const auto actualAmmo =
                edit.setUnitAmmo(currentUnit, weaponIndex, ammo);
            if (ammo != actualAmmo || text.isEmpty())
                setWidgetText(widgetName, formatAmmo(actualAmmo));
            // Not needed atm as ammo is not displayed in the TreeView.
            // populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's waiting \c CheckBox signals.
     * @param widgetName The full name of the \c CheckBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitWaitingCheckBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Changed") {
            edit.map.waitUnit(currentUnit, isWidgetChecked(widgetName));
            // Not needed atm as the waiting state is not displayed in the
            // TreeView.
            // populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's capturing \c CheckBox signals.
     * @param widgetName The full name of the \c CheckBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitCapturingCheckBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Changed") {
            edit.map.unitCapturing(currentUnit, isWidgetChecked(widgetName));
            // Not needed atm as the capturing state is not displayed in the
            // TreeView.
            // populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's hiding \c CheckBox signals.
     * @param widgetName The full name of the \c CheckBox.
     * @param signalName The name of the signal emitted.
     */
    private void unitHidingCheckBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Changed") {
            edit.map.unitHiding(currentUnit, isWidgetChecked(widgetName));
            populateUnitTreeView(tilesUnit);
        }
    }

    /**
     * Handles the unit's create and load \c BitmapButton signals.
     * @param widgetName The full name of the \c BitmapButton.
     * @param signalName The name of the signal emitted.
     */
    private void unitCreateAndLoadButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            const auto unitTypeSel =
                cast<UnitType>(CurrentlySelectedUnitType.object);
            const auto currentUnitType = edit.map.getUnitType(currentUnit);
            // The user must have a unit type selected, and it must be able to
            // load the selected type onto the current unit!
            if (unitTypeSel is null) {
                awe::OpenMessageBox(mbName, "alert", "pleaseselectunittype", null,
                    mbDisableThis, mbEnableThis);
                addMessageBoxButton(mbName, "ok");
            } else if (!currentUnitType.canLoad[unitTypeSel.scriptName]) {
                awe::OpenMessageBox(mbName, "alert", "invalidunittypeforload",
                    { any(translate(unitTypeSel.name)),
                        any(translate(currentUnitType.name)) },
                    mbDisableThis, mbEnableThis);
                addMessageBoxButton(mbName, "ok");
            } else {
                edit.createAndLoadUnit(currentUnit, unitTypeSel);
            }
        }
    }

    /**
     * Handles the unit's delete \c BitmapButton signals.
     * @param widgetName The full name of the \c BitmapButton.
     * @param signalName The name of the signal emitted.
     */
    private void unitDeleteButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            // deleteUnit() will update the TilePropertiesWindow for us.
            edit.deleteUnit(currentUnit);
        }
    }

    /// Name of the \c MessageBox to create.
    private string mbName;

    /// Name of the widget to disable when opening a \c MessageBox.
    private string mbDisableThis;

    /// Name of the widget to enable when opening a \c MessageBox.
    private string mbEnableThis;

    /// Stores the ID of the unit that is on the currently selected tile.
    private UnitID tilesUnit = 0;

    /// Stores the ID of the unit that's been selected in the \c TreeView.
    private UnitID currentUnit = 0;

    /// Cache of the \c ScrollablePanel's scrollbar width.
    private string scrollBarWidth;

    /// Caches the name of the \c ChildWindow.
    private string window;

    /// Caches the name of the \c VerticalLayout.
    private string layout;

    /// Caches the name of the error message \c Label.
    private string errorMessageLabel;

    /// Caches the name of the \c VerticalLayout containing unit widgets.
    private string unitLayout;

    /// Name of the tile's Picture widget.
    private string tilePicture;

    /// Name of the tile's type label.
    private string tileTypeLabel;

    /// Name of the tile's terrain label.
    private string terrainTypeLabel;

    /// Name of the tile's X label.
    private string tileXLabel;

    /// Name of the tile's Y label.
    private string tileYLabel;

    /// Name of the tile's HP editbox.
    private string tileHPEditBox;

    /// Name of the tile's max HP label.
    private string tileMaxHPLabel;

    /// Name of the tile's owner combobox.
    private string tileOwnerComboBox;

    /// Name of the tile's defence label.
    private string tileDefenceLabel;

    /// Name of the unit treeview.
    private string unitTreeView;

    /// Name of the unit picture.
    private string unitPicture;

    /// Name of the unit's type label.
    private string unitTypeLabel;

    /// Name of the unit's movement type label.
    private string unitMovementType;

    /// Name of the unit's price label.
    private string unitPrice;

    /// Name of the unit's HP editbox.
    private string unitHPEditBox;

    /// Name of the unit's max HP label.
    private string unitMaxHPLabel;

    /// Name of the unit's fuel editbox.
    private string unitFuelEditBox;

    /// Name of the unit's max fuel label.
    private string unitMaxFuelLabel;

    /// Name of the unit's MP label.
    private string unitMPLabel;

    /// Name of the unit's vision label.
    private string unitVisionLabel;

    /// Name of the unit's load limit label.
    private string unitLoadLimitLabel;

    /// Names of each unit weapon layout.
    private array<string> unitAmmoLayouts;

    /// Name of the unit's waiting checkbox.
    private string unitWaitingCheckBox;

    /// Name of the unit's capturing checkbox.
    private string unitCapturingCheckBox;

    /// Name of the unit's hiding checkbox.
    private string unitHidingCheckBox;

    /// Name of the unit's army label.
    private string unitArmyLabel;

    /// Name of the unit's create and load unit button.
    private string unitCreateAndLoadButton;
}

/**
 * When the \c TileProperties window is closed, we need to deselect the tile.
 * @param close Always close the window.
 */
void MapMakerMenu_TileProperties_Closing(bool&out close) {
    if (edit !is null) edit.deselectTile();
}
