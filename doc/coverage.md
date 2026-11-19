# Coverage trace and optional HTML

Configure with `--enable-code-coverage` and build first. CI runs `make check`
once, then:

```sh
make code-coverage-trace CODE_COVERAGE_OUTPUT_FILE=mupnp-coverage.info
```

The project-owned target in `config/coverage.mk` captures and removes records
using the existing configured LCOV options/patterns. It removes stale output
first, propagates command failures, and validates the final trace with Python 3.9 or later.
It does not run tests or genhtml. The validator requires complete records,
consistent line summaries, nonnegative counts and exercised line/function
records from real `src/mupnp` C sources. It rejects empty, truncated, test-only,
missing-source and out-of-bounds production reports. It does not repair the
known function-hit/no-contained-line metadata inconsistencies: LCOV warnings
remain visible, and function coverage must be interpreted with that limitation.
Existing patterns are preserved; test/system records are not newly excluded.

The existing Codecov action uploads only `mupnp-coverage.info`, with automatic
file search disabled and upload errors treated as failures. Destination and
token source remain unchanged. Tests, trace capture/removal or validation
failure prevent upload. No coverage fixture data or private logs are committed.

After the upload attempt, a separate CI step renders that existing trace:

```sh
make code-coverage-html CODE_COVERAGE_OUTPUT_FILE=mupnp-coverage.info
```

It runs when trace validation succeeded, even if upload failed, and uses the
existing genhtml options without new suppression. HTML failure remains a failed
step/job result; it cannot prevent an earlier upload. A successful upload does
not establish successful HTML generation or fully consistent function metadata.

The AX_CODE_COVERAGE local `make check-code-coverage` and
`make code-coverage-capture` targets retain their original test/capture/HTML
behavior. The new targets live outside the generated Autoconf Archive include,
so bootstrap cannot silently overwrite the CI separation.
