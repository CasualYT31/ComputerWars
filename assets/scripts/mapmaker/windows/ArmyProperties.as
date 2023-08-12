/**
 * @file ArmyProperties.as
 * Defines the GUI used to observe and amend armies.
 */
 
/**
 * Represents an \c ArmyLayout and all of its widgets.
 */
class ArmyLayout {
    /// @sa \c ArmyPropertiesWindow::ArmyPropertiesWindow().
    ArmyLayout(const string&in messageBoxName, const string&in disableThis,
        const string&in enableThis) {
        mbName = messageBoxName;
        mbDisableThis = disableThis;
        mbEnableThis = enableThis;
    }

    /**
     * Does this \c ArmyLayout exist?
     * @return \c FALSE if this \c ArmyLayout hasn't been added to the window
     *         yet. \c TRUE if it has.
     */
    bool exists() const { return !layout.isEmpty(); }

    /// Removes this \c ArmyLayout from the window.
    void remove() { removeWidget(layout); layout = ""; }

    /**
     * Reorders the \c ArmyLayout to the given index.
     * @param newIndex New index of the base layout.
     */
    void reorder(const uint64 newIndex) { setWidgetIndex(layout, newIndex); }

    /**
     * Creates and adds an \c ArmyLayout to the window.
     * @param parent The parent to add the \c ArmyLayout to.
     * @param id     Unique ID of this \c ArmyLayout.
     */
    void create(const string&in parent, const ArmyID id) {
        army = id; const auto countryType = country[country.scriptNames[id]];
        layout = parent + ".ArmyLayout" + formatArmyID(id);
        addWidget("VerticalLayout", layout);

        // Top row.
        const auto topLayout = layout + ".TopLayout";
        addWidget("HorizontalLayout", topLayout);
        setSpaceBetweenWidgets(topLayout, 5.0f);

        // Country icon.
        const auto iconGroup = topLayout + ".IconGroup";
        addWidget("Group", iconGroup);

        const auto icon = iconGroup + ".Icon";
        addWidget("Picture", icon);
        setWidgetSprite(icon, "icon", countryType.iconName);
        setWidgetOrigin(icon, 1.0f, 0.5f);
        setWidgetPosition(icon, "100%", "50%");

        // Country name.
        const auto nameGroup = topLayout + ".NameGroup";
        addWidget("Group", nameGroup);

        const auto name = nameGroup + ".Name";
        addWidget("Label", name);
        setWidgetText(name, countryType.name);
        setWidgetOrigin(name, 0.0f, 0.5f);
        setWidgetPosition(name, "0%", "50%");

        // Team.
        const auto teamLabelGroup = topLayout + ".TeamLabelGroup";
        addWidget("Group", teamLabelGroup);

        const auto teamLabel = teamLabelGroup + ".TeamLabel";
        addWidget("Label", teamLabel);
        setWidgetText(teamLabel, "team");
        setWidgetOrigin(teamLabel, 1.0f, 0.5f);
        setWidgetPosition(teamLabel, "100%", "50%");

        const auto teamGroup = topLayout + ".TeamGroup";
        addWidget("Group", teamGroup);

        team = teamGroup + ".Team";
        addWidget("EditBox", team);
        onlyAcceptUIntsInEditBox(team);
        setWidgetSize(team, "100%", "20px");
        setWidgetOrigin(team, 0.0f, 0.5f);
        setWidgetPosition(team, "0%", "50%");

        // Funds.
        const auto fundsLabelGroup = topLayout + ".FundsLabelGroup";
        addWidget("Group", fundsLabelGroup);

        const auto fundsLabel = fundsLabelGroup + ".FundsLabel";
        addWidget("Label", fundsLabel);
        setWidgetText(fundsLabel, "funds");
        setWidgetOrigin(fundsLabel, 1.0f, 0.5f);
        setWidgetPosition(fundsLabel, "100%", "50%");

        const auto fundsGroup = topLayout + ".FundsGroup";
        addWidget("Group", fundsGroup);

        funds = fundsGroup + ".Funds";
        addWidget("EditBox", funds);
        onlyAcceptUIntsInEditBox(funds);
        setWidgetSize(funds, "100%", "20px");
        setWidgetOrigin(funds, 0.0f, 0.5f);
        setWidgetPosition(funds, "0%", "50%");
        
        // Empty group to ensure top and bottom row widget counts match.
        addWidget("Group", topLayout + ".Placeholder");

        // Bottom row.
        const auto bottomLayout = layout + ".BottomLayout";
        addWidget("HorizontalLayout", bottomLayout);
        setSpaceBetweenWidgets(bottomLayout, 5.0f);

        // Current CO.
        const auto currentCOLabelGroup = bottomLayout + ".CurrentCOLabelGroup";
        addWidget("Group", currentCOLabelGroup);

        const auto currentCOLabel = currentCOLabelGroup + ".CurrentCOLabel";
        addWidget("Label", currentCOLabel);
        setWidgetText(currentCOLabel, "currentco");
        setWidgetOrigin(currentCOLabel, 1.0f, 0.5f);
        setWidgetPosition(currentCOLabel, "100%", "50%");

        const auto currentCOGroup = bottomLayout + ".CurrentCOGroup";
        addWidget("Group", currentCOGroup);

        currentCO = currentCOGroup + ".CurrentCO";
        addWidget("ComboBox", currentCO);
        setItemsToDisplay(currentCO, 5);
        awe::addCOsToList(currentCO, false);
        setWidgetSize(currentCO, "100%", "20px");
        setWidgetOrigin(currentCO, 0.5f, 0.5f);
        setWidgetPosition(currentCO, "50%", "50%");
        
        const auto currentCOIconGroup = bottomLayout + ".CurrentCOIconGroup";
        addWidget("Group", currentCOIconGroup);

        currentCOIcon = currentCOIconGroup + ".CurrentCOIcon";
        addWidget("Picture", currentCOIcon);
        setWidgetOrigin(currentCOIcon, 0.0f, 0.5f);
        setWidgetPosition(currentCOIcon, "0%", "50%");

        // Tag CO.
        const auto tagCOLabelGroup = bottomLayout + ".TagCOLabelGroup";
        addWidget("Group", tagCOLabelGroup);

        const auto tagCOLabel = tagCOLabelGroup + ".TagCOLabel";
        addWidget("Label", tagCOLabel);
        setWidgetText(tagCOLabel, "tagco");
        setWidgetOrigin(tagCOLabel, 1.0f, 0.5f);
        setWidgetPosition(tagCOLabel, "100%", "50%");

        const auto tagCOGroup = bottomLayout + ".TagCOGroup";
        addWidget("Group", tagCOGroup);

        tagCO = tagCOGroup + ".TagCO";
        addWidget("ComboBox", tagCO);
        setItemsToDisplay(tagCO, 5);
        awe::addCOsToList(tagCO, true);
        setWidgetSize(tagCO, "100%", "20px");
        setWidgetOrigin(tagCO, 0.5f, 0.5f);
        setWidgetPosition(tagCO, "50%", "50%");
        
        const auto tagCOIconGroup = bottomLayout + ".TagCOIconGroup";
        addWidget("Group", tagCOIconGroup);

        tagCOIcon = tagCOIconGroup + ".TagCOIcon";
        addWidget("Picture", tagCOIcon);
        setWidgetOrigin(tagCOIcon, 0.0f, 0.5f);
        setWidgetPosition(tagCOIcon, "0%", "50%");

        // Delete button.
        const auto deleteButtonGroup = bottomLayout + ".DeleteButtonGroup";
        addWidget("Group", deleteButtonGroup);

        const auto deleteButton = deleteButtonGroup + ".DeleteButton";
        addWidget("Button", deleteButton);
        setWidgetText(deleteButton, "delete");
        setWidgetSize(deleteButton, "100%", "20px");
        setWidgetOrigin(deleteButton, 0.5f, 0.5f);
        setWidgetPosition(deleteButton, "50%", "50%");
        connectSignalHandler(deleteButton,
            SignalHandler(this.deleteButtonSignalHandler));
    }

