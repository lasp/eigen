# CI in this fork

This is a LASP fork of Eigen 3.4. The CI here is deliberately small: it covers what
the fork actually needs rather than upstream's full validation matrix.

| Workflow | What it does |
|---|---|
| `smoketests.yml` | Builds and runs Eigen's smoke test subset (`cmake/EigenSmokeTestList.cmake`, 105 resolve in the CI configuration) under gcc-13, with `EIGEN_TEST_CXX11` on and off. |

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

## Why there is no clang job

clang-18 cannot compile Eigen 3.4.0's own `meta` test:

```
Eigen/src/Core/util/Meta.h:250:34: error: allocating an object of abstract class type 'MyInterface'
  250 |   enum { value = sizeof(test<To>(*ms_from, 0))==sizeof(yes) };
  ...
test/meta.cpp:122: note: in instantiation of template class
                         'Eigen::internal::is_convertible<MyImpl, MyInterface>' requested here
```

`Eigen::internal::is_convertible` forms an abstract class by value in an
unevaluated context, which newer clang rejects. Upstream later reworked
`is_convertible` to defer to `std::is_convertible`; 3.4.0 predates that. Since
`meta` is in the smoke test list and is the first target built, there is no way to
keep a clang job without either backporting that rework into this fork or ignoring
the failure. Neither belongs in CI plumbing, so clang is simply not built here.

gcc-13 is the compiler the downstream flight software uses, so it is the one whose
breakage would actually matter. If clang coverage is wanted later, backporting the
upstream `is_convertible` change is the honest fix.

## Note on `.gitignore`

`.gitignore` carries an upstream `*build*` pattern that also matched
`.github/workflows/build.yml`, silently keeping a workflow out of the repository.
A `!.github/**` negation now prevents that. Watch for it when adding files whose
names contain "build".
