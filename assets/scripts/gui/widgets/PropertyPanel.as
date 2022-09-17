/**
 * @file PropertyPanel.as
 * Declares the property panel widget type.
 * A property panel is made up of an icon, a name, and a grid of a few properties.
 */

/**
 * Stores the size of a property panel.
 */
const Vector2 PROPERTY_PANEL_SIZE(60, 100);

/**
 * Stores the width of a property panel as a pixel string.
 */
const string PROPERTY_PANEL_WIDTH = formatUInt(PROPERTY_PANEL_SIZE.x) + "px";

/**
 * Stores the height of a property panel as a pixel string.
 */
const string PROPERTY_PANEL_HEIGHT = formatUInt(PROPERTY_PANEL_SIZE.y) + "px";

/**
 * Used to create the widgets that represent a property panel in-game.
 */
class PropertyPanel {
	/**
	 * Name of the panel's base layout widget.
	 */
	string layout;
	
	/**
	 * Doesn't create any widgets.
	 */
	PropertyPanel() {}

	/**
	 * Creates the widgets that represent a property panel.
	 * @param panelName The full name of the panel which will contain all of the
	 *                  other widgets.
	 */
	PropertyPanel(const string&in layoutName)
		explicit {
		addWidget("Panel", layoutName);
		setWidgetBackgroundColour(layoutName, Colour(255,255,255,128));
		setWidgetBorderSize(layoutName, 0.0);
		layout = layoutName + ".layout";
		addWidget("VerticalLayout", layout);
		addWidget("Panel", layout + ".iconPanel");
		setWidgetBackgroundColour(layout + ".iconPanel", NO_COLOUR);
		addWidget("Picture", layout + ".iconPanel.icon");
		setWidgetOrigin(layout + ".iconPanel.icon", 0.5, 0.5);
		setWidgetPosition(layout + ".iconPanel.icon", "50%", "50%");
		addWidget("Panel", layout + ".namePanel");
		setWidgetBackgroundColour(layout + ".namePanel", NO_COLOUR);
		addWidget("Label", layout + ".namePanel.name");
		_configureLabel(layout + ".namePanel.name", 16);
		setWidgetOrigin(layout + ".namePanel.name", 0.5, 0.5);
		setWidgetPosition(layout + ".namePanel.name", "50%", "50%");
		addWidget("Grid", layout + ".properties");
		setWidgetRatioInLayout(layout, 0, 72.5);
		setWidgetRatioInLayout(layout, 1, 30.0);
		setWidgetRatioInLayout(layout, 2, 87.5);
	}

	/**
	 * Updates the panel's icon.
	 */
	void setIcon(const string&in sheet, const string&in sprite) {
		setWidgetSprite(layout + ".iconPanel.icon", sheet, sprite);
	}

	/**
	 * Updates the panel's name label.
	 */
	void setName(const string&in newName) {
		setWidgetText(layout + ".namePanel.name", newName);
	}

	/**
	 * Updates a property's icon.
	 */
	void setPropertyIcon(const uint index, const string&in sheet,
		const string&in sprite) {
		_ensurePropertyCreation(index);
		setWidgetSprite(layout + ".properties.icon" + formatUInt(index),
			sheet, sprite);
	}

	/**
	 * Updates a property's text.
	 */
	void setPropertyText(const uint index, const string&in text) {
		const string i = formatUInt(index);
		_ensurePropertyCreation(index);
		setWidgetText(layout + ".properties.label" + formatUInt(index), text);
	}

	/**
	 * Shows or hides a property.
	 */
	void setPropertyVisibility(const uint index, const bool visible) {
		_ensurePropertyCreation(index);
		setWidgetVisibility(
			layout + ".properties.icon" + formatUInt(index), visible);
		setWidgetVisibility(
			layout + ".properties.label" + formatUInt(index), visible);
	}

	/**
	 * Sets a label's properties.
	 */
	void _configureLabel(const string&in name, const uint size) {
		setWidgetTextSize(name, size);
		setWidgetTextColour(name, Colour(255,255,255,255));
		setWidgetTextOutlineColour(name, Colour(0,0,0,255));
		setWidgetTextOutlineThickness(name, 1.5);
	}

	void _ensurePropertyCreation(const uint index) {
		const string i = formatUInt(index);
		if (!widgetExists(layout + ".properties.icon" + i)) {
			addWidgetToGrid("Picture", layout + ".properties.icon" + i, index, 0);
			setWidgetAlignmentInGrid(layout + ".properties", index, 0,
				WidgetAlignment::Right);
		}
		if (!widgetExists(layout + ".properties.label" + i)) {
			addWidgetToGrid("Label", layout + ".properties.label" + i, index, 1);
			_configureLabel(layout + ".properties.label" + i, 12);
			setWidgetAlignmentInGrid(layout + ".properties", index, 1,
				WidgetAlignment::Left);
		}
	}
}