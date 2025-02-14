import argparse
import re
import pathlib

CHANGELOG_PATH = pathlib.Path(__file__).parent.parent.joinpath("CHANGELOG.md")

# Regular expressions for searched contents
VERSION_RE = re.compile(r'^v?(?P<version>[0-9]+\.[0-9]+\.[0-9]+)$')

def CURRENT_CHANGELOG_RE(version: str):
    return re.compile(
        r'(?P<current_changelog>##\s+\[' + re.escape(version) + r'\].+?)\s*(?:\n##\s+\[[0-9]+\.[0-9]+\.[0-9]+\]|$)',
        re.DOTALL
    )

# Handling of input argument
def version_type(arg):
    res = VERSION_RE.search(arg)
    if not res:
        raise argparse.ArgumentTypeError("Wrong format of version.")
    return res.group("version")

if __name__ == "__main__":
    # Register argument parser, argument and parse
    parser = argparse.ArgumentParser(
        description = "Parses CHANGELOG.md and extracts changes for a specific version."
    )

    parser.add_argument(
        "-v", "--version",
        help     = "Specify number of the version.",
        action   = "store",
        type     = version_type,
        required = True
    )

    args = parser.parse_args()

    # Search for given version in changelog
    parsed_changelog = CURRENT_CHANGELOG_RE(args.version).search(CHANGELOG_PATH.read_text())
    if not parsed_changelog:
        raise ValueError(f"Could not find version '{args.version}' in file '{CHANGELOG_PATH}'.")

    # Write parsed changelog to file
    out_path = pathlib.Path(CHANGELOG_PATH.parent.joinpath(f"CHANGELOG_parsed.md"))
    out_path.write_text(parsed_changelog.group("current_changelog"))