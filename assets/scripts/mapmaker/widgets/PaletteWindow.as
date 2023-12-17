/**
 * @file PaletteWindow.as
 * Defines the palette window.
 */

/**
 * Allows the user to select different types of objects for painting.
 */
class PaletteWindow : ChildWindow {
    /**
     * Sets up the palette window.
     */
    PaletteWindow() {
        // Setup the child window.
        setText("objectdialog");
        add(tabContainer);
        tabContainer.connect(SelectionChanged,
            SingleSignalHandler(this.tabContainerSelectionChanged));

        // Setup each palette.
        panels.resize(2);
        @panels[0] = TerrainPalette(@tabContainer);
        @panels[1] = TileTypePalette(@tabContainer);
        tabContainer.setSelectedTab(0);
    }

    /**
     * Restores and docks the palette window to the right of its parent.
     */
    void dock() {
        const auto defaultWidth =
            (currentTab >= 0 ? panels[currentTab].defaultWidth() : "200");
        setSize(defaultWidth, "100%");
        if (currentTab >= 0) panels[currentTab].paletteWindowSizeChanged();
        open("100%-" + defaultWidth, "0%");
    }

    /**
     * When a new palette has been selected, update the child window's handlers.
     */
    private void tabContainerSelectionChanged() {
        disconnectAll();
        currentTab = tabContainer.getSelectedTab();
        // If a new tab has been selected, connect it.
        if (currentTab >= 0) {
            auto handler = SingleSignalHandler(
                panels[currentTab].paletteWindowSizeChanged);
            connect(SizeChanged, handler);
            connect(Minimized, handler);
            connect(Maximized, handler);
            panels[currentTab].paletteWindowSizeChanged();
        }
    }

    /**
     * Cache of the currently selected tab.
     */
    private int currentTab = -1;

    /**
     * The tab container containing the different palettes.
     */
    private TabContainer tabContainer;

    /**
     * The palettes available.
     */
    private array<PalettePanel@> panels;
}

/**
 * Defines constants that the \c PalettePanel classes use internally.
 */
namespace PalettePanelConstants {
    /**
     * The default height of widgets that top and bottom the button panel.
     */
    const string WidgetHeight = "35";
}

/**
 * Base class for panels in a palette window.
 */
abstract class PalettePanel {
    /**
     * Sets up a palette panel.
     * @param tabContainer         The tab container to create the tab and panel
     *                             in.
     * @param tabText              The text to apply to the new tab.
     * @param selectedObjectWidget The selected object widget managed by the
     *                             subclass that displays the current selection.
     * @param scriptNames          An array of widget names to assign to each
     *                             \c BitmapButton.
     * @param buttonSize           The width and height of each button.
     * @param defaultColumnCount   When docking the palette window with this panel
     *                             selected, it will be given a width that will
     *                             show this number of buttons per row.
     * @param neutral              \c TRUE to include neutral in the country
     *                             combobox. \c FALSE to exclude neutral.
     */
    PalettePanel(TabContainer@ const tabContainer, const string&in tabText,
        SelectedObjectWidget@ const selectedObjectWidget,
        const array<string>@ const scriptNames, const float buttonSize,
        const uint defaultColumnCount, const bool neutral) {
        panelID = tabContainer.addTabAndPanel(tabText);

        // Setup the selected object widget with a default height.
        @selectedObject = selectedObjectWidget;
        selectedObject.setSize("", PalettePanelConstants::WidgetHeight);
        selectedObject.setAutoLayout(AutoLayout::Top);
        ::add(panelID, selectedObject);

        // Setup the country combobox.
        @ownerList = CountryComboBox(neutral, 5 + (neutral ? 1 : 0),
            CountryComboBoxCallback(this.countryComboboxCallback));
        ownerList.setSize("", PalettePanelConstants::WidgetHeight);
        ownerList.setAutoLayout(AutoLayout::Bottom);
        ::add(panelID, ownerList);

        // Setup the button containers.
        buttonPanel.add(buttonWrap);

        buttonPanel.setVerticalScrollbarAmount(25);
        buttonPanel.setHorizontalScrollbarPolicy(ScrollbarPolicy::Never);
        buttonPanel.setAutoLayout(AutoLayout::Fill);
        ::add(panelID, buttonPanel);

        // Add each object button.
        objectButtonSize = buttonSize;
        objectButtons.resize(scriptNames.length());
        for (uint64 i = 0, len = scriptNames.length(); i < len; ++i) {
            @objectButtons[i] = BitmapButton();
            objectButtons[i].setName(scriptNames[i]);
            objectButtons[i].setSize(formatFloat(buttonSize),
                formatFloat(buttonSize));
            objectButtons[i].connect(
                MultiSignalHandler(this.objectButtonSignalHandler));
            buttonWrap.add(objectButtons[i]);
        }
        defaultWidthOfWindow = formatFloat(
            buttonSize * defaultColumnCount + buttonPanel.getScrollbarWidth() + 2
        );

        // Ensure the scrollable panel's size is initialised.
        paletteWindowSizeChanged();
    }

