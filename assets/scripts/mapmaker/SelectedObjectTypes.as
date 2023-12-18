/**
 * @file SelectedObjectTypes.as
 * Defines all the different types of selected object.
 * For an example on how to add a \c SelectedObject type with its own palette
 * panel, consult https://github.com/CasualYT31/ComputerWars/commit/5e8a7d4c6e2c06e57371c12f52ab763806fe3600
 * and surrounding commits.
 */

/**
 * The currently selected terrain.
 */
class SelectedTerrain : SelectedObject {
    /**
     * Points to the terrain currently selected.
     */
    private const Terrain@ terrainType;
    const Terrain@ type {
        set {
            const bool changed = terrainType !is value;
            @terrainType = value;
            if (changed) update();
        }
        get const {
            return terrainType;
        }
    }

    /**
     * The selected owner of the terrain.
     * Set to empty if there is no owner.
     */
    private string terrainOwner;
    string owner {
        set {
            const auto changed = terrainOwner != value;
            terrainOwner = value;
            if (changed) update();
        }
        get const {
            return terrainOwner;
        }
    }

    /**
     * Notifies observers when a change is made to the state of this object.
     */
    private void update() override {
        if (type is null) update("", "", "");
        else if (owner.isEmpty()) update(type.name, "tile.normal",
            type.primaryTileType.neutralTileSprite);
        else update(type.name, "tile.normal",
            type.primaryTileType.ownedTileSprite(owner));
    }
}

/**
 * The currently selected tile type.
 */
class SelectedTileType : SelectedObject {
    /**
     * Points to the tile type currently selected.
     */
    private const TileType@ tileType;
    const TileType@ type {
        set {
            const bool changed = tileType !is value;
            @tileType = value;
            if (changed) update();
        }
        get const {
            return tileType;
        }
    }

    /**
     * The selected owner of the tile type.
     * Set to empty if there is no owner.
     */
    private string tileTypeOwner;
    string owner {
        set {
            const auto changed = tileTypeOwner != value;
            tileTypeOwner = value;
            if (changed) update();
        }
        get const {
            return tileTypeOwner;
        }
    }

    /**
     * Notifies observers when a change is made to the state of this object.
     */
    private void update() override {
        if (type is null) update("", "", "");
        else if (owner.isEmpty())
            update(type.type.name, "tile.normal", type.neutralTileSprite);
        else update(type.type.name, "tile.normal", type.ownedTileSprite(owner));
    }
}

/**
 * The currently selected unit type.
 */
class SelectedUnitType : SelectedObject {
    /**
     * Points to the unit type currently selected.
     */
    private const UnitType@ unitType;
    const UnitType@ type {
        set {
            const bool changed = unitType !is value;
            @unitType = value;
            if (changed) update();
        }
        get const {
            return unitType;
        }
    }

    /**
     * The selected owner of the unit type.
     */
    private string unitTypeOwner = country.scriptNames[0];
    string owner {
        set {
            const auto changed = unitTypeOwner != value;
            unitTypeOwner = value;
            if (changed) update();
        }
        get const {
            return unitTypeOwner;
        }
    }

    /**
     * Notifies observers when a change is made to the state of this object.
     */
    private void update() override {
        if (type is null) update("", "", "");
        else update(type.name, "unit", type.unitSprite(owner));
    }
}

/**
 * The currently selected structure.
 */
class SelectedStructure : SelectedObject {
    /**
     * Points to the structure currently selected.
     */
    private const Structure@ structureType;
    const Structure@ type {
        set {
            const bool changed = structureType !is value;
            @structureType = value;
            if (changed) update();
        }
        get const {
            return structureType;
        }
    }

    /**
     * The selected owner of the structure.
     * Set to empty if there is no owner.
     */
    private string structureOwner = country.scriptNames[0];
    string owner {
        set {
            const auto changed = structureOwner != value;
            structureOwner = value;
            if (changed) update();
        }
        get const {
            return structureOwner;
        }
    }

    /**
     * \c TRUE if the structure is in the destroyed state, \c FALSE if it's in the
     * normal state.
     */
    private bool isDestroyed = false;
    bool destroyed {
        set {
            const auto changed = isDestroyed != value;
            isDestroyed = value;
            if (changed) update();
        }
        get const {
            return isDestroyed;
        }
    }

    /**
     * Notifies observers when a change is made to the state of this object.
     */
    private void update() override {
        if (type is null) update("", "", "");
        else if (destroyed)
            update(type.destroyedName, "structure", type.destroyedIconName);
        else if (owner.isEmpty()) update(type.name, "structure", type.iconName);
        else update(type.name, "structure", type.ownedIconName(owner));
    }
}
