USAGE: str = """
Small script that generates macros that define a Game Property Class with the given number of fields.
Usage: py ./macgen.py 7
Will generate a macro defining a class with 7 fields, and copy it to the clipboard.
py ./macgen.py 2 7 10
Will generate three macros defining classes with 2, 7, and 10 fields respectively, and copy it to the clipboard.
"""

from sys import argv
from pyperclip import copy

try:
    for p in argv[1:]:
        test = int(p)
        if test < 1: raise SystemExit
except:
    print(USAGE)
    raise SystemExit

def gen(tmpl: str, FC: int) -> str:
    ret: str = ''
    for i in range(1, FC + 1): ret += tmpl.replace('~', f'{i}')
    return ret

MACROS: str = ""

for p in argv[1:]:
    FC = int(p)
    MACROS += f"""

#define GAME_PROPERTY_{FC}(cc, ac, gp, i, {gen('p~, t~, e~, ', FC)}e, a) \\
    GAME_PROPERTY_DECLARE(cc) \\
    {gen('    p~(j, logger), \\\n    ', FC)}GAME_PROPERTY_REGISTER(cc, ac, e) \\
    {gen('    p~##_::Register(engine); \\\n    ', FC)}GAME_PROPERTY_SCRIPTNAME() \\
{gen('    PROPERTY(cc, ac, p~, t~, i, e~) \\\n', FC)}    GAME_PROPERTY_END(cc, ac, gp, a)"""

copy(MACROS)

print(f"Macro{'s' if len(argv) > 2 else ''} now copied to clipboard!")
