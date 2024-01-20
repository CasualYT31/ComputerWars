/**
 * @file DetailedInfoMenu.as
 * The menu that shows details on all armies, as well as the currently selected
 * tile.
 */

/**
 * The detailed info menu.
 */
class DetailedInfoMenu : Menu, Panel {
    /**
     * Set up the menu.
     */
    DetailedInfoMenu() {
        // Setup the army panel.
        armyPanel.setHorizontalScrollbarAmount(100);
        armyPanel.setVerticalScrollbarAmount(50);
        day.setPosition("2%", "2%");
        day.setTextSize(32);
        day.setTextColour(White);
        day.setTextOutlineColour(Black);
        day.setTextOutlineThickness(3.0);
        armyPanel.add(day);

        // Setup the tile widgets.
        tileDefenceIcon.setSprite("icon", "defstar");
        tileHPIcon.setSprite("icon", "hp");
        tilePicture.setOrigin(1.0, 0.5);
        tilePicture.setPosition("100%", "50%");
        tileDefenceIconGroup.add(tileDefenceIcon);
        tileDefenceRatingGroup.add(tileDefenceRating);
        tileHPIconGroup.add(tileHPIcon);
        tileHPGroup.add(tileHP);
        tilePicturePanel.add(tilePicture);

        // Setup the tile layouts.
        tilePanel.setPadding("5%");
        tilePanel.add(tileBaseLayout);
        tileBaseLayout.add(tileInfoAndPictureLayout);
        tileBaseLayout.add(tileDescription);
        tileBaseLayout.add(tileMoveCostsLayout);
        tileBaseLayout.setRatioOfWidget(0, 1.5);
        tileInfoAndPictureLayout.add(tileInfoLayout);
        tileInfoAndPictureLayout.add(tilePicturePanel);
        tileInfoLayout.add(tileLongName);
        tileInfoLayout.add(tileDefenceLayout);
        tileInfoLayout.add(tileHPLayout);
        tileDefenceLayout.add(tileDefenceIconGroup);
        tileDefenceLayout.add(tileDefenceRatingGroup);
        tileHPLayout.add(tileHPIconGroup);
        tileHPLayout.add(tileHPGroup);

        // Setup the unit widgets.
        unitPriceCaption.setText("price", { any("") });
        unitFuelIcon.setSprite("icon", "fuel");
        unitAmmoIcon.setSprite("icon", "ammo");
        unitMPIcon.setSprite("icon", "move");
        unitVisionIcon.setSprite("icon", "vision");
        unitPicture.setPosition("50%", "50%");
        unitPicture.setOrigin(0.5, 0.5);
        unitRangeCaption.setText("range");
        unitRangeCaption.setOrigin(1.0, 0.5);
        unitRangeCaption.setPosition("100%", "50%");
        unitRangeIcon.setSprite("icon", "range");
        unitPicturePanel.add(unitPicture);
        unitRangeCaptionGroup.add(unitRangeCaption);

        // Setup the unit layouts.
        unitPanel.setPadding("5%");
        unitPanel.add(unitBaseLayout);
        unitBaseLayout.add(unitInfoAndPictureLayout);
        unitBaseLayout.add(unitDescription);
        unitInfoAndPictureLayout.add(unitInfoLayout);
        unitInfoAndPictureLayout.add(unitPictureAndRangeLayout);
        unitInfoAndPictureLayout.setRatioOfWidget(0, 1.5);
        unitInfoAndPictureLayout.setRatioOfWidget(1, 1.0);
        unitInfoLayout.add(unitName);
        unitInfoLayout.add(unitStatsGrid);
        unitInfoLayout.setRatioOfWidget(0, 1.0);
        unitInfoLayout.setRatioOfWidget(1, 2.0);
        unitInfoLayout.setPadding("", "", "", "20%");
        unitStatsGrid.add(unitPriceCaption, 0, 0);
        unitStatsGrid.add(unitFuelIcon, 1, 0);
        unitStatsGrid.add(unitAmmoIcon, 2, 0);
        unitStatsGrid.add(unitPrice, 0, 1);
        unitStatsGrid.add(unitFuel, 1, 1);
        unitStatsGrid.add(unitAmmo, 2, 1);
        unitStatsGrid.add(unitMPIcon, 0, 2);
        unitStatsGrid.add(unitMoveTypeIcon, 1, 2);
        unitStatsGrid.add(unitVisionIcon, 2, 2);
        unitStatsGrid.add(unitMP, 0, 3);
        unitStatsGrid.add(unitMoveType, 1, 3);
        unitStatsGrid.add(unitVision, 2, 3);
        unitPictureAndRangeLayout.add(unitPicturePanel);
        unitPictureAndRangeLayout.add(unitRangeLayout);
        unitPictureAndRangeLayout.setRatioOfWidget(0, 9.0);
        unitPictureAndRangeLayout.setRatioOfWidget(1, 1.0);
        unitRangeLayout.add(unitRangeCaptionGroup);
        unitRangeLayout.add(unitRangeGrid);
        unitRangeGrid.add(unitLowerRange, 0, 0);
        unitRangeGrid.add(unitRangeIcon, 0, 1);
        unitRangeGrid.add(unitHigherRange, 0, 2);

        // Setup the layout.
        armyPanel.setDirectionalFlow(null, null, unitPanel, tilePanel);
        tilePanel.setDirectionalFlow(null, null, armyPanel, unitPanel);
        unitPanel.setDirectionalFlow(null, null, tilePanel, armyPanel);
        layout.add(armyPanel);
        layout.add(tilePanel);
        layout.add(unitPanel);

        // Setup the root panel widget and menu.
        setWidgetDirectionalFlowStart("DetailedInfoMenu", armyPanel);
        add(layout);
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        game.map.queuePlay("sound", "openinfopage");
        setVisibility(true);

        // Army panel.
        day.setText("day", { any(game.map.getDay()) });
        const auto armyIDs = game.map.getArmyIDs();
        armies.resize(armyIDs.length());
        for (uint a = 0, armyCount = armies.length(); a < armyCount; ++a) {
            @armies[a] = ArmyWidget();
            armies[a].setAlignment(ArmyWidgetAlignment::Left);
            armies[a].update(armyIDs[a]);
            armies[a].setPosition("2%", "2% + " +
                formatFloat(55 + (ArmyWidgetConstants::Height + 10) * a));
            armyPanel.add(armies[a]);
        }

        // Tile panel.
        const auto currentTile = game.map.getSelectedTile();
        const auto terrainType = game.map.getTileType(currentTile).type;
        tileLongName.setText(terrainType.name);
        tileDefenceRating.setText("~" + formatUInt(terrainType.defence));
        tileHP.setText("~" + formatHP(game.map.getTileHP(currentTile)) + " / " +
            formatUInt(terrainType.maxHP));
        tileHP.setVisibility(terrainType.maxHP != 0);
        tilePicture.setSprite("tilePicture.normal", getTilePicture(currentTile));
        tileDescription.setText(terrainType.description);
        const auto movementTypeNames = movement.scriptNames;
        const auto movementTypeCount = movementTypeNames.length();
        for (uint64 moveID = 0; moveID < movementTypeCount; ++moveID) {
            const auto movementTypeName = movementTypeNames[moveID];
            if (terrainType.moveCost[movementTypeName] < 0) continue;
            tileMoveCosts.insertLast(MoveCostRow(movement[movementTypeName],
                terrainType.moveCost[movementTypeName]));
            tileMoveCostsLayout.add(tileMoveCosts[tileMoveCosts.length() - 1]);
        }

        // Unit panel.
        const auto unitID = game.map.getUnitOnTile(currentTile);
        const auto visible = unitID != NO_UNIT &&
            game.map.isUnitVisible(unitID, game.map.getSelectedArmy());
        unitPanel.setVisibility(visible);
        if (!visible) return;
        const auto unitType = game.map.getUnitType(unitID);
        const auto movementType = unitType.movementType;
        unitName.setText(unitType.name);
        unitPrice.setText("~" + formatUInt(unitType.cost));
        unitFuel.setText("~" + formatFuel(game.map.getUnitFuel(unitID)) + " / " +
            formatInt(unitType.maxFuel));
        // TODO-1 {
        if (unitType.weaponCount > 0 && !unitType.weapon(0).hasInfiniteAmmo)
            unitAmmo.setText("~" + formatAmmo(game.map.getUnitAmmo(unitID,
                unitType.weapon(0).scriptName)) + " / " +
                    formatInt(unitType.weapon(0).maxAmmo));
        else unitAmmo.setText("~");
        // }
        unitMP.setText("~" + formatUInt(unitType.movementPoints));
        unitMoveTypeIcon.setSprite("icon", movementType.iconName);
        unitMoveType.setText(movementType.shortName);
        unitVision.setText("~" + formatUInt(unitType.vision));
        unitPicture.setSprite("unitPicture", unitType.pictureSprite(
            game.map.getArmyCountry(game.map.getArmyOfUnit(unitID)).turnOrder));
        unitRangeLayout.setVisibility(unitType.weaponCount > 0);
        if (unitType.weaponCount > 0) {
            unitLowerRange.setText("~" + formatUInt(unitType.weapon(0).range.x));
            unitHigherRange.setText("~" + formatUInt(unitType.weapon(0).range.y));
        }
        unitDescription.setText(unitType.description);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        game.map.queuePlay("sound", "back");
        setVisibility(false);
        armies.resize(0);
        tileMoveCosts.resize(0);
    }

