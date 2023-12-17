/**
 * @file SelectedObject.as
 * Defines base code for selected objects.
 */

/**
 * Base class for all selected objects.
 */
abstract class SelectedObject {
    /**
     * Attach an observer to this selected object.
     * @param o The observer to attach.
     */
    void attach(Observer@ const o) final {
        observers.insertLast(o);
    }

    /**
     * Completely detach an observer from this selected object.
     * @param o The observer to detach.
     */
    void detach(Observer@ const o) final {
        for (int i = int(observers.length()) - 1; i >= 0; --i)
            if (observers[i] is o) {
                error("DETACH");
                observers.removeAt(i);
            }
        error(formatUInt(observers.length()));
    }

    /**
     * Produces a new \c SelectedObjectWidget, attaches it to this selected
     * object, and makes an initial \c refresh() call by invoking the subclass's
     * \c update() method.
     * @warning When you wish to dispose of the widget, don't forget to detach it!
     * @return  Points to the new selected object widget.
     */
    SelectedObjectWidget@ widgetFactory() {
        SelectedObjectWidget@ widget = SelectedObjectWidget();
        attach(widget);
        update();
        return widget;
    }

    /**
     * Method to be overridden by subclasses which invokes the other \c update()
     * method with the necessary values based on its own state.
     */
    protected void update() {}

    /**
     * Notifies all observers when a new object has been selected.
     * @param name   The name of the new object.
     * @param sheet  The sheet from which the sprite is accessed.
     * @param sprite The key of the sprite of the new object.
     */
    protected void update(const string&in name, const string&in sheet,
        const string&in sprite) final {
        for (uint i = 0, len = observers.length(); i < len; ++i)
            observers[i].refresh(any(array<string> = { name, sheet, sprite }));
    }

    /**
     * Observers that are notified when the selected object changes.
     */
    private array<Observer@> observers;
}

/**
 * A widget that displays a \c SelectedObject and automatically updates when the
 * \c SelectedObject changes.
 */
class SelectedObjectWidget : Observer, HorizontalLayout {
    /**
     * Sets up the selected object widget.
     */
    SelectedObjectWidget() {
        setSpaceBetweenWidgets(10);

        // Setup the icon.
        icon.setOrigin(1.0, 0.5);
        icon.setPosition("100%", "50%");
        iconGroup.add(icon);
        add(iconGroup);

        // Setup the name.
        name.setOrigin(0.0, 0.5);
        name.setPosition("0%", "50%");
        nameGroup.add(name);
        add(nameGroup);
    }

    /**
     * The selected object was changed.
     * @param data Should be an array of three strings. The first string stores
     *             the name of the object, the second string stores the
     *             spritesheet of the icon, and the third string stores the key of
     *             the sprite to display as the icon.
     */
    void refresh(any&in data) override {
        array<string> strings;
        // If the retrieval doesn't work, use three blank strings.
        strings.resize(3);
        data.retrieve(strings);
        name.setText(strings[0]);
        icon.setSprite(strings[1], strings[2]);
    }

    /**
     * The group containing the icon of the selected object.
     */
    private Group iconGroup;

    /**
     * The icon of the selected object.
     */
    private Picture icon;
    
    /**
     * The group containing the name of the selected object.
     */
    private Group nameGroup;

    /**
     * The name of the selected object.
     */
    private Label name;
}
