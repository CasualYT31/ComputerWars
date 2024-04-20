from argparse import ArgumentParser
from os import chdir
from os.path import dirname, abspath

# Change the CWD to the root of the repository so this script
# will function correctly when executed from anywhere.
chdir(dirname(abspath(__file__)))

from python.macgen import main, repeat_type

GAME_PROPERTIES_PATH: str = "./awe/include/tpp/bank-v2-macros.tpp"
GAME_PROPERTIES_HEADER: str = "./python/gamepropertyheader.cpp"
GAME_PROPERTIES_TEMPLATE: str = "./python/gameproperty.cpp"

PLAIN_OLD_DATATYPES_PATH: str = "./engine/include/tpp/pod.tpp"
PLAIN_OLD_DATATYPES_HEADER: str = "./python/podheader.cpp"
PLAIN_OLD_DATATYPES_TEMPLATE: str = "./python/pod.cpp"

parser = ArgumentParser(
    prog="Computer Wars C++ Macro File Generator",
    description="Generates C++ macro files for the C++ Computer Wars engine",
    epilog="Please keep this script at the root of the repo! "
           f"See ./python/macgen.py for detailed information"
)
parser.add_argument(
    "-g", "--gp", "--game-properties",
    help="The number of GAME_PROPERTY_N macros to generate. "
         f"These will be written to: {GAME_PROPERTIES_PATH}",
    type=repeat_type
)
parser.add_argument(
    "-p", "--pod", "--plain-old-datatypes",
    help="The number of POD_N macros to generate. "
         f"These will be written to: {PLAIN_OLD_DATATYPES_PATH}",
    type=repeat_type
)
args = parser.parse_args()

if args.gp is not None:
    main([
        GAME_PROPERTIES_TEMPLATE,
        str(args.gp),
        "--output",
        GAME_PROPERTIES_PATH,
        "--header",
        GAME_PROPERTIES_HEADER,
        "--yes"
    ])

if args.pod is not None:
    main([
        PLAIN_OLD_DATATYPES_TEMPLATE,
        str(args.pod),
        "--output",
        PLAIN_OLD_DATATYPES_PATH,
        "--header",
        PLAIN_OLD_DATATYPES_HEADER,
        "--yes"
    ])
