// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

// The header undefines NAMEOF_DETAIL_TEST_FALLBACK at its end.
#if defined(NAMEOF_DETAIL_TEST_FALLBACK)
#  define NAMEOF_TEST_FALLBACK 1
#endif

#include <nameof.hpp>

#include <array>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

#include "test_shared_config.hpp"

// Clang: numeric-first lookup of a class-template enum must see its enumerators (detail::enum_definition), so each API gets its own enum without a named access before.
namespace definition_test {

template <typename, typename>
struct Host {
  enum class E : unsigned { A = 1, B = 4 };
  enum class Collision { nameof_detail_enum_definition = 1 };
};

using Numeric = Host<void(int, int), int(*)[2]>::E;
static_assert(nameof::nameof_enum(static_cast<Numeric>(1)) == "A");
using Constant = Host<long, void>::E;
using Fallback = Host<char, void>::E;
using Flags = Host<short, void>::E;
using Collision = Host<int, void>::Collision;
static_assert(nameof::nameof_enum<static_cast<Constant>(4)>() == "B");
static_assert(nameof::nameof_enum(static_cast<Collision>(1)) == "nameof_detail_enum_definition");

struct Incomplete;
template <typename T, typename = void>
struct HasName : std::false_type {};
template <typename T>
struct HasName<T, std::void_t<decltype(nameof::nameof_enum(std::declval<T>()))>> : std::true_type {};
static_assert(!HasName<int>::value && !HasName<Incomplete>::value);

bool check() {
  return nameof::nameof_enum_or(static_cast<Fallback>(4), "?") == "B" &&
         nameof::nameof_enum_flag(static_cast<Flags>(5)) == "A|B";
}

} // namespace definition_test

namespace backend_test {

template <int N>
struct Boundary {
  enum class E : int { First = 0, Middle = N / 2, Last = N - 1, Outside = N };
};

enum class Aliases { High = 40, First = 3, Alias = 3, Middle = 12 };
enum class Deprecated { Old [[deprecated("use Current")]] = 1, Current = 2 };
typedef enum { TypedefFirst, TypedefSecond } TypedefEnum;
typedef enum : unsigned { FixedFirst, FixedSecond } FixedTypedefEnum;
enum UnfixedHigh { Low = 1, High = 1 << 30 };
enum UnfixedSigned { Negative = -1, SignedLow = 1, SignedHigh = 1 << 29 };

enum class Embedded { Value = 1 };
inline constexpr char custom_chars[] = {'a', '\0', 'b', 'x'};

template <typename, typename>
struct Host {
  enum class E { First = 1, Last = 4 };
};

// Clang spells these arguments as literals (CharHost<'<', L'>'>). GCC 16 prints '\'' unescaped (`'''`), so that block takes the per-value fallback.
template <char, wchar_t>
struct CharHost {
  enum class E { First = 1, Last = 4 };
};

struct Operand {};

} // namespace backend_test

#define NAMEOF_TEST_BOUNDARY(N)                                                    \
  template <> struct nameof::customize::enum_range<backend_test::Boundary<N>::E> { \
    static constexpr int min = 0;                                                  \
    static constexpr int max = N - 1;                                              \
  };
NAMEOF_TEST_BOUNDARY(255)
NAMEOF_TEST_BOUNDARY(256)
NAMEOF_TEST_BOUNDARY(257)
NAMEOF_TEST_BOUNDARY(511)
NAMEOF_TEST_BOUNDARY(512)
NAMEOF_TEST_BOUNDARY(513)
#undef NAMEOF_TEST_BOUNDARY

template <>
constexpr nameof::string_view nameof::customize::enum_name<backend_test::Embedded>(backend_test::Embedded) noexcept {
  return {backend_test::custom_chars, 3};
}

template <int N>
constexpr bool check_boundary() {
  using E = typename backend_test::Boundary<N>::E;
  static_assert(nameof::nameof_enum(E::First) == "First");
  static_assert(nameof::nameof_enum(E::Middle) == "Middle");
  static_assert(nameof::nameof_enum(E::Last) == "Last");
  static_assert(nameof::nameof_enum<E::Outside>() == "Outside");
  static_assert(nameof::nameof_enum(static_cast<E>(1)).empty());
#if defined(NAMEOF_TEST_STD_REFLECTION)
  static_assert(nameof::nameof_enum(E::Outside) == "Outside");
#else
  static_assert(nameof::nameof_enum(E::Outside).empty());
#endif
  return true;
}

static_assert(check_boundary<255>() && check_boundary<256>() && check_boundary<257>());
static_assert(check_boundary<511>() && check_boundary<512>() && check_boundary<513>());

// A value of an enum in a global operator can have an abbreviated MSVC scope.
int operator-(backend_test::Operand, int) {
  enum class Local { First = 1, Last = 4 };
  static_assert(nameof::nameof_enum(Local::Last) == "Last");
  static_assert(nameof::nameof_enum<Local::First>() == "First");
  return 0;
}

template <typename T>
constexpr bool local_context() {
  struct Local {
    enum class E { First = 1, Last = 4 };
  };
  static_assert(nameof::nameof_enum(Local::E::Last) == "Last");
  return [](T) {
    enum class Nested { First = 1, Last = 4 };
    return nameof::nameof_enum(Nested::First) == "First";
  }(T{});
}

