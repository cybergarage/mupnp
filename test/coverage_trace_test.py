"""In-memory/local-file negative cases for the coverage upload gate."""
import importlib.util
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


if __name__ == "__main__":
    unittest.main()
