"""In-memory/local-file negative cases for the coverage upload gate."""
import importlib.util
import io
import subprocess
import sys
from pathlib import Path
import tempfile
import unittest

spec = importlib.util.spec_from_file_location("coverage_validator", Path(__file__).resolve().parents[1] / "config/validate-coverage.py")
validator = importlib.util.module_from_spec(spec)
spec.loader.exec_module(validator)


class CoverageTraceTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        source = self.root / "src/mupnp/example.c"
        source.parent.mkdir(parents=True)
        source.write_text("int value(void) {\n  return 1;\n}\n")
        self.report = self.root / "coverage.info"
        self.valid = "TN:fixture\nSF:src/mupnp/example.c\nFNL:0,1,3\nFNA:0,1,value\nFNF:1\nFNH:1\nDA:1,1\nDA:2,1\nLF:2\nLH:2\nend_of_record\n"

    def check(self, text):
        self.report.write_text(text)
        return validator.validate(self.report, self.root)

    def test_real_production_record_and_legacy_functions(self):
        self.assertEqual(self.check(self.valid), (1, 1, 2, 2, 1))
        self.assertEqual(self.check(self.valid.replace("FNL:0,1,3\nFNA:0,1,value", "FN:1,3,value\nFNDA:1,value")), (1, 1, 2, 2, 1))

    def test_truncation_empty_and_test_only_fail(self):
        for text in ("", self.valid.replace("end_of_record\n", ""), self.valid.replace("src/mupnp/example.c", "test/example.c")):
            with self.subTest(text=text), self.assertRaises(ValueError):
                self.check(text)

    def test_nonexistent_escape_and_out_of_bounds_sources_fail(self):
        for text in (self.valid.replace("example.c", "missing.c"), self.valid.replace("src/mupnp/example.c", "src/mupnp/../../example.c"), self.valid.replace("DA:2,1", "DA:99,1")):
            with self.subTest(text=text), self.assertRaises(ValueError):
                self.check(text)

    def test_invalid_counts_duplicate_lines_and_zero_hits_fail(self):
        for text in (self.valid.replace("DA:2,1", "DA:2,-1"), self.valid.replace("DA:2,1", "DA:1,1"), self.valid.replace("LH:2", "LH:0"), self.valid.replace("DA:1,1", "DA:1,0").replace("DA:2,1", "DA:2,0").replace("LH:2", "LH:0")):
            with self.subTest(text=text), self.assertRaises(ValueError):
                self.check(text)

    def test_known_function_line_inconsistency_is_not_repaired(self):
        # One hit function with no hit line remains allowed as metadata, provided
        # other real production lines were exercised. LCOV reports its warning.
        text = self.valid.replace("FNF:1", "FNL:1,3,3\nFNA:1,1,inconsistent\nFNF:2").replace("FNH:1", "FNH:2")
        self.assertEqual(self.check(text), (1, 1, 2, 2, 2))

    def test_parsing_is_separate_from_semantic_validation(self):
        text = self.valid.replace("LH:2", "LH:0")
        records = list(validator.parse_records(io.StringIO(text)))
        self.assertEqual(len(records), 1)
        self.assertEqual(records[0][1], 11)
        with self.assertRaisesRegex(ValueError, "line 11: line summary does not match DA records"):
            self.check(text)

    def test_parse_failure_keeps_its_input_line_number(self):
        text = self.valid.replace("DA:2,1", "DA:2,-1")
        with self.assertRaisesRegex(ValueError, "line 8: invalid nonnegative integer"):
            list(validator.parse_records(io.StringIO(text)))

    def run_cli(self):
        return subprocess.run([sys.executable, validator.__file__, str(self.report), "--source-root", str(self.root)], capture_output=True, text=True)

    def test_cli_malformed_utf8_is_rejected_without_traceback(self):
        self.report.write_bytes(self.valid.encode("utf-8") + b"\xff")
        result = self.run_cli()
        self.assertEqual(result.returncode, 1)
        self.assertEqual(result.stdout, "")
        self.assertIn("Coverage validation failed:", result.stderr)
        self.assertIn("utf-8", result.stderr)
        self.assertNotIn("Traceback", result.stderr)

    def test_cli_status_and_output_for_success_parse_and_semantic_failures(self):
        self.report.write_text(self.valid)
        result = self.run_cli()
        self.assertEqual(result.returncode, 0)
        self.assertIn("Validated 1 LCOV records; production: 1 files, 2/2 hit lines, 1 function definitions", result.stdout)
        self.assertEqual(result.stderr, "")
        for text in ("DA:1,1\n", self.valid.replace("LH:2", "LH:0")):
            with self.subTest(text=text):
                self.report.write_text(text)
                result = self.run_cli()
                self.assertEqual(result.returncode, 1)
                self.assertEqual(result.stdout, "")
                self.assertIn("Coverage validation failed:", result.stderr)
                self.assertNotIn("Traceback", result.stderr)


if __name__ == "__main__":
    unittest.main()