static_assert(local_context<int>());
static_assert(nameof::nameof_enum(backend_test::Host<void(int, int), int(*)[2]>::E::Last) == "Last");
static_assert(nameof::nameof_enum(backend_test::CharHost<'\'', L'>'>::E::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<backend_test::CharHost<'\'', L'>'>::E>(2)).empty());
static_assert(nameof::nameof_enum(backend_test::CharHost<'<', L'>'>::E::Last) == "Last");
static_assert(nameof::nameof_enum(backend_test::TypedefSecond) == "TypedefSecond");
static_assert(nameof::nameof_enum(backend_test::FixedSecond) == "FixedSecond");
static_assert(nameof::nameof_short_type<backend_test::TypedefEnum>() == "TypedefEnum");
static_assert(nameof::nameof_short_type<backend_test::FixedTypedefEnum>() == "FixedTypedefEnum");
// Refer to the underlying value to test reflection without using the deprecated spelling.
static_assert(nameof::nameof_enum(backend_test::Deprecated{1}) == "Old");
static_assert(nameof::nameof_enum<backend_test::Deprecated{1}>() == "Old");

using backend_test::Aliases;
static_assert(std::is_same_v<decltype(nameof::nameof_enum<Aliases::High>()), const nameof::cstring<4>&>);
static_assert(std::is_same_v<decltype(nameof::nameof_enum(Aliases::High)), nameof::string_view>);
static_assert(std::is_same_v<decltype(nameof::nameof_enum_or(Aliases::High, "?")), nameof::string>);
static_assert(std::is_same_v<decltype(nameof::nameof_enum_flag(Aliases::High)), nameof::string>);
static_assert(noexcept(nameof::nameof_enum(Aliases::High)) && noexcept(nameof::nameof_enum<Aliases::High>()));
static_assert(!noexcept(nameof::nameof_enum_or(Aliases::High, "?")) && !noexcept(nameof::nameof_enum_flag(Aliases::High)));
#if defined(NAMEOF_TEST_STD_REFLECTION)
static_assert(nameof::nameof_enum(Aliases::Alias) == "First");
template <const char* P>
struct ReflectedPointer { static constexpr const char* value = P; };
static_assert(ReflectedPointer<nameof::nameof_enum(Aliases::High).data()>::value[0] == 'H');
#else
static_assert(nameof::nameof_enum(Aliases::Alias) == nameof::nameof_enum(Aliases::First));
#endif

static_assert(nameof::nameof_enum(backend_test::Embedded::Value).data() == backend_test::custom_chars);
static_assert(nameof::nameof_enum(backend_test::Embedded::Value).size() == 3);
static_assert(nameof::nameof_enum(backend_test::Embedded::Value).data()[3] == 'x');
static_assert(nameof::nameof_enum<backend_test::Embedded::Value>().size() == 3);
static_assert(nameof::nameof_enum<backend_test::Embedded::Value>().data()[1] == '\0');
static_assert(nameof::nameof_enum<backend_test::Embedded::Value>().data()[3] == '\0');

int backend_check() {
  using namespace backend_test;
  if ((Operand{} - 1) != 0 || nameof::nameof_enum_flag(High) != "High" ||
      nameof::nameof_enum_flag(static_cast<UnfixedSigned>(SignedLow | SignedHigh)) != "SignedLow|SignedHigh") {
    return 1;
  }
  const auto copied = nameof::nameof_enum_or(Embedded::Value, "?");
  return copied.size() == 3 && copied[0] == 'a' && copied[1] == '\0' && copied[2] == 'b' ? 0 : 2;
}