    /**
     * Called periodically whilst the menu is open.
     * @param ui            Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered.
     * @param mouse         Maps from control names to bools, where \c TRUE is
     *                      stored if a control is currently being triggered by
     *                      the mouse.
     * @param previousMouse The position of the mouse during the previous
     *                      iteration of the game loop.
     * @param currentMouse  The position of the mouse right now.
     */
    void Periodic(const dictionary ui, const dictionary mouse,
        const MousePosition&in previousMouse,
        const MousePosition&in currentMouse) {
        if (bool(ui["back"]) || bool(ui["info"])) setGUI(PREVIOUS_MENU);
    }

    /**
     * Determines which tile picture to present to the user.
     * @param  pos The location of the tile whose picture is to be shown.
     * @return The \c tilePicture.normal sprite to show.
     */
    private string getTilePicture(const Vector2&in pos) {
        const auto t = game.map.getTileType(pos).type;
        // If the tile is owned, attempt to retrieve the owned terrain picture.
        const auto ownerID = game.map.getTileOwner(pos);
        if (ownerID != NO_ARMY) {
            const auto owned =
                t.picture(game.map.getArmyCountry(ownerID).scriptName);
            if (!owned.isEmpty()) return owned;
        }
        // If it can't be retrieved, or if the tile isn't owned, present the icon
        // instead.
        return t.iconName;
    }

