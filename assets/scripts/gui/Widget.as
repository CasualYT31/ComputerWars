/**
 * @file Widget.as
 * Defines the \c Widget class.
 */

/**
 * Represents a widget in a GUI.
 */
shared abstract class Widget {
    /**
     * Create the widget.
     * @param widgetType The name of the type of widget to create.
     */
    protected Widget(const string&in widgetType) {
        id = createWidget(widgetType);
    }

    /**
     * When a widget no longer has any references to it, it should be deleted.
     */
    ~Widget() {
        ::deleteWidget(this);
    }

    void connect(const string&in signal, SingleSignalHandler@ const f) {
        ::connectSignal(this, signal, f);
    }

    void connect(MultiSignalHandler@ const f) {
        ::connectSignal(this, f);
    }

    void disconnectAll() {
        ::disconnectSignals({ this });
    }

    WidgetID getParent() const {
        return ::getParent(this);
    }

    void setPosition(const string&in x, const string&in y) {
        setWidgetPosition(this, x, y);
    }

    void setOrigin(const float x, const float y) {
        setWidgetOrigin(this, x, y);
    }

    void setSize(const string&in w, const string&in h) {
        setWidgetSize(this, w, h);
    }

    Vector2f getFullSize() const {
        return getWidgetFullSize(this);
    }

    void setEnabled(const bool e) {
        setWidgetEnabled(this, e);
    }

    bool getEnabled() const {
        return getWidgetEnabled(this);
    }

    void setVisibility(const bool visible) {
        setWidgetVisibility(this, visible);
    }

    bool getVisibility() const {
        return getWidgetVisibility(this);
    }

    void setFocus() {
        setWidgetFocus(this);
    }

    void setTextSize(const uint size) {
        setWidgetTextSize(this, size);
    }

    void setFont(const string&in f) {
        ::setWidgetFont(this, f);
    }

    void setName(const string&in n) {
        setWidgetName(this, n);
    }

    string getName() const {
        return getWidgetName(this);
    }

    void setIndex(const uint64 i) {
        setWidgetIndex(this, i);
    }

    void moveToFront() {
        moveWidgetToFront(this);
    }

    void moveToBack() {
        moveWidgetToBack(this);
    }

    void setAutoLayout(const AutoLayout l) {
        setWidgetAutoLayout(this, l);
    }

    void setDirectionalFlow(const Widget@ const up, const Widget@ const down,
        const Widget@ const left, const Widget@ const right) {
        setWidgetDirectionalFlow(this, (up is null ? NO_WIDGET : up),
            (down is null ? NO_WIDGET : down), (left is null ? NO_WIDGET : left),
            (right is null ? NO_WIDGET : right));
    }

    /**
     * Allow \c Widget objects to be implicitly converted to \c WidgetID.
     * @return The ID of the widget.
     */
    WidgetID opImplConv() const {
        return id;
    }

    /// The unique identifier of this widget.
    private WidgetID id;
    /// Provides read-only access to the unique identifier of this widget.
    WidgetID ID {
        get const {
            return id;
        }
    }
}
