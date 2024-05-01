#template UnitSpriteImpl
/* Expected parameters:
$1 = UnitType Script Name
$2, $4, $6, etc. = Country script names
$3, $5, $7, etc. = Value to override with, string literal
*/
#assert $0 == country.length() * 2 + 1; "Invalid number of params!"
namespace UnitType {
    namespace $1 {
        namespace Country {
            `2`$`2`namespace $$a {
                void spriteInfo(UnitSpriteInfo& info) {
                    info.sprite = $$b;
                    SELECTED_SHEET_FOR_COUNTRY.get("$$a", info.selectedSheet);
                }
            }`
            `
        }
    }
}
