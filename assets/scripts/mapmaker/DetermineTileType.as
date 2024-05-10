/**
 * @file DetermineTileType.as
 * Defines code which determines what a given tile's type should be, based on its
 * surrounding tiles.
 */

namespace awe {
    /// The signature for each function responsible for determining the correct
    /// tile type to use.
    funcdef string DetermineTileTypeFunc(const TileTypeView@ const,
        const TileTypeView@ const, const TileTypeView@ const,
        const TileTypeView@ const, const TileTypeView@ const,
        const TileTypeView@ const, const TileTypeView@ const,
        const TileTypeView@ const);
    
    ////////////
    // HELPER //
    ////////////

    /// Returns \c TRUE if the given tile type is a building.
    bool isBuilding(const TileTypeView@ const t) {
        if (t is null) return false;
        const auto terrainType = t.terrain();
        return terrainType == "MISSILESILO" || terrainType == "CITY" ||
            terrainType == "BASE" || terrainType == "AIRPORT" ||
            terrainType == "PORT" || terrainType == "COMTOWER" ||
            terrainType == "LAB" || terrainType == "HQ";
    }

    /// Returns \c TRUE if the given tile type will cause a shadow to be cast.
    bool castsShadow(const TileTypeView@ const t) {
        if (t is null) return false;
        const auto terrainType = t.terrain();
        return isBuilding(t) || terrainType == "BLACKCRYSTAL" ||
            terrainType == "MOUNTAIN" || terrainType == "MINICANNON" ||
            terrainType == "BLACKLASER";
    }

    /// Returns \c TRUE if the given tile is land.
    bool isLand(const TileTypeView@ const t) {
        if (t is null) return false;
        const auto terrainType = t.terrain();
        return !(terrainType == "SEA" || terrainType == "BRIDGE" ||
            terrainType == "RIVER" || terrainType == "REEF" ||
            terrainType == "SHOAL");
    }

    /// Returns \c TRUE if the given tile type is a shoal.
    bool isShoal(const TileTypeView@ const t) {
        if (t is null) return true;
        return t.terrain() == "SHOAL";
    }

    /// Returns \c TRUE if the given tile type is a destination for a road tile.
    bool roadDestination(const TileTypeView@ const t) {
        if (t is null) return true;
        const auto terrainType = t.terrain();
        return terrainType == "ROAD" || terrainType == "BRIDGE" || isBuilding(t);
    }

    /// Returns \c TRUE if the given tile type is a destination for a river tile.
    bool riverDestination(const TileTypeView@ const t) {
        // We want null to be true.
        return !isLand(t);
    }

    /// Returns \c TRUE if the given tile type is a destination for a pipe tile.
    bool pipeDestination(const TileTypeView@ const t) {
        if (t is null) return true;
        const auto terrainType = t.terrain();
        return terrainType == "PIPE" || terrainType == "PIPESEAM" ||
            terrainType == "BASE";
    }

    //////////////
    // TERRAINS //
    //////////////

    /// Determine which plains tile to use.
    DetermineTileTypeFunc@ determineForPlain =
        function(UL, U, UR, LE, R, LL, L, LR) {
        if (castsShadow(LE)) return "002plainshadow";
        return "";
    };

