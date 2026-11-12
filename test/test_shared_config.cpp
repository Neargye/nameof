// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include "test_shared_config.hpp"

nameof::string_view shared_config::from_other(Value value) {
  return nameof::nameof_enum(value);
}

nameof::string shared_config::flags_from_other(Flags value) {
  return nameof::nameof_enum_flag(value);
}

const nameof::cstring<4>& shared_config::constant_from_other() {
  return nameof::nameof_enum<Value::High>();
}
