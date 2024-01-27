/**
 * @file MapPropertiesWindow.as
 * Defines code that manages the map properties child window.
 */

/**
 * Allows the user to view and edit the current map's properties.
 */
class MapPropertiesWindow : Observer, ButtonsWindow {
    /**
     * Sets up the map properties child window.
     */
    MapPropertiesWindow() {
        // Setup child window.
        super({ ButtonsWindowButton("cancel", SingleSignalHandler(this.cancel)),
            ButtonsWindowButton("ok", SingleSignalHandler(this.ok)) });
        setText("mapprops");
        setSize("565", "275");
        setResizable(false);
        setTitleButtons(TitleButton::Close);
        close(false);

        // Setup the edit boxes, comboboxes, and checkboxes.
        day.setValidator(VALIDATOR_UINT);
        width.setValidator(VALIDATOR_UINT);
        height.setValidator(VALIDATOR_UINT);
        differentWeatherDay.setValidator(VALIDATOR_UINT);
        environmentComboBox.setSize("", "48");
        fogOfWar.setSize("20", "");
        randomWeather.setSize("20", "");
        weatherComboBox.setSize("", "35");
        defaultWeatherComboBox.setSize("", "35");
        differentWeatherArmyComboBox.setSize("", "35");

        // Setup the size layout.
        width.setSize("100%", "100%");
        widthGroup.add(width);
        xLabel.setText("~x");
        xLabel.setPosition("50%", "50%");
        xLabel.setOrigin(0.5, 0.5);
        xGroup.add(xLabel);
        height.setSize("100%", "100%");
        heightGroup.add(height);
        sizeLayout.add(widthGroup);
        sizeLayout.add(xGroup);
        sizeLayout.add(heightGroup);
        sizeLayout.setRatioOfWidget(1, 0.3);

        // Setup property layouts.
        propertiesLayout.setSize("50%", "");
        propertiesLayout.setOrigin(0.0, 0.5);
        propertiesLayout.setPosition("0%", "50%");
        propertiesLayout.setRatioOfWidget(3, 1.25);
        clientArea.add(propertiesLayout);
        weatherPropertiesLayout.setSize("50%", "");
        weatherPropertiesLayout.setOrigin(1.0, 0.5);
        weatherPropertiesLayout.setPosition("100%", "50%");
        weatherPropertiesLayout.setRatioOfWidget(3, 1.25);
        weatherPropertiesLayout.setRatioOfWidget(4, 1.25);
        clientArea.add(weatherPropertiesLayout);

        // Setup the confirm resize window.
        selectedTileType.attach(confirmResize);
    }

    /**
     * Ensures the confirm resize window is detached from the selected tile type
     * object to allow for the window to be deleted.
     */
    ~MapPropertiesWindow() {
        selectedTileType.detach(confirmResize);
    }

    /**
     * Restore the map properties window.
     */
    void open(const string&in x = "50", const string&in y = "35") override {
        ChildWindow::open(x, y);
        if (!confirmResize.isOpen()) refresh(); else confirmResize.open();
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
        closeConfirmResize();
    }

    /**
     * Refreshes the map properties window.
     */
    void refresh(any&in data = any()) override {
        // The map's size may have changed, so we should cancel any in-progress
        // resize confirmation.
        closeConfirmResize();
        name.setText(edit.map.getMapName());
        day.setText(formatDay(edit.map.getDay()));
        const auto size = edit.map.getMapSize();
        width.setText(formatUInt(size.x));
        height.setText(formatUInt(size.y));
        environmentComboBox.select(uint(environment.scriptNames.find(
            edit.map.getEnvironment().scriptName)));
        fogOfWar.setChecked(edit.map.isFoWEnabled());
        weatherComboBox.select(uint(weather.scriptNames.find(
            edit.map.getWeather().scriptName)));
        defaultWeatherComboBox.select(uint(weather.scriptNames.find(
            edit.getDefaultWeather().scriptName)));
        randomWeather.setChecked(edit.isRandomWeatherEnabled());
        differentWeatherDay.setText(formatDay(
            edit.getDayDifferentWeatherStartedOn()));
        differentWeatherArmyComboBox.select(
            edit.getArmyDifferentWeatherStartedOn());
    }

