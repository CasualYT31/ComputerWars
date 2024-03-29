/**
 * @file PaletteWindow.as
 * Defines the palette window.
 */

/**
 * The different types of palette.
 */
enum Palette {
    Terrain,
    TileType,
    UnitType,
    Structure,
    Count
}

/**
 * Allows the user to select different types of objects for painting.
 */
class PaletteWindow : ChildWindow {
    /**
     * Sets up the palette window.
     * @param toolBar Handle to the tool bar widget to select the paint tool of when
     *                a change is made to any palette panel by the user.
     */
    PaletteWindow(ToolBar@ const toolBar) {
        @this.toolBar = toolBar;

        // Setup the child window.
        setText("objectdialog");
        add(tabContainer);
        tabContainer.connect(SelectionChanged,
            SingleSignalHandler(this.tabContainerSelectionChanged));

        // Setup each palette.
        panels.resize(Palette::Count);
        @panels[Palette::Terrain] = TerrainPalette(@tabContainer, toolBar);
        @panels[Palette::TileType] = TileTypePalette(@tabContainer, toolBar);
        @panels[Palette::UnitType] = UnitTypePalette(@tabContainer, toolBar);
        @panels[Palette::Structure] = StructurePalette(@tabContainer, toolBar);
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
     * Returns the palette that is currently open.
     * @return The \c Palette ID. \c Palette::Count if no palette is currently
     *         selected.
     */
    Palette getSelectedPalette() const {
        return currentTab < 0 ? Palette::Count : Palette(currentTab);
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
            toolBar.selectPaint();
        }
    }

    /**
     * Handle to the tool bar widget.
     */
    private ToolBar@ toolBar;

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
        neutralIncluded = neutral;
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
            "100%-" + (buttonPanel.isVerticalScrollbarVisible() ?
                formatFloat(buttonPanel.getScrollbarWidth()) : "0"),
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
     * When the owner of the selected object changes, the country combobox will
     * need manually updating using this method.
     * @param newOwner Script name of the new owner.
     */
    protected void refreshCountryCombobox(const string&in newOwner) final {
        if (newOwner.isEmpty()) ownerList.select(0);
        else ownerList.select(country[newOwner].turnOrder +
            (neutralIncluded ? 1 : 0));
    }

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
     * \c TRUE if the neutral option has been included in the country combobox.
     */
    private bool neutralIncluded = false;

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
     * @param toolBar      Handle to a tool bar widget. If an object is selected,
     *                     the paint tool from this tool bar will also be selected.
     */
    TerrainPalette(TabContainer@ const tabContainer, ToolBar@ const toolBar) {
        super(
            tabContainer,
            "terraindialog",
            // Don't bother detaching, we'll never need to.
            selectedTerrain.widgetFactory(),
            generateScriptNames(),
            // A more robust implementation would not rely on just one of the tile
            // spritesheets, but they should all be the same size anyway, so the
            // effort to make this more robust is not worth it.
            getHeightOfTallestFrame("tile.normal") + 10,
            3,
            true
        );
        @this.toolBar = toolBar;
        // Attach this palette to the selectedTerrain to receive a refresh
        // request when the selected owner changes in any way.
        selectedTerrain.attach(this);
        regenerateButtonSprites();
    }

    /**
     * Update the sprites on the buttons.
     */
    private void refresh(any&in data = any()) override {
        regenerateButtonSprites();
        refreshCountryCombobox(selectedTerrain.owner);
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected terrain.
     */
    private void regenerateButtonSprites() {
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            buttons[i].setSprite(awe::getEditMapEnvironmentSpritesheet(),
                selectedTerrain.owner.isEmpty() ?
                terrain[scriptName].primaryTileType.neutralTileSprite :
                terrain[scriptName].primaryTileType.ownedTileSprite(
                    selectedTerrain.owner)
            );
        }
    }

