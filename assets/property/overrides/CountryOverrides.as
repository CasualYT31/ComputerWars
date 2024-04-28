#template CountryOverrides
/* Expected parameters:
$1 = Base Game Property Type
$2 = Base Script Name
$3 = Name of field that's being overridden
$4, $6, $8, etc. = Country script names
$5, $7, $9, etc. = Value to override with, string literal
*/
#assert $0 == country.length() * 2 + 3; "Invalid number of params!"
namespace $1 {
    namespace $2 {
        namespace Country {
            `4`$`2`namespace $$a {
                const string $3($$b);
            }`
            `
        }
    }
}
