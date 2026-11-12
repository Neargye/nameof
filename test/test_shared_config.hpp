// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#pragma once

#include <nameof.hpp>

namespace shared_config {
enum class Value { Low = 0, High = 500, Renamed = 501 };
enum class Flags : unsigned { None = 0, Low = 1, High = 128 };
nameof::string_view from_other(Value value);
nameof::string flags_from_other(Flags value);
const nameof::cstring<4>& constant_from_other();
} // namespace shared_config

template <>
struct nameof::customize::enum_range<shared_config::Value> {
  static constexpr int min = 0;
  static constexpr int max = 501;
};

template <>
constexpr nameof::string_view nameof::customize::enum_name<shared_config::Value>(shared_config::Value value) noexcept {
  return value == shared_config::Value::Renamed ? "Custom" : static_cast<int>(value) == 900 ? "Synthetic" : "";
}

template <>
constexpr nameof::string_view nameof::customize::enum_name<shared_config::Flags>(shared_config::Flags value) noexcept {
  return static_cast<unsigned>(value) == 2u ? "SyntheticBit" : "";
}
