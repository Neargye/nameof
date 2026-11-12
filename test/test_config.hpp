// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

// Included by nameof.hpp through NAMEOF_CONFIG_FILE.

#define NAMEOF_TEST_CONFIG_FILE 1

// Must take effect before the backend is selected: standard reflection would name values outside the scan range.
#if !defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION)
#  define NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION 1
#endif

inline int nameof_test_assert_failures = 0;

// Counts failed checks instead of aborting. Holding checks stay constant expressions.
#define NAMEOF_ASSERT(...) ((__VA_ARGS__) ? 0 : ++nameof_test_assert_failures)