    /// Refreshes the layout with up-to-date army information.
    void refresh() {
        disconnectSignalHandlers();
        setWidgetText(team, formatTeamID(edit.map.getArmyTeam(army)));
        setWidgetText(funds, formatFunds(edit.map.getArmyFunds(army)));
        const auto current = edit.map.getArmyCurrentCO(army),
            tag = edit.map.getArmyTagCO(army);
        const auto currentIndex = commander.scriptNames.find(current),
            tagIndex = commander.scriptNames.find(tag);
        if (currentIndex < 0) {
            deselectItem(currentCO);
            clearWidgetSprite(currentCOIcon);
        } else {
            setSelectedItem(currentCO, uint(currentIndex));
            setWidgetSprite(currentCOIcon, "co", commander[current].iconName);
        }
        if (tagIndex < 0) {
            setSelectedItem(tagCO, 0);
            clearWidgetSprite(tagCOIcon);
        } else {
            setSelectedItem(tagCO, uint(tagIndex) + 1);
            setWidgetSprite(tagCOIcon, "co", commander[tag].iconName);
        }
        connectSignalHandlers();
    }

    /// Connects all signal handlers (except for the delete button).
    private void connectSignalHandlers() {
        connectSignalHandler(team,
            SignalHandler(this.armyTeamEditBoxSignalHandler));
        connectSignalHandler(funds,
            SignalHandler(this.armyFundsEditBoxSignalHandler));
        connectSignalHandler(currentCO,
            SignalHandler(this.currentCOComboBoxSignalHandler));
        connectSignalHandler(tagCO,
            SignalHandler(this.tagCOComboBoxSignalHandler));
    }

