/**
 * @file CommandWidget.as
 * Declares the command menu widget type.
 * This widget is a vertical layout of bitmap buttons. The widget will ensure that
 * all buttons remain the same height, by increasing the height of the layout each
 * time a button is added.
 */

/**
 * Used to create widgets that represent a command menu in-game.
 */
class CommandWidget : VerticalLayout {
    /**
     * Creates the vertical layout which will hold all of the command buttons.
     * @param menu   The name of the menu this command widget is being added to.
     * @param width  The width to keep the layout at.
     * @param height The height to assign to each command button.
     */
    CommandWidget(const string&in menu, const string&in width = "25%",
        const float height = 25.0) {
        menuName = menu;
        layoutWidth = width;
        commandHeight = height;
    }

    /**
     * Adds a command button.
     * @param  widgetText The text to assign to the command button.
     * @param  spriteName The key of the sprite to assign to this command button.
     * @param  handler    The \c Clicked signal handler for this button.
     * @param  sheetName  The name of the sheet which contains the sprite.
     * @return The 0-based index of the newly added command.
     */
    uint addCommand(const string&in widgetText, const string&in spriteName,
        SingleSignalHandler@ const handler, const string&in sheetName = "icon") {
        const auto i = bitmapButtons.length();
        bitmapButtons.resize(i + 1);
        add(bitmapButtons[i]);
        bitmapButtons[i].connect(Clicked, handler);
        bitmapButtons[i].setSprite(sheetName, spriteName);
        bitmapButtons[i].setText(widgetText);
        configureDirectionalFlow(bitmapButtons[i]);
        resizeLayout();
        return i;
    }

    /**
     * Updates a command button's text.
     * @param index      The 0-based index of the command to update.
     * @param widgetText The text to assign.
     */
    void setCommandText(const uint index, const string&in widgetText) {
        bitmapButtons[index].setText(widgetText);
    }

    /**
     * Removes all command buttons from the layout.
     */
    void removeAllCommands() {
        // The Widget destructor should remove as well as delete the widgets.
        bitmapButtons.resize(0);
        resizeLayout();
    }

    /**
     * Resizes the layout based on the number of command buttons present.
     */
    private void resizeLayout() {
        setSize(layoutWidth, formatFloat(widgetCount() * commandHeight));
    }

    /**
     * Reconfigures the directional flow of each BitmapButton.
     * @param newWidget The new widget being added.
     */
    private void configureDirectionalFlow(const Widget@ const newWidget) {
        // Better implementation would be to implement getWidgetDirectionalFlow()
        // in C++ and then just change the widgets I need to.
        const auto len = bitmapButtons.length();
        if (len == 1) {
            setWidgetDirectionalFlowStart(menuName, newWidget);
        } else {
            for (uint i = 0; i < len; ++i) {
                const auto@ up =
                    i == 0 ? bitmapButtons[len - 1] : bitmapButtons[i - 1];
                const auto@ down =
                    i == len - 1 ? bitmapButtons[0] : bitmapButtons[i + 1];
                bitmapButtons[i].setDirectionalFlow(up, down, null, null);
            }
        }
    }

    /**
     * Each command in the menu.
     */
    private array<BitmapButton> bitmapButtons;

    /**
     * The name of the menu this command widget is in.
     */
    private string menuName;

    /**
     * The width of the layout.
     */
    private string layoutWidth;

    /**
     * The height of each command button, in pixels.
     */
    private float commandHeight;
}
