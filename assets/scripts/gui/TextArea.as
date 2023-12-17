/**
 * @file TextArea.as
 * Defines the \c TextArea class.
 */

/**
 * Represents a text area in a GUI.
 */
shared class TextArea : Widget {
    /**
     * Create the text area.
     */
    TextArea() {
        super(TextAreaEngineName);
    }

    void setText(const string&in t) {
        ::setWidgetText(this, t);
    }
    
    string getText() const {
        return ::getWidgetText(this);
    }

    void getCaretLineAndColumn(uint64&out l, uint64&out c) const {
        ::getCaretLineAndColumn(this, l, c);
    }

    void optimiseForMonospaceFont(const bool o) {
        ::optimiseTextAreaForMonospaceFont(this, o);
    }
}
