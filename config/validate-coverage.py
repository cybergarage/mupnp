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


def parse_line_data(record, value):
    fields = value.split(",")
    if len(fields) not in (2, 3):
        raise ValueError("malformed line record")
    position, count = integer(fields[0]), integer(fields[1])
    if position == 0 or position in record["lines"]:
        raise ValueError("zero or duplicate line")
    record["lines"][position] = count


def parse_function_range(record, value):
    index, start, end = value.split(",")
    index, start, end = integer(index), integer(start), integer(end)
    if start == 0 or end < start or index in record["functions"]:
        raise ValueError("invalid function range")
    record["functions"].add(index)


def parse_function_alias(record, value):
    index, count, name = value.split(",", 2)
    if integer(index) not in record["functions"] or not name:
        raise ValueError("function alias lacks definition")
    integer(count)


def parse_legacy_function(record, value):
    start, name = value.split(",", 1)
    start = integer(start)
    fields = name.split(",", 1)
    if len(fields) == 2 and fields[0].isascii() and fields[0].isdecimal():
        if integer(fields[0]) < start:
            raise ValueError("invalid function end line")
        name = fields[1]
    if start == 0 or not name:
        raise ValueError("invalid function definition")
    record["functions"].add(name)


def parse_legacy_function_count(record, value):
    count, name = value.split(",", 1)
    if name not in record["functions"]:
        raise ValueError("function count lacks definition")
    integer(count)


SUMMARY_TAGS = ("LF", "LH", "FNF", "FNH")
FIELD_PARSERS = {
    "DA": parse_line_data,
    "FNL": parse_function_range,
    "FNA": parse_function_alias,
    "FN": parse_legacy_function,
    "FNDA": parse_legacy_function_count,
}


def parse_field(record, line):
    tag, separator, value = line.partition(":")
    if not separator:
        raise ValueError("malformed LCOV field")
    if tag in SUMMARY_TAGS:
        if tag in record:
            raise ValueError("duplicate summary field")
        record[tag] = integer(value)
    elif tag in FIELD_PARSERS:
        FIELD_PARSERS[tag](record, value)
    elif tag not in ("VER", "BRDA", "BRF", "BRH"):
        raise ValueError("unsupported LCOV field " + tag)


def parse_record_line(current, line):
    """Return (current record, completed record) without source-file checks."""
    if not line:
        return current, None
    if line.startswith("TN:"):
        if current is not None:
            raise ValueError("new test name before end_of_record")
        return current, None
    if line.startswith("SF:"):
        if current is not None or not line[3:]:
            raise ValueError("missing or nested source record")
        return {"source": line[3:], "lines": {}, "functions": set()}, None
    if current is None:
        raise ValueError("data outside source record")
    if line == "end_of_record":
        return None, current
    parse_field(current, line)
    return current, None


def parse_records(stream):
    """Yield complete LCOV records with their closing line for diagnostics."""
    current = None
    for number, raw in enumerate(stream, 1):
        try:
            current, completed = parse_record_line(current, raw.rstrip("\r\n"))
        except ValueError as error:
            raise ValueError(f"line {number}: {error}") from error
        if completed is not None:
            yield completed, number
    if current is not None:
        raise ValueError("truncated record (missing end_of_record)")


def validate_record_summary(record):
    for tag in SUMMARY_TAGS:
        if tag not in record:
            raise ValueError("record missing " + tag)
    if record["LF"] != len(record["lines"]) or record["LH"] != sum(n > 0 for n in record["lines"].values()):
        raise ValueError("line summary does not match DA records")
    if record["FNH"] > record["FNF"]:
        raise ValueError("function hits exceed function count")
    if record["FNF"] and not record["functions"]:
        raise ValueError("function summary has no definitions")


def production_counts(record, source_root):
    source = Path(record["source"])
    if not source.is_absolute():
        source = source_root / source
    source = source.resolve()
    production_root = source_root / "src" / "mupnp"
    if not source.is_relative_to(production_root) or source.suffix not in (".c", ".h"):
        return (0, 0, 0, 0)
    if not source.is_file():
        raise ValueError("repository production source does not exist")
    length = len(source.read_bytes().splitlines())
    if any(line > length for line in record["lines"]):
        raise ValueError("production line lies outside real source file")
    if not record["lines"]:
        return (0, 0, 0, 0)
    return (1, len(record["lines"]), sum(n > 0 for n in record["lines"].values()), record["FNF"])


def validated_counts(record, number, source_root):
    try:
        validate_record_summary(record)
        return (1, *production_counts(record, source_root))
    except ValueError as error:
        raise ValueError(f"line {number}: {error}") from error


def validate(report, source_root):
    source_root = Path(source_root).resolve()
    totals = (0, 0, 0, 0, 0)
    with Path(report).open(encoding="utf-8") as stream:
        for record, number in parse_records(stream):
            counts = validated_counts(record, number, source_root)
            totals = tuple(total + count for total, count in zip(totals, counts))
    if not totals[0] or not totals[1] or not totals[3] or not totals[4]:
        raise ValueError("report lacks exercised real repository production lines/functions")
    return totals


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("report", type=Path)
    parser.add_argument("--source-root", type=Path, required=True)
    args = parser.parse_args()
    try:
        records, files, lines, hits, functions = validate(args.report, args.source_root)
    except (OSError, ValueError) as error:
        parser.exit(1, f"Coverage validation failed: {error}\n")
    print(f"Validated {records} LCOV records; production: {files} files, {hits}/{lines} hit lines, {functions} function definitions")
    print("Function metadata consistency is not repaired; retain the LCOV warnings when interpreting function coverage.")


if __name__ == "__main__":
    main()
