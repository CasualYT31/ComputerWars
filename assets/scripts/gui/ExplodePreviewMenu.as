/**
 * @file ExplodePreviewMenu.as
 * Contains code which runs the menu that displays when a user is previewing an
 * explosion.
 */

/**
 * The HP of damage to deal in the explosion, in \em displayed format.
 */
HP EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 0;

/**
 * The range of the explosion.
 */
Vector2 EXPLODE_PREVIEW_MENU_RANGE(0, 0);

/**
 * Allow the user to move the cursor?
 */
bool EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = true;

/**
 * Defines the signature that the \c EXPLODE_PREVIEW_MENU_CALLBACK anonymous
 * function should take.
 */
funcdef void EXPLODE_PREVIEW_MENU_EXPLOSION_CALLBACK();

/**
 * Allows code that invokes this menu to call custom code before performing an
 * explosion.
 */
EXPLODE_PREVIEW_MENU_EXPLOSION_CALLBACK@ EXPLODE_PREVIEW_MENU_CALLBACK = null;

/**
 * Cache of the tile that was selected when the menu was opened.
 */
Vector2 EXPLODE_PREVIEW_MENU_SELECTED_TILE;

/**
 * This menu has no widgets.
 */
void ExplodePreviewMenuSetUp() {}

/**
 * Find all tiles within the range and add them to the available tile list.
 * @param fromTile The tile to draw the explosion preview from.
 */
void updateExplosionTiles(const Vector2&in fromTile) {
    const auto availableTiles = game.map.getAvailableTiles(fromTile,
        EXPLODE_PREVIEW_MENU_RANGE.x, EXPLODE_PREVIEW_MENU_RANGE.y);
    if (EXPLODE_PREVIEW_MENU_RANGE.x == 0) availableTiles.insertLast(fromTile);
    game.map.clearAvailableTiles();
    for (uint i = 0, len = availableTiles.length(); i < len; ++i)
        game.map.addAvailableTile(availableTiles[i]);
}

/**
 * When the menu is opened, push select the currently selected unit, add the tiles
 * in the configured range to the available list, and disable the closed list.
 */
void ExplodePreviewMenuOpen() {
    EXPLODE_PREVIEW_MENU_SELECTED_TILE = game.map.getSelectedTile();
    game.enableClosedList(false);
    game.setAttackCursorSprites();
    game.map.pushSelectedUnit(game.map.getSelectedUnit());
    game.map.setAvailableTileShader(AvailableTileShader::Red);
    // Fix the range vector if the values aren't in the correct fields.
    if (EXPLODE_PREVIEW_MENU_RANGE.x > EXPLODE_PREVIEW_MENU_RANGE.y) {
        uint copy = EXPLODE_PREVIEW_MENU_RANGE.y;
        EXPLODE_PREVIEW_MENU_RANGE.y = EXPLODE_PREVIEW_MENU_RANGE.x;
        EXPLODE_PREVIEW_MENU_RANGE.x = copy;
    }
    updateExplosionTiles(EXPLODE_PREVIEW_MENU_SELECTED_TILE);
}

/**
 * Explosion has been either cancelled or accepted, so we can pop the data
 * required for that now.
 */
void clearExplodeSelectedUnitData() {
    game.map.popSelectedUnit();
    game.enableClosedList(true);
    game.setNormalCursorSprites();
}

/**
 * When the menu is closed, reverse the effects made when the menu was opened.
 */
void ExplodePreviewMenuClose() {
    // As a safety precaution, reset the state of the global variables.
    EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL = 0;
    EXPLODE_PREVIEW_MENU_RANGE.x = 0;
    EXPLODE_PREVIEW_MENU_RANGE.y = 0;
    EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE = true;
    @EXPLODE_PREVIEW_MENU_CALLBACK = null;
}

/**
 * Allows the user to change the selected tile, if configured to, and allows the
 * them to cancel the preview, or go ahead with the explosion.
 * @param controls         The control map given by the engine.
 * @param mouseInputs      Stores which controls are being triggered by the mouse.
 * @param previousPosition The previous mouse position.
 * @param currentPosition  The current mouse position.
 */
void ExplodePreviewMenuHandleInput(const dictionary controls,
    const dictionary mouseInputs, const MousePosition&in previousPosition,
    const MousePosition&in currentPosition) {
    if (EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE) {
        HandleCommonGameInput(controls, mouseInputs, previousPosition,
            currentPosition);
        updateExplosionTiles(game.map.getSelectedTile());
    }
    if (bool(controls["back"])) {
        // Force the selection to go back to the originally selected unit (as the
        // currently selected tile could be moved whilst in this menu, and this
        // menu relies on the selected tile being on the original unit as the menu
        // opens).
        game.map.setSelectedTile(EXPLODE_PREVIEW_MENU_SELECTED_TILE);
        clearExplodeSelectedUnitData();
        setGUI(PREVIOUS_MENU);
    } else if (bool(controls["select"])) {
        // If the cursor can be moved in this menu, the select control is being
        // input by the mouse, and the mouse is not inside the map's graphic, then
        // drop the input.
        if (EXPLODE_PREVIEW_MENU_ALLOW_TILE_SELECTION_CHANGE &&
            bool(mouseInputs["select"]) &&
            !game.map.getMapBoundingBox().contains(currentPosition)) return;
        // Clear selected unit render data for this menu so that if the callback
        // needs to move a unit, it's free to do so.
        clearExplodeSelectedUnitData();
        // Perform any custom code before exploding.
        if (EXPLODE_PREVIEW_MENU_CALLBACK !is null)
            EXPLODE_PREVIEW_MENU_CALLBACK();
        // Perform explosion.
        game.damageUnitsInRange(game.map.getSelectedTile(),
            EXPLODE_PREVIEW_MENU_RANGE.x, EXPLODE_PREVIEW_MENU_RANGE.y,
            EXPLODE_PREVIEW_MENU_DISPLAYED_HP_TO_DEAL, { "OOZIUM" });
        // Go back to the Map menu.
        setGUI("Map");
    }
}
