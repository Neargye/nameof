// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define NAMEOF_ENUM_RANGE_MIN -32769
#define NAMEOF_ENUM_RANGE_MAX -32768
#include <nameof.hpp>

enum class NegativeOnlyRange { First = -32769, Last = -32768 };

static_assert(nameof::nameof_enum(NegativeOnlyRange::First) == "First");
static_assert(nameof::nameof_enum(NegativeOnlyRange::Last) == "Last");

int main() {}