    /// Determine which sea tile to use.
    DetermineTileTypeFunc@ determineForSea =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string sea = "";
        // Firstly, determine where land is in relation to the sea tile.
        if (isLand(U) && isLand(LE) && isLand(R) && isLand(L))
            sea = "008seasingle";
        else if (isLand(U) && isLand(LE) && isLand(R))
            sea = "00Cseaopeningdown";
        else if (isLand(U) && isLand(LE) && isLand(L))
            sea = "00Aseaopeningright";
        else if (isLand(U) && isLand(R) && isLand(L))
            sea = "009seaopeningleft";
        else if (isLand(LE) && isLand(R) && isLand(L))
            sea = "00Bseaopeningup";
        else if (isLand(U) && isLand(L))
            sea = "021seaedgetopdown";
        else if (isLand(LE) && isLand(R))
            sea = "020seaedgeleftright";
        else if (isLand(U) && isLand(LE)) {
            if (isLand(LR)) sea = "seacornerupleftvertex";
            else sea = "00Eseacornerupleft";
        } else if (isLand(U) && isLand(R)) {
            if (isLand(LL)) sea = "seacorneruprightvertex";
            else sea = "00Dseacornerupright";
        } else if (isLand(L) && isLand(LE)) {
            if (isLand(UR)) sea = "seacornerdownleftvertex";
            else sea = "00Fseacornerdownleft";
        } else if (isLand(L) && isLand(R)) {
            if (isLand(UL)) sea = "seacornerdownrightvertex";
            else sea = "010seacornerdownright";
        } else if (isLand(U)) {
            if (isLand(LL) && isLand(LR)) sea = "seavertexdownleftrightedgetop";
            else if (isLand(LL)) sea = "01Aseavertexdownleftedgetop";
            else if (isLand(LR)) sea = "017seavertexdownrightedgetop";
            else sea = "01Fseaedgetop";
        } else if (isLand(LE)) {
            if (isLand(UR) && isLand(LR)) sea = "seavertexrightupdownedgeleft";
            else if (isLand(UR)) sea = "seavertexuprightedgeleft";
            else if (isLand(LR)) sea = "seavertexdownrightedgeleft";
            else sea = "01Eseaedgeleft";
        } else if (isLand(R)) {
            if (isLand(UL) && isLand(LL)) sea = "seavertexleftupdownedgeright";
            else if (isLand(UL)) sea = "seavertexupleftedgeright";
            else if (isLand(LL)) sea = "seavertexdownleftedgeright";
            else sea = "01Cseaedgeright";
        } else if (isLand(L)) {
            if (isLand(UL) && isLand(UR)) sea = "016seavertexupleftright";
            else if (isLand(UL)) sea = "seavertexupleftedgedown";
            else if (isLand(UR)) sea = "019seavertexuprightedgedown";
            else sea = "01Dseaedgedown";
        } else {
            if (isLand(UL) && isLand(UR) && isLand(LL) && isLand(LR))
                sea = "seavertexall";
            else if (isLand(UL) && isLand(UR) && isLand(LL))
                sea = "seavertexnodownright";
            else if (isLand(UL) && isLand(UR) && isLand(LR))
                sea = "seavertexnodownleft";
            else if (isLand(UL) && isLand(LL) && isLand(LR))
                sea = "seavertexnoupright";
            else if (isLand(UR) && isLand(LL) && isLand(LR))
                sea = "seavertexnoupleft";
            else if (isLand(UL) && isLand(UR))
                sea = "seavertexupleftrightnoedge";
            else if (isLand(UL) && isLand(LR))
                sea = "seavertexleftupdownright";
            else if (isLand(UL) && isLand(LL))
                sea = "018seavertexleftupdown";
            else if (isLand(UR) && isLand(LR))
                sea = "01Bseavertexrightupdown";
            else if (isLand(UR) && isLand(LL))
                sea = "seavertexrightupdownleft";
            else if (isLand(LL) && isLand(LR))
                sea = "015seavertexdownleftright";
            else if (isLand(UL))
                sea = "012seavertexupleft";
            else if (isLand(UR))
                sea = "011seavertexupright";
            else if (isLand(LL))
                sea = "013seavertexdownleft";
            else if (isLand(LR))
                sea = "014seavertexdownright";
        }
        // Then, determine if a waterfall should be present due to a river. For
        // now, we'll only support what the original games supported: four
        // waterfall tiles.
        if (U !is null && U.terrain() == "RIVER" && !isLand(LE) &&
            !isLand(R) && !isLand(LL) && !isLand(L) && !isLand(LR))
            sea = "007seariverexitup";
        else if (LE !is null && LE.terrain() == "RIVER" && !isLand(U) &&
            !isLand(UR) && !isLand(R) && !isLand(LR) && !isLand(L))
            sea = "004seariverexitleft";
        else if (L !is null && L.terrain() == "RIVER" && !isLand(UL) &&
            !isLand(U) && !isLand(UR) && !isLand(LE) && !isLand(R))
            sea = "005seariverexitdown";
        else if (R !is null && R.terrain() == "RIVER" && !isLand(UL) &&
            !isLand(U) && !isLand(LE) && !isLand(LL) && !isLand(L))
            sea = "006seariverexitright";
        return sea;
    };

    /// Determine which road tile to use.
    DetermineTileTypeFunc@ determineForRoad =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string road = "";
        // First, determine which way the road should run.
        if (roadDestination(U) && roadDestination(LE) && roadDestination(R) &&
            roadDestination(L)) road = "024roadcross";
        else if (roadDestination(U) && roadDestination(LE) && roadDestination(R))
            road = "02Broadleftupright";
        else if (roadDestination(U) && roadDestination(LE) && roadDestination(L))
            road = "029roadupleftdown";
        else if (roadDestination(U) && roadDestination(R) && roadDestination(L))
            road = "028roaduprightdown";
        else if (roadDestination(LE) && roadDestination(R) && roadDestination(L))
            road = "026roadleftdownright";
        else if (roadDestination(U) && roadDestination(LE))
            road = "02Croadupleft";
        else if (roadDestination(U) && roadDestination(R))
            road = "02Aroadupright";
        else if (roadDestination(L) && roadDestination(LE))
            road = "027roaddownleft";
        else if (roadDestination(L) && roadDestination(R))
            road = "025roaddownright";
        else if (roadDestination(LE) || roadDestination(R))
            road = "023roadhori";
        else
            road = "022roadvert";
        // Then, figure out if we should cast a shadow onto it.
        if (castsShadow(LE)) road += "shadow";
        return road;
    };

    /// Determine which bridge tile to use.
    DetermineTileTypeFunc@ determineForBridge =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string bridge = "";
        if (isLand(U) || isLand(L)) bridge = "030bridgevert";
        if (isLand(LE) || isLand(R)) bridge = "031bridgehori";
        // Bridges should always flow when possible.
        if ((U !is null && U.terrain() == "BRIDGE") ||
            (L !is null && L.terrain() == "BRIDGE"))
            bridge = "030bridgevert";
        if ((R !is null && R.terrain() == "BRIDGE") ||
            (LE !is null && LE.terrain() == "BRIDGE"))
            bridge = "031bridgehori";
        return bridge;
    };

    /// Determine which mountain tile to use.
    DetermineTileTypeFunc@ determineForMountain =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which wood tile to use.
    DetermineTileTypeFunc@ determineForWood =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which river tile to use.
    DetermineTileTypeFunc@ determineForRiver =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string river = "";
        if (riverDestination(U) && riverDestination(LE) && riverDestination(R) &&
            riverDestination(L)) river = "03Erivercross";
        else if (riverDestination(U) && riverDestination(LE) &&
            riverDestination(R))
            river = "037riverleftupright";
        else if (riverDestination(U) && riverDestination(LE) &&
            riverDestination(L))
            river = "038riverleftupdown";
        else if (riverDestination(U) && riverDestination(R) &&
            riverDestination(L))
            river = "036riveruprightdown";
        else if (riverDestination(LE) && riverDestination(R) &&
            riverDestination(L))
            river = "039riverleftdownright";
        else if (riverDestination(U) && riverDestination(LE))
            river = "03Ariverleftup";
        else if (riverDestination(U) && riverDestination(R))
            river = "03Driverupright";
        else if (riverDestination(L) && riverDestination(LE))
            river = "03Briverleftdown";
        else if (riverDestination(L) && riverDestination(R))
            river = "03Criverdownright";
        else if (riverDestination(LE) || riverDestination(R))
            river = "034riverhori";
        else
            river = "035riververt";
        return river;
    };

    /// Determine which reef tile to use.
    DetermineTileTypeFunc@ determineForReef =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which shoal tile to use.
    DetermineTileTypeFunc@ determineForShoal =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string shoal = "";
        if (isLand(U) && isLand(LE) && isLand(R) && isLand(L))
            shoal = "shoalsingle";
        else if (isLand(U) && isLand(LE) && isLand(R))
            shoal = "04Bshoalleftupright";
        else if (isLand(U) && isLand(LE) && isLand(L))
            shoal = "048shoalleftupdown";
        else if (isLand(L) && isLand(LE) && isLand(R))
            shoal = "04Ashoalleftdownright";
        else if (isLand(U) && isLand(R) && isLand(L))
            shoal = "049shoaluprightdown";
        else if (isLand(U) && isLand(LE)) {
            if (isShoal(R) || isShoal(L)) shoal = "044shoalupleft";
            else shoal = "044shoalupleft";
        } else if (isLand(U) && isLand(R)) {
            if (isShoal(R) || isShoal(L)) shoal = "045shoalupright";
            else shoal = "045shoalupright";
        } else if (isLand(L) && isLand(LE)) {
            if (isShoal(R) || isShoal(L)) shoal = "047shoaldownleft";
            else shoal = "047shoaldownleft";
        } else if (isLand(L) && isLand(R)) {
            if (isShoal(R) || isShoal(L)) shoal = "046shoaldownright";
            else shoal = "046shoaldownright";
        } else if (isLand(U)) {
            if (isShoal(LE) && isShoal(R)) shoal = "040shoalup";
            else if (isShoal(LE)) shoal = "051shoalupendright";
            else if (isShoal(R)) shoal = "050shoalupendleft";
            else shoal = "040shoalup";
        } else if (isLand(L)) {
            if (isShoal(LE) && isShoal(R)) shoal = "042shoaldown";
            else if (isShoal(LE)) shoal = "04Cshoaldownendright";
            else if (isShoal(R)) shoal = "04Dshoaldownendleft";
            else shoal = "042shoaldown";
        } else if (isLand(LE)) {
            if (isShoal(U) && isShoal(L)) shoal = "043shoalleft";
            else if (isShoal(U)) shoal = "052shoalleftenddown";
            else if (isShoal(L)) shoal = "053shoalleftendtop";
            else shoal = "043shoalleft";
        } else if (isLand(R)) {
            if (isShoal(U) && isShoal(L)) shoal = "041shoalright";
            else if (isShoal(U)) shoal = "04Fshoalrightenddown";
            else if (isShoal(L)) shoal = "04Eshoalrightendtop";
            else shoal = "041shoalright";
        }
        return shoal;
    };

    /// Determine which pipe tile to use.
    DetermineTileTypeFunc@ determineForPipe =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string pipe = "";
        // Special corner cases. If the pipe is adjacent to two null tiles and two
        // pipe tiles, make this tile a corner by treating the null tiles as
        // non-destination tiles.
        if (U is null && LE is null && R !is null && L !is null &&
            pipeDestination(R) && pipeDestination(L))
            pipe = "05Apipecornerdownright";
        else if (U is null && R is null && LE !is null && L !is null &&
            pipeDestination(LE) && pipeDestination(L))
            pipe = "05Bpipecornerdownleft";
        else if (L is null && R is null && LE !is null && U !is null &&
            pipeDestination(LE) && pipeDestination(U))
            pipe = "05Cpipecornerupleft";
        else if (L is null && LE is null && R !is null && U !is null &&
            pipeDestination(R) && pipeDestination(U))
            pipe = "05Dpipecornerupright";
        // Normal cases.
        else if (pipeDestination(U) && pipeDestination(L))
            pipe = "057pipevert";
        else if (pipeDestination(LE) && pipeDestination(R))
            pipe = "054pipehori";
        else if (pipeDestination(U) && pipeDestination(R))
            pipe = "05Dpipecornerupright";
        else if (pipeDestination(U) && pipeDestination(LE))
            pipe = "05Cpipecornerupleft";
        else if (pipeDestination(L) && pipeDestination(R))
            pipe = "05Apipecornerdownright";
        else if (pipeDestination(L) && pipeDestination(LE))
            pipe = "05Bpipecornerdownleft";
        else if (pipeDestination(U))
            pipe = "058pipedeadenddown";
        else if (pipeDestination(LE))
            pipe = "055pipedeadendright";
        else if (pipeDestination(R))
            pipe = "056pipedeadendleft";
        else if (pipeDestination(L))
            pipe = "059pipedeadendup";
        return pipe;
    };

    /// Determine which pipe seam tile to use.
    DetermineTileTypeFunc@ determineForPipeSeam =
        function(UL, U, UR, LE, R, LL, L, LR) {
        string seam = "";
        if (pipeDestination(U) || pipeDestination(L))
            seam = "05Fpipeseamvert";
        else if (pipeDestination(LE) || pipeDestination(R))
            seam = "05Epipeseamhori";
        return seam;
    };

    /// Determine which missile silo tile to use.
    DetermineTileTypeFunc@ determineForMissileSilo =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which city tile to use.
    DetermineTileTypeFunc@ determineForCity =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which base tile to use.
    DetermineTileTypeFunc@ determineForBase =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which airport tile to use.
    DetermineTileTypeFunc@ determineForAirport =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which port tile to use.
    DetermineTileTypeFunc@ determineForPort =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which COM tower tile to use.
    DetermineTileTypeFunc@ determineForCOMTower =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /// Determine which lab tile to use.
    DetermineTileTypeFunc@ determineForLab =
        function(UL, U, UR, LE, R, LL, L, LR) { return ""; };

    /**
     * Determine what a tile's type should be, given its desired terrain, and the
     * types of the tiles that surround it.
     * If a tile is out-of-bounds, its relevant \c TileTypeView@ parameter must be
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
     * @return Script name of the tile type that the changing tile should be.
     *         An empty string should be returned when the primary tile type of
     *         the given terrain is to be used.
     */
    string DetermineTileType(
        const TileTypeView@ const UL,
        const TileTypeView@ const U,
        const TileTypeView@ const UR,
        const TileTypeView@ const LE,
        const TerrainView@ const C,
        const TileTypeView@ const R,
        const TileTypeView@ const LL,
        const TileTypeView@ const L,
        const TileTypeView@ const LR) {
        if (C.scriptName == "PLAINS")
            return awe::determineForPlain(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "SEA")
            return awe::determineForSea(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "ROAD")
            return awe::determineForRoad(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "BRIDGE")
            return awe::determineForBridge(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "MOUNTAIN")
            return awe::determineForMountain(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "WOODS")
            return awe::determineForWood(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "RIVER")
            return awe::determineForRiver(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "REEF")
            return awe::determineForReef(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "SHOAL")
            return awe::determineForShoal(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "PIPE")
            return awe::determineForPipe(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "PIPESEAM")
            return awe::determineForPipeSeam(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "MISSILESILO")
            return awe::determineForMissileSilo(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "CITY")
            return awe::determineForCity(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "BASE")
            return awe::determineForBase(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "AIRPORT")
            return awe::determineForAirport(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "PORT")
            return awe::determineForPort(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "COMTOWER")
            return awe::determineForCOMTower(UL, U, UR, LE, R, LL, L, LR);
        else if (C.scriptName == "LAB")
            return awe::determineForLab(UL, U, UR, LE, R, LL, L, LR);
        else return "";
    }
}
