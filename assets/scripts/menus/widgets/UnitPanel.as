/**
 * @file UnitPanel.as
 * Holds code that manages a \c BaseMenu panel.
 */

/**
 * Defines constants that the \c UnitPanel class uses internally.
 */
namespace UnitPanelConstants {
    /**
     * Stores the height of a unit panel button.
     */
    const uint ButtonHeight = 30;

    /**
     * Stores the height of a unit panel button as a string.
     */
    const string ButtonHeightStr = formatUInt(ButtonHeight);

    /**
     * Stores the button text size.
    */
    const uint ButtonTextSize = 12;
}

/**
 * Holds a list of units that can be selected.
 */
class UnitPanel : ScrollablePanel {
    /**
     * Sets up a unit panel with a list of units that have the given movement
     * types.
     * @param movementTypeNames If a unit has one of the movement types, it will
     *                          be included in the panel.
     * @param colCount          The number of columns this panel will have.
     * @param handler           The \c MultiSignalHandler to assign to every
     *                          button.
     */
    UnitPanel(const array<string>@ const movementTypeNames, const uint colCount,
        MultiSignalHandler@ const handler) {
        // Setup the root scrollable panel.
        setVisibility(false);
        setHorizontalScrollbarPolicy(ScrollbarPolicy::Never);

        // Find the unit types that pass the movement types filter.
        const auto unitTypeScriptNames = unittype.scriptNames;
        const uint unitTypeCount = unitTypeScriptNames.length();
        for (uint i = 0; i < unitTypeCount; ++i) {
            const auto type = unittype[unitTypeScriptNames[i]];
            if (movementTypeNames.find(type.movementType.scriptName) >= 0)
                unitsMatchingMovement.insertLast(unitTypeScriptNames[i]);
        }
        const auto unitsMatchingMovementLength = unitsMatchingMovement.length();

        // Create unit type buttons.
        unitButtons.resize(colCount);
        for (uint i = 0; i < colCount; ++i)
            unitButtons[i].resize(uint(unitsMatchingMovementLength / colCount) +
                (i < unitsMatchingMovementLength % colCount ? 1 : 0));
        
        // Setup unit type buttons.
        for (uint i = 0, len = unitsMatchingMovement.length(); i < len; ++i) {
            const auto type = unittype[unitsMatchingMovement[i]];
            BitmapButton@ button = unitButtons[i % colCount][uint(i / colCount)];
            button.setName(type.scriptName);
            button.setTextSize(UnitPanelConstants::ButtonTextSize);
            button.setSize(formatFloat(100.0 / double(colCount)) + "%",
                UnitPanelConstants::ButtonHeightStr);
            button.setPosition(
                formatFloat(100 / double(colCount) * (i % colCount)) + "%",
                formatUInt(uint(i / colCount) * UnitPanelConstants::ButtonHeight)
            );
            button.setText("~" + translate(type.name) + " (" +
                translate("price", { any(type.cost) }) + ")");
            button.connect(handler);
            add(button);
        }

        // Setup directional flow.
        for (uint col = 0, cols = unitButtons.length(); col < cols; ++col) {
            for (uint row = 0, rows = unitButtons[col].length(); row < rows;
                ++row) {
                const auto@ up = row == 0 ? @unitButtons[col][rows - 1] :
                    @unitButtons[col][row - 1];
                const auto@ down = row == rows - 1 ? @unitButtons[col][0] :
                    @unitButtons[col][row + 1];
                BitmapButton@ right = null;
                // If there isn't a unit to the immediate right, pick the unit
                // that is on the row above, if possible.
                if (col < cols - 1) {
                    if (unitButtons[col + 1].length() <= row) {
                        if (row > 0) {
                            @right = @unitButtons[col + 1][row - 1];
                        }
                    }
                }
                if (right is null) {
                    @right = col == cols - 1 ? @unitButtons[0][row] :
                        @unitButtons[col + 1][row];
                }
                BitmapButton@ left = null;
                // If there isn't a unit to the immediate left, pick the unit that
                // is on the row above, if possible.
                if (col == 0) {
                    if (unitButtons[cols - 1].length() <= row) {
                        if (row > 0) {
                            @left = @unitButtons[cols - 1][row - 1];
                        }
                    }
                }
                if (left is null) {
                    @left = col == 0 ? @unitButtons[cols - 1][row] :
                        @unitButtons[col - 1][row];
                }
                unitButtons[col][row].setDirectionalFlow(up, down, left, right);
            }
        }
    }

    /**
     * Opens the panel, and updates the unit sprites with the current army's
     * country.
     * @return Handle to the first button in the panel, if there is one.
     */
    Widget@ open() {
        const auto country =
            game.map.getArmyCountry(game.map.getSelectedArmy()).scriptName;
        for (uint col = 0, cols = unitButtons.length(); col < cols; ++col) {
            for (uint row = 0, rows = unitButtons[col].length(); row < rows;
                ++row) {
                const auto type = unittype[unitButtons[col][row].getName()];
                unitButtons[col][row].setSprite("unit", type.unitSprite(country));
            }
        }
        setVisibility(true);
        if (unitButtons.length() == 0 || unitButtons[0].length() == 0)
            return null;
        return @unitButtons[0][0];
    }

    /**
     * The list of units that are on this panel.
     */
    private array<array<BitmapButton>> unitButtons;

    /**
     * Caches a list of unit types that match the movement types filter given at
     * construction.
     */
    private array<string> unitsMatchingMovement;

    /**
     * Cache of the length of \c unitsMatchingMovement;
     */
    private uint unitsMatchingMovementLength;
}
