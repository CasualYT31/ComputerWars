USAGE: str = """
Small script that generates a macro that defines a Game Property Class with the given number of fields.
Usage: py ./macgen.py 7
Will generate a macro defining a class with 7 fields, and copy it to the clipboard.
"""

from sys import argv
from pyperclip import copy

try:
    FC = int(argv[1])
    if FC < 1: raise SystemExit
except:
    print(USAGE)
    raise SystemExit

def gen(tmpl: str) -> str:
    ret: str = ''
    for i in range(1, FC + 1): ret += tmpl.replace('~', f'{i}')
    return ret

copy(f"""

#define GAME_PROPERTY_{FC}(cc, ac, i, {gen('p~, t~, ')}e) \\
    GAME_PROPERTY_DECLARE(cc) \\
    {gen('    p~(j, logger) \\\n    ')}GAME_PROPERTY_REGISTER(cc, ac, e) \\
    {gen('    p~##_::Register(engine); \\\n    ')}GAME_PROPERTY_SCRIPTNAME() \\
{gen('    PROPERTY(cc, ac, p~, t~, i) \\\n')}}};""")

print("Macro now copied to clipboard!")