    /**
     * When ok is pressed, attempt to apply the changes, and close the window.
     */
    private void ok() {
        // Cannot set map name illegally.
        if (name.getText().findFirst("~") >= 0) {
            mapMaker.openMessageBox(null, { "ok" }, "cannotsetmaptoillegalname",
                { any("~") });
            return;
        }
        // Cannot set map to empty size.
        const Vector2 newSize(parseUInt(width.getText()),
            parseUInt(height.getText()));
        if (newSize.x * newSize.y == 0) {
            mapMaker.openMessageBox(null, { "ok" }, "cannotresizeto0");
            return;
        }
        // If resizing, ask for confirmation.
        const Vector2 oldSize = edit.map.getMapSize();
        if (newSize != oldSize) {
            openConfirmResize(oldSize.x > newSize.x || oldSize.y > newSize.y);
            return;
        }
        // Carry out map property changes.
        applyChangesAndClose();
    }

    /**
     * When cancel is pressed, close the window.
     */
    private void cancel() {
        close();
    }

    /**
     * Apply the updated map properties and close the window/s.
     */
    private void applyChangesAndClose() {
        edit.setMapProperties(
            name.getText(),
            parseDay(day.getText()),
            environment.scriptNames[uint(environmentComboBox.getSelected())],
            fogOfWar.getChecked()
        );
        edit.setWeatherProperties(
            weather.scriptNames[uint(weatherComboBox.getSelected())],
            weather.scriptNames[uint(defaultWeatherComboBox.getSelected())],
            randomWeather.getChecked(),
            parseDay(differentWeatherDay.getText()),
            ArmyID(differentWeatherArmyComboBox.getSelected())
        );
        const Vector2 oldSize = edit.map.getMapSize(),
            newSize(parseUInt(width.getText()), parseUInt(height.getText()));
        // Check if we've resized first, because in rare cases there may not be a
        // selected tile type, and there may be no actual resize, which means the
        // confirm resize window did not get a chance to prevent no selected tile
        // type.
        if (oldSize != newSize) {
            if (selectedTileType.owner.isEmpty())
                edit.setMapSize(newSize, selectedTileType.type.scriptName);
            else
                edit.setMapSize(newSize, selectedTileType.type.scriptName,
                    country[selectedTileType.owner].turnOrder);
        }
        close();
    }

    /**
     * Opens the confirm resize window and adds it to the parent of this window.
     * @param smaller \c TRUE if the map is getting smaller in at least one
     *                dimension, \c FALSE otherwise.
     */
    private void openConfirmResize(const bool smaller) {
        ::add(getParent(), confirmResize);
        confirmResize.open(
            smaller ? "resizemapconfirmationsmaller" : "resizemapconfirmation");
        setEnabled(false);
    }

    /**
     * Closes/deletes the confirm resize window.
     */
    private void closeConfirmResize() {
        confirmResize.close();
        setEnabled(true);
    }

    /**
     * Window displayed when resizing the map to ask for confirmation.
     */
    private ConfirmTileTypeWindow confirmResize("resizemapconfirmationnotile",
        "resizemapconfirmationtile",
        SingleSignalHandler(this.applyChangesAndClose),
        SingleSignalHandler(this.closeConfirmResize)
    );

    /**
     * The layout in which each of the map's properties are shown.
     */
    private VerticalLayout propertiesLayout;

    /**
     * The name edit box.
     */
    private EditBox name;

    /**
     * The day edit box.
     */
    private EditBox day;

    /**
     * The layout containing the map size edit boxes.
     */
    private HorizontalLayout sizeLayout;

    /**
     * The group within which the width edit box resides.
     */
    private Group widthGroup;

    /**
     * The width edit box.
     */
    private EditBox width;

    /**
     * The group within which the "x" label resides.
     */
    private Group xGroup;

    /**
     * The "x" label that separates the width and height edit boxes.
     */
    private Label xLabel;

