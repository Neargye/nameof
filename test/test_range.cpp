// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define NAMEOF_ENUM_RANGE_MIN 32767
#define NAMEOF_ENUM_RANGE_MAX 33023
#include <nameof.hpp>

enum class PositiveOnlyRange { First = 32767, Last = 33023 };

static_assert(nameof::nameof_enum(PositiveOnlyRange::First) == "First");
static_assert(nameof::nameof_enum(PositiveOnlyRange::Last) == "Last");

int main() {}
