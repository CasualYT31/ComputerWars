/**
 * @file ButtonsWindow.as
 * Defines generic code for a window with buttons along the button.
 */

/**
 * The properties associated with each button in a \c ButtonsWindow.
 */
class ButtonsWindowButton {
    /**
     * Default constructor.
     */
    ButtonsWindowButton() {}
    
    /**
     * Initialises this object.
     * @param t Assigned to \c text.
     * @param h Assigned to \c handler.
     */
    ButtonsWindowButton(const string&in t, SingleSignalHandler@ const h) {
        text = t;
        @handler = h;
    }
    
    /**
     * The text of the button.
     */
    string text;

    /**
     * When the button is pressed, trigger this handler.
     */
    SingleSignalHandler@ handler;
}

/**
 * A child window that has up to four buttons along the bottom.
 */
abstract class ButtonsWindow : ChildWindow {
    /**
     * Sets up the buttons of the child window.
     */
    ButtonsWindow(const array<ButtonsWindowButton>&in props) {
        // Setup each button.
        buttons.resize(props.length());
        for (uint i = 0, len = buttons.length(); i < len; ++i) {
            @buttons[i] = Button();
            buttons[i].setText(props[i].text);
            buttons[i].setSize("100%", "100%");
            buttonGroups[i].add(buttons[i]);
            buttons[i].connect(MouseReleased, props[i].handler);
        }

        // Setup each button group.
        for (int i = int(buttonGroups.length()) - 1; i >= 0; --i) {
            buttonGroups[i].setPadding("5", "", "5", "7.5");
            buttonsLayout.add(buttonGroups[i]);
        }

        // Setup the button layout and client area.
        clientArea.setAutoLayout(AutoLayout::Fill);
        add(clientArea);
        buttonsLayout.setSize("", "30");
        buttonsLayout.setAutoLayout(AutoLayout::Bottom);
        add(buttonsLayout);
    }

    /**
     * The group that contains the entirety of the client area of the child window
     * outside of the buttons layout.
     * Subclasses must use this to add their custom widgets.
     */
    protected Group clientArea;

    /**
     * The buttons layout.
     */
    private HorizontalLayout buttonsLayout;

    /**
     * The groups for each button slot in the buttons layout.
     * This array starts from the right and ends at the left. See \c buttons.
     */
    private array<Group@> buttonGroups = { Group(), Group(), Group(), Group() };

    /**
     * The button slots in the buttons layout.
     * This array starts from the right and ends at the left. E.g. the first
     * button will be the right-most button and the last button will be the
     * left-most button.
     */
    protected array<Button@> buttons;
}