// Lookup tables, custom subviews and flags of every width.
enum class Pages : int {
  V000 = 0, V001 = 3, V002 = 6, V003 = 9, V004 = 12, V005 = 15, V006 = 18, V007 = 21,
  V008 = 24, V009 = 27, V010 = 30, V011 = 33, V012 = 36, V013 = 39, V014 = 42, V015 = 45,
  V016 = 48, V017 = 51, V018 = 54, V019 = 57, V020 = 60, V021 = 63, V022 = 66, V023 = 69,
  V024 = 72, V025 = 75, V026 = 78, V027 = 81, V028 = 84, V029 = 87, V030 = 90, V031 = 93,
  V032 = 96, V033 = 99, V034 = 102, V035 = 105, V036 = 108, V037 = 111, V038 = 114, V039 = 117,
  V040 = 120, V041 = 123, V042 = 126, V043 = 129, V044 = 132, V045 = 135, V046 = 138, V047 = 141,
  V048 = 144, V049 = 147, V050 = 150, V051 = 153, V052 = 156, V053 = 159, V054 = 162, V055 = 165,
  V056 = 168, V057 = 171, V058 = 174, V059 = 177, V060 = 180, V061 = 183, V062 = 186, V063 = 189,
  V064 = 192, V065 = 195, V066 = 198, V067 = 201, V068 = 204, V069 = 207, V070 = 210, V071 = 213,
  V072 = 216, V073 = 219, V074 = 222, V075 = 225, V076 = 228, V077 = 231, V078 = 234, V079 = 237,
  V080 = 240, V081 = 243, V082 = 246, V083 = 249, V084 = 252, V085 = 255, V086 = 258, V087 = 261,
  V088 = 264, V089 = 267, V090 = 270, V091 = 273, V092 = 276, V093 = 279, V094 = 282, V095 = 285,
  V096 = 288, V097 = 291, V098 = 294, V099 = 297, V100 = 300, V101 = 303, V102 = 306, V103 = 309,
  V104 = 312, V105 = 315, V106 = 318, V107 = 321, V108 = 324, V109 = 327, V110 = 330, V111 = 333,
  V112 = 336, V113 = 339, V114 = 342, V115 = 345, V116 = 348, V117 = 351, V118 = 354, V119 = 357,
  V120 = 360, V121 = 363, V122 = 366, V123 = 369, V124 = 372, V125 = 375, V126 = 378, V127 = 381,
  V128 = 384, V129 = 387, V130 = 390, V131 = 393, V132 = 396, V133 = 399, V134 = 402, V135 = 405,
  V136 = 408, V137 = 411, V138 = 414, V139 = 417, V140 = 420, V141 = 423, V142 = 426, V143 = 429,
  V144 = 432, V145 = 435, V146 = 438, V147 = 441, V148 = 444, V149 = 447, V150 = 450, V151 = 453,
  V152 = 456, V153 = 459, V154 = 462, V155 = 465, V156 = 468, V157 = 471, V158 = 474, V159 = 477,
  V160 = 480, V161 = 483, V162 = 486, V163 = 489, V164 = 492, V165 = 495, V166 = 498, V167 = 501,
  V168 = 504, V169 = 507, V170 = 510, V171 = 513, V172 = 516, V173 = 519, V174 = 522, V175 = 525,
  V176 = 528, V177 = 531, V178 = 534, V179 = 537, V180 = 540, V181 = 543, V182 = 546, V183 = 549,
  V184 = 552, V185 = 555, V186 = 558, V187 = 561, V188 = 564, V189 = 567, V190 = 570, V191 = 573,
  V192 = 576, V193 = 579, V194 = 582, V195 = 585, V196 = 588, V197 = 591, V198 = 594, V199 = 597,
  V200 = 600, V201 = 603, V202 = 606, V203 = 609, V204 = 612, V205 = 615, V206 = 618, V207 = 621,
  V208 = 624, V209 = 627, V210 = 630, V211 = 633, V212 = 636, V213 = 639, V214 = 642, V215 = 645,
  V216 = 648, V217 = 651, V218 = 654, V219 = 657, V220 = 660, V221 = 663, V222 = 666, V223 = 669,
  V224 = 672, V225 = 675, V226 = 678, V227 = 681, V228 = 684, V229 = 687, V230 = 690, V231 = 693,
  V232 = 696, V233 = 699, V234 = 702, V235 = 705, V236 = 708, V237 = 711, V238 = 714, V239 = 717,
  V240 = 720, V241 = 723, V242 = 726, V243 = 729, V244 = 732, V245 = 735, V246 = 738, V247 = 741,
  V248 = 744, V249 = 747, V250 = 750, V251 = 753, V252 = 756, V253 = 759, V254 = 762, V255 = 765,
  V256 = 768
};

template <>
struct nameof::customize::enum_range<Pages> {
  static constexpr int min = 0;
  static constexpr int max = 768;
};

inline constexpr char custom_view[] = "_short_tail";

template <>
constexpr nameof::string_view nameof::customize::enum_name<Pages>(Pages value) noexcept {
  if (value == Pages::V128) {
    return {custom_view + 1, 5};
  } else if (static_cast<int>(value) == 100000) {
    return "synthetic";
  }
  return {};
}

enum class SignedBits : std::int64_t {
  B0 = 1, B1 = 2, B2 = 4, B3 = 8, B4 = 16, B5 = 32, B6 = 64, Sign = INT64_MIN
};

template <>
constexpr nameof::string_view nameof::customize::enum_name<SignedBits>(SignedBits value) noexcept {
  if (value == SignedBits::B2) {
    return {custom_view + 1, 5};
  }
  return static_cast<std::int64_t>(value) == 128 ? nameof::string_view{"Synthetic"} : nameof::string_view{};
}

enum class AllBits : std::uint64_t {
  B00 = std::uint64_t{1} << 0, B01 = std::uint64_t{1} << 1, B02 = std::uint64_t{1} << 2, B03 = std::uint64_t{1} << 3,
  B04 = std::uint64_t{1} << 4, B05 = std::uint64_t{1} << 5, B06 = std::uint64_t{1} << 6, B07 = std::uint64_t{1} << 7,
  B08 = std::uint64_t{1} << 8, B09 = std::uint64_t{1} << 9, B10 = std::uint64_t{1} << 10, B11 = std::uint64_t{1} << 11,
  B12 = std::uint64_t{1} << 12, B13 = std::uint64_t{1} << 13, B14 = std::uint64_t{1} << 14, B15 = std::uint64_t{1} << 15,
  B16 = std::uint64_t{1} << 16, B17 = std::uint64_t{1} << 17, B18 = std::uint64_t{1} << 18, B19 = std::uint64_t{1} << 19,
  B20 = std::uint64_t{1} << 20, B21 = std::uint64_t{1} << 21, B22 = std::uint64_t{1} << 22, B23 = std::uint64_t{1} << 23,
  B24 = std::uint64_t{1} << 24, B25 = std::uint64_t{1} << 25, B26 = std::uint64_t{1} << 26, B27 = std::uint64_t{1} << 27,
  B28 = std::uint64_t{1} << 28, B29 = std::uint64_t{1} << 29, B30 = std::uint64_t{1} << 30, B31 = std::uint64_t{1} << 31,
  B32 = std::uint64_t{1} << 32, B33 = std::uint64_t{1} << 33, B34 = std::uint64_t{1} << 34, B35 = std::uint64_t{1} << 35,
  B36 = std::uint64_t{1} << 36, B37 = std::uint64_t{1} << 37, B38 = std::uint64_t{1} << 38, B39 = std::uint64_t{1} << 39,
  B40 = std::uint64_t{1} << 40, B41 = std::uint64_t{1} << 41, B42 = std::uint64_t{1} << 42, B43 = std::uint64_t{1} << 43,
  B44 = std::uint64_t{1} << 44, B45 = std::uint64_t{1} << 45, B46 = std::uint64_t{1} << 46, B47 = std::uint64_t{1} << 47,
  B48 = std::uint64_t{1} << 48, B49 = std::uint64_t{1} << 49, B50 = std::uint64_t{1} << 50, B51 = std::uint64_t{1} << 51,
  B52 = std::uint64_t{1} << 52, B53 = std::uint64_t{1} << 53, B54 = std::uint64_t{1} << 54, B55 = std::uint64_t{1} << 55,
  B56 = std::uint64_t{1} << 56, B57 = std::uint64_t{1} << 57, B58 = std::uint64_t{1} << 58, B59 = std::uint64_t{1} << 59,
  B60 = std::uint64_t{1} << 60, B61 = std::uint64_t{1} << 61, B62 = std::uint64_t{1} << 62, B63 = std::uint64_t{1} << 63
};