    /**
     * Calculates the width that the palette window has to be for this palette
     * panel to fit the number of columns given at construction
     * (\c defaultColumnCount).
     * @return A pixel value in string form.
     */
    string defaultWidth() const {
        return defaultWidthOfWindow;
    }

    /**
     * When the palette window is resized, resize the button wrap to allow the
     * scrollable panel to produce the correct scroll bars.
     * It's slow as balls when called repeatedly but it'll have to do.
     */
    void paletteWindowSizeChanged() {
        const auto widthOfPanel = buttonPanel.getFullSize().x;
        auto columns = widthOfPanel / objectButtonSize;
        if (columns < 1.0f) columns = 1.0f;
        const auto rows = ceil(buttonWrap.widgetCount() / floor(columns));
        buttonWrap.setSize(
            "100%-" + formatFloat(buttonPanel.getScrollbarWidth()),
            formatFloat(rows * objectButtonSize)
        );
    }

    /**
     * All object buttons will have this signal handler, to be implemented by the
     * subclass.
     * Each object button will have its associated script name assigned to its
     * widget name.
     */
    protected void objectButtonSignalHandler(const WidgetID, const string&in) {}

    /**
     * Country combobox callback, to be implemented by the subclass.
     */
    protected void countryComboboxCallback(const ArmyID) {}

    /**
     * The ID of the panel whose resources are managed by the engine.
     */
    private WidgetID panelID;
    /**
     * The panel's ID is exposed to subclasses so that they can add additional
     * widgets if needed.
     */
    protected WidgetID panel {
        get const {
            return panelID;
        }
    }

    /**
     * Stores the width the palette window needs to be when docking, when this
     * panel is open.
     */
    private string defaultWidthOfWindow;

    /**
     * The size of the object buttons.
     */
    private float objectButtonSize;

    /**
     * Previews the selected object in this palette.
     */
    private SelectedObjectWidget@ selectedObject;

    /**
     * The scrollable panel that contains the button layout.
     */
    private ScrollablePanel buttonPanel;

    /**
     * The layout in which all of the object buttons are placed.
     */
    private HorizontalWrap buttonWrap;

    /**
     * The object buttons.
     */
    private array<BitmapButton@> objectButtons;

    /**
     * Returns the number of buttons.
     */
    protected uint64 buttonCount {
        get const {
            return objectButtons.length();
        }
    }

    /**
     * Provides access to the buttons to allow for their sprites to be set.
     */
    protected BitmapButton@ get_buttons(uint i) property {
        return objectButtons[i];
    }

    /**
     * Allows the user to define the owner associated with the selected object.
     */
    private CountryComboBox@ ownerList;
}

/**
 * The palette panel for \c selectedTerrain.
 */