    /**
     * The group within which the height edit box resides.
     */
    private Group heightGroup;

    /**
     * The height edit box.
     */
    private EditBox height;

    /**
     * The map's environment.
     */
    private EnvironmentComboBox environmentComboBox(4, null);

    /**
     * Allows the user to enable or disable Fog of War.
     */
    private CheckBox fogOfWar;

    /**
     * The map properties.
     */
    private array<MapPropertyRow@> rows = {
        MapPropertyRow(propertiesLayout, "name", name, 0.3, 0.7),
        MapPropertyRow(propertiesLayout, "daylabel", day, 0.3, 0.7),
        MapPropertyRow(propertiesLayout, "size", sizeLayout, 0.3, 0.7),
        MapPropertyRow(propertiesLayout, "environment", environmentComboBox, 0.3,
            0.7),
        MapPropertyRow(propertiesLayout, "fow", fogOfWar, 0.3, 0.7)
    };

    /**
     * The layout in which each of the map's weather properties are shown.
     */
    private VerticalLayout weatherPropertiesLayout;

    /**
     * The map's weather.
     */
    private WeatherComboBox weatherComboBox(4, null);

    /**
     * The map's default weather.
     */
    private WeatherComboBox defaultWeatherComboBox(4, null);

    /**
     * Allows the user to enable or disable random weather.
     */
    private CheckBox randomWeather;

    /**
     * The day when the current weather started (if it's different from the
     * default).
     */
    private EditBox differentWeatherDay;

    /**
     * The army whose turn the current weather started on (if it's different from
     * the default).
     */
    private CountryComboBox differentWeatherArmyComboBox(false, 5, null);
    
    /**
     * The map's weather properties.
     */
    private array<MapPropertyRow@> weatherRows = {
        MapPropertyRow(weatherPropertiesLayout, "currentweather",
            weatherComboBox, 0.4, 0.6),
        MapPropertyRow(weatherPropertiesLayout, "defaultweather",
            defaultWeatherComboBox, 0.4, 0.6),
        MapPropertyRow(weatherPropertiesLayout, "randomweather",
            randomWeather, 0.4, 0.6),
        MapPropertyRow(weatherPropertiesLayout, "dayweather",
            differentWeatherDay, 0.4, 0.6),
        MapPropertyRow(weatherPropertiesLayout, "armyweather",
            differentWeatherArmyComboBox, 0.4, 0.6)
    };
}

/**
 * A single map property in the map properties window.
 */
class MapPropertyRow : HorizontalLayout {
    /**
     * Sets up a map property row and adds it to a vertical layout.
     * @param parent       The layout to add this new row to.
     * @param text         The caption of the map property.
     * @param editor       The widget owned by the client that's responsible for
     *                     allowing the user to change the property. It will be
     *                     sized and positioned automatically by this class, but
     *                     you can override these values if you wish.
     * @param captionRatio The ratio the caption group will have in the horizontal
     *                     layout.
     * @param editorRatio  The ratio the editor group will have in the horizontal
     *                     layout.
     */
    MapPropertyRow(VerticalLayout@ const parent, const string&in text,
        Widget@ const editor, const float captionRatio, const float editorRatio) {
        caption.setText(text);
        caption.setPosition("100%", "50%");
        caption.setOrigin(1.0, 0.5);
        captionGroup.add(caption);
        captionGroup.setPadding("5", "5", "2.5", "5");
        editor.setPosition("0%", "50%");
        editor.setOrigin(0.0, 0.5);
        editor.setSize("100%", "20");
        editorGroup.add(editor);
        editorGroup.setPadding("2.5", "5", "5", "5");
        add(captionGroup);
        add(editorGroup);
        setRatioOfWidget(0, captionRatio);
        setRatioOfWidget(1, editorRatio);
        parent.add(this);
    }

    /**
     * The group of the caption of the map property.
     */
    private Group captionGroup;

    /**
     * The caption of the map property.
     */
    private Label caption;

    /**
     * The group containing the editor of the map property.
     */
    private Group editorGroup;
}