static_assert(nameof::nameof_enum(Pages::V000) == "V000");
static_assert(nameof::nameof_enum(Pages::V127) == "V127");
static_assert(nameof::nameof_enum(Pages::V128) == "short");
static_assert(nameof::nameof_enum(Pages::V128).data()[5] == '_');
static_assert(nameof::nameof_enum(Pages::V255) == "V255");
static_assert(nameof::nameof_enum(Pages::V256) == "V256");
static_assert(nameof::nameof_enum(Pages::V256).data()[4] == '\0');
static_assert(nameof::nameof_enum(static_cast<Pages>(100000)) == "synthetic");
static_assert(nameof::nameof_enum(static_cast<Pages>(767)).empty());
static_assert(nameof::nameof_enum<Pages::V256>() == "V256");

// Sparse tables of 16 and 17 values with negative values: linear and binary search paths of the scanner.
enum class Sparse16 : int {
  M60 = -60, M52 = -52, M44 = -44, M36 = -36, M28 = -28, M20 = -20, M12 = -12, M04 = -4,
  P04 = 4, P12 = 12, P20 = 20, P28 = 28, P36 = 36, P44 = 44, P52 = 52, P60 = 60
};

enum class Sparse17 : int {
  M64 = -64, M56 = -56, M48 = -48, M40 = -40, M32 = -32, M24 = -24, M16 = -16, M08 = -8, Z00 = 0,
  P08 = 8, P16 = 16, P24 = 24, P32 = 32, P40 = 40, P48 = 48, P56 = 56, P64 = 64
};

static_assert(nameof::nameof_enum(Sparse17::M64) == "M64");
static_assert(nameof::nameof_enum(Sparse17::Z00) == "Z00");
static_assert(nameof::nameof_enum(Sparse17::P64) == "P64");
static_assert(nameof::nameof_enum(static_cast<Sparse17>(-63)).empty());
static_assert(nameof::nameof_enum(static_cast<Sparse17>(65)).empty());

template <typename E>
bool check_sparse(int first, int last) {
  for (int value = first - 8; value <= last + 8; ++value) {
    const auto name = nameof::nameof_enum(static_cast<E>(value));
    if (value < first || value > last || (value - first) % 8 != 0) {
      if (!name.empty()) {
        return false;
      }
      continue;
    }
    const auto mag = value < 0 ? -value : value;
    const char expected[] = {value < 0 ? 'M' : value > 0 ? 'P' : 'Z', static_cast<char>('0' + mag / 10), static_cast<char>('0' + mag % 10)};
    if (name != nameof::string_view{expected, 3}) {
      return false;
    }
  }
  return true;
}

template <typename T>
struct WidthFlags {
  enum class E : T {
    None = 0, B0 = 1, B1 = 2, B2 = 4, B3 = 8, B4 = 16, B5 = 32, B6 = 64,
    High = T{1} << (std::numeric_limits<T>::digits - 1)
  };
};

template <typename T>
bool check_width() {
  using E = typename WidthFlags<T>::E;
  constexpr auto bits = static_cast<unsigned>(std::numeric_limits<T>::digits);
  for (unsigned bit = 0; bit < bits; ++bit) {
    nameof::string expected;
    if (bit < 7) {
      const char name[] = {'B', static_cast<char>('0' + bit)};
      expected = nameof::string{name, 2};
    } else if (bit == bits - 1) {
      expected = "High";
    }
    const auto value = static_cast<E>(static_cast<T>(T{1} << bit));
    if (nameof::nameof_enum_flag(value) != expected) {
      return false;
    }
  }
  return nameof::nameof_enum_flag(static_cast<E>(T{0})).empty();
}

