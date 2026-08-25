#!/usr/bin/env bash
# check_freestanding.sh
#
# Compiles .github/freestanding/smoketest.cpp in both configurations this fork
# has to support, and fails if the compiler says anything at all.
#
# Why "anything at all" rather than just a non-zero exit: the defects this check
# exists to catch are not errors. When the freestanding guards were added,
# GeneralBlockPanelKernel.h's four #undef PACKET_DECL_COND* lines ended up inside
# an #ifndef EIGEN_FREESTANDING block, so in freestanding builds the macros
# leaked out of Eigen/Core and one of them was redefined in
# GeneralMatrixVector.h. Every downstream translation unit warned; nothing
# failed. Same for the unconditional #pragma message in all_freestanding.hpp.
# Both are invisible to a check that only looks at the exit status.
#
# Both configurations are checked because the two can break independently: the
# macro leak appeared only with EIGEN_FREESTANDING defined, and a careless fix
# could break the hosted path instead.
#
# Usage:  check_freestanding.sh
# Environment:
#   EIGEN3_DIR  Root of the Eigen checkout (the directory containing 'Eigen/').
#               Defaults to the repository root, so it normally needs no setting.
#   CXX         Compiler to use. Defaults to g++-13, the version the downstream
#               flight software builds with.

set -euo pipefail

CXX="${CXX:-g++-13}"

if ! REPO_ROOT="$(git rev-parse --show-toplevel 2>/dev/null)"; then
  REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
fi
EIGEN3_DIR="${EIGEN3_DIR:-$REPO_ROOT}"

if [[ ! -d "${EIGEN3_DIR}/Eigen" ]]; then
  echo "Eigen not found under ${EIGEN3_DIR}" >&2
  exit 2
fi
if ! command -v "${CXX}" >/dev/null 2>&1; then
  echo "Compiler '${CXX}' not found; set CXX to an available compiler." >&2
  exit 2
fi

TU="${REPO_ROOT}/.github/freestanding/smoketest.cpp"
if [[ ! -f "$TU" ]]; then
  echo "Translation unit not found: $TU" >&2
  exit 2
fi

FORCE_INCLUDE="${EIGEN3_DIR}/Eigen/src/Freestanding/all_freestanding.hpp"
# -Wundef matters specifically: Eigen's own test suite compiles with it, and the
# freestanding guards are the kind of thing that gets written as `!EIGEN_FREESTANDING`
# instead of `!defined(EIGEN_FREESTANDING)`, which silently evaluates to 0 in every
# hosted translation unit. Not -Wall/-Wextra yet: the freestanding stubs in
# Memory.h have pre-existing unused-parameter warnings that need fixing first.
COMMON=(-std=gnu++23 -fsyntax-only -Wundef -I"${EIGEN3_DIR}")

FAILURES=0

# $1 = human-readable configuration name, remaining args = extra compiler flags
check_config() {
  local name="$1"
  shift

  local output
  if ! output="$("${CXX}" "${COMMON[@]}" "$@" "$TU" 2>&1)" || [[ -n "$output" ]]; then
    echo "::error::${name} configuration is not clean"
    echo "$output" | sed 's/^/    /'
    FAILURES=$((FAILURES + 1))
  else
    echo "  ${name}: clean"
  fi
}

echo "Checking ${TU#"$REPO_ROOT"/} with ${CXX}"
check_config "freestanding" -DEIGEN_FREESTANDING=1 -include "${FORCE_INCLUDE}"
check_config "hosted"

# Approximates a non-x86 target on an x86 runner. EIGEN_HAS_CXX11_MATH is 1 on
# x86 (Macros.h requires EIGEN_ARCH_i386_OR_x86_64), which short-circuits
# MathFunctions.h:503 and hides anything the branch below it depends on. Forcing
# it to 0 exercises that path without a cross toolchain -- it is how the
# EIGEN_HAS_C99_MATH block was found to be trapped inside a freestanding guard.
check_config "freestanding, non-x86 math path" \
  -DEIGEN_FREESTANDING=1 -DEIGEN_HAS_CXX11_MATH=0 -include "${FORCE_INCLUDE}"

if [[ $FAILURES -gt 0 ]]; then
  echo ""
  echo "${FAILURES} configuration(s) produced compiler output."
  exit 1
fi

echo "All configurations compile silently."
