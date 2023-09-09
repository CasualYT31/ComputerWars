/**
 * @file DetermineTileType.as
 * Defines code which determines what a given tile's type should be, based on its
 * surrounding tiles.
 */

namespace awe {
    /**
     * Determine what a tile's type should be, given its desired terrain, and the
     * types of the tiles that surround it.
     * If a tile is out-of-bounds, its relevant \c TileType@ parameter must be
     * \c null. \c C must never be \c null.
     * @param  UL The type of the tile that is one tile up and one tile to the
     *            left of the changing tile.
     * @param  U  The type of the tile that is one tile up from the changing tile.
     * @param  UL The type of the tile that is one tile up and one tile to the
     *            right of the changing tile.
     * @param  LE The type of the tile that is one tile to the left of the
     *            changing tile.
     * @param  C  The changing tile's new terrain.
     * @param  R  The type of the tile that is one tile to the right of the
     *            changing tile.
     * @param  LL The type of the tile that is one tile down and one tile to the
     *            left of the changing tile.
     * @param  L  The type of the tile that is one tile down from the changing
     *            tile.
     * @param  LL The type of the tile that is one tile down and one tile to the
     *            right of the changing tile.
     * @return Handle to the tile type that the changing tile should be. \c null
     *         should be returned when the primary tile type of the given terrain
     *         is to be used.
     */
    const TileType@ const DetermineTileType(
        const TileType@ const UL,
        const TileType@ const U,
        const TileType@ const UR,
        const TileType@ const LE,
        const Terrain@ const C,
        const TileType@ const R,
        const TileType@ const LL,
        const TileType@ const L,
        const TileType@ const LR) {
        return null;
    }
}