int lookup_check() {
  using Byte = WidthFlags<std::uint8_t>::E;
  for (unsigned mask = 0; mask < 256; ++mask) {
    nameof::string expected;
    for (unsigned bit = 0; bit < 8; ++bit) {
      if ((mask & (1u << bit)) != 0) {
        if (!expected.empty()) {
          expected.append(1, '|');
        }
        if (bit == 7) {
          expected.append("High");
        } else {
          const char name[] = {'B', static_cast<char>('0' + bit)};
          expected.append(name, 2);
        }
      }
    }
    if (nameof::nameof_enum_flag(static_cast<Byte>(static_cast<std::uint8_t>(mask))) != expected) {
      return 9;
    }
  }
  if (nameof::nameof_enum_flag(static_cast<SignedBits>(255)) != "B0|B1|short|B3|B4|B5|B6|Synthetic") {
    return 10;
  }
  if (!check_width<std::uint8_t>() || !check_width<std::uint16_t>() ||
      !check_width<std::uint32_t>() || !check_width<std::uint64_t>()) {
    return 8;
  }
  for (int value = 0; value <= 769; ++value) {
    const auto name = nameof::nameof_enum(static_cast<Pages>(value));
    if (value % 3 != 0) {
      if (!name.empty()) {
        return 1;
      }
    } else if (value == 384) {
      if (name != "short") {
        return 2;
      }
    } else {
      const auto index = value / 3;
      const char expected[] = {'V', static_cast<char>('0' + index / 100),
                               static_cast<char>('0' + (index / 10) % 10), static_cast<char>('0' + index % 10)};
      if (name != nameof::string_view{expected, 4} || name.data()[name.size()] != '\0') {
        return 3;
      }
    }
  }
  for (unsigned bit = 0; bit < 64; ++bit) {
    const auto value = std::uint64_t{1} << bit;
    const char expected[] = {'B', static_cast<char>('0' + bit / 10), static_cast<char>('0' + bit % 10)};
    if (nameof::nameof_enum_flag(static_cast<AllBits>(value)) != nameof::string{expected, 3}) {
      return 4;
    }
    const auto signed_name = nameof::nameof_enum_flag(static_cast<SignedBits>(static_cast<std::int64_t>(value)));
    if (bit < 7) {
      const char short_name[] = {'B', static_cast<char>('0' + bit)};
      if (signed_name != (bit == 2 ? nameof::string{"short"} : nameof::string{short_name, 2})) {
        return 5;
      }
    } else if (signed_name != (bit == 63 ? "Sign" : bit == 7 ? "Synthetic" : "")) {
      return 6;
    }
  }
  if (nameof::nameof_enum_flag(static_cast<SignedBits>(INT64_MIN + 127), ':') != "B0:B1:short:B3:B4:B5:B6:Sign") {
    return 7;
  }
  if (!check_sparse<Sparse16>(-60, 60) || !check_sparse<Sparse17>(-64, 64)) {
    return 11;
  }
  return 0;
}

namespace shared_config {

int check() {
  constexpr Value values[] = {Value::Low, Value::High, Value::Renamed, Value{900}, Value{42}};
  constexpr nameof::string_view expected[] = {"Low", "High", "Custom", "Synthetic", ""};
  for (std::size_t i = 0; i < 5; ++i) {
    const auto view = from_other(values[i]);
    if (view != expected[i] || view != nameof::nameof_enum(values[i])) {
      return 1;
    }
    if (i < 2 && view.data()[view.size()] != '\0') {
      return 2;
    }
  }
  const auto& constant = constant_from_other();
  if (constant != "High" || constant.data()[constant.size()] != '\0') {
    return 3;
  }
  return flags_from_other(static_cast<Flags>(131)) == "Low|SyntheticBit|High" &&
         flags_from_other(Flags::None).empty() && flags_from_other(static_cast<Flags>(4)).empty() ? 0 : 4;
}

} // namespace shared_config

#if defined(NAMEOF_TEST_STD_REFLECTION)
enum class Ordered { High = 900, First = -2, Alias = -2, Middle = 12, Old [[deprecated]] = 7 };
enum class Unsigned : std::uint64_t { High = (std::numeric_limits<std::uint64_t>::max)(), Low = 0 };
enum class Empty {};
enum class Lazy { First = 42, Alias = 42 };
enum class CustomOpaque : unsigned;
enum class Changing { Value = 1 };

bool runtime_custom = false;

template <>
constexpr nameof::string_view nameof::customize::enum_name<CustomOpaque>(CustomOpaque) noexcept {
  return "OpaqueCustom";
}

template <>
constexpr nameof::string_view nameof::customize::enum_name<Changing>(Changing) noexcept {
  return !std::is_constant_evaluated() && runtime_custom ? "RuntimeCustom" : "";
}

// A constant-value query must remain independent of every runtime lookup path.
template <> constexpr auto nameof::detail::reflection::enum_name<Lazy>(Lazy) noexcept = delete;

static_assert(nameof::nameof_enum<Lazy::Alias>() == "First");
static_assert(nameof::nameof_enum<static_cast<CustomOpaque>(7)>() == "OpaqueCustom");
static_assert(std::is_same_v<decltype(nameof::nameof_enum<Lazy::Alias>()), const nameof::cstring<5>&>);
static_assert(nameof::detail::reflection::entries_v<Ordered>.size() == 5);
static_assert(nameof::detail::reflection::entries_v<Ordered>[0].value == static_cast<int>(Ordered::High));
static_assert(nameof::detail::reflection::entries_v<Ordered>[1].name == "First");
static_assert(nameof::detail::reflection::entries_v<Ordered>[2].name == "Alias");
static_assert(nameof::detail::reflection::entries_v<Ordered>[3].value == static_cast<int>(Ordered::Middle));
static_assert(nameof::nameof_enum(Ordered::Alias) == "First");
static_assert(nameof::nameof_enum<Ordered::Alias>() == "First");
static_assert(nameof::nameof_enum(Ordered{7}) == "Old");
static_assert(nameof::nameof_enum(Ordered{8}).empty());
static_assert(nameof::nameof_enum<Ordered{8}>().empty());
static_assert(nameof::nameof_enum(Unsigned::High) == "High");
static_assert(nameof::nameof_enum<Unsigned::High>() == "High");
static_assert(nameof::detail::reflection::entries_v<Empty>.empty());
static_assert(nameof::nameof_enum(static_cast<Empty>(0)).empty());
static_assert(nameof::nameof_enum(Changing::Value) == "Value");

