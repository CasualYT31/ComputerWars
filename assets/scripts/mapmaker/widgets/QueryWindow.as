/**
 * @file QueryWindow.as
 * Defines the base class for all basic query windows.
 */

/**
 * Window used to query the user for a value.
 */
abstract class QueryWindow : ButtonsWindow {
    /**
     * Performs common setup tasks for a query window.
     * @param text        The text to apply to the explanation label.
     * @param inputWidget Points to the widget managed by the subclass that
     *                    accepts user input. It will be automatically centred in
     *                    its new group.
     * @param okHandler   Code to execute when the ok button is pressed. Note that
     *                    the ok button will be disabled by default.
     */
    QueryWindow(const string&in text, Widget@ const inputWidget,
        SingleSignalHandler@ const okHandler) {
        // Setup the child window.
        super({ ButtonsWindowButton("ok", okHandler) });
        enableOk(false);
        setTitleButtons(TitleButton::Close);
        setResizable(false);
        setSize("300", "175");
        setOrigin(0.5, 0.5);
        setPosition("50%", "50%");
        setPositionLocked(true);

        // Setup the label.
        label.setText(text);
        label.setPosition("50%", "50%");
        label.setOrigin(0.5, 0.5);
        label.setTextAlignment(HorizontalAlignment::Centre,
            VerticalAlignment::Centre);
        
        // Setup the input widget.
        inputWidget.setPosition("50%", "50%");
        inputWidget.setOrigin(0.5, 0.5);

        // Setup the layout.
        labelGroup.add(label);
        inputWidgetGroup.add(inputWidget);
        baseLayout.add(labelGroup);
        baseLayout.add(inputWidgetGroup);
        clientArea.add(baseLayout);
    }

    /**
     * Enable or disable the ok button.
     * @param enabled \c TRUE to enable the ok button, \c FALSE to disable.
     */
    void enableOk(const bool enabled) {
        buttons[0].setEnabled(enabled);
    }

    /**
     * Is the ok button enabled?
     * @return \c TRUE if enabled, \c FALSE if disabled.
     */
    bool isOkEnabled() const {
        return buttons[0].getEnabled();
    }

    /**
     * The base layout of the client area.
     */
    private VerticalLayout baseLayout;

    /**
     * The group containing the explanation label.
     */
    private Group labelGroup;

    /**
     * The label explaining what's expected in the input.
     */
    private Label label;

    /**
     * The group containing the input widget managed by the subclass.
     */
    private Group inputWidgetGroup;
}
