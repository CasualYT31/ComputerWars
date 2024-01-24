/**
 * @file ArmyWidget.as
 * Declares the army widget type.
 */

/**
 * The different alignment options available for army widgets.
 */
enum ArmyWidgetAlignment {
    Left,
    Right
}

/**
 * Defines constants that the \c ArmyWidget class uses internally.
 */
namespace ArmyWidgetConstants {
    /**
     * Stores the width of an ArmyWidget.
     */
    const float Width = 250.0;

    /**
     * Stores the width of an ArmyWidget as a string.
     */
    const string WidthStr = formatFloat(Width);

    /**
     * Stores the height of an ArmyWidget.
     */
    const float Height = 60.0;

    /**
     * Stores the height of an ArmyWidget as a string.
     */
    const string HeightStr = formatFloat(Height);

    /**
     * Stores the radius of an ArmyWidget's border.
     */
    const float Radius = Height / 2.0;

    /**
     * Stores the radius of an ArmyWidget's border as a string.
     */
    const string RadiusStr = formatFloat(Radius);

    /**
     * Stores the size of an ArmyWidget's border.
     */
    const float BorderSize = 2.0;

    /**
     * Stores the size of an ArmyWidget's border as a string.
     */
    const string BorderSizeStr = formatFloat(BorderSize);

    /**
     * Stores the text size of the team ID label.
     */
    const uint TeamIDTextSize = 24;

    /**
     * Stores the text size of the funds label.
     */
    const uint FundsTextSize = 14;

    /**
     * Stores the X offset of the team ID label.
     */
    const string XOffsetTeamID = "3px";
}

/**
 * Used to create the widgets that represent an army panel in-game.
 */
class ArmyWidget : Panel {
    /**
     * Constructs an army widget.
     * @param transparency The transparency of the panel's background.
     */
    ArmyWidget(const uint8 transparency = 255) {
        alpha = transparency;
        
        // Setup the base panel.
        setSize(ArmyWidgetConstants::WidthStr, ArmyWidgetConstants::HeightStr);
        setBorderSize(ArmyWidgetConstants::BorderSize);
        setBorderRadius(ArmyWidgetConstants::Radius);
        setBorderColour(Black);

        // Setup the team ID label.
        team.setTextSize(ArmyWidgetConstants::TeamIDTextSize);
        team.setTextColour(White);
        team.setTextOutlineColour(Black);
        team.setTextOutlineThickness(2.0);
        add(team);

        // Setup the CO pictures.
        add(currentCO);
        add(tagCO);

        // Setup the funds label.
        funds.setTextSize(ArmyWidgetConstants::FundsTextSize);
        funds.setTextColour(White);
        funds.setTextOutlineColour(Black);
        funds.setTextOutlineThickness(2.0);
        add(funds);

        setAlignment(ArmyWidgetAlignment::Left);
    }

    /**
     * Configures an army widget to be either left-aligned or right-aligned.
     * This method will reposition everything inside the panel, but the panel
     * itself will not be repositioned.
     * @param alignment The alignment. If an invalid value is given, an error will
     *                  be logged and the widget will not be changed.
     */
    void setAlignment(const ArmyWidgetAlignment alignment) {
        const float coX = ArmyWidgetConstants::Radius + 5;
        switch (alignment) {
        case ArmyWidgetAlignment::Left:
            team.setOrigin(0.0f, 0.5f);
            team.setPosition(ArmyWidgetConstants::XOffsetTeamID,
                ArmyWidgetConstants::RadiusStr);
            currentCO.setOrigin(0.0f, 0.0f);
            currentCO.setPosition(formatFloat(coX), "5");
            tagCO.setOrigin(0.0f, 0.0f);
            tagCO.setPosition(formatFloat(coX), "30");
            funds.setOrigin(0.0f, 0.0f);
            funds.setPosition("140", "5");
            break;
        case ArmyWidgetAlignment::Right:
            team.setOrigin(1.0f, 0.5f);
            team.setPosition("100%-" + ArmyWidgetConstants::XOffsetTeamID,
                ArmyWidgetConstants::RadiusStr);
            currentCO.setOrigin(1.0f, 0.0f);
            currentCO.setPosition("100%-" + formatFloat(coX), "5");
            tagCO.setOrigin(1.0f, 0.0f);
            tagCO.setPosition("100%-" + formatFloat(coX), "30");
            funds.setOrigin(1.0f, 0.0f);
            funds.setPosition("100%-140", "5");
            break;
        default:
            error("Attempted to set the ArmyWidget \"" + ID + "\"'s alignment to "
                + alignment + ", which is invalid.");
        }
    }

    /**
     * Configures an army widget to display information on a given army.
     * @param armyID The ID of the army on the current map to display information
     *               on.
     */
    void update(const ArmyID armyID) {
        // If the army doesn't exist, update with default values.
        if (game.map.isArmyPresent(armyID)) {
            Colour colour = game.map.getArmyCountry(armyID).colour;
            colour.a = alpha;
            setBackgroundColour(colour);

            TeamID teamID = game.map.getArmyTeam(armyID);
            team.setText("~" + formatUInt(teamID));

            string currentCommanderStr = game.map.getArmyCurrentCO(armyID);
            if (currentCommanderStr.isEmpty()) {
                currentCO.setVisibility(false);
            } else {
                const auto currentCommander = commander[currentCommanderStr];
                currentCO.setSprite("co", currentCommander.iconName);
                currentCO.setVisibility(true);
            }

            if (game.map.tagCOIsPresent(armyID)) {
                const auto tagCommander =
                    commander[game.map.getArmyTagCO(armyID)];
                tagCO.setSprite("co", tagCommander.iconName);
                tagCO.setVisibility(true);
            } else {
                tagCO.setVisibility(false);
            }

            const auto currentArmy = game.map.getOverriddenSelectedArmy();
            if (game.map.isFoWEnabled() && (currentArmy == NO_ARMY ||
                game.map.getArmyTeam(currentArmy) != teamID))
                funds.setText("price", { any("?????") });
            else funds.setText("price", { any(game.map.getArmyFunds(armyID)) });
        } else {
            setBackgroundColour(Colour(255, 255, 255, alpha));
            team.setText("~");
            currentCO.setVisibility(false);
            tagCO.setVisibility(false);
            funds.setText("~");
        }
    }

    /**
     * Display's the army's team ID.
     */
    private Label team;

    /**
     * Display's the army's funds.
     */
    private Label funds;

    /**
     * Display's the army's current CO.
     */
    private Picture currentCO;

    /**
     * Display's the army's tag CO.
     */
    private Picture tagCO;

    /**
     * The alpha channel that's applied to the background colour of the panel.
     */
    private uint8 alpha;
}
