namespace Weather {
    namespace CLEAR {
        namespace Commander {
            namespace JAKE {
                const string longName("Jake's Clear Weather");
                string shortName("shortie");

                // void longName(string& name) {
                //     name += "APPEND";
                // }

                void particles(ParticleDataArray& particles) {
                    particles.array.resize(5);
                    particles.array[4].vector.x = -1000.5;
                }
            }
        }
    }
}

#expand CountryOverrides Structure HQ ownedIcon ORANGE "hqos" BLUE "hqbm" GREEN "hqge" YELLOW "hqyc" BLACK "hqbh" RED "hqrf" GREY "hqgs" BROWN "hqbd" AMBER "hqab" JADE "hqjs" COBALT "hqci" PINK "hqpc" TEAL "hqtg" PURPLE "hqpl" ACID "hqar" WHITE "hqwn"

#expand CountryOverrides TileType _064city tile ORANGE "cityos" BLUE "citybm" GREEN "cityge" YELLOW "cityyc" BLACK "citybh" RED "cityrf" GREY "citygs" BROWN "citybd" AMBER "cityab" JADE "cityjs" COBALT "cityci" PINK "citypc" TEAL "citytg" PURPLE "citypl" ACID "cityar" WHITE "citywn"
#expand CountryOverrides TileType _065base tile ORANGE "baseos" BLUE "basebm" GREEN "basege" YELLOW "baseyc" BLACK "basebh" RED "baserf" GREY "basegs" BROWN "basebd" AMBER "baseab" JADE "basejs" COBALT "baseci" PINK "basepc" TEAL "basetg" PURPLE "basepl" ACID "basear" WHITE "basewn"
#expand CountryOverrides TileType _066airport tile ORANGE "airportos" BLUE "airportbm" GREEN "airportge" YELLOW "airportyc" BLACK "airportbh" RED "airportrf" GREY "airportgs" BROWN "airportbd" AMBER "airportab" JADE "airportjs" COBALT "airportci" PINK "airportpc" TEAL "airporttg" PURPLE "airportpl" ACID "airportar" WHITE "airportwn"
#expand CountryOverrides TileType _067port tile ORANGE "portos" BLUE "portbm" GREEN "portge" YELLOW "portyc" BLACK "portbh" RED "portrf" GREY "portgs" BROWN "portbd" AMBER "portab" JADE "portjs" COBALT "portci" PINK "portpc" TEAL "porttg" PURPLE "portpl" ACID "portar" WHITE "portwn"
#expand CountryOverrides TileType _068comtower tile ORANGE "comtoweros" BLUE "comtowerbm" GREEN "comtowerge" YELLOW "comtoweryc" BLACK "comtowerbh" RED "comtowerrf" GREY "comtowergs" BROWN "comtowerbd" AMBER "comtowerab" JADE "comtowerjs" COBALT "comtowerci" PINK "comtowerpc" TEAL "comtowertg" PURPLE "comtowerpl" ACID "comtowerar" WHITE "comtowerwn"
#expand CountryOverrides TileType _069lab tile ORANGE "labos" BLUE "labbm" GREEN "labge" YELLOW "labyc" BLACK "labbh" RED "labrf" GREY "labgs" BROWN "labbd" AMBER "labab" JADE "labjs" COBALT "labci" PINK "labpc" TEAL "labtg" PURPLE "labpl" ACID "labar" WHITE "labwn"
#expand CountryOverrides TileType _06Ahq tile ORANGE "hqos" BLUE "hqbm" GREEN "hqge" YELLOW "hqyc" BLACK "hqbh" RED "hqrf" GREY "hqgs" BROWN "hqbd" AMBER "hqab" JADE "hqjs" COBALT "hqci" PINK "hqpc" TEAL "hqtg" PURPLE "hqpl" ACID "hqar" WHITE "hqwn"

#expand CountryOverrides TileType _064city capturingProperty ORANGE "oscity" BLUE "bmcity" GREEN "gecity" YELLOW "yccity" BLACK "bhcity" RED "rfcity" GREY "gscity" BROWN "bdcity" AMBER "abcity" JADE "jscity" COBALT "cicity" PINK "pccity" TEAL "tgcity" PURPLE "plcity" ACID "arcity" WHITE "wncity"
#expand CountryOverrides TileType _065base capturingProperty ORANGE "osbase" BLUE "bmbase" GREEN "gebase" YELLOW "ycbase" BLACK "bhbase" RED "rfbase" GREY "gsbase" BROWN "bdbase" AMBER "abbase" JADE "jsbase" COBALT "cibase" PINK "pcbase" TEAL "tgbase" PURPLE "plbase" ACID "arbase" WHITE "wnbase"
#expand CountryOverrides TileType _066airport capturingProperty ORANGE "osairport" BLUE "bmairport" GREEN "geairport" YELLOW "ycairport" BLACK "bhairport" RED "rfairport" GREY "gsairport" BROWN "bdairport" AMBER "abairport" JADE "jsairport" COBALT "ciairport" PINK "pcairport" TEAL "tgairport" PURPLE "plairport" ACID "arairport" WHITE "wnairport"
#expand CountryOverrides TileType _067port capturingProperty ORANGE "osport" BLUE "bmport" GREEN "geport" YELLOW "ycport" BLACK "bhport" RED "rfport" GREY "gsport" BROWN "bdport" AMBER "abport" JADE "jsport" COBALT "ciport" PINK "pcport" TEAL "tgport" PURPLE "plport" ACID "arport" WHITE "wnport"
#expand CountryOverrides TileType _068comtower capturingProperty ORANGE "oscomtower" BLUE "bmcomtower" GREEN "gecomtower" YELLOW "yccomtower" BLACK "bhcomtower" RED "rfcomtower" GREY "gscomtower" BROWN "bdcomtower" AMBER "abcomtower" JADE "jscomtower" COBALT "cicomtower" PINK "pccomtower" TEAL "tgcomtower" PURPLE "plcomtower" ACID "arcomtower" WHITE "wncomtower"
#expand CountryOverrides TileType _069lab capturingProperty ORANGE "oslab" BLUE "bmlab" GREEN "gelab" YELLOW "yclab" BLACK "bhlab" RED "rflab" GREY "gslab" BROWN "bdlab" AMBER "ablab" JADE "jslab" COBALT "cilab" PINK "pclab" TEAL "tglab" PURPLE "pllab" ACID "arlab" WHITE "wnlab"
#expand CountryOverrides TileType _06Ahq capturingProperty ORANGE "oshq" BLUE "bmhq" GREEN "gehq" YELLOW "ychq" BLACK "bhhq" RED "rfhq" GREY "gshq" BROWN "bdhq" AMBER "abhq" JADE "jshq" COBALT "cihq" PINK "pchq" TEAL "tghq" PURPLE "plhq" ACID "arhq" WHITE "wnhq"

#expand CountryOverrides Terrain HQ icon ORANGE "oshq" BLUE "bmhq" GREEN "gehq" YELLOW "ychq" BLACK "bhhq" RED "rfhq" GREY "gshq" BROWN "bdhq" AMBER "abhq" JADE "jshq" COBALT "cihq" PINK "pchq" TEAL "tghq" PURPLE "plhq" ACID "arhq" WHITE "wnhq"
#expand CountryOverrides Terrain LAB icon ORANGE "oshq" BLUE "bmhq" GREEN "gehq" YELLOW "ychq" BLACK "bhhq" RED "rfhq" GREY "gshq" BROWN "bdhq" AMBER "abhq" JADE "jshq" COBALT "cihq" PINK "pchq" TEAL "tghq" PURPLE "plhq" ACID "arhq" WHITE "wnhq"

const dictionary SELECTED_SHEET_FOR_COUNTRY = {
    { "ORANGE", "unit.right" },
    { "BLUE", "unit.left" },
    { "GREEN", "unit.right" },
    { "YELLOW", "unit.left" },
    { "BLACK", "unit.left" },
    { "RED", "unit.left" },
    { "GREY", "unit.right" },
    { "BROWN", "unit.right" },
    { "AMBER", "unit.left" },
    { "JADE", "unit.right" },
    { "COBALT", "unit.right" },
    { "PINK", "unit.left" },
    { "TEAL", "unit.left" },
    { "PURPLE", "unit.right" },
    { "ACID", "unit.left" },
    { "WHITE", "unit.right" }
};
#assert SELECTED_SHEET_FOR_COUNTRY.getSize() == country.length(); "Ensure there is a selectedSheet for every country! Please update SELECTED_SHEET_FOR_COUNTRY!"

#expand UnitSprite INFANTRY inf
#expand UnitPicture INFANTRY infantry
#expand CountryOverrides UnitType INFANTRY capturingSprite ORANGE "osinfcapturing" BLUE "bminfcapturing" GREEN "geinfcapturing" YELLOW "ycinfcapturing" BLACK "bhinfcapturing" RED "rfinfcapturing" GREY "gsinfcapturing" BROWN "bdinfcapturing" AMBER "abinfcapturing" JADE "jsinfcapturing" COBALT "ciinfcapturing" PINK "pcinfcapturing" TEAL "tginfcapturing" PURPLE "plinfcapturing" ACID "arinfcapturing" WHITE "wninfcapturing"
#expand CountryOverrides UnitType INFANTRY finishedCapturingSprite ORANGE "osinfcaptured" BLUE "bminfcaptured" GREEN "geinfcaptured" YELLOW "ycinfcaptured" BLACK "bhinfcaptured" RED "rfinfcaptured" GREY "gsinfcaptured" BROWN "bdinfcaptured" AMBER "abinfcaptured" JADE "jsinfcaptured" COBALT "ciinfcaptured" PINK "pcinfcaptured" TEAL "tginfcaptured" PURPLE "plinfcaptured" ACID "arinfcaptured" WHITE "wninfcaptured"
#expand UnitMoveSounds INFANTRY ROAD "move.foot.road" MOUNTAIN "move.foot.mount" WOODS "move.foot.woods" RIVER "move.foot.river" SHOAL "move.foot.shoal"
namespace UnitType {
    namespace INFANTRY {
        namespace Terrain {
            namespace MOUNTAIN {
                void vision(uint&out vis) {
                    vis += 3;
                }
            }
        }
    }
    namespace MECH {
        namespace Terrain {
            namespace MOUNTAIN {
                void vision(uint&out vis) {
                    vis += 3;
                }
            }
        }
    }
}

#expand UnitSprite MECH mech
#expand UnitPicture MECH mech
#expand CountryOverrides UnitType MECH capturingSprite ORANGE "osinfcapturing" BLUE "bminfcapturing" GREEN "geinfcapturing" YELLOW "ycinfcapturing" BLACK "bhinfcapturing" RED "rfinfcapturing" GREY "gsinfcapturing" BROWN "bdinfcapturing" AMBER "abinfcapturing" JADE "jsinfcapturing" COBALT "ciinfcapturing" PINK "pcinfcapturing" TEAL "tginfcapturing" PURPLE "plinfcapturing" ACID "arinfcapturing" WHITE "wninfcapturing"
#expand CountryOverrides UnitType MECH finishedCapturingSprite ORANGE "osinfcaptured" BLUE "bminfcaptured" GREEN "geinfcaptured" YELLOW "ycinfcaptured" BLACK "bhinfcaptured" RED "rfinfcaptured" GREY "gsinfcaptured" BROWN "bdinfcaptured" AMBER "abinfcaptured" JADE "jsinfcaptured" COBALT "ciinfcaptured" PINK "pcinfcaptured" TEAL "tginfcaptured" PURPLE "plinfcaptured" ACID "arinfcaptured" WHITE "wninfcaptured"
#expand UnitMoveSounds MECH ROAD "move.foot.road" MOUNTAIN "move.foot.mount" WOODS "move.foot.woods" RIVER "move.foot.river" SHOAL "move.foot.shoal"

#expand UnitSprite RECON recon
#expand UnitPicture RECON recon

#expand UnitSprite TANK tank
#expand UnitPicture TANK tank

#expand UnitSprite MDTANK md
#expand UnitPicture MDTANK mdtank

#expand UnitSprite NEOTANK neo
#expand UnitPicture NEOTANK neotank

#expand UnitSprite MEGATANK mega
#expand UnitPicture MEGATANK megatank

#expand UnitSprite APC apc
#expand UnitPicture APC apc

#expand UnitSprite ANTIAIR anti
#expand UnitPicture ANTIAIR antiair

#expand UnitSprite ARTILLERY art
#expand UnitPicture ARTILLERY artillery

#expand UnitSprite ROCKETS rocket
#expand UnitPicture ROCKETS rockets

#expand UnitSprite MISSILES miss
#expand UnitPicture MISSILES missiles

#expand UnitSprite PIPERUNNER pipe
#expand UnitPicture PIPERUNNER piperunner

#expand UnitSprite TCOPTER t
#expand UnitPicture TCOPTER tcopter

#expand UnitSprite BCOPTER b
#expand UnitPicture BCOPTER bcopter

#expand UnitSprite FIGHTER fighter
#expand UnitPicture FIGHTER fighter

#expand UnitSprite BOMBER bomber
#expand UnitPicture BOMBER bomber

#expand UnitSprite STEALTH stealth
#expand UnitPicture STEALTH stealth

#expand UnitSprite BLACKBOMB bbomb
#expand UnitPicture BLACKBOMB blackbomb

#expand UnitSprite BLACKBOAT bboat
#expand UnitPicture BLACKBOAT blackboat

#expand UnitSprite LANDER lander
#expand UnitPicture LANDER lander

#expand UnitSprite CRUISER cruiser
#expand UnitPicture CRUISER cruiser

#expand UnitSprite SUB sub
#expand UnitPicture SUB sub

#expand UnitSprite BATTLESHIP battle
#expand UnitPicture BATTLESHIP battleship

#expand UnitSprite CARRIER carrier
#expand UnitPicture CARRIER carrier

#expand UnitSprite OOZIUM oozium
#expand UnitPicture OOZIUM oozium
#expand CountryOverrides UnitType OOZIUM destroyEffectSprite ORANGE "ooziumos" BLUE "ooziumbm" GREEN "ooziumge" YELLOW "ooziumyc" BLACK "ooziumbh" RED "ooziumrf" GREY "ooziumgs" BROWN "ooziumbd" AMBER "ooziumab" JADE "ooziumjs" COBALT "ooziumci" PINK "ooziumpc" TEAL "ooziumtg" PURPLE "ooziumpl" ACID "ooziumar" WHITE "ooziumwn"

namespace Weapon {
    namespace ROCKETS_PRIMARY {
        namespace UnitType {
            namespace PIPERUNNER {
                const string longName("PIPERUNNER_PRIMARY_WEAPON_longname");
                const string shortName("PIPERUNNER_PRIMARY_WEAPON_shortname");
                const string description("PIPERUNNER_PRIMARY_WEAPON_description");
                const int32 ammo = 9;
                const Vector2 range(2, 5);
                void unitTable(WeaponDamageArray& table) {
                    table.array.insertLast(WeaponDamage("FIGHTER", 65, -1));
                    table.array.insertLast(WeaponDamage("BOMBER", 75, -1));
                    table.array.insertLast(WeaponDamage("STEALTH", 75, -1));
                    table.array.insertLast(WeaponDamage("BCOPTER", 105, -1));
                    table.array.insertLast(WeaponDamage("TCOPTER", 105, -1));
                    table.array.insertLast(WeaponDamage("BLACKBOMB", 120, -1));
                }
            }
            namespace BATTLESHIP {
                const string longName("BATTLESHIP_PRIMARY_WEAPON_longname");
                const string shortName("BATTLESHIP_PRIMARY_WEAPON_shortname");
                const string description("BATTLESHIP_PRIMARY_WEAPON_description");
                const int32 ammo = 9;
                const Vector2 range(2, 6);
                void unitTable(WeaponDamageArray& table) {
                    for (uint i = 0, end = table.array.length(); i < end; ++i) {
                        if (table.array[i].target == "OOZIUM") {
                            table.array[i].damage = 20;
                        } else if (table.array[i].target == "CRUISER") {
                            table.array[i].damage = 95;
                        } else if (table.array[i].target == "SUB") {
                            table.array[i].damage = 95;
                        } else if (table.array[i].target == "BATTLESHIP") {
                            table.array[i].damage = 50;
                        } else if (table.array[i].target == "LANDER") {
                            table.array[i].damage = 95;
                        } else if (table.array[i].target == "BLACKBOAT") {
                            table.array[i].damage = 95;
                        }
                    }
                }
            }
        }
    }
    namespace MISSILES_PRIMARY {
        namespace UnitType {
            namespace CARRIER {
                const int32 ammo = 9;
                const Vector2 range(3, 8);
            }
        }
    }
}

