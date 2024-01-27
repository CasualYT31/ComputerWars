/**
 * @file ScriptsMap.as
 * Defines code common to both the game logic and map maker logic.
 */

/**
 * Base class for all <tt>Map</tt>-based classes.
 */
shared abstract class ScriptsMap {
    ////////////////////////////
    // CONSTRUCTION & LOADING //
    ////////////////////////////
    /**
     * Common setup code for all map classes.
     * Also parses additional map data and updates private fields accordingly.
     * @param mapToAdd Handle to the map that subclasses interface with.
     */
    ScriptsMap(Map@ const mapToAdd) {
        if (mapToAdd is null) {
            error("An invalid Map handle was given to the constructor of a "
                "ScriptMap; the game will crash soon!");
            return;
        }
        @map = mapToAdd;
        map.addMementoStateChangedCallback(
            MementoStateChangedCallback(this.loadAdditionalData));
        loadAdditionalData();
        map.setMapScalingFactor(_mapScalingFactor, false);
        setNormalCursorSprites();
    }

    /////////////////////
    // ADDITIONAL DATA //
    /////////////////////
    /**
     * Enables or disables random weather.
     * @param enabled \c TRUE to enable, \c FALSE to disable.
     */
    void enableRandomWeather(const bool enabled) {
        randomWeather = enabled;
        saveAdditionalData();
    }

    /**
     * Is random weather enabled on this map?
     * @return \c TRUE if enabled, \c FALSE if disabled.
     */
    bool isRandomWeatherEnabled() const {
        return randomWeather;
    }

    /**
     * Sets the default weather of this map.
     * @param defWeather Handle to the properties of the default weather.
     */
    void setDefaultWeather(const Weather@ const defWeather) {
        @defaultWeather = defWeather;
        saveAdditionalData();
    }

    /**
     * Retrieves the properties of this map's default weather.
     * @return Handle to the properties of the default weather.
     */
    const Weather@ getDefaultWeather() const {
        return defaultWeather;
    }

    /**
     * If a weather different to the default has been set in \c map, this method
     * should be called to store which day that weather started.
     * @param d The day the new weather started on.
     */
    void setDayDifferentWeatherStartedOn(const Day d) {
        dayDifferentWeatherStartedOn = d;
        saveAdditionalData();
    }

    /**
     * On what day did the current weather start?
     * @return The day the current weather started on if it's different from the
     *         default.
     */
    Day getDayDifferentWeatherStartedOn() const {
        return dayDifferentWeatherStartedOn;
    }

    /**
     * If a weather different to the default has been set in \c map, this method
     * should be called to store which turn that weather started.
     * @param a The ID of the army whose turn the new weather started on.
     */
    void setArmyDifferentWeatherStartedOn(const ArmyID a) {
        armyDifferentWeatherStartedOn = a;
        saveAdditionalData();
    }

    /**
     * On what turn did the current weather start?
     * @return The ID of the army whose turn the current weather started on if
     *         it's different from the default. Please note that this army may or
     *         may not exist.
     */
    Day getArmyDifferentWeatherStartedOn() const {
        return armyDifferentWeatherStartedOn;
    }

    /**
     * Update all of the additional weather properties at once.
     */
    void updateAdditionalWeatherProperties(const bool enabled,
        const Weather@ const defWeather, const Day d, const ArmyID a) {
        randomWeather = enabled;
        @defaultWeather = defWeather;
        dayDifferentWeatherStartedOn = d;
        armyDifferentWeatherStartedOn = a;
        saveAdditionalData();
    }

    /**
     * Resets additional data to the default values.
     */
    private void resetAdditionalData() {
        randomWeather = false;
        @defaultWeather = weather.first;
        dayDifferentWeatherStartedOn = 1;
        armyDifferentWeatherStartedOn = 0;
    }

    /**
     * Deserialises the additional data stored in the map and updates the
     * corresponding fields in this map.
     * Examples of the format: <tt>"05,CLEAR19,1"</tt> random weather disabled,
     * <tt>"14,RAIN19,1"</tt> random weather enabled, with RAIN default weather,
     * and whatever the current weather is (if it's different from RAIN) started
     * on day 19, on the army with ID 1's turn. 4 stores the length of the weather
     * script name string. Commas are used when reading numbers to mark when they
     * end. The first byte (0 or 1 in these exmaples) is always read directly and
     * does not form part of the script name length number. 0 means random weather
     * is disabled, any other character means it is enabled.
     */
    private void loadAdditionalData() {
        resetAdditionalData();
        const auto data = map.getAdditionalData();
        const auto firstComma = data.findFirst(',');
        const auto secondComma = data.findFirst(',', firstComma + 1);
        // If either of these are < 0, the format of the additional data is not
        // valid, leave to the defaults.
        if (firstComma < 0 || secondComma < 0) {
            error("Unrecognised additional data format, leaving additional data "
                "to default values.");
            return;
        }

        int64 pos = 0;
        const auto randomWeatherWillBeEnabled = data.substr(pos++, 1) != "0";
        const auto defaultWeatherScriptNameSizeStr =
            data.substr(pos, firstComma - pos);
        pos += defaultWeatherScriptNameSizeStr.length() + 1 /* comma */;
        const auto defaultWeatherScriptNameSize =
            parseUInt(defaultWeatherScriptNameSizeStr);
        const auto defaultWeatherScriptName =
            data.substr(pos, defaultWeatherScriptNameSize);
        pos += defaultWeatherScriptNameSize;
        if (!weather.contains(defaultWeatherScriptName)) {
            error("Invalid default weather \"" + defaultWeatherScriptName + "\", "
                "leaving additional data to default values.");
            return;
        }
        const auto dayStartedOnStr = data.substr(pos, secondComma - pos);
        pos += dayStartedOnStr.length() + 1 /* comma */;
        const auto dayStartedOn = parseDay(dayStartedOnStr);
        const auto armyStartedOnStr = data.substr(pos);
        const auto armyStartedOn = parseArmyID(armyStartedOnStr);

        // Data may still be invalid at this point, but it should not cause
        // crashes at least.
        randomWeather = randomWeatherWillBeEnabled;
        @defaultWeather = weather[defaultWeatherScriptName];
        dayDifferentWeatherStartedOn = dayStartedOn;
        armyDifferentWeatherStartedOn = armyStartedOn;
    }

    /**
     * Serialises the additional data and stores it in the map.
     * @sa \c loadAdditionalData().
     */
    private void saveAdditionalData() {
        string data = randomWeather ? "1" : "0";
        data += defaultWeather.scriptName.length() + "," +
            defaultWeather.scriptName;
        data += formatDay(dayDifferentWeatherStartedOn) + ",";
        data += formatArmyID(armyDifferentWeatherStartedOn);
        map.setAdditionalData(data);
    }

    /**
     * Is random weather enabled?
     */
    private bool randomWeather = false;

    /**
     * The default weather of this map, if random weather is enabled.
     */
    private const Weather@ defaultWeather = weather.first;

    /**
     * If a weather different to the default has been set, this will store the day
     * it started on.
     */
    private Day dayDifferentWeatherStartedOn = 1;

    /**
     * If a weather different to the default has been set, this will store the
     * army it started on.
     */
    private ArmyID armyDifferentWeatherStartedOn = 0;

    /////////////
    // ZOOMING //
    /////////////
    /**
     * Increases the map scaling factor by \c 1.0.
     * The map scaling factor does not go above \c 5.0.
     */
    void zoomIn() {
        _mapScalingFactor += 1.0;
        if (_mapScalingFactor > 5.0) _mapScalingFactor = 5.0;
        map.setMapScalingFactor(_mapScalingFactor);
    }
    
    /**
     * Decreases the map scaling factor by \c 1.0.
     * The map scaling factor does not go below \c 1.0.
     */
    void zoomOut() {
        _mapScalingFactor -= 1.0;
        if (_mapScalingFactor < 1.0) _mapScalingFactor = 1.0;
        map.setMapScalingFactor(_mapScalingFactor);
    }

    /**
     * Retrieves the current map scaling factor.
     * @return The scaling factor last set with \c zoomIn() or \c zoomOut().
     */
    float getMapScalingFactor() const final {
        return _mapScalingFactor;
    }

    ////////////
    // CURSOR //
    ////////////
    /**
     * Reverts the cursor back to the normal sprites.
     */
    void setNormalCursorSprites() {
        map.setULCursorSprite("ulcursor");
        map.setURCursorSprite("urcursor");
        map.setLLCursorSprite("llcursor");
        map.setLRCursorSprite("lrcursor");
    }

    /**
     * Sets the cursor to have the attack cursor sprites.
     */
    void setAttackCursorSprites() {
        map.setULCursorSprite("ulattackcursor");
        map.setURCursorSprite("urattackcursor");
        map.setLLCursorSprite("llattackcursor");
        map.setLRCursorSprite("lrattackcursor");
    }

    /**
     * Sets the cursor to have the delete cursor sprites.
     */
    void setDeleteCursorSprites() {
        map.setULCursorSprite("deletecursor");
        map.setURCursorSprite("deletecursor");
        map.setLLCursorSprite("deletecursor");
        map.setLRCursorSprite("deletecursor");
    }

    ////////////////////
    // TILE SELECTION //
    ////////////////////
    /**
     * Moves the cursor up one tile.
     */
    void moveSelectedTileUp() {
        commonTileSelectionCode(map.moveSelectedTileUp());
    }
    
    /**
     * Moves the cursor down one tile.
     */
    void moveSelectedTileDown() {
        commonTileSelectionCode(map.moveSelectedTileDown());
    }
    
    /**
     * Moves the cursor left one tile.
     */
    void moveSelectedTileLeft() {
        commonTileSelectionCode(map.moveSelectedTileLeft());
    }
    
    /**
     * Moves the cursor right one tile.
     */
    void moveSelectedTileRight() {
        commonTileSelectionCode(map.moveSelectedTileRight());
    }
    
    /**
     * Selects a tile based on a given pixel.
     * @param pixel The tile underneath this pixel will be selected.
     */
    void setSelectedTileByPixel(const MousePosition&in pixel) {
        commonTileSelectionCode(map.setSelectedTileByPixel(pixel));
    }

    /**
     * Common code to execute when changing which tile is selected.
     * Overrides of this method should invoke the base class method.
     * @param playSound \c TRUE if the "move cursor" sound should be played.
     */
    protected void commonTileSelectionCode(const bool playSound) {
        if (playSound) map.queuePlay("sound", "movecursor");
    }

    //////////
    // DATA //
    //////////
    /**
     * The map.
     * @warning Although read-write access to the map is given here, it is assumed
     *          that the state of the map will be changed via \c PlayableMap's
     *          operations wherever possible. Also, do not update the handle
     *          itself. I would have made it constant if I could.
     */
    Map@ map;

    /**
     * The map scaling factor.
     */
    private float _mapScalingFactor = 2.0f;
}