template <const char* P>
struct NamePointer { static constexpr const char* value = P; };
static_assert(NamePointer<nameof::nameof_enum(Ordered::Alias).data()>::value[0] == 'F');
static_assert(NamePointer<nameof::nameof_enum(Unsigned::High).data()>::value[4] == '\0');

enum class DenseSigned : std::int8_t { First = -4, Second = -3, Low = -2, Middle = -1, High = 0 };
enum class DenseMax : std::uint64_t { First = UINT64_MAX - 4, Second = UINT64_MAX - 3, Middle = UINT64_MAX - 2, Low = UINT64_MAX - 1, High = UINT64_MAX };
enum class DenseMin : std::int64_t { Low = INT64_MIN, Second = INT64_MIN + 1, Middle = INT64_MIN + 2, Fourth = INT64_MIN + 3, High = INT64_MIN + 4 };
enum class DenseBool : bool { No = false, Yes = true };
enum class Wrap : std::uint64_t { High = UINT64_MAX, Low = 0, One = 1, Two = 2, Three = 3 };
enum class DenseAlias { Low = 0, Alias = 0, Next = 1, Middle = 2, High = 3 };
enum class Unordered { High = 4, Low = 0, Middle = 2, One = 1, Three = 3 };
static_assert(nameof::nameof_enum(DenseSigned::High) == "High");
static_assert(nameof::nameof_enum(DenseSigned::Middle) == "Middle");
static_assert(nameof::nameof_enum(static_cast<DenseSigned>(1)).empty());
static_assert(nameof::nameof_enum(DenseMax::High) == "High");
static_assert(nameof::nameof_enum(DenseMin::High) == "High");
static_assert(nameof::nameof_enum(DenseBool::Yes) == "Yes");
static_assert(nameof::nameof_enum(DenseBool::No) == "No");
static_assert(nameof::nameof_enum(Wrap::Low) == "Low");
static_assert(nameof::nameof_enum(DenseAlias::Alias) == "Low");
static_assert(nameof::nameof_enum(Unordered::Middle) == "Middle");
static_assert(nameof::nameof_enum(static_cast<DenseSigned>(1)).data()[0] == '\0');

enum class TinyAliases { First = 1, Alias = 1, Third = 3, Last = 7 };
enum class DenseNames { First = -2, Second = -1, Middle = 0, Fourth = 1, Last = 2 };
static_assert(nameof::nameof_enum(TinyAliases::Alias) == "First");
static_assert(nameof::nameof_enum(TinyAliases::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<TinyAliases>(2)).empty());
static_assert(nameof::nameof_enum(DenseNames::First) == "First");
static_assert(nameof::nameof_enum(DenseNames::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<DenseNames>(5)).empty());
static_assert(NamePointer<nameof::nameof_enum(DenseNames::Last).data()>::value[4] == '\0');

int metadata_check() {
  for (const auto& item : nameof::detail::reflection::entries_v<Ordered>) {
    if (item.name.empty() || item.name.data()[item.name.size()] != '\0') {
      return 1;
    }
  }
  runtime_custom = true;
  if (nameof::nameof_enum(Changing::Value) != "RuntimeCustom") {
    return 2;
  }
  runtime_custom = false;
  if (nameof::nameof_enum(Changing::Value) != "Value") {
    return 3;
  }
  return nameof::nameof_enum_or(static_cast<Empty>(0), "fallback") == "fallback" ? 0 : 4;
}
#else
// Batch signature parsing and block storage of the compiler-specific scanner.
using nameof::detail::signature_names;

constexpr auto gcc_names = signature_names<3>("auto nb() [with auto ...V = {E::A, (E)-2, E::B}");
constexpr auto clang_names = signature_names<3>("auto nb() [V = <E::A, (E)2, E::B>");
constexpr auto msvc_names = signature_names<3>("auto __cdecl nb<E::A,(enum E)0x2,E::B");
static_assert(gcc_names.valid && gcc_names.count == 2 && gcc_names.names[0] == "A" && gcc_names.names[2] == "B");
static_assert(clang_names.valid && clang_names.count == 2 && clang_names.names[1].empty());
static_assert(msvc_names.valid && msvc_names.count == 2 && msvc_names.names[2] == "B");
constexpr auto simple_casts = signature_names<5>("auto nb() [V = <(E)-12, (enum N::E)0xFF, (E)001, (E)-0x1, E::A>");
static_assert(simple_casts.valid && simple_casts.count == 1 && simple_casts.names[4] == "A");
constexpr auto quoted_casts = signature_names<3>("auto __cdecl nb<(enum `anonymous namespace'::E)0x1,(enum `void __cdecl f(int,int)'::E)-0x2,E::A");
static_assert(quoted_casts.valid && quoted_casts.count == 1 && quoted_casts.names[2] == "A");
static_assert(!signature_names<1>("auto nb<(enum `unterminated::E)1").valid);
constexpr auto repeated_casts = signature_names<4>("auto nb() [V = <(Host<void(int,int), int[2]>::E)-1, (Host<void(int,int), int[2]>::E)2, (Other<void(int,int), int[2]>::E)3, (Other<void(int,int), int[2]>::E)4>");
static_assert(repeated_casts.valid && repeated_casts.count == 0);
constexpr auto parenthesized_name = signature_names<2>("auto nb() [V = <(E)1, (E)::A>");
static_assert(parenthesized_name.valid && parenthesized_name.names[1] == "A");
constexpr auto reused_stack = signature_names<4>("auto nb() [V = <Host<int,int>::E::A, (E)3, Other<int[2]>::E::B, (N::E)-7>");
static_assert(reused_stack.valid && reused_stack.count == 2 && reused_stack.names[0] == "A" && reused_stack.names[2] == "B");
static_assert(!signature_names<2>("auto nb() [V = <(E)1,>").valid);
static_assert(!signature_names<2>("auto nb() [V = <(E)1, (E)0x>").valid);
static_assert(!signature_names<2>("auto nb() [V = <(E)1, (E)2junk>").valid);

