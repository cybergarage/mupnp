# Scanner image

Build from the repository root with BuildKit (Docker Desktop / `docker buildx`):

```sh
docker build -f .oss-scanner/Dockerfile -t mupnp-scanner .
docker run --rm --network none mupnp-scanner id
```

The image keeps the existing `/src`, `/src/build`, `/src/build-asan` layout and
all five CMake examples. Package installation runs as root, then both builds,
smoke checks and the default runtime use `scanner:scanner` (UID/GID 10001).
`/src`, the build trees, `/home/scanner` and `/tmp` are writable by that user.

Explicit copies and the Dockerfile-specific ignore allowlist admit C/C++ source,
headers, CMake files and the advertised host scripts, not the whole checkout.
Root `.git`, local configuration, arbitrary context files, old build trees and
nested `.env`/key/JSON files are excluded before transfer to the builder. Source
files admitted by the allowlist remain trusted inputs; do not put secrets into
source code. Revisit the allowlist when build inputs change.

The supported contract is the source and binaries baked into this image.
A bind mount over `/src` hides those binaries, as with any overlaid directory.
For externally mounted source, provide files readable by UID 10001 and build
out of tree into a writable location such as `/tmp/build`. A read-only source
mount is supported this way; it cannot support an in-source `/src/build` write.
The image does not change host ownership or repair permissions on mounted files.
External Scanner orchestration is not present in this repository, so any
additional mount convention must be validated with that service separately.

`python3 .oss-scanner/verify.py` builds a temporary test image using synthetic
excluded files, verifies its user/content, executes offline fixtures and checks
read-only source-mount behavior. It removes its image and synthetic files; it
never prunes shared Docker caches or changes Docker configuration.

The Dockerfile's historical smoke wrappers still print environment-dependent
CTest failures without rejecting the build. The verification script separately
requires the relevant offline tests and host regressions to pass.