    /**
     * When a terrain button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != Clicked) return;
        @selectedTerrain.type = terrain[::getWidgetName(button)];
        toolBar.selectPaint();
    }

    /**
     * When a new owner is selected, update it in \c selectedTerrain.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        if (owner == NO_ARMY) selectedTerrain.owner = "";
        else selectedTerrain.owner = country.scriptNames[owner];
        toolBar.selectPaint();
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
     * Handle to the tool bar widget.
     */
    private ToolBar@ toolBar;
}

/**
 * The palette panel for \c selectedTileType.
 */
class TileTypePalette : Observer, PalettePanel {
    /**
     * Sets up the tile type palette.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param toolBar      Handle to a tool bar widget. If an object is selected,
     *                     the paint tool from this tool bar will also be selected.
     */
    TileTypePalette(TabContainer@ const tabContainer, ToolBar@ const toolBar) {
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
        @this.toolBar = toolBar;
        // Attach this palette to the selectedTileType to receive a refresh
        // request when the selected owner changes in any way.
        selectedTileType.attach(this);
        regenerateButtonSprites();
    }

    /**
     * Update the sprites on the buttons.
     */
    private void refresh(any&in data = any()) override {
        regenerateButtonSprites();
        refreshCountryCombobox(selectedTileType.owner);
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected tile type.
     */
    private void regenerateButtonSprites() {
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            buttons[i].setSprite(awe::getEditMapEnvironmentSpritesheet(),
                selectedTileType.owner.isEmpty() ?
                tiletype[scriptName].neutralTileSprite :
                tiletype[scriptName].ownedTileSprite(selectedTileType.owner)
            );
        }
    }

    /**
     * When a tile type button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != Clicked) return;
        @selectedTileType.type = tiletype[::getWidgetName(button)];
        toolBar.selectPaint();
    }

    /**
     * When a new owner is selected, update it in \c selectedTileType.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        if (owner == NO_ARMY) selectedTileType.owner = "";
        else selectedTileType.owner = country.scriptNames[owner];
        toolBar.selectPaint();
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
     * Handle to the tool bar widget.
     */
    private ToolBar@ toolBar;
}

/**
 * The palette panel for \c selectedUnitType.
 */
class UnitTypePalette : Observer, PalettePanel {
    /**
     * Sets up the unit type palette.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param toolBar      Handle to a tool bar widget. If an object is selected,
     *                     the paint tool from this tool bar will also be selected.
     */
    UnitTypePalette(TabContainer@ const tabContainer, ToolBar@ const toolBar) {
        super(
            tabContainer,
            "unitdialog",
            // Don't bother detaching, we'll never need to.
            selectedUnitType.widgetFactory(),
            unittype.scriptNames,
            // Left intentionally to tile.normal.
            getHeightOfTallestFrame("tile.normal") + 10,
            4,
            false
        );
        @this.toolBar = toolBar;
        // Attach this palette to the selectedUnitType to receive a refresh
        // request when the selected owner changes in any way.
        selectedUnitType.attach(this);
        regenerateButtonSprites();
    }

    /**
     * When the owner changes, update the sprites on the buttons.
     */
    private void refresh(any&in data = any()) override {
        if (latestOwner != selectedUnitType.owner) {
            regenerateButtonSprites();
            refreshCountryCombobox(latestOwner);
        }
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected unit type.
     */
    private void regenerateButtonSprites() {
        latestOwner = selectedUnitType.owner;
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            buttons[i].setSprite("unit",
                unittype[scriptName].unitSprite(latestOwner));
        }
    }

    /**
     * When a unit type button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != Clicked) return;
        @selectedUnitType.type = unittype[::getWidgetName(button)];
        toolBar.selectPaint();
    }

    /**
     * When a new owner is selected, update it in \c selectedUnitType.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        selectedUnitType.owner = country.scriptNames[owner];
        toolBar.selectPaint();
    }

    /**
     * Handle to the tool bar widget.
     */
    private ToolBar@ toolBar;

    /**
     * When the owner of the selected unit type changes, it is updated here.
     */
    private string latestOwner;
}

/**
 * The palette panel for \c selectedStructure.
 */
