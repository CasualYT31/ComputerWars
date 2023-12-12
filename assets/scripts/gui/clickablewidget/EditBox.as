/**
 * @file EditBox.as
 * Defines the \c EditBox class.
 */

/**
 * Represents an edit box in a GUI.
 */
shared class EditBox : ClickableWidget {
    /**
     * Create the edit box.
     */
    EditBox() {
        super(EditBoxEngineName);
    }

    string getText() const {
        return getWidgetText(this);
    }
}
