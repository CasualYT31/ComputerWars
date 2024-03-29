/**
 * @file ExplodePreviewMenu.as
 * The menu that allows the current army to preview an explosion before comitting
 * to it.
 */

/**
 * Defines the signature that the \c ExplodePreviewMenu::explosionHandler
 * anonymous function should take.
 * @param <tt>const Vector2&in</tt> The tile that was selected when the menu was
 *                                  opened, \b not the tile that was selected
 *                                  \em whilst this menu was open!
 * @param <tt>const Vector2&in</tt> The tile that was selected whilst this menu
 *                                  was open.
 */
funcdef
    void ExplodePreviewMenuExplosionCallback(const Vector2&in, const Vector2&in);

/**
 * Defines the signature that the \c ExplodePreviewMenu::postExplosionHandler
 * anonymous function should take.
 * @param <tt>const Vector2&in</tt> The tile that was selected whilst this menu
 *                                  was open.
 */
funcdef void ExplodePreviewMenuPostExplosionCallback(const Vector2&in);

/**
 * The preview explosion menu.
 */
class ExplodePreviewMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    ExplodePreviewMenu() {
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        selectedTileCache = game.map.getSelectedTile();
        game.enableClosedList(false);
        game.setAttackCursorSprites();
        game.map.pushSelectedUnit(game.map.getSelectedUnit());
        game.map.setAvailableTileShader(AvailableTileShader::Red);
        updateExplosionTiles();
        setVisibility(true);
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        damageToDeal = 0;
        range.x = 0;
        range.y = 0;
        allowCursorMovement = true;
        @explosionHandler = null;
        @postExplosionHandler = null;
        setVisibility(false);
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
        // If the user can re-select the current tile, allow them to.
        if (allowCursorMovement) {
            HandleCommonGameInput(ui, mouse, previousMouse, currentMouse);
            updateExplosionTiles();
        }
        // Handle cancellation and acceptance.
        if (bool(ui["back"])) {
            clearSelectedUnitData();
            setGUI(PREVIOUS_MENU);
        } else if (bool(ui["select"])) {
            // If the cursor can be moved in this menu, the select control is
            // being input by the mouse, and the mouse is not inside the map's
            // graphic, then drop the input.
            if (allowCursorMovement && bool(mouse["select"]) &&
                !game.map.getMapBoundingBox().contains(currentMouse)) return;
            // Clear selected unit render data for this menu so that if the
            // callback needs to move a unit, it's free to do so.
            clearSelectedUnitData();
            const auto selectedTile = game.map.getSelectedTile();
            // Perform any custom code before exploding.
            if (explosionHandler !is null)
                explosionHandler(selectedTileCache, selectedTile);
            // Perform explosion.
            game.damageUnitsInRange(selectedTile, range.x, range.y,
                damageToDeal, { "OOZIUM" });
            // Perform any custom code after exploding.
            if (postExplosionHandler !is null) postExplosionHandler(selectedTile);
            // Go back to the game screen.
            setGUI("GameScreen");
        }
    }

    /**
     * The method that needs to be called before opening this menu.
     * @param damage                   The damage to deal towards each target in
     *                                 the explosion. In \em displayed format.
     * @param newRange                 The range of the explosion. Will be
     *                                 automatically adjusted to ensure \c x is
     *                                 lower than or equal to \c y.
     * @param allowTileSelectionChange Allow the user to move the cursor whilst
     *                                 this menu is open?
     * @param callback                 Carry out custom code before making an
     *                                 explosion.
     * @param postCallback             Carry out custom code after making an
     *                                 explosion.
     */
    void Initialise(const HP damage, const Vector2&in newRange,
        const bool allowTileSelectionChange,
        ExplodePreviewMenuExplosionCallback@ const callback,
        ExplodePreviewMenuPostExplosionCallback@ const postCallback = null) {
        damageToDeal = damage;
        range = newRange;
        if (range.x > range.y) {
            uint copy = range.y;
            range.y = range.x;
            range.x = copy;
        }
        allowCursorMovement = allowTileSelectionChange;
        @explosionHandler = callback;
        @postExplosionHandler = postCallback;
    }

    /**
     * Find all tiles within the range of the currently selected tile and add them
     * to the available tile list.
     */
    private void updateExplosionTiles() {
        const auto fromTile = game.map.getSelectedTile();
        const auto availableTiles = game.map.getAvailableTiles(fromTile, range.x,
            range.y);
        if (range.x == 0) availableTiles.insertLast(fromTile);
        game.map.clearAvailableTiles();
        for (uint i = 0, len = availableTiles.length(); i < len; ++i)
            game.map.addAvailableTile(availableTiles[i]);
    }

    /**
     * Explosion has been either cancelled or accepted, so we can pop the data
     * required for this explosion menu now.
     */
    private void clearSelectedUnitData() {
        game.map.popSelectedUnit();
        game.enableClosedList(true);
        game.setNormalCursorSprites();
    }

    /**
     * Cache of the tile that was selected when the menu was opened.
     */
    private Vector2 selectedTileCache;

    /**
     * The damage (in displayed HP format) to deal to each target in the
     * explosion.
     */
    private HP damageToDeal = 0;

    /**
     * The range of the explosion.
     * \c x must be lower than or equal to \c y.
     */
    private Vector2 range;

    /**
     * \c TRUE if the user can reselect the current tile, \c FALSE if not.
     */
    private bool allowCursorMovement = true;

    /**
     * Callback invoked just before an explosion takes place.
     */
    private ExplodePreviewMenuExplosionCallback@ explosionHandler = null;

    /**
     * Callback invoked just after an explosion takes place.
     */
    private ExplodePreviewMenuPostExplosionCallback@ postExplosionHandler = null;
}