class StructurePalette : Observer, PalettePanel {
    /**
     * Sets up the structure palette.
     * @param tabContainer The tab container to create the tab and panel in.
     * @param toolBar      Handle to a tool bar widget. If an object is selected,
     *                     the paint tool from this tool bar will also be selected.
     */
    StructurePalette(TabContainer@ const tabContainer, ToolBar@ const toolBar) {
        super(
            tabContainer,
            "structuredialog",
            // Don't bother detaching, we'll never need to.
            selectedStructure.widgetFactory(),
            generateScriptNames(),
            getHeightOfTallestFrame("structure") + 10,
            4,
            false
        );
        @this.toolBar = toolBar;
        // Attach this palette to the selectedStructure to receive a refresh
        // request when the selected owner or destroyed flag change in any way.
        selectedStructure.attach(this);
        regenerateButtonSprites();
        // Setup the destroyed widgets.
        destroyed.setText("destroyed");
        destroyed.setOrigin(0.5, 0.5);
        destroyed.setPosition("50%", "50%");
        destroyed.connect(Changed, SingleSignalHandler(this.destroyedChanged));
        destroyedGroup.add(destroyed);
        destroyedGroup.setSize("", PalettePanelConstants::WidgetHeight);
        destroyedGroup.setAutoLayout(AutoLayout::Bottom);
        ::add(panel, destroyedGroup);
    }

    /**
     * Update the sprites on the buttons.
     * Also make sure the destroyed checkbox is up-to-date.
     */
    private void refresh(any&in data = any()) override {
        regenerateButtonSprites();
        refreshCountryCombobox(selectedStructure.owner);
        destroyed.setChecked(selectedStructure.destroyed);
    }

    /**
     * Update the sprites on the buttons according to the selected owner of the
     * selected structure, as well as its destroyed flag.
     */
    private void regenerateButtonSprites() {
        for (uint64 i = 0, len = buttonCount; i < len; ++i) {
            const auto scriptName = buttons[i].getName();
            if (selectedStructure.destroyed) {
                buttons[i].setSprite(
                    awe::getEditMapEnvironmentStructureIconSpritesheet(),
                    structure[scriptName].destroyedIconName
                );
            } else {
                buttons[i].setSprite(
                    awe::getEditMapEnvironmentStructureIconSpritesheet(),
                    selectedStructure.owner.isEmpty() ?
                    structure[scriptName].iconName :
                    structure[scriptName].ownedIconName(selectedStructure.owner)
                );
            }
        }
    }

    /**
     * When a structure button is pressed, select it.
     */
    private void objectButtonSignalHandler(const WidgetID button,
        const string&in signal) override {
        if (signal != Clicked) return;
        @selectedStructure.type = structure[::getWidgetName(button)];
        toolBar.selectPaint();
    }

    /**
     * When a new owner is selected, update it in \c selectedStructure.
     */
    private void countryComboboxCallback(const ArmyID owner) override {
        if (owner == NO_ARMY) selectedStructure.owner = "";
        else selectedStructure.owner = country.scriptNames[owner];
        toolBar.selectPaint();
    }

    /**
     * When the destroyed checkbox is checked or unchecked, update the flag in
     * \c selectedStructure.
     */
    private void destroyedChanged() {
        selectedStructure.destroyed = destroyed.getChecked();
        toolBar.selectPaint();
    }

    /**
     * Filters structures based on whether they are paintable or not.
     * @return A list of script names for structures that are paintable.
     */
    private array<string>@ generateScriptNames() const {
        array<string>@ names = array<string>();
        for (uint64 i = 0, len = structure.scriptNames.length(); i < len; ++i) {
            const auto name = structure.scriptNames[i];
            if (structure[name].isPaintable) names.insertLast(name);
        }
        return names;
    }

    /**
     * Handle to the tool bar widget.
     */
    private ToolBar@ toolBar;

    /**
     * The group containing the destroyed checkbox.
     */
    private Group destroyedGroup;

    /**
     * The destroyed checkbox.
     */
    private CheckBox destroyed;
}
