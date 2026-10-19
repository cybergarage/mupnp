# Release preparation and publication

The 3.1.4 preparation updates metadata after merged PR #59.
It includes the control point fix merged at
`f3f259305b655c5100214c4b221fd9204364ad31`.
It does not publish a tag or GitHub Release. Preserve the preparation PR's review
state; merging and publication are separate maintainer actions. The prior
3.1.3 preparation is [PR #58](https://github.com/cybergarage/mupnp/pull/58).

Version sources follow the 3.1.3 release: `configure.ac` / committed `configure`,
`include/mupnp/version.h`, the Doxygen version in `include/mupnp/upnp.h`, and
`idf_component.yml`. `mupnp.pc` and source archive names derive from Autotools
`VERSION`. CMake and the legacy Objective-C bundle have no synchronized release
version in the preceding release and keep their existing build conventions.

The committed configure script uses Autoconf 2.71. For a version-only update,
keep its generated version strings synchronized without unrelated generator
changes. Do not create backup or staging copies of the checkout. The current
`version.h.gen` script is legacy; check the resulting quoted `MUPNP_VER` value
rather than assuming regeneration produces a usable C string macro.

Before publishing:

1. Confirm the 3.1.4 preparation PR has merged, including the approved control
   point description-fetch changes from PR #59. Fetch `master` and identify
   the merged commit containing the complete approved 3.1.4 metadata changes.
2. Verify the merged commit's CI (Autotools build/check/coverage, ESP-IDF,
   platform matrix and gateway fixtures) and Sonar result. PR-head validation
   alone is not merged-commit validation. Re-run any missing required checks.
3. Confirm both `3.1.4` and `v3.1.4` are still unused as remote tags/releases.
   The established tag spelling is `3.1.4`, without a `v` prefix.
4. After publication is authorized, create an annotated `3.1.4` tag at that
   verified merged commit and push that tag only. Do not tag a PR-head commit
   or whichever commit happens to be local HEAD.
5. Create the GitHub Release from the existing tag, using the 3.1.4 section of
   `ChangeLog` as the release notes. The previous 3.1.3 release used the title
   `3.1.3`, target `master`, and no uploaded assets; GitHub provides source
   archives automatically. Follow the same convention with title `3.1.4`.
6. Verify the remote tag resolves to the intended commit, the release and source
   links are accessible, and the source metadata reports 3.1.4.

Publishing to the ESP Component Registry is outside this procedure. The existing
license-metadata review caveat in [the ESP-IDF guide](espidf.md) remains. No new
physical-device or ESP32 hardware validation is implied by a source release.