    /// Disconnects all signal handlers (except for the delete button).
    /// Used to prevent \c refresh() from applying changes to the map that already
    /// exist.
    private void disconnectSignalHandlers() {
        ::disconnectSignalHandlers({ team, funds, currentCO, tagCO });
    }

    /**
     * Handles the army's team \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void armyTeamEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto text = getWidgetText(widgetName);
            const auto team = parseTeamID(text);
            edit.map.setArmyTeam(army, team);
            if (text.isEmpty()) setWidgetText(widgetName, formatTeamID(team));
        }
    }

    /**
     * Handles the army's funds \c EditBox signals.
     * @param widgetName The full name of the \c EditBox.
     * @param signalName The name of the signal emitted.
     */
    private void armyFundsEditBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "TextChanged") {
            const auto text = getWidgetText(widgetName);
            const auto funds = parseFunds(text);
            const auto actualFunds = edit.setArmyFunds(army, funds);
            if (funds != actualFunds || text.isEmpty())
                setWidgetText(widgetName, formatFunds(actualFunds));
        }
    }

    /**
     * Handles the army's current CO \c ComboBox signals.
     * @param widgetName The full name of the \c ComboBox.
     * @param signalName The name of the signal emitted.
     */
    private void currentCOComboBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ItemSelected") {
            const auto co = getSelectedItem(widgetName);
            const auto scriptName = commander.scriptNames[uint64(co)];
            edit.map.setArmyCurrentCO(army, scriptName);
            setWidgetSprite(currentCOIcon, "co", commander[scriptName].iconName);
        }
    }

    /**
     * Handles the army's tag CO \c ComboBox signals.
     * @param widgetName The full name of the \c ComboBox.
     * @param signalName The name of the signal emitted.
     */
    private void tagCOComboBoxSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "ItemSelected") {
            const auto co = getSelectedItem(widgetName);
            if (co == 0) {
                edit.map.setArmyTagCO(army, "");
                clearWidgetSprite(tagCOIcon);
            } else {
                const auto scriptName = commander.scriptNames[uint64(co - 1)];
                edit.map.setArmyTagCO(army, scriptName);
                setWidgetSprite(tagCOIcon, "co", commander[scriptName].iconName);
            }
        }
    }

    /**
     * Handles the delete army \c Button signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void deleteButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") edit.deleteArmy(army);
    }

    /// Name of the \c MessageBox to create.
    private string mbName;

    /// Name of the widget to disable when opening a \c MessageBox.
    private string mbDisableThis;

    /// Name of the widget to enable when opening a \c MessageBox.
    private string mbEnableThis;

    /// The ID of the army this layout displays information on.
    private ArmyID army;

    /// The base \c VerticalLayout.
    private string layout;

    /// The name of the team \c EditBox.
    private string team;

    /// The name of the funds \c EditBox.
    private string funds;

    /// The name of the current CO \c ComboBox.
    private string currentCO;

    /// The name of the current CO \c Picture.
    private string currentCOIcon;

    /// The name of the tag CO \c ComboBox.
    private string tagCO;

    /// The name of the tag CO \c Picture.
    private string tagCOIcon;
}

/**
 * Represents a window that displays editable army properties.
 */
class ArmyPropertiesWindow {
    /**
     * The width of an ArmyLayout.
     */
    string ARMY_LAYOUT_WIDTH = "800";

