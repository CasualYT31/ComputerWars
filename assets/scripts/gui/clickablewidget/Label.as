/**
 * @file Label.as
 * Defines the \c Label class.
 */

/**
 * Represents any type of label widget in a GUI.
 */
shared class Label : ClickableWidget {
    /**
     * Create the label.
     * Should not be called by subclasses that represent a subtype of this widget.
     * E.g. \c RichTextLabel.
     */
    Label() {
        super(LabelEngineName);
    }

    /**
     * Create the label.
     * Should be called by subclasses that represent a subtype of this widget.
     * E.g. \c RichTextLabel.
     * @param widgetType The name of the type of widget to create.
     */
    Label(const string&in widgetType) {
        super(widgetType);
    }

    void setText(const string&in caption, array<any>@ const vars = null) {
        setWidgetText(this, caption, vars);
    }

    void setTextColour(const Colour&in c) {
        setWidgetTextColour(this, c);
    }

    void setTextOutlineColour(const Colour&in c) {
        setWidgetTextOutlineColour(this, c);
    }

    void setTextOutlineThickness(const float s) {
        setWidgetTextOutlineThickness(this, s);
    }

    void setTextAlignment(const HorizontalAlignment h,
        const VerticalAlignment v) {
        setWidgetTextAlignment(this, h, v);
    }

    void setTextMaximumWidth(const float w) {
        ::setWidgetTextMaximumWidth(this, w);
    }
}
