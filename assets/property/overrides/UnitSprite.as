#template UnitSprite
/* Expected parameters:
$1 = UnitType script name.
$2 = Prefix to provide for each country's sprite key.
*/
#assert $0 == 2; "Invalid number of params!"
#expand UnitSpriteImpl $1 ORANGE "$2os" BLUE "$2bm" GREEN "$2ge" YELLOW "$2yc" BLACK "$2bh" RED "$2rf" GREY "$2gs" BROWN "$2bd" AMBER "$2ab" JADE "$2js" COBALT "$2ci" PINK "$2pc" TEAL "$2tg" PURPLE "$2pl" ACID "$2ar" WHITE "$2wn"