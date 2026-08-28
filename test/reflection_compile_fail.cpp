// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

#ifndef NAMEOF_REFLECTION_COMPILE_FAIL_CASE
#  error NAMEOF_REFLECTION_COMPILE_FAIL_CASE must select a compile-fail scenario.
#endif

#if NAMEOF_REFLECTION_COMPILE_FAIL_CASE == 1

enum class ForwardDeclared : unsigned;

constexpr auto incomplete_name = nameof::nameof_enum(ForwardDeclared{0});

#else

#  error Unknown NAMEOF_REFLECTION_COMPILE_FAIL_CASE.

#endif

int main() {}
