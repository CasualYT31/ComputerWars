/**
 * @file ToolBar.as
 * Defines code that manages the map maker's tool bar.
 */

/**
 * The tools available.
 */
enum Tool {
    Paint,
    Delete,
    RectPaint,
    RectDelete,
    Count
}

/**
 * Defines constants that the \c ToolBar class uses internally.
 */
namespace ToolBarConstants {
    /**
     * The width and height of each button in the tool bar.
     */
    const string ButtonSize = "30";

    /**
     * The padding to apply around every button.
     */
    const string ButtonPadding = "5";

    /**
     * The number of buttons in the tool bar.
     */
    const string ButtonCount = formatUInt(Tool::Count);

    /**
     * The icon sprites which represent each \c Tool.
     */
    const array<string> ButtonSprites = {
        "painttool",
        "deletetool",
        "recttool",
        "rectdeletetool"
    };
}

/**
 * The map maker's tool bar.
 */
class ToolBar : ChildWindow {
    /**
     * Sets up the tool bar.
     */
    ToolBar() {
        // Setup the child window.
        setText("toolbar");
        setTitleButtons(TitleButton::Close);
        setResizable(false);
        setSize(
            ToolBarConstants::ButtonPadding + "+(" + ToolBarConstants::ButtonSize
                + "+" + ToolBarConstants::ButtonPadding + ")*" +
                ToolBarConstants::ButtonCount,
            ToolBarConstants::ButtonPadding + "*2+" + ToolBarConstants::ButtonSize
                + "+" + formatFloat(getTitleBarHeight())
        );
        dock();

        // Setup each button.
        buttons.resize(Tool::Count);
        for (uint64 i = 0; i < Tool::Count; ++i) {
            const auto name = formatUInt(i);
            @buttons[i] = BitmapButton();
            buttons[i].setName(name);
            buttons[i].setSprite("icon", ToolBarConstants::ButtonSprites[i]);
            buttons[i].setSize(
                ToolBarConstants::ButtonSize,
                ToolBarConstants::ButtonSize
            );
            buttons[i].setPosition(
                ToolBarConstants::ButtonPadding + "+(" +
                    ToolBarConstants::ButtonSize + "+" +
                    ToolBarConstants::ButtonPadding + ")*" + name,
                ToolBarConstants::ButtonPadding
            );
            buttons[i].setDisabledBackgroundColour(Colour(200, 200, 200, 255));
            add(buttons[i]);
            buttons[i].connect(MultiSignalHandler(this.buttonSignalHandler));
            // Automatically select the first tool.
            if (i == 0) buttonSignalHandler(buttons[i], Clicked);
        }
    }

    /**
     * Restores and docks the tool bar to the top of its parent.
     */
    void dock() {
        setOrigin(0.5, 0.0);
        open("50%", "0%");
    }

    /**
     * Checks if a given tool is selected.
     * @param  t The tool to test for.
     * @return \c TRUE if the given tool is currently selected, \c FALSE if not.
     */
    bool selected(const Tool t) const {
        return uint(t) == currentTool;
    }

    /**
     * When a tool is selected, disable its button, and re-enable the previous
     * tool's button.
     */
    private void buttonSignalHandler(const WidgetID id, const string&in signal) {
        if (signal != Clicked) return;
        // Re-enable old tool button.
        if (currentTool < buttons.length())
            buttons[currentTool].setEnabled(true);
        // Disable selected tool button.
        currentTool = parseUInt(::getWidgetName(id));
        buttons[currentTool].setEnabled(false);
    }

    /**
     * ID of the tool currently selected.
     */
    private uint64 currentTool = Tool::Count;

    /**
     * The buttons representing each tool.
     */
    private array<BitmapButton@> buttons;
}
