# Repository Guidelines

## Project Structure & Module Organization
Core C sources live in `src/mupnp/`, with public headers under `include/mupnp/`. Static library build logic is in `lib/` (`lib/unix` for autotools). Unit/integration tests are in `test/` (primary runner: `test/unix`). Sample applications are in `examples/` (`upnpdump`, `upnpigddump`, `upnpavdump`, `clock`). Optional Objective-C wrapper code is in `wrapper/objc/`, and docs/Doxygen inputs are in `doc/`.

## Build, Test, and Development Commands
- `./bootstrap` regenerates autotools files (used in CI before configure).
- `./configure --enable-test --enable-examples` configures local dev builds.
- `make -j$(nproc)` builds library, tests, and enabled examples.
- `make check` runs the `mupnptest` suite from `test/unix`.
- `./coverage` runs a full coverage workflow (`configure`, `make`, `make check`, `make check-code-coverage`).
- `cmake -S . -B build && cmake --build build` builds the static library with CMake.

## Coding Style & Naming Conventions
Use 2-space indentation and Stroustrup-style braces (`.clang-format`, based on WebKit). Run the repository formatter script before submitting:

```bash
./clang-format
```

`clang-tidy` naming rules expect:
- functions: `lower_case`
- constants: `UPPER_CASE`
- structs/enums/classes: `CamelCase`
- variables/parameters/members: `camelBack`

Follow existing legacy API prefixes (`mupnp_`, `CG*`) where applicable.

## Testing Guidelines
Add tests in `test/` and wire them into `test/unix/Makefile.am` so they run via `make check`. Prefer naming files `*Test.cpp` (e.g., `NetworkTest.cpp`, `ServiceTest.cpp`) and keep fixtures in shared helper files (`TestDevice.*`) when reusable. Validate changes with `make check`; for protocol/path coverage changes, also run `make check-code-coverage`.

## Commit & Pull Request Guidelines
Recent history favors short, imperative subjects (`Update README.md`) and occasional Conventional Commit scopes (`fix(cmake): ...`, `chore(cmake): ...`). Use one clear subject line, keep it focused, and explain behavior changes in the body when needed.

PRs should include:
- what changed and why
- impacted modules/paths
- exact validation commands run (e.g., `make check`)
- linked issue(s) when applicable
- logs/screenshots only when they add diagnostic value
