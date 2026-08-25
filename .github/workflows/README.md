# CI in this fork

This is a LASP fork of Eigen 3.4 whose purpose is freestanding support, so the CI
here covers that, not upstream's full validation matrix.

| Workflow | What it does |
|---|---|
| `freestanding.yml` | Compiles `Eigen/{Core,Geometry,Eigenvalues,SVD,LU}` in three configurations — freestanding, hosted, and freestanding with `EIGEN_HAS_CXX11_MATH=0` (which approximates a non-x86 target) — and fails on *any* compiler output. Seconds to run. |
| `smoketests.yml` | Builds and runs Eigen's smoke test subset (`cmake/EigenSmokeTestList.cmake`, 105 resolve in the CI configuration) under gcc-13 with `EIGEN_TEST_CXX11` on and off, and under clang-18 with it on. |

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

## Why clang is only built with C++11

clang-18 builds and passes the smoke tests with `EIGEN_TEST_CXX11=on`. It cannot
compile Eigen 3.4.0's own `meta` test with `EIGEN_TEST_CXX11=off`, which configures
`-std=c++03`:

```
Eigen/src/Core/util/Meta.h:250:34: error: allocating an object of abstract class type 'MyInterface'
  250 |   enum { value = sizeof(test<To>(*ms_from, 0))==sizeof(yes) };
  ...
test/meta.cpp:122: note: in instantiation of template class
                         'Eigen::internal::is_convertible<MyImpl, MyInterface>' requested here
```

`Eigen::internal::is_convertible` is not conditional on the language version; the
same code compiles under C++11 and fails under C++03, so the difference is in how
the compiler treats it, not in which path Eigen takes. Upstream knows the construct
is fragile -- `test/meta.cpp` says so directly above the failing check:

> See http://eigen.tuxfamily.org/bz/show_bug.cgi?id=1752, basically, a fix in the
> c++ standard breaks our c++98 implementation of is_convertible for abstract
> classes. So the following tests are expected to fail with recent compilers.

Making a clang + C++03 job work would mean fixing that C++98 implementation, which
is upstream's problem rather than this fork's.

That combination is not covered because nothing downstream needs it: the flight
software builds with `-std=gnu++23`. gcc-13 still covers C++03, so the older
language mode is not unwatched.

## Note on `.gitignore`

`.gitignore` carries an upstream `*build*` pattern that also matched
`.github/workflows/build.yml`, silently keeping a workflow out of the repository.
A `!.github/**` negation now prevents that. Watch for it when adding files whose
names contain "build".
