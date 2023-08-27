/**
 * @file CurrentlySelectedObjectGroup.as
 * Defines code that manages instances of \c CurrentlySelectedObject widgets.
 */

/**
 * Manages all instances of a particular type of CurrentlySelectedObject widgets.
 */
class CurrentlySelectedObject {
    /**
     * Signature of the callback that is invoked when a new object or owner is
     * selected.
     */
    funcdef void Callback();

    /**
     * Signature of the callback that is used to determine each registered
     * widget's icon and name when a new object or owner is set.
     * @param  const ref@      The object that is set. Will never be \c null.
     * @param  const string&in The owner that is set.
     * @return An array of size three. The first string stores the spritesheet.
     *         The second string stores the sprite ID. The third string stores the
     *         name to display.
     */
    funcdef array<string> UpdateCallback(const ref@, const string&in);
    
    /**
     * Constructs a \c CurrentlySelectedObject type with a given update callback.
     */
    CurrentlySelectedObject(CurrentlySelectedObject::UpdateCallback@ callback) {
        @updateCallback = callback;
    }

    /**
     * Adds a \c CurrentlySelectedObject widget and registers it with
     * \c currentlySelectedObjectWidgetList.
     * @param layout   Name of the layout widget containing the
     *                 \c CurrentlySelectedObject widgets.
     * @param callback Code to run when the selected object is updated.
     */
    void addWidget(const string&in layout, Callback@ callback = null) {
        ::addWidget(HorizontalLayout, layout);
        setWidgetSize(layout, "100%", "100%");
        setSpaceBetweenWidgets(layout, 10.0f);

        const auto leftGroup = layout + ".IconGroup";
        ::addWidget(Group, leftGroup);
        
        const auto tile = leftGroup + ".ObjIcon";
        ::addWidget(Picture, tile);
        setWidgetOrigin(tile, 1.0f, 0.5f);
        setWidgetPosition(tile, "100%", "50%");

        const auto rightGroup = layout + ".NameGroup";
        ::addWidget(Group, rightGroup);

        const auto name = rightGroup + ".ObjName";
        ::addWidget(Label, name);
        setWidgetOrigin(name, 0.0f, 0.5f);
        setWidgetPosition(name, "0%", "50%");

        currentlySelectedObjectWidgetList.insertLast(layout);
        if (!(callback is null)) callbacks.insertLast(@callback);
        _set(false);
    }

    /**
     * The currently selected object.
     */
    private ref@ object;
    const ref@ object {
        set {
            @object = value;
            _set(true);
        }
        get const {
            return object;
        }
    }

    /**
     * The currently selected owner.
     */
    private string owner;
    string owner {
        set {
            owner = value;
            _set(true);
        }
        get const {
            return owner;
        }
    }

    /**
     * Updates all registered \c CurrentlySelectedObject widgets.
     * @param invokeCallbacks If \c TRUE, invokes all of the stored callbacks.
     */
    private void _set(const bool invokeCallbacks) {
        if (object is null || updateCallback is null) return;
        const auto props = updateCallback(object, owner);
        for (uint i = 0, len = currentlySelectedObjectWidgetList.length();
            i < len; ++i) {
            const auto layout = currentlySelectedObjectWidgetList[i];
            setWidgetSprite(layout + ".IconGroup.ObjIcon", props[0], props[1]);
            setWidgetText(layout + ".NameGroup.ObjName", props[2]);
        }
        if (invokeCallbacks) {
            for (uint i = 0, len = callbacks.length(); i < len; ++i)
                callbacks[i]();
        }
    }

    /**
     * A list of all the \c CurrentlySelectedObject widgets that have been added.
     */
    private array<string> currentlySelectedObjectWidgetList;

    /**
     * A list of all the callbacks that have been given to \c AddWidget().
     */
    private array<CurrentlySelectedObject::Callback@> callbacks;

    /**
     * The callback used in \c _set() to determine sprite and caption.
     */
    private CurrentlySelectedObject::UpdateCallback@ updateCallback;
}

/**
 * Manages the currently selected tile type and owner.
 */
CurrentlySelectedObject CurrentlySelectedTileType(
    function(object, owner) {
        const auto@ type = cast<TileType>(object);
        if (owner.isEmpty())
            return { "tile.normal", type.neutralTileSprite, type.type.name };
        else
            return { "tile.normal", type.ownedTileSprite(owner), type.type.name };
    }
);

/**
 * Manages the currently selected unit type and owner.
 */
CurrentlySelectedObject CurrentlySelectedUnitType(
    function(object, owner) {
        const auto@ type = cast<UnitType>(object);
        if (owner.isEmpty())
            return { "", "", "" };
        else
            return { "unit", type.unitSprite(owner), type.name };
    }
);
