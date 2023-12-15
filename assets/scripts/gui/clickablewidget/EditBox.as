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

    void setText(const string&in t) {
        ::setWidgetText(this, t);
    }

    string getText() const {
        return getWidgetText(this);
    }

    void setValidator(const string&in v) {
        ::setEditBoxRegexValidator(this, v);
    }
}
