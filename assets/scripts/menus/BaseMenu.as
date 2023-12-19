/**
 * @file BaseMenu.as
 * The menu that allows the current army to deploy units.
 */

/**
 * The unit production menu.
 */
class BaseMenu : Menu, Group {
    /**
     * Set up the menu.
     */
    BaseMenu() {
        // Setup the unit panels.
        setUpUnitPanel(ground);
        setUpUnitPanel(air);
        setUpUnitPanel(naval);

        // Setup the calc panel.
        calc.setOrigin(0.5, 1.0);
        calc.setPosition("50%", "UnitPanel.top");
        calc.setSize("60%", "30");
        add(calc);

        // Setup the calc label.
        label.setSize("100%", "100%");
        label.setTextSize(16);
        label.setTextColour(White);
        label.setTextOutlineColour(Black);
        label.setTextOutlineThickness(1.5);
        label.setTextAlignment(HorizontalAlignment::Centre,
            VerticalAlignment::Centre);
        calc.add(label);

        // Setup the root group widget.
        ::add(ROOT_WIDGET, this);
    }

    /**
     * Invoked when the menu is opened.
     * @param oldMenu     Handle to the menu that was open before this one.
     * @param oldMenuName Name of the type of the \c Menu object.
     */
    void Open(Menu@ const oldMenu, const string&in oldMenuName) {
        ground.setVisibility(false);
        air.setVisibility(false);
        naval.setVisibility(false);
        const string terrain =
            game.map.getTileType(game.map.getSelectedTile()).type.scriptName;
        setVisibility(true);
        if (terrain == "BASE")
            setWidgetDirectionalFlowSelection("BaseMenu", ground.open());
        else if (terrain == "AIRPORT")
            setWidgetDirectionalFlowSelection("BaseMenu", air.open());
        else if (terrain == "PORT")
            setWidgetDirectionalFlowSelection("BaseMenu", naval.open());
    }

    /**
     * Invoked when the menu is closed.
     * @param newMenu     Handle to the menu that will be opened after this one.
     * @param newMenuName Name of the type of the \c Menu object.
     */
    void Close(Menu@ const newMenu, const string&in newMenuName) {
        setVisibility(false);
        label.setText("~");
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
        if (bool(ui["back"])) setGUI("GameScreen");
    }

    /**
     * Sets up a unit panel.
     * @param p Handle to the unit panel.
     */
    private void setUpUnitPanel(UnitPanel@ const p) {
        p.setSize("60%", "60%");
        p.setOrigin(0.5, 0.5);
        p.setPosition("50%", "50%");
        p.setName("UnitPanel");
        add(p);
    }

    /**
     * Handles all signals emitted by button widgets in every unit panel.
     * @param widget The ID of the button that emitted the signal.
     * @param signal The signal emitted. Use this to determine if a button was
     *               clicked or if the mouse entered a button.
     */
    private void unitButtonHandler(const WidgetID widget,
        const string&in signal) {
        const auto type = unittype[::getWidgetName(widget)];
        if (signal == MouseReleased) {
            const ArmyID army = game.map.getSelectedArmy();
            if (game.buyUnit(type, army, game.map.getSelectedTile()))
                setGUI("GameScreen");
        } else if (signal == MouseEntered) {
            const ArmyID army = game.map.getSelectedArmy();
            const Funds current = game.map.getArmyFunds(army);
            const Funds cost = type.cost;
            const Funds result = current - cost;
            label.setText("~" + translate("price", { any(current) }) + " - " +
                translate("price", { any(cost) }) + " = " +
                translate("price", { any(result) }));
        }
    }

    /**
     * The ground units panel.
     */
    private UnitPanel ground({ "TREAD", "TIRES", "INFANTRY", "MECH", "PIPELINE" },
        2, MultiSignalHandler(this.unitButtonHandler));

    /**
     * The air units panel.
     */
    private UnitPanel air({ "AIR" }, 1,
        MultiSignalHandler(this.unitButtonHandler));

    /**
     * The naval units panel.
     */
    private UnitPanel naval({ "SHIPS", "TRANSPORT" }, 1,
        MultiSignalHandler(this.unitButtonHandler));

    /**
     * The panel containing the funds calculation label.
     */
    private Panel calc;

    /**
     * The label containing the funds calculation.
     */
    private Label label;
}
