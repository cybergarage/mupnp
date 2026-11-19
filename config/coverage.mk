# Project-owned trace/HTML entry points. AX_CODE_COVERAGE's existing local
# check-code-coverage and code-coverage-capture targets remain unchanged.
.PHONY: code-coverage-trace code-coverage-html

if CODE_COVERAGE_ENABLED
MUPNP_COVERAGE_PYTHON ?= python3

# CI runs make check first. Do not repeat tests or run genhtml here.
code-coverage-trace: code-coverage-capture-hook
	$(AM_V_at)$(RM) "$(CODE_COVERAGE_OUTPUT_FILE)" "$(CODE_COVERAGE_OUTPUT_FILE).tmp"
	$(code_coverage_v_lcov_cap)$(LCOV) $(code_coverage_quiet) $(addprefix --directory ,$(CODE_COVERAGE_DIRECTORY)) --capture --output-file "$(CODE_COVERAGE_OUTPUT_FILE).tmp" --test-name "$(call code_coverage_sanitize,$(PACKAGE_NAME)-$(PACKAGE_VERSION))" --no-checksum --compat-libtool $(CODE_COVERAGE_LCOV_SHOPTS) $(CODE_COVERAGE_LCOV_OPTIONS)
	$(code_coverage_v_lcov_ign)$(LCOV) $(code_coverage_quiet) $(addprefix --directory ,$(CODE_COVERAGE_DIRECTORY)) --remove "$(CODE_COVERAGE_OUTPUT_FILE).tmp" $(CODE_COVERAGE_IGNORE_PATTERN) --output-file "$(CODE_COVERAGE_OUTPUT_FILE)" $(CODE_COVERAGE_LCOV_SHOPTS) $(CODE_COVERAGE_LCOV_RMOPTS)
	$(AM_V_at)$(RM) "$(CODE_COVERAGE_OUTPUT_FILE).tmp"
	$(AM_V_at)$(MUPNP_COVERAGE_PYTHON) "$(top_srcdir)/config/validate-coverage.py" "$(CODE_COVERAGE_OUTPUT_FILE)" --source-root "$(abs_top_srcdir)"

# Render an existing trace; never recapture or repeat tests.
code-coverage-html:
	@test -s "$(CODE_COVERAGE_OUTPUT_FILE)"
	$(code_coverage_v_genhtml)LANG=C $(GENHTML) $(code_coverage_quiet) $(addprefix --prefix ,$(CODE_COVERAGE_DIRECTORY)) --output-directory "$(CODE_COVERAGE_OUTPUT_DIRECTORY)" --title "$(PACKAGE_NAME)-$(PACKAGE_VERSION) Code Coverage" --legend --show-details "$(CODE_COVERAGE_OUTPUT_FILE)" $(CODE_COVERAGE_GENHTML_OPTIONS)
	@echo "file://$(abs_builddir)/$(CODE_COVERAGE_OUTPUT_DIRECTORY)/index.html"
else
code-coverage-trace code-coverage-html:
	@echo 'Configure with --enable-code-coverage first' >&2; exit 1
endif