    /**
     * The layout in which the army, tile, and unit groups reside.
     */
    private HorizontalLayout layout;

    ////////////////
    // ARMY PANEL //
    ////////////////

    /**
     * The army panel.
     */
    private ScrollablePanel armyPanel;

    /**
     * The day label.
     */
    private Label day;

    /**
     * The army widgets.
     */
    private array<ArmyWidget@> armies;

    ////////////////
    // TILE PANEL //
    ////////////////

    /**
     * The tile panel.
     */
    private ScrollablePanel tilePanel;

    /**
     * The base layout of the tile panel.
     */
    private VerticalLayout tileBaseLayout;

    /**
     * The layout containing the tile's information layout and picture group.
     */
    private HorizontalLayout tileInfoAndPictureLayout;

    /**
     * The layout containing the tile's information.
     */
    private VerticalLayout tileInfoLayout;

    /**
     * The long name of the tile's terrain.
     */
    private Label tileLongName;

    /**
     * The layout containing the defence icon and rating.
     */
    private HorizontalLayout tileDefenceLayout;

    /**
     * The group containing the defence icon.
     */
    private Group tileDefenceIconGroup;

    /**
     * The defence icon.
     */
    private Picture tileDefenceIcon;

    /**
     * The group containing the defence rating.
     */
    private Group tileDefenceRatingGroup;

    /**
     * The defence rating.
     */
    private Label tileDefenceRating;

    /**
     * The layout containing the HP icon and the tile's current HP.
     */
    private HorizontalLayout tileHPLayout;

    /**
     * The group containing the HP icon.
     */
    private Group tileHPIconGroup;

    /**
     * The HP icon.
     */
    private Picture tileHPIcon;

    /**
     * The group containing the HP stat.
     */
    private Group tileHPGroup;

    /**
     * The HP stat.
     */
    private Label tileHP;

    /**
     * The panel containing the tile's picture.
     */
    private Panel tilePicturePanel;

    /**
     * The tile's terrain picture.
     */
    private Picture tilePicture;