    /**
     * The height of an ArmyLayout.
     */
    string ARMY_LAYOUT_HEIGHT = "80";

    /**
     * The space between ArmyLayouts.
     */
    string SPACE_BETWEEN_ARMY_LAYOUTS = "30";

    /**
     * Sets up \c MessageBox related data that can't be changed after
     * construction.
     * @param messageBoxName \c name parameter passed into \c OpenMessageBox()
     *                       when a \c MessageBox is required to be displayed.
     *                       Intended to be \c SIMPLE_MESSAGE_BOX.
     * @param disableThis    \c disableThis parameter pass into
     *                       \c OpenMessageBox() when a \c MessageBox is required
     *                       to be displayed. Intended to be \c BASE_GROUP.
     * @param enableThis     \c enableThis parameter pass into \c OpenMessageBox()
     *                       when a \c MessageBox is required to be displayed.
     *                       Intended to be \c MESSAGE_BOX_GROUP.
     */
    ArmyPropertiesWindow(const string&in messageBoxName,
        const string&in disableThis, const string&in enableThis) {
        mbName = messageBoxName;
        mbDisableThis = disableThis;
        mbEnableThis = enableThis;
    }

    /**
     * Sets up the window.
     * @param parent The parent of the child window.
     */
    void setUp(const string&in parent) {
        armyLayouts.resize(country.length());
        for (uint i = 0, len = armyLayouts.length(); i < len; ++i)
            @armyLayouts[i] = ArmyLayout(mbName, mbDisableThis, mbEnableThis);

        // Child window.
        window = parent + ".ArmyProperties";
        addWidget("ChildWindow", window);
        setWidgetText(window, "armyprops");
        closeChildWindow(window);
        const auto BORDER_WIDTH = formatFloat(getBorderWidths(window)[0]);
        const auto TITLEBAR_HEIGHT = formatFloat(getTitleBarHeight(window));

        // Scrollable panel.
        const auto PADDING = "5";
        const auto panel = window + ".ScrollablePanel";
        addWidget("ScrollablePanel", panel);
        setGroupPadding(panel, PADDING);
        setVerticalScrollbarAmount(panel, 25);
        setHorizontalScrollbarAmount(panel, 30);
        const auto SCROLLBAR_WIDTH = formatFloat(getScrollbarWidth(panel));
        
        // The default \c ChildWindow size.
        setWidgetSize(window,
            ARMY_LAYOUT_WIDTH + "+" + PADDING + "*2+" + BORDER_WIDTH + "*2+" +
                SCROLLBAR_WIDTH,
            ARMY_LAYOUT_HEIGHT + "+" + PADDING + "+" + TITLEBAR_HEIGHT + "+" +
                SCROLLBAR_WIDTH
        );

        // Vertical layout.
        layout = panel + ".ArmyLayout";
        addWidget("VerticalLayout", layout);
        setWidgetSize(layout, ARMY_LAYOUT_WIDTH, ARMY_LAYOUT_HEIGHT);
        setSpaceBetweenWidgets(layout, parseFloat(SPACE_BETWEEN_ARMY_LAYOUTS));

        // Create Army button, country list, and description label.
        const auto controlsGroup = layout + ".ControlsGroup";
        addWidget("Group", controlsGroup);

        const auto controlsLayout = controlsGroup + ".ControlsLayout";
        addWidget("HorizontalLayout", controlsLayout);

        const auto descriptionGroup = controlsLayout + ".ArmyDescriptionGroup";
        addWidget("Group", descriptionGroup);
        setGroupPadding(descriptionGroup, PADDING);

        const auto description = descriptionGroup + ".ArmyDescription";
        addWidget("Label", description);
        setWidgetText(description, "mapmakerarmydescription");
        setWidgetSize(description, "100%", "100%");
        setWidgetTextAlignment(description, HorizontalAlignment::Centre,
            VerticalAlignment::Centre);

        const auto countryGroup = controlsLayout + ".CountryGroup";
        addWidget("Group", countryGroup);
        setGroupPadding(countryGroup, PADDING);

        countryComboBox = countryGroup + ".CountryComboBox";
        addWidget("ComboBox", countryComboBox);
        setItemsToDisplay(countryComboBox, 5);
        awe::addCountriesToList(countryComboBox, false);
        setWidgetSize(countryComboBox, "100%", "30px");
        setWidgetOrigin(countryComboBox, 0.5f, 0.5f);
        setWidgetPosition(countryComboBox, "50%", "50%");

        const auto createButtonGroup = controlsLayout + ".CreateButtonGroup";
        addWidget("Group", createButtonGroup);
        setGroupPadding(createButtonGroup, PADDING);

        const auto createArmyButton = createButtonGroup + ".CreateArmyButton";
        addWidget("Button", createArmyButton);
        setWidgetText(createArmyButton, "createarmy");
        setWidgetSize(createArmyButton, "100%", "30px");
        setWidgetOrigin(createArmyButton, 0.5f, 0.5f);
        setWidgetPosition(createArmyButton, "50%", "50%");
        connectSignalHandler(createArmyButton,
            SignalHandler(this.createButtonSignalHandler));

        setWidgetRatioInLayout(controlsLayout, 0, 0.8f);
        setWidgetRatioInLayout(controlsLayout, 1, 0.2f);
        setWidgetRatioInLayout(controlsLayout, 2, 0.2f);
    }

