/**
 * @file ArmyPropertiesWindow.as
 * Defines code that manages the army properties child window.
 */

/**
 * Allows the user to view and edit the current map's armies.
 */
class ArmyPropertiesWindow : Observer, ChildWindow {
    /**
     * Sets up the army properties window.
     */
    ArmyPropertiesWindow() {
        close();
        setText("armyprops");

        // Setup the scrollable panel.
        panel.setPadding("5");
        panel.setVerticalScrollbarAmount(100);
        panel.setHorizontalScrollbarAmount(75);
        add(panel);

        // Setup each army layout.
        const auto armyCount = country.length();
        armies.resize(armyCount);
        for (uint64 i = 0, len = armies.length(); i < len; ++i) {
            @armies[i] = ArmyPropertiesPanel(country[country.scriptNames[i]],
                i + 1 == len);
            layout.add(armies[i]);
        }

        // Setup the base army layout.
        layout.setSize("900", formatUInt(80 * armyCount));
        panel.add(layout);
    }

    /**
     * Restore the army properties window.
     */
    void open() {
        maximise();
        refresh();
    }

    /**
     * We usually want to close this window immediately without emitting any
     * signal.
     */
    void close(const bool emitClosingSignal = false) override {
        ChildWindow::close(emitClosingSignal);
    }

    /**
     * Refreshes the army properties window.
     */
    void refresh(any&in data = any()) override {
        for (uint64 i = 0, l = armies.length(); i < l; ++i) armies[i].refresh();
    }

    /**
     * The base scrollable panel.
     */
    private ScrollablePanel panel;

    /**
     * The layout in which all the army layouts are contained.
     */
    private VerticalLayout layout;

    /**
     * Each of the army layouts.
     */
    private array<ArmyPropertiesPanel@> armies;
}

/**
 * Defines constants that the \c ArmyPropertiesPanel class uses internally.
 */
namespace ArmyPropertiesPanelConstants {
    /**
     * The height edit boxes and buttons and set to.
     */
    const string InputWidgetHeight = "20";
}

/**
 * A layout containing all of an army's properties.
 */
class ArmyPropertiesPanel : Panel {
    /**
     * Sets up the army properties layout.
     * @param c The country of the army to display information on.
     * @param lastPanel \c TRUE if this panel should not have a border on the
     *                  bottom side.
     */
    ArmyPropertiesPanel(const Country@ const c, const bool lastPanel) {
        @country = c;
        if (!lastPanel) {
            setBorderSizes("0", "0", "0", "1");
            setBorderColour(Black);
        }

        // Country information.
        countryIcon.setSprite("icon", country.iconName);
        countryIcon.setOrigin(1.0, 0.5);
        countryIcon.setPosition("100%", "50%");
        countryIconGroup.add(countryIcon);
        setupLabel(@countryNameGroup, @countryName, 0.0, 0.5, "0%", "50%",
            country.name);
        
        // Team information.
        setupLabel(@teamLabelGroup, @teamLabel, 1.0, 0.5, "100%", "50%", "team");
        setupEditBox(@teamGroup, @team);

        // Funds information.
        setupLabel(@fundsLabelGroup, @fundsLabel, 1.0, 0.5, "100%", "50%",
            "funds");
        setupEditBox(@fundsGroup, @funds);

        // Current Army? radiobutton.
        currentArmy.setText("currentarmy");
        currentArmy.setOrigin(0.0, 0.5);
        currentArmy.setPosition("0%", "50%");
        currentArmyGroup.add(currentArmy);

        // Current CO information.
        setupLabel(@currentCOLabelGroup, @currentCOLabel, 1.0, 0.5, "100%", "50%",
            "currentco");
        setupComboBox(@currentCOGroup, @currentCO, false);
        currentCOIcon.setOrigin(0.0, 0.5);
        currentCOIcon.setPosition("0%", "50%");
        currentCOIconGroup.add(currentCOIcon);
        
        // Tag CO information.
        setupLabel(@tagCOLabelGroup, @tagCOLabel, 1.0, 0.5, "100%", "50%",
            "tagco");
        setupComboBox(@tagCOGroup, @tagCO, true);
        tagCOIcon.setOrigin(0.0, 0.5);
        tagCOIcon.setPosition("0%", "50%");
        tagCOIconGroup.add(tagCOIcon);
        
        // Action button.
        actionButton.setSize("100%",
            ArmyPropertiesPanelConstants::InputWidgetHeight);
        actionButton.setOrigin(0.5, 0.5);
        actionButton.setPosition("50%", "50%");
        actionButton.connect(MouseReleased,
            SingleSignalHandler(this.actionTriggered));
        actionButtonGroup.add(actionButton);

        // Setup the top, bottom, and base layouts.
        top.setSpaceBetweenWidgets(5.0);
        top.add(countryIconGroup);
        top.add(countryNameGroup);
        top.add(teamLabelGroup);
        top.add(teamGroup);
        top.add(fundsLabelGroup);
        top.add(fundsGroup);
        top.add(currentArmyGroup);
        base.add(top);
        bottom.setSpaceBetweenWidgets(5.0);
        bottom.add(currentCOLabelGroup);
        bottom.add(currentCOGroup);
        bottom.add(currentCOIconGroup);
        bottom.add(tagCOLabelGroup);
        bottom.add(tagCOGroup);
        bottom.add(tagCOIconGroup);
        bottom.add(actionButtonGroup);
        base.add(bottom);
        add(base);
        updateMainControls(false);
        connectSignalHandlers();
    }