    /**
     * The tile's terrain description.
     */
    private Label tileDescription;

    /**
     * The layout containing the tile's movement costs.
     */
    private VerticalLayout tileMoveCostsLayout;

    /**
     * Each of the tile's movement costs.
     */
    private array<MoveCostRow@> tileMoveCosts;

    ////////////////
    // UNIT PANEL //
    ////////////////

    /**
     * The unit panel.
     * Made invisible if there is no unit on the current tile.
     */
    private ScrollablePanel unitPanel;

    /**
     * The base layout of the unit panel.
     */
    private VerticalLayout unitBaseLayout;

    /**
     * The layout containing the information and picture of the unit.
     */
    private HorizontalLayout unitInfoAndPictureLayout;

    /**
     * The layout containing most of the information of the unit.
     */
    private VerticalLayout unitInfoLayout;

    /**
     * The unit's name.
     */
    private Label unitName;

    /**
     * The grid containing the unit's stats.
     */
    private Grid unitStatsGrid;

    // COLUMN //

    /**
     * The price caption.
     */
    private Label unitPriceCaption;

    /**
     * The fuel icon.
     */
    private Picture unitFuelIcon;

    /**
     * The ammo icon.
     */
    private Picture unitAmmoIcon;

    // COLUMN //

    /**
     * The unit's price.
     */
    private Label unitPrice;

    /**
     * The unit's fuel.
     */
    private Label unitFuel;

    /**
     * The unit's price.
     */
    private Label unitAmmo;
    
    // COLUMN //

    /**
     * The MP icon.
     */
    private Picture unitMPIcon;

    /**
     * The movement type icon.
     */
    private Picture unitMoveTypeIcon;

    /**
     * The vision icon.
     */
    private Picture unitVisionIcon;

    // COLUMN //

    /**
     * The unit's MP.
     */
    private Label unitMP;

    /**
     * The unit's movement type.
     */
    private Label unitMoveType;

    /**
     * The unit's vision.
     */
    private Label unitVision;

    // END GRID //

    /**
     * The layout containing the picture and range of the unit.
     */
    private VerticalLayout unitPictureAndRangeLayout;

    /**
     * The panel containing the unit's picture.
     */
    private Panel unitPicturePanel;

    /**
     * The unit's picture.
     */
    private Picture unitPicture;

    /**
     * The layout containing the unit's range and the range caption.
     */
    private HorizontalLayout unitRangeLayout;

    /**
     * The group containing the range caption.
     */
    private Group unitRangeCaptionGroup;

    /**
     * The range caption.
     */
    private Label unitRangeCaption;

    /**
     * The grid containing the unit's range.
     */
    private Grid unitRangeGrid;

    /**
     * The unit's lower range.
     */
    private Label unitLowerRange;

    /**
     * The range icon.
     */
    private Picture unitRangeIcon;

    /**
     * The unit's higher range.
     */
    private Label unitHigherRange;

    /**
     * The unit's description.
     */
    private Label unitDescription;
}

/**
 * Represents a single move cost row within the move costs vertical layout.
 */
class MoveCostRow : HorizontalLayout {
    /**
     * Constructs the widgets for a move cost row.
     * @param movementType Handle to the movement type information to create a row
     *                     from.
     * @param cost         The cost to put in this row.
     */
    MoveCostRow(const Movement@ const movementType, const int cost) {
        movementTypeIcon.setSprite("icon", movementType.iconName);
        movementTypeShortName.setText(movementType.shortName);
        movementTypeCost.setText("~" + formatInt(cost));
        movementTypeIconGroup.add(movementTypeIcon);
        movementTypeShortNameGroup.add(movementTypeShortName);
        movementTypeCostGroup.add(movementTypeCost);
        add(movementTypeIconGroup);
        add(movementTypeShortNameGroup);
        add(movementTypeCostGroup);
    }

    /**
     * The group containing the movement type icon.
     */
    private Group movementTypeIconGroup;

    /**
     * The movement type icon.
     */
    private Picture movementTypeIcon;
    
    /**
     * The group containing the movement type short name.
     */
    private Group movementTypeShortNameGroup;

    /**
     * The movement type short name.
     */
    private Label movementTypeShortName;
    
    /**
     * The group containing the movement type cost.
     */
    private Group movementTypeCostGroup;

    /**
     * The movement type cost.
     */
    private Label movementTypeCost;
}
