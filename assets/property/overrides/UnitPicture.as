#template UnitPicture
/* Expected parameters:
$1 = UnitType script name.
$2 = Postfix to provide for each country's sprite key.
*/
#assert $0 == 2; "Invalid number of params!"
#expand CountryOverrides UnitType $1 picture ORANGE "os$2" BLUE "bm$2" GREEN "ge$2" YELLOW "yc$2" BLACK "bh$2" RED "rf$2" GREY "gs$2" BROWN "bd$2" AMBER "ab$2" JADE "js$2" COBALT "ci$2" PINK "pc$2" TEAL "tg$2" PURPLE "pl$2" ACID "ar$2" WHITE "wn$2"