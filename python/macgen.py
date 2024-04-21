from argparse import ArgumentParser, ArgumentTypeError, RawTextHelpFormatter
from os.path import isfile, isdir

def in_file_type(x: str) -> str:
    if not isfile(x):
        raise ArgumentTypeError(f"Input file \"{x}\" does not exist!")
    return x

def out_file_type(x: str) -> str:
    if isdir(x):
        raise ArgumentTypeError(f"Output file \"{x}\" already exists as a directory!")
    return x

def repeat_type(x: str) -> int:
    x = int(x)
    if x < 1:
        raise ArgumentTypeError("You must repeat the template at least one time")
    return x

def main(args=None):
    parser = ArgumentParser(
        prog="C++ Macro File Generator",
        description="Generates a C++ code file based on a given template",
        epilog="""The final code file will be composed like so:

    [Header]
    Template N=1
    Template N=2
    ...
    Template N=repeat

    Each template is divided up into sections delimited by a ` character. In
    odd sections (i.e. the first section, the third section, etc.), all
    occurrences of the $ character will be replaced with N. Even sections (i.e.
    (the second section, the fourth section, etc.), will be repeated N times, and
    all occurrences of $ characters will be replaced by the one-based iteration
    index. Even sections can also optionally be split up using £ characters, and
    if there's at least two subsections, the right-most subsection will NOT be
    included in the final code file, if $ is at the final index.

    For example, the following template:

    This template has been insert $ times so far.
    `$$`
    This is the third section.`
    Repeat sections can also contain newlines.`
    `if (i == $) return $£;`
    else return 0;

    Will produce the following final code, if N == 3:

    This template has been insert 1 times so far.
    11
    This is the third section.
    Repeat sections can also contain newlines.
    if (i == 1) return 1
    else return 0;
    This template has been insert 2 times so far.
    1122
    This is the third section.
    Repeat sections can also contain newlines.
    Repeat sections can also contain newlines.
    if (i == 1) return 1;if (i == 2) return 2
    else return 0;
    This template has been insert 3 times so far.
    112233
    This is the third section.
    Repeat sections can also contain newlines.
    Repeat sections can also contain newlines.
    Repeat sections can also contain newlines.
    if (i == 1) return 1;if (i == 2) return 2;if (i == 3) return 3
    else return 0;
    """,
        formatter_class=RawTextHelpFormatter
    )
    parser.add_argument(
        "template",
        help="File containing the template to generate the final code file with",
        type=in_file_type
    )
    parser.add_argument(
        "repeat",
        help="The number of times to repeat the template in the final code file",
        type=repeat_type
    )
    parser.add_argument(
        "-o", "--output",
        help="Where the final code file should be written to. "
            "If not provided, the code will be output to the terminal",
        type=out_file_type
    )
    parser.add_argument(
        "-y", "--yes",
        help="Always overwrite the output file",
        action="store_true"
    )
    parser.add_argument(
        "--header",
        help="File containing code to insert as the header of the resulting code file",
        type=in_file_type
    )
    parser.add_argument(
        "-c", "--copy",
        help="If provided, the final code file will be copied to the clipboard "
            "(REQUIRES PYPERCLIP)",
        action="store_true"
    )
    args = parser.parse_args(args)
    if not args.yes and args.output is not None and isfile(args.output):
        answer = input(f"Output file \"{args.output}\" already exists, "
                    f"do you wish to overwrite (y/n (default is n))? ")
        if answer.strip().lower() != "y": exit()

    def load_file(path: str) -> str:
        try:
            with open(path, mode="r", encoding="utf-8") as f:
                return f.read()
        except:
            print(f"Could not load input file {path}:")
            raise

    TEMPLATE: str = load_file(args.template).split('`')

    def generate(FC: int) -> str:
        # Build macro using the template.
        macro: str = ""
        for i, t in enumerate(TEMPLATE):
            if i % 2 == 0:
                # Every element with an even index uses $ substitution as normal,
                # where every occurrence of $ is replaced with FC.
                macro += t.replace('$', f'{FC}')
            else:
                # Every element with an odd index also uses $ substitution,
                # but the string is also repeated FC times. Each iteration has a
                # different $ substitution, starting with 1 and ending with FC.
                # E.g. first iteration replaces $ with 1, second with 2, etc.
                # `p$, t$` with FC of 3 becomes: p1, t1p2, t2p3, t3
                # Before this step, the string is split using £. The entire string
                # will undergo the above $ substitution, but the right-most £
                # section will NOT be included if x == the last in the FC range.
                sections = t.split('£')
                for x in range(1, FC + 1):
                    for j, section in enumerate(sections):
                        if len(sections) > 1 and j == len(sections) - 1 and \
                            x == FC: break
                        macro += section.replace('$', f'{x}')
        return macro

    GENERATED_FILE: str = "" if args.header is None else load_file(args.header)
    for p in range(1, args.repeat + 1): GENERATED_FILE += f"\n{generate(p)}"

    if args.output is None:
        print(GENERATED_FILE)
    else:
        try:
            with open(args.output, mode='w', encoding='utf-8') as f:
                f.write(GENERATED_FILE)
            print(f"Final code file has been written to:\n{args.output}")
        except:
            print(f"Couldn't write to {args.output}:")
            raise

    if args.copy:
        from pyperclip import copy
        copy(GENERATED_FILE)
        print(f"Final code file has been copied to clipboard")

if __name__ == "__main__":
    main()
