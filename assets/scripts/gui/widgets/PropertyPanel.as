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
		setWidgetSize(layout, PROPERTY_PANEL_WIDTH, PROPERTY_PANEL_HEIGHT);
		addWidget("Panel", layout + ".iconPanel");
		setWidgetBackgroundColour(layout + ".iconPanel", Colour(0,0,0,0));
		addWidget("Picture", layout + ".iconPanel.icon");
		setWidgetOrigin(layout + ".iconPanel.icon", 0.5, 0.5);
		setWidgetPosition(layout + ".iconPanel.icon", "50%", "50%");
		addWidget("Panel", layout + ".namePanel");
		setWidgetBackgroundColour(layout + ".namePanel", Colour(0,0,0,0));
		addWidget("Label", layout + ".namePanel.name");
		setWidgetOrigin(layout + ".namePanel.name", 0.5, 0.5);
		setWidgetPosition(layout + ".namePanel.name", "50%", "50%");
		addWidget("Grid", layout + ".properties");
		setWidgetRatioInLayout(layout, 0, 92.5);
		setWidgetRatioInLayout(layout, 1, 5.0);
		setWidgetRatioInLayout(layout, 2, 92.5);
	}

	/**
	 * Removes the widgets from the menu.
	 */
	void remove() {
		removeWidget(layout);
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
	 * Updates a property.
	 */
	void setProperty(const uint index, const string&in text,
		const string&in sheet, const string&in sprite) {
		const string i = formatUInt(index);
		if (!widgetExists(layout + ".properties.icon" + i)) {
			addWidgetToGrid("Picture", layout + ".properties.icon" + i, index, 0);
			setWidgetAlignmentInGrid(layout + ".properties", index, 0,
				WidgetAlignment::Right);
			addWidgetToGrid("Label", layout + ".properties.label" + i, index, 1);
			setWidgetAlignmentInGrid(layout + ".properties", index, 1,
				WidgetAlignment::Left);
		}
		setWidgetSprite(layout + ".properties.icon" + i, sheet, sprite);
		setWidgetText(layout + ".properties.label" + i, text);
	}

	/**
	 * Name of the panel's base layout widget.
	 */
	string layout;
}