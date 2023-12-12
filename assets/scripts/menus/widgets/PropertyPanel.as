/**
 * @file PropertyPanel.as
 * Declares the property panel widget type.
 * A property panel is made up of an icon, a name, and a grid of a few properties.
 */

/**
 * Defines constants that the \c PropertyPanel class uses internally.
 */
namespace PropertyPanelConstants {
    /**
     * Stores the width of a property panel.
     */
    const uint Width = 60;

    /**
     * Stores the width of a property panel as a string.
     */
    const string WidthStr = formatUInt(Width);

    /**
     * Stores the height of a property panel.
     */
    const uint Height = 100;

    /**
     * Stores the height of a property panel as a string.
     */
    const string HeightStr = formatUInt(Height);

    /**
     * Stores the text size of the text in the \c namePanel.
     */
    const uint NameTextSize = 10;

    /**
     * Stores the text size of properties.
     */
    const uint PropertyTextSize = 8;
}

/**
 * The widgets that make up a single property.
 * We store handles to prevent creating many widgets if a non-zero index is given
 * for the first property. See \c PropertyPanel::ensurePropertyIsCreated.
 */
class PropertyWidgets {
    /// The icon of the property.
    Picture@ icon;

    /// The label of the property.
    Label@ label;
}

/**
 * Used to create the widgets that represent a property panel in-game.
 */
class PropertyPanel : Panel {
    /**
     * Creates the widgets that represent a property panel.
     */
    PropertyPanel() {
        // Setup the icon panel.
        iconPanel.setBackgroundColour(Transparent);
        baseLayout.add(iconPanel);

        // Setup the icon.
        icon.setOrigin(0.5, 0.5);
        icon.setPosition("50%", "50%");
        iconPanel.add(icon);

        // Setup the name panel.
        namePanel.setBackgroundColour(Transparent);
        baseLayout.add(namePanel);

        // Setup the name.
        name.setOrigin(0.5, 0.5);
        name.setPosition("50%", "50%");
        configureLabel(name, PropertyPanelConstants::NameTextSize);
        namePanel.add(name);

        // Setup the property grid.
        baseLayout.add(grid);

        // Setup the base layout.
        baseLayout.setRatioOfWidget(0, 72.5);
        baseLayout.setRatioOfWidget(1, 30.0);
        baseLayout.setRatioOfWidget(2, 87.5);

        // Setup the base panel.
        setBackgroundColour(Colour(255, 255, 255, 128));
        setBorderSize(0.0);
        add(baseLayout);
    }

    /**
     * Updates the panel's icon.
     * @param sheet  The spritesheet to pull the sprite from.
     * @param sprite The key of the sprite to assign.
     */
    void setIcon(const string&in sheet, const string&in sprite) {
        icon.setSprite(sheet, sprite);
    }

    /**
     * Updates the panel's name label.
     * @param The new name to assign to the property panel.
     */
    void setName(const string&in newName) {
        name.setText(newName);
    }

    /**
     * Updates a property's icon.
     * @param index  The 0-based index of the property to update.
     * @param sheet  The spritesheet to pull the sprite from.
     * @param sprite The key of the sprite to apply.
     */
    void setPropertyIcon(const uint index, const string&in sheet,
        const string&in sprite) {
        ensurePropertyIsCreated(index);
        properties[index].icon.setSprite(sheet, sprite);
    }

    /**
     * Updates a property's text.
     * @param index The 0-based index of the property to update.
     * @param text  The text to apply to the property's label.
     */
    void setPropertyText(const uint index, const string&in text) {
        ensurePropertyIsCreated(index);
        properties[index].label.setText(text);
    }

    /**
     * Shows or hides a property.
     * @param index   The 0-based index of the property to update.
     * @param visible \c TRUE to make the property visible, \c FALSE to hide it.
     */
    void setPropertyVisibility(const uint index, const bool visible) {
        ensurePropertyIsCreated(index);
        properties[index].icon.setVisibility(visible);
        properties[index].label.setVisibility(visible);
    }

    /**
     * Sets a label's properties.
     * @param label Handle to the label to configure.
     * @param size  The text size to apply to the label.
     */
    private void configureLabel(Label@ const label, const uint size) {
        label.setTextSize(size);
        label.setTextColour(White);
        label.setTextOutlineColour(Black);
        label.setTextOutlineThickness(1.5);
    }

    /**
     * Ensures that a property panel's widgets exist.
     * @param index The index of the panel to update.
     */
    private void ensurePropertyIsCreated(const uint index) {
        if (properties.length() <= index) properties.resize(index + 1);
        if (properties[index].icon is null) {
            @properties[index].icon = Picture();
            grid.add(properties[index].icon, index, 0);
            grid.setAlignmentOfWidget(index, 0, WidgetAlignment::Right);
        }
        if (properties[index].label is null) {
            @properties[index].label = Label();
            grid.add(properties[index].label, index, 1);
            configureLabel(properties[index].label,
                PropertyPanelConstants::PropertyTextSize);
            grid.setAlignmentOfWidget(index, 1, WidgetAlignment::Left);
        }
    }

    /**
     * The layout in which property is placed.
     */
    private VerticalLayout baseLayout;

    /**
     * Contains the icon of the property panel.
     */
    private Panel iconPanel;

    /**
     * Stores the icon of the property panel.
     */
    private Picture icon;

    /**
     * Contains the name of the property panel.
     */
    private Panel namePanel;

    /**
     * Stores the name of the property panel.
     */
    private Label name;

    /**
     * Stores the properties.
     */
    private Grid grid;

    /**
     * The widgets of each property.
     */
    private array<PropertyWidgets> properties;
}