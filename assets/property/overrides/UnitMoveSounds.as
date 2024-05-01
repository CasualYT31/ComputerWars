#template UnitMoveSounds
/* Expected parameters:
$1 = Base (UnitType) Script Name
$2, $4, $6, etc. = Overrider (Terrain) Script Name
$3, $5, $7, etc. = Value to override with, string literal
*/
#assert $0 % 2 == 1; "Each terrain script name must be paired with a string literal!"
namespace UnitType {
    namespace $1 {
        namespace Terrain {
            `2`$`2`namespace $$a {
                void soundInfo(UnitSoundInfo& info) {
                    info.move = $$b;
                }
            }`
            `
        }
    }
}