namespace Terrain {
    namespace PLAINS {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 2; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace SEA {
        namespace MovementType {
            namespace AIR { const int32 movementCost = 1; }
            namespace SHIPS { const int32 movementCost = 1; }
            namespace TRANSPORT { const int32 movementCost = 1; }
        }
    }
    namespace ROAD {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace BRIDGE {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace MOUNTAIN {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 2; }
            namespace MECH { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace WOODS {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 3; }
            namespace TREAD { const int32 movementCost = 2; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace RIVER {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 2; }
            namespace MECH { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace REEF {
        namespace MovementType {
            namespace AIR { const int32 movementCost = 1; }
            namespace SHIPS { const int32 movementCost = 2; }
            namespace TRANSPORT { const int32 movementCost = 2; }
        }
    }
    namespace SHOAL {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace TRANSPORT { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace PIPE {
        namespace MovementType {
            namespace PIPELINE { const int32 movementCost = 1; }
        }
    }
    namespace PIPESEAM {
        namespace MovementType {
            namespace PIPELINE { const int32 movementCost = 1; }
        }
    }
    namespace MISSILESILO {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace CITY {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace BASE {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace PIPELINE { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace AIRPORT {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace PORT {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SHIPS { const int32 movementCost = 1; }
            namespace TRANSPORT { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace COMTOWER {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace LAB {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
    namespace HQ {
        namespace MovementType {
            namespace INFANTRY { const int32 movementCost = 1; }
            namespace MECH { const int32 movementCost = 1; }
            namespace TIRES { const int32 movementCost = 1; }
            namespace TREAD { const int32 movementCost = 1; }
            namespace AIR { const int32 movementCost = 1; }
            namespace SLIME { const int32 movementCost = 1; }
        }
    }
}

namespace Structure {
    namespace BHDSKYFORTRESS {
        namespace Commander {
            namespace JAKE {
                void dependent(DependentStructureTileArray& tiles) {
                    tiles.array[14].offset.x = 50;
                }
            }
        }
    }
}

namespace Environment {
    namespace NORMAL {
        namespace Weather {
            namespace CLEAR {
                void icon(string&out icon) {
                    icon += "-clear";
                }
            }
        }
        namespace Commander {
            namespace JAKE {
                void icon(string&out icon) {
                    icon += "-jake";
                }
            }
        }
    }
}

namespace Country {
    namespace ORANGE {
        namespace Environment {
            namespace _NORMAL {
                void longName(string&out name) {
                    name += "-normal";
                }
            }
        }
        namespace Weather {
            namespace CLEAR {
                void longName(string&out name) {
                    name += "-clear";
                }
            }
        }
        namespace Commander {
            namespace JAKE {
                void longName(string&out name) {
                    name += "-jake";
                }
            }
        }
    }
}

// // Don't use pod&out here, I think it frees it early after the function ends?
// void func(pod& passing, const pod&in around) {
//     passing.message = around.message;
// }

// void main(Coords simCoords) {
//     pod test();
//     pod another("another");
//     pod j(another);
//     info(test.message);
//     info(another.message);
//     info(j.message += "2");
//     if (another == j) {
//         info("Yes!");
//     }
//     info((test = another).message);
//     func(another, j);
//     info(another.message);
//     pod finalTest = another;
//     if (finalTest == another) info("EQUAL!");
//     info("?" + finalTest.message);
//     info(j.message);
//     simCoords.x += -0.5;
//     info(formatFloat(simCoords.x, "", 0, 4));
//     Coords copy = simCoords;
//     copy.x += 0.00001;
//     copy.y -= 0.000001;
//     if (copy == simCoords) info("YES!");
//     copy.z = 10000.78;
//     if (copy != simCoords) info("NO!");
//     CoordPair pair1(simCoords, copy);
//     info(formatFloat(pair1.b.z, "", 0, 4));
//     info("Zeroed");
//     Coords zeroTest;
//     info(formatFloat(zeroTest.x, "", 0, 4));
// }

// #expand Test a b c d e f g h i j k l m n o p q r s t u v w x y z
#expand Test 1  3 4 5 6 7 8 9 0 1 2 3 4 5 6

// #expand ThisIsATest

// #assert true;
// #assert true; "Proper"
// #assert true; "Proper
// #assert true; Proper"
// #assert true; "Pro""er"
// #assert true; Pro""er
// #assert true; None!
// #assert true; """
// #assert true; "