static_assert(signature_names<0>("auto nb() [V = <>").valid);
static_assert(signature_names<0>("auto nb() [with auto ...V = {}").valid);
static_assert(signature_names<0>("auto __cdecl nb<").valid);

constexpr auto nested = signature_names<3>("auto nb() [V = <Host<void(int, int), int(*)[2]>::E::A, ((anonymous namespace)::E)2, f()::(anonymous class)::E::B>");
static_assert(nested.valid && nested.count == 2 && nested.names[0] == "A" && nested.names[2] == "B");
constexpr auto scopes = signature_names<2>("auto __cdecl nb<`anonymous namespace'::E::A,`void __cdecl f<int,int>(void)'::E::B");
static_assert(scopes.valid && scopes.names[0] == "A" && scopes.names[1] == "B");
constexpr auto literals = signature_names<2>("auto nb() [V = <S<'<', u8'>'>::E::A, S<\"a,b>\", L'\\''>::E::B>");
static_assert(literals.valid && literals.names[0] == "A" && literals.names[1] == "B");
constexpr auto literal_casts = signature_names<3>("auto nb() [V = <(S<L'a', u'('>::E)1, (S<L'a', u'('>::E)2, S<L'a', u'('>::E::A>");
static_assert(literal_casts.valid && literal_casts.count == 1 && literal_casts.names[2] == "A");
static_assert(signature_names<1>("auto nb() [V = <f()->T::E::A>").valid);
static_assert(signature_names<1>("auto __cdecl nb<-::E::A").names[0] == "A");
static_assert(signature_names<1>("auto __cdecl nb<A").names[0] == "A");

// `auto nb() [V = <T<T<...>>::E::A>`: the delimiter stack holds 64 levels, one more rejects the block instead of overflowing.
template <std::size_t Depth>
constexpr auto nested_signature() {
  constexpr nameof::string_view head = "auto nb() [V = <", tail = "::E::A>";
  constexpr std::size_t open = head.size() + 2 * Depth, close = open + Depth;
  std::array<char, close + tail.size()> text{};
  // Indexed by i alone, so MSVC /analyze can bound every write (C28020).
  for (std::size_t i = 0; i < text.size(); ++i) {
    if (i < head.size()) {
      text[i] = head[i];
    } else if (i < open) {
      text[i] = (i - head.size()) % 2 == 0 ? 'T' : '<';
    } else if (i < close) {
      text[i] = '>';
    } else {
      text[i] = tail[i - close];
    }
  }
  return text;
}

template <std::size_t Depth>
inline constexpr auto nested_signature_v = nested_signature<Depth>();

constexpr auto deepest = signature_names<1>({nested_signature_v<64>.data(), nested_signature_v<64>.size()});
static_assert(deepest.valid && deepest.names[0] == "A");
static_assert(!signature_names<1>({nested_signature_v<65>.data(), nested_signature_v<65>.size()}).valid);

#if defined(__cpp_consteval)
consteval
#else
constexpr
#endif
bool rejects_malformed() {
  constexpr nameof::string_view inputs[] = {
    "", "invalid", "V = {", "V = <", "auto nb() [V = <>",
    "auto nb() [V = <E::A>", "auto nb() [V = <E::A,>", "auto nb() [V = <,E::B>",
    "auto nb() [V = <E::A,,E::B>", "auto nb() [V = <E::A, E::B, E::C>",
    "auto nb() [V = <E::A E::B>", "auto nb() [V = <E::A, E::B>extra",
    "auto nb() [V = <E::A, S<int::B>", "auto nb() [V = <E::A, S([)]::B>",
    "auto nb() [V = <E::A, (E)>", "auto nb() [V = <E::A, (E)-", "auto nb() [V = <E::A, (E)0x>",
    "auto nb() [V = <E::A, (E)1garbage>", "auto nb() [V = <E::A, 1+2>",
    "auto nb() [V = <E::A, S<'x>::B>", "auto nb() [V = <E::A, S<\"x>::B>", "auto nb() [V = <E::A, S<1'0>::B>",
    "auto nb() [V = <E::A, (E)--1>", "auto nb() [V = <E::A, ()>", "auto nb() [V = <E::A, -1>",
    "auto nb() [V = <E::A, E::B,>", "auto nb() [V = <(E)1, (E)2,>", "auto nb<E::A,E::B,", "auto nb<(enum E)0x1,(enum E)0x2,",
    "auto nb<E::A, `unterminated::B", "auto nb<E::A, S<R\"(x)\">::B"
  };
  for (const auto input : inputs) {
    const auto parsed = signature_names<2>(input);
    if (parsed.valid || parsed.count != 0 || !parsed.names[0].empty() || !parsed.names[1].empty()) {
      return false;
    }
  }
  return true;
}
static_assert(rejects_malformed());
static_assert(!signature_names<6>({"invalid", 7}).valid);

