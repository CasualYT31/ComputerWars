/**
 * @file Globals.as
 * Defines all the global variables that the scripts have access to.
 */

/**
 * Global point of access to the map that's currently being played.
 */
PlayableMap@ game;

/**
 * Global point of access to the map that's being editted in the map maker.
 */
EditableMap@ edit;

/**
 * Global point of access to the map maker menu.
 * Initialised in \c MapMaker::Open().
 */
MapMaker@ mapMaker;

/**
 * Global point of access to the currently selected terrain in the map maker.
 */
SelectedTerrain selectedTerrain;

/**
 * Global point of access to the currently selected tile type in the map maker.
 */
SelectedTileType selectedTileType;

/**
 * Global point of access to the currently selected unit type in the map maker.
 */
SelectedUnitType selectedUnitType;

/**
 * Global point of access to the currently selected structure in the map maker.
 */
SelectedStructure selectedStructure;
