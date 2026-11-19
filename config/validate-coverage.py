#!/usr/bin/env python3
"""Validate LCOV structure and exercised repository C sources before upload.
Does not repair function/line metadata inconsistencies or change report scope.
"""
import argparse
from pathlib import Path
import sys


def integer(value):
    if not value.isascii() or not value.isdecimal():
        raise ValueError("invalid nonnegative integer")
    return int(value)


def validate(report, source_root):
    source_root = Path(source_root).resolve()
    production_root = source_root / "src" / "mupnp"
    records = production_files = production_lines = production_hits = 0
    production_functions = 0
    current = None

    def finish(record):
        nonlocal records, production_files, production_lines, production_hits, production_functions
        for tag in ("LF", "LH", "FNF", "FNH"):
            if tag not in record:
                raise ValueError("record missing " + tag)
        if record["LF"] != len(record["lines"]) or record["LH"] != sum(n > 0 for n in record["lines"].values()):
            raise ValueError("line summary does not match DA records")
        if record["FNH"] > record["FNF"]:
            raise ValueError("function hits exceed function count")
        if record["FNF"] and not record["functions"]:
            raise ValueError("function summary has no definitions")
        records += 1
        source = Path(record["source"])
        if not source.is_absolute():
            source = source_root / source
        source = source.resolve()
        if not source.is_relative_to(production_root) or source.suffix not in (".c", ".h"):
            return
        if not source.is_file():
            raise ValueError("repository production source does not exist")
        length = len(source.read_bytes().splitlines())
        if any(line > length for line in record["lines"]):
            raise ValueError("production line lies outside real source file")
        if record["lines"]:
            production_files += 1
            production_lines += len(record["lines"])
            production_hits += sum(n > 0 for n in record["lines"].values())
            production_functions += record["FNF"]

    with Path(report).open(encoding="utf-8") as stream:
        for number, raw in enumerate(stream, 1):
            line = raw.rstrip("\r\n")
            try:
                if not line:
                    continue
                if line.startswith("TN:"):
                    if current is not None:
                        raise ValueError("new test name before end_of_record")
                    continue
                if line.startswith("SF:"):
                    if current is not None or not line[3:]:
                        raise ValueError("missing or nested source record")
                    current = {"source": line[3:], "lines": {}, "functions": set()}
                    continue
                if current is None:
                    raise ValueError("data outside source record")
                if line == "end_of_record":
                    finish(current)
                    current = None
                    continue
                tag, separator, value = line.partition(":")
                if not separator:
                    raise ValueError("malformed LCOV field")
                if tag == "DA":
                    fields = value.split(",")
                    if len(fields) not in (2, 3):
                        raise ValueError("malformed line record")
                    position, count = integer(fields[0]), integer(fields[1])
                    if position == 0 or position in current["lines"]:
                        raise ValueError("zero or duplicate line")
                    current["lines"][position] = count
                elif tag in ("LF", "LH", "FNF", "FNH"):
                    if tag in current:
                        raise ValueError("duplicate summary field")
                    current[tag] = integer(value)
                elif tag == "FNL":
                    index, start, end = value.split(",")
                    index, start, end = integer(index), integer(start), integer(end)
                    if start == 0 or end < start or index in current["functions"]:
                        raise ValueError("invalid function range")
                    current["functions"].add(index)
                elif tag == "FNA":
                    index, count, name = value.split(",", 2)
                    if integer(index) not in current["functions"] or not name:
                        raise ValueError("function alias lacks definition")
                    integer(count)
                elif tag == "FN":
                    start, name = value.split(",", 1)
                    start = integer(start)
                    fields = name.split(",", 1)
                    if len(fields) == 2 and fields[0].isascii() and fields[0].isdecimal():
                        if integer(fields[0]) < start:
                            raise ValueError("invalid function end line")
                        name = fields[1]
                    if start == 0 or not name:
                        raise ValueError("invalid function definition")
                    current["functions"].add(name)
                elif tag == "FNDA":
                    count, name = value.split(",", 1)
                    if name not in current["functions"]:
                        raise ValueError("function count lacks definition")
                    integer(count)
                elif tag not in ("VER", "BRDA", "BRF", "BRH"):
                    raise ValueError("unsupported LCOV field " + tag)
            except ValueError as error:
                raise ValueError(f"line {number}: {error}") from error
    if current is not None:
        raise ValueError("truncated record (missing end_of_record)")
    if not records or not production_files or not production_hits or not production_functions:
        raise ValueError("report lacks exercised real repository production lines/functions")
    return records, production_files, production_lines, production_hits, production_functions


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    parser.add_argument("--source-root", type=Path, required=True)
    args = parser.parse_args()
    try:
        records, files, lines, hits, functions = validate(args.report, args.source_root)
    except (OSError, UnicodeError, ValueError) as error:
        parser.exit(1, f"Coverage validation failed: {error}\n")
    print(f"Validated {records} LCOV records; production: {files} files, {hits}/{lines} hit lines, {functions} function definitions")
    print("Function metadata consistency is not repaired; retain the LCOV warnings when interpreting function coverage.")


if __name__ == "__main__":
    main()