    /**
     * Refreshes the layout based on the currently open map.
     */
    void refresh() {
        const auto army = country.turnOrder;
        const bool present = edit.map.isArmyPresent(army);
        updateMainControls(present);
        if (!present) return;
        disconnectSignalHandlers();
        team.setText(formatTeamID(edit.map.getArmyTeam(army)));
        funds.setText(formatFunds(edit.map.getArmyFunds(army)));
        currentArmy.setChecked(army == edit.map.getSelectedArmy());
        const auto newCurrentCO =
            commander.scriptNames.find(edit.map.getArmyCurrentCO(army)),
            newTagCO = commander.scriptNames.find(edit.map.getArmyTagCO(army));
        if (newCurrentCO < 0) {
            currentCO.deselectItem();
            currentCOIcon.setSprite("", "");
        } else {
            currentCO.setSelectedItem(uint(newCurrentCO));
            currentCOIcon.setSprite("co",
                commander[commander.scriptNames[uint(newCurrentCO)]].iconName);
        }
        if (newTagCO < 0) {
            tagCO.setSelectedItem(0);
            tagCOIcon.setSprite("", "");
        } else {
            tagCO.setSelectedItem(uint(newTagCO) + 1);
            tagCOIcon.setSprite("co",
                commander[commander.scriptNames[uint(newTagCO)]].iconName);
        }
        connectSignalHandlers();
    }

    /**
     * Connects signal handlers to widgets that will cause the \c edit object's
     * state to be updated.
     */
    private void connectSignalHandlers() {
        team.connect(ReturnOrUnfocused, SingleSignalHandler(this.teamChanged));
        funds.connect(ReturnOrUnfocused, SingleSignalHandler(this.fundsChanged));
        currentArmy.connect(Checked, SingleSignalHandler(this.currentArmyIsSet));
        currentCO.connect(ItemSelected, SingleSignalHandler(this.newCurrentCO));
        tagCO.connect(ItemSelected, SingleSignalHandler(this.newTagCO));
    }

    /**
     * Disconnects widget signal handlers that will cause the \c edit object's
     * state to be updated.
     * Used to prevent updates to \c edit whilst refreshing.
     */
    private void disconnectSignalHandlers() {
        team.disconnectAll();
        funds.disconnectAll();
        currentArmy.disconnectAll();
        currentCO.disconnectAll();
        tagCO.disconnectAll();
    }

    /**
     * Shows or hides every widget, except for the country icon, country name, and
     * the action button.
     * The action button's text is also updated to either "Create Army" or "Delete
     * Army" depending on if the widgets are hidden or not (respectively).
     */
    private void updateMainControls(const bool show) {
        teamLabelGroup.setVisibility(show);
        teamGroup.setVisibility(show);
        fundsLabelGroup.setVisibility(show);
        fundsGroup.setVisibility(show);
        currentArmyGroup.setVisibility(show);
        currentCOLabelGroup.setVisibility(show);
        currentCOGroup.setVisibility(show);
        currentCOIconGroup.setVisibility(show);
        tagCOLabelGroup.setVisibility(show);
        tagCOGroup.setVisibility(show);
        tagCOIconGroup.setVisibility(show);
        if (show) actionButton.setText("deletearmy");
        else actionButton.setText("createarmy");
    }

    /**
     * Sets up a label.
     * @param group Points to the group to add the label to.
     * @param label Points to the label to set up.
     * @param oX    Origin X of the label.
     * @param oY    Origin Y of the label.
     * @param x     X position of the label.
     * @param y     Y position of the label.
     * @param c     The caption to apply to the label.
     */
    private void setupLabel(Group@ const group, Label@ const label,
        const float oX, const float oY, const string&in x, const string&in y,
        const string&in c) {
        label.setOrigin(oX, oY);
        label.setPosition(x, y);
        label.setText(c);
        group.add(label);
    }

    /**
     * Sets up an edit box.
     * @param group   Points to the group to add the edit box to.
     * @param editbox Points to the edit box to set up.
     */
    private void setupEditBox(Group@ const group, EditBox@ const editbox) {
        editbox.setValidator(VALIDATOR_UINT);
        editbox.setSize("100%", ArmyPropertiesPanelConstants::InputWidgetHeight);
        editbox.setOrigin(0.0, 0.5);
        editbox.setPosition("0%", "50%");
        group.add(editbox);
    }

    /**
     * Sets up a CO combobox.
     * @param group       The group to add the combobox to.
     * @param combobox    The combobox to add COs to.
     * @param includeNoCO If \c TRUE, a "-None-" option is included as the first
     *                    item. Does nothing extra if \c FALSE.
     */
    private void setupComboBox(Group@ const group, ComboBox@ const combobox,
        const bool includeNoCO) {
        if (includeNoCO) combobox.addItem("noco");
        const auto coScriptNames = commander.scriptNames;
        for (uint c = 0, len = coScriptNames.length(); c < len; ++c)
            combobox.addItem(commander[coScriptNames[c]].name);
        combobox.setItemsToDisplay(5);
        combobox.setSize("100%", ArmyPropertiesPanelConstants::InputWidgetHeight);
        combobox.setOrigin(0.5, 0.5);
        combobox.setPosition("50%", "50%");
        group.add(combobox);
    }

    /**
     * Invoked when the user has changed the team of this army.
     */
    private void teamChanged() {
        edit.setArmyTeam(country.turnOrder, parseTeamID(team.getText()));
    }

    /**
     * Invoked when the user has changed the funds belonging to this army.
     */
    private void fundsChanged() {
        edit.setArmyFunds(country.turnOrder, parseFunds(funds.getText()));
    }

    /**
     * Invoked when the user has set this army to be the current army.
     */
    private void currentArmyIsSet() {
        edit.setSelectedArmy(country.turnOrder);
    }

    /**
     * Invoked when the user has selected a new "current CO" for this army.
     */
    private void newCurrentCO() {
        const auto item = currentCO.getSelectedItem();
        if (item < 0) edit.setArmyCurrentCO(country.turnOrder, "");
        else edit.setArmyCurrentCO(country.turnOrder,
            commander.scriptNames[uint64(item)]);
    }

    /**
     * Invoked when the user has selected a new "tag CO" for this army.
     */
    private void newTagCO() {
        const auto item = tagCO.getSelectedItem();
        if (item <= 0) edit.setArmyTagCO(country.turnOrder, "");
        else edit.setArmyTagCO(country.turnOrder,
            commander.scriptNames[uint64(item) - 1]);
    }

    /**
     * When this army is to be either created or deleted.
     */
    private void actionTriggered() {
        const auto id = country.turnOrder;
        if (!edit.map.isArmyPresent(id)) edit.createArmy(id);
        else edit.deleteArmy(id);
    }

    /**
     * Points to the country this army fights for.
     */
    private const Country@ country;

    /**
     * The base layout.
     */
    private VerticalLayout base;

    /**
     * The top row of widgets.
     */
    private HorizontalLayout top;

    /**
     * The group containing the country icon.
     */
    private Group countryIconGroup;

    /**
     * The country icon.
     */
    private Picture countryIcon;

    /**
     * The group containing the country name.
     */
    private Group countryNameGroup;

    /**
     * The country name.
     */
    private Label countryName;

    /**
     * The group containing the team label.
     */
    private Group teamLabelGroup;

    /**
     * The team label.
     */
    private Label teamLabel;

    /**
     * The group containing the team edit box.
     */
    private Group teamGroup;

    /**
     * The team edit box.
     */
    private EditBox team;

    /**
     * The group containing the funds label.
     */
    private Group fundsLabelGroup;

    /**
     * The funds label.
     */
    private Label fundsLabel;

    /**
     * The group containing the funds edit box.
     */
    private Group fundsGroup;

    /**
     * The funds edit box.
     */
    private EditBox funds;

    /**
     * The group containing the "Current Army?" radiobutton.
     */
    private Group currentArmyGroup;

    /**
     * The "Current Army?" radiobutton.
     */
    private RadioButton currentArmy;
    
    /**
     * The bottom row of widgets.
     */
    private HorizontalLayout bottom;

    /**
     * The group containing the "current CO" label.
     */
    private Group currentCOLabelGroup;

    /**
     * The "current CO" label.
     */
    private Label currentCOLabel;

    /**
     * The group containing the "current CO" combobox.
     */
    private Group currentCOGroup;

    /**
     * The "current CO" combobox.
     */
    private ComboBox currentCO;

    /**
     * The group containing the "current CO" icon.
     */
    private Group currentCOIconGroup;

    /**
     * The "current CO" icon.
     */
    private Picture currentCOIcon;

    /**
     * The group containing the "tag CO" label.
     */
    private Group tagCOLabelGroup;

    /**
     * The "tag CO" label.
     */
    private Label tagCOLabel;

    /**
     * The group containing the "tag CO" combobox.
     */
    private Group tagCOGroup;

    /**
     * The "tag CO" combobox.
     */
    private ComboBox tagCO;

    /**
     * The group containing the "tag CO" icon.
     */
    private Group tagCOIconGroup;

    /**
     * The "tag CO" icon.
     */
    private Picture tagCOIcon;

    /**
     * The group containing the action button.
     */
    private Group actionButtonGroup;

    /**
     * Either "Create Army" or "Delete Army," depending on if the army exists or
     * not.
     */
    private Button actionButton;
}