class TerrainPalette : Observer, PalettePanel {
    /**
     * Sets up the terrain palette.
     * @param tabContainer The tab container to create the tab and panel in.
     */
    TerrainPalette(TabContainer@ const tabContainer) {
        super(
            tabContainer,
            "terraindialog",
            // Don't bother detaching, we'll never need to.
            selectedTerrain.widgetFactory(),
            generateScriptNames(),
            getHeightOfTallestFrame("tile.normal") + 10,
            3,
            true
        );
        // Attach this palette to the selectedTerrain to receive a refresh
        // request when the selected owner changes in any way.
        selectedTerrain.attach(this);
        regenerateButtonSprites();
    }

    /**
     * When the owner changes, update the sprites on the buttons.
     */
    private void refresh(any&in data = any()) override {
        if (latestOwner != selectedTerrain.owner) regenerateButtonSprites();
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected terrain.
     */
    private void regenerateButtonSprites() {
        latestOwner = selectedTerrain.owner;
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            buttons[i].setSprite("tile.normal", latestOwner.isEmpty() ?
                terrain[scriptName].primaryTileType.neutralTileSprite :
                terrain[scriptName].primaryTileType.ownedTileSprite(latestOwner)
            );
        }
    }

    /**
     * When a terrain button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != MouseReleased) return;
        @selectedTerrain.type = terrain[::getWidgetName(button)];
    }

    /**
     * When a new owner is selected, update it in \c selectedTerrain.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        if (owner == NO_ARMY) selectedTerrain.owner = "";
        else selectedTerrain.owner = country.scriptNames[owner];
    }

    /**
     * Filters terrains based on whether they are paintable or not.
     * @return A list of script names for terrains that are paintable.
     */
    private array<string>@ generateScriptNames() const {
        array<string>@ names = array<string>();
        for (uint64 i = 0, len = terrain.scriptNames.length(); i < len; ++i) {
            const auto name = terrain.scriptNames[i];
            if (terrain[name].isPaintable) names.insertLast(name);
        }
        return names;
    }

    /**
     * When the owner of the selected tile type changes, it is updated here.
     */
    private string latestOwner;
}

/**
 * The palette panel for \c selectedTileType.
 */
class TileTypePalette : Observer, PalettePanel {
    /**
     * Sets up the tile type palette.
     * @param tabContainer The tab container to create the tab and panel in.
     */
    TileTypePalette(TabContainer@ const tabContainer) {
        super(
            tabContainer,
            "tiledialog",
            // Don't bother detaching, we'll never need to.
            selectedTileType.widgetFactory(),
            generateScriptNames(),
            getHeightOfTallestFrame("tile.normal") + 10,
            6,
            true
        );
        // Attach this palette to the selectedTileType to receive a refresh
        // request when the selected owner changes in any way.
        selectedTileType.attach(this);
        regenerateButtonSprites();
    }

    /**
     * When the owner changes, update the sprites on the buttons.
     */
    private void refresh(any&in data = any()) override {
        if (latestOwner != selectedTileType.owner) regenerateButtonSprites();
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected tile type.
     */
    private void regenerateButtonSprites() {
        latestOwner = selectedTileType.owner;
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            buttons[i].setSprite("tile.normal", latestOwner.isEmpty() ?
                tiletype[scriptName].neutralTileSprite :
                tiletype[scriptName].ownedTileSprite(latestOwner)
            );
        }
    }

    /**
     * When a tile type button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != MouseReleased) return;
        @selectedTileType.type = tiletype[::getWidgetName(button)];
    }

    /**
     * When a new owner is selected, update it in \c selectedTileType.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        if (owner == NO_ARMY) selectedTileType.owner = "";
        else selectedTileType.owner = country.scriptNames[owner];
    }

    /**
     * Filters tile types based on whether they are paintable or not.
     * @return A list of script names for tile types that are paintable.
     */
    private array<string>@ generateScriptNames() const {
        array<string>@ names = array<string>();
        for (uint64 i = 0, len = tiletype.scriptNames.length(); i < len; ++i) {
            const auto name = tiletype.scriptNames[i];
            if (tiletype[name].isPaintable) names.insertLast(name);
        }
        return names;
    }

    /**
     * When the owner of the selected tile type changes, it is updated here.
     */
    private string latestOwner;
}
