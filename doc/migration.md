# Migration

Only changes that need action are listed. Apply every section newer than your version.

## Upgrading to 0.12.0

* `nameof_enum<V>()`, `nameof_type`, `nameof_full_type`, `nameof_short_type`, `nameof_member`, `nameof_pointer` and their macros return `nameof::name_view` by value instead of `const nameof::cstring<N>&`. Replace `auto&`, `nameof::cstring<N>` and `const nameof::cstring<N>&` with `auto` or `nameof::name_view`.

* With fmt 10, remove a user-defined `fmt::formatter<nameof::cstring<N>>`. nameof formats `cstring` itself, and the two are ambiguous.

* `NAMEOF_ENUM_SUPPORTED_ALIASES` is removed. Use `NAMEOF_ENUM_SUPPORTED`.

* Clang 9 and Xcode 11 are the minimum versions.

* `NAMEOF` and `NAMEOF_FULL` fail to compile for `static_cast<T>(x)`, `long(x)` and `sizeof(x)` instead of returning the keyword. Pass the operand or use `NAMEOF_RAW`.

* With compiler-specific reflection, GCC rejects `nameof_enum(value).data()` as a pointer template argument. Use `nameof_enum<V>().data()` (C++20 and newer).

* Clang warns with `-Wdangling` about a kept `NAMEOF(x).c_str()` or `NAMEOF(x).data()`, which dangles. Save `NAMEOF(x)` in a variable first.

## Upgrading to 0.11.0

* Enum APIs use C++26 standard reflection when available, which ignores enum ranges. Define `NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` to keep the range scan.

* GCC 9 and Visual Studio 2019 are the minimum versions.

* `NAMEOF_ENUM_RANGE_MAX` defaults to 127 instead of 128. For an enumerator equal to 128, define it or specialize `customize::enum_range`.

* `nameof_enum(value)` returns custom names without copying. Their storage must outlive the result.

* Short names of arithmetic types keep the whole name, so stored names change: `std::int64_t` is `"long int"` instead of `"int"` on GCC, `std::uint64_t` is `"unsigned long"` instead of `"long"` on Clang. Short type name APIs reject array, pointer, member pointer and function types.

* `nameof_member` and `nameof_pointer` return an empty name for operators. `nameof_member` returns `"nullptr"` for a null member pointer.

* `NAMEOF_FULL_TYPE_RTTI` writes `const volatile` instead of `volatile const` and places qualifiers of pointer, array, function and member pointer types differently.

* `cstring::value_type` is `char` and `size_type` is `std::size_t`. Names longer than `UINT16_MAX` characters fail to compile instead of being truncated.

## Upgrading to 0.10.6

* Since 0.10.5 (0.10.6 for `nameof_type` and `nameof_full_type`), names of types, members, pointers and `nameof_enum<V>()` are no longer `string_view`. Where code needs other `string_view` members, `std::hash`, `?:` with a string literal or an implicit conversion, write `std::string_view name = NAMEOF_TYPE(T);` instead of `auto`.

* `NAMEOF(x)` cannot be passed directly to a `string_view` parameter. Save it in a variable first.

* With fmt 9, include `<fmt/format.h>` before `<nameof.hpp>` to format `NAMEOF(x)`.
