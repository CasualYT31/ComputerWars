/**
 * @file SelectedObjectTypes.as
 * Defines all the different types of selected object.
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
            @terrainType = value;
            update();
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
            terrainOwner = value;
            update();
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
            @tileType = value;
            update();
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
            tileTypeOwner = value;
            update();
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