    /**
     * Restores the window.
     */
    void restore() {
        openChildWindow(window, "50px", "35px");
    }

    /**
     * Refreshes the window with accurate army data.
     */
    void refresh() {
        // Go through each potential army layout, and either add, delete, or
        // update as necessary.
        uint64 indexCounter = 0;
        array<ArmyID> excludeCountries;
        for (uint c = 0, len = armyLayouts.length(); c < len; ++c) {
            const auto armyID = ArmyID(c);
            auto@ armyLayout = armyLayouts[c];
            if (edit !is null && edit.map.isArmyPresent(armyID)) {
                if (!armyLayout.exists()) armyLayout.create(layout, armyID);
                armyLayout.refresh();
                armyLayout.reorder(indexCounter++);
                excludeCountries.insertLast(armyID);
            } else if (armyLayout.exists()) {
                armyLayout.remove();
            }
        }
        // Resize the base layout depending on how many armies there are.
        setWidgetSize(layout, ARMY_LAYOUT_WIDTH,
            "(" + ARMY_LAYOUT_HEIGHT + "+" + SPACE_BETWEEN_ARMY_LAYOUTS + ")*" +
            getWidgetCount(layout) + "-" + SPACE_BETWEEN_ARMY_LAYOUTS);
        // Add and remove countries from the ComboBox based on the presence of
        // countries on the map.
        clearItems(countryComboBox);
        awe::addCountriesToList(countryComboBox, false, excludeCountries);
    }

    /**
     * Handles the create army \c Button signals.
     * @param widgetName The full name of the \c Button.
     * @param signalName The name of the signal emitted.
     */
    private void createButtonSignalHandler(const string&in widgetName,
        const string&in signalName) {
        if (signalName == "Pressed") {
            if (edit is null) {
                awe::OpenMessageBox(mbName, "alert", "nomapisopen", null,
                    mbDisableThis, mbEnableThis);
                addMessageBoxButton(mbName, "ok");
                return;
            }
            const auto selectedArmy = getSelectedItem(countryComboBox);
            if (selectedArmy < 0) {
                awe::OpenMessageBox(mbName, "alert", "pleaseselectcountry", null,
                    mbDisableThis, mbEnableThis);
                addMessageBoxButton(mbName, "ok");
                return;
            }
            const auto longName = getSelectedItemText(countryComboBox);
            ArmyID id = 0;
            const auto@ const scriptNames = country.scriptNames;
            for (const uint64 len = scriptNames.length(); id < len; ++id) {
                if (translate(country[scriptNames[uint64(id)]].name) == longName)
                    break;
            }
            if (id == country.length()) {
                error("Couldn't find ArmyID of selected country in "
                    "createButtonSignalHandler()! Can't create army!");
            } else edit.createArmy(id);
        }
    }

    /// Name of the \c MessageBox to create.
    private string mbName;

    /// Name of the widget to disable when opening a \c MessageBox.
    private string mbDisableThis;

    /// Name of the widget to enable when opening a \c MessageBox.
    private string mbEnableThis;

    /// Caches the name of the \c ChildWindow.
    private string window;

    /// Caches the name of the base \c VerticalLayout.
    private string layout;

    /// Caches the name of the countries \c ComboBox.
    private string countryComboBox;

    /// Caches each \c ArmyLayout.
    private array<ArmyLayout@> armyLayouts;
}
