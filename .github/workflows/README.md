# CI in this fork

This is a LASP fork of Eigen 3.4. The CI here is deliberately small: it covers what
the fork actually needs rather than upstream's full validation matrix.

| Workflow | What it does |
|---|---|
| `smoketests.yml` | Builds and runs Eigen's smoke test subset (`cmake/EigenSmokeTestList.cmake`, 105 resolve in the CI configuration) under gcc-13 and clang-18, with `EIGEN_TEST_CXX11` on and off. |

Upstream's own CI is GitLab and lives in `.gitlab-ci.yml` plus `ci/`. It still runs
against upstream; nothing here replaces it.

## Why the full test matrix is not run here

`.github/workflows/tests.yml` used to define a 16-entry matrix mirroring
`ci/README.md` (4 compilers x cxx11 on/off x the `Official` and `Unsupported`
labels). It was removed because it could not work on GitHub-hosted runners:

- **It never ran successfully.** Every job died in `actions/checkout`, because the
  jobs ran in `container: ubuntu:18.04` (glibc 2.27) and GitHub now forces
  JavaScript actions onto Node 24, which needs glibc >= 2.28:
  `/__e/node24/bin/node: ... version 'GLIBC_2.28' not found`.
- Two further problems sat behind that one. They were never actually reached,
  because checkout failed first, but both would have to be solved to revive the
  matrix: Ubuntu 18.04 is EOL, so its apt archives have moved off
  `archive.ubuntu.com`; and `gcc-4.8`/`clang-10` are not in bionic and are not
  published by `ppa:ubuntu-toolchain-r/test` (clang needs `apt.llvm.org`), so half
  the matrix names packages that cannot be installed.
- **The full suite is too large for hosted runners.** `buildtests` covers 282
  `ei_add_test` calls, each expanding into several binaries — hours per job, times
  16 entries.

Upstream ran that same image successfully because GitLab executed it on Eigen's
self-hosted runners, which inject no Node helper into the container.

To run the full suite locally:

```sh
mkdir builddir && cd builddir
cmake -G Ninja -DEIGEN_TEST_CXX11=on ..
cmake --build . --target buildtests --parallel
ctest -L Official -j "$(nproc)"      # or: -L Unsupported
```

Restoring old-compiler coverage (`gcc-4.8`, `clang-10`) needs either a self-hosted
runner or purpose-built container images published to a registry; it cannot be done
with the stock hosted images.

## Note on `.gitignore`

`.gitignore` carries an upstream `*build*` pattern that also matched
`.github/workflows/build.yml`, silently keeping a workflow out of the repository.
A `!.github/**` negation now prevents that. Watch for it when adding files whose
names contain "build".