enum class Batched { First = 0, Alias = 0, Last = 512 };
enum class Fallback { First = 0, Alias = 0, Last = 512 };
enum class Rejected { First = 0, Last = 1 };
enum class Lazy { Value = 0 };
enum class Wide { First = 0, Last = 4096 };
enum Unfixed { Zero = 0, Top = 7 };

template <> struct nameof::customize::enum_range<Batched> { static constexpr int min = 0, max = 512; };
template <> struct nameof::customize::enum_range<Fallback> { static constexpr int min = 0, max = 512; };
template <> struct nameof::customize::enum_range<Rejected> { static constexpr int min = 0, max = 1; };
template <> struct nameof::customize::enum_range<Wide> { static constexpr int min = 0, max = 4096; };
template <> struct nameof::customize::enum_range<Lazy> { static constexpr int min = 1, max = 0; };
template <> constexpr bool nameof::detail::batch_supported<Fallback>() noexcept { return false; }
// Inject a rejected parse into the same dispatch used by real block signatures.
template <> inline constexpr auto nameof::detail::block_names_v<Rejected, false, 2, 0, 0> = signature_names<2>("auto nb<Rejected::First,,Rejected::Last");

#if !defined(NAMEOF_TEST_FALLBACK)
static_assert(nameof::detail::block_names_v<Batched, false, 513, 0, 0>.valid);
static_assert(nameof::detail::block_names_v<Batched, false, 513, 0, 512>.valid);
static_assert(nameof::detail::block_names_v<Wide, false, 4097, 0, 512>.valid);
static_assert(nameof::detail::block_names_v<backend_test::CharHost<'<', L'>'>::E, false, 256, -128, 0>.valid);
// Candidates outside the value range [0, 7]: Clang 16-19 need -Wenum-constexpr-conversion ignored, Clang 20+ the cast filter.
static_assert(nameof::detail::block_names_v<Unfixed, false, 256, -128, 0>.valid);
static_assert(sizeof(nameof::detail::block_offsets_v<Wide, false, 4097, 0, 512>.offsets) == sizeof(std::uint16_t));
#endif
static_assert(!nameof::detail::block_names_v<Fallback, false, 513, 0, 0>.valid);
static_assert(!nameof::detail::block_names_v<Rejected, false, 2, 0, 0>.valid);
static_assert(nameof::nameof_enum(Rejected::First) == "First" && nameof::nameof_enum(Rejected::Last) == "Last");
static_assert(nameof::nameof_enum<Lazy::Value>() == "Value");
static_assert(nameof::nameof_enum(Wide::Last) == "Last" && nameof::detail::count_v<Wide> == 2);
static_assert(nameof::detail::block_offsets_v<Wide, false, 4097, 0, 512>.count == 0);
static_assert(nameof::detail::count_v<Unfixed> == 2);
// Values are stored as the underlying type, so enums of one underlying type and size share the array.
static_assert(std::is_same_v<decltype(nameof::detail::values_v<Wide>), decltype(nameof::detail::values_v<Rejected>)>);
static_assert(nameof::nameof_enum(Zero) == "Zero" && nameof::nameof_enum(Top) == "Top");
static_assert(nameof::detail::name_chars_v<Unfixed, false>.size() == sizeof("Zero") + sizeof("Top"));
static_assert(nameof::detail::names_v<Unfixed>[1].data() == nameof::detail::names_v<Unfixed>[0].data() + sizeof("Zero"));

bool batch_check() {
  for (int i = 0; i <= 513; ++i) {
    const auto a = nameof::nameof_enum(static_cast<Batched>(i));
    const auto b = nameof::nameof_enum(static_cast<Fallback>(i));
    if (a != b || (!a.empty() && (a.data()[a.size()] != '\0' || b.data()[b.size()] != '\0'))) {
      return false;
    }
  }
  return true;
}
#endif

#if defined(__cpp_generic_lambdas) && __cpp_generic_lambdas >= 201707L
// GCC 13/14 rechecks private access of cstring storage in templated lambdas (magic_enum#452).
enum class LambdaValue { First = 1, Second = 2 };
inline constexpr nameof::cstring<0> empty_name{};

bool lambda_check() {
  const auto check = []<std::size_t... K>(std::index_sequence<K...>) {
    constexpr std::array<nameof::string_view, 4> names = {
      nameof::nameof_enum<LambdaValue::First>(),
      nameof::nameof_enum<LambdaValue::Second>(),
      nameof::nameof_type<int>(),
      empty_name
    };
    return (((names[K].empty() || names[K].data()[0] != '\0') && names[K].data()[names[K].size()] == '\0') && ...);
  };
  return check(std::make_index_sequence<4>{});
}
#endif

int main() {
  if (!definition_test::check()) {
    return 1;
  }
  if (const auto rc = backend_check(); rc != 0) {
    return 10 + rc;
  }
  if (const auto rc = lookup_check(); rc != 0) {
    return 20 + rc;
  }
  if (const auto rc = shared_config::check(); rc != 0) {
    return 40 + rc;
  }
#if defined(NAMEOF_TEST_STD_REFLECTION)
  if (const auto rc = metadata_check(); rc != 0) {
    return 50 + rc;
  }
#else
  if (!batch_check()) {
    return 50;
  }
#endif
#if defined(__cpp_generic_lambdas) && __cpp_generic_lambdas >= 201707L
  if (!lambda_check()) {
    return 60;
  }
#endif
  return 0;
}
