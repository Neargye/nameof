# Limitations

Type, member, and pointer names come from compiler-specific function signatures. See [compiler compatibility](reference.md#compiler-compatibility).

Complex expressions, raw string literals and operator names in template arguments may not parse correctly. Use `NAMEOF_RAW` for expression text.

## C++26 Standard Reflection

Standard reflection is selected automatically for enum APIs when available.

* Runtime enum lookup covers declared enumerators and ignores `NAMEOF_ENUM_RANGE_MIN`, `NAMEOF_ENUM_RANGE_MAX`, and `customize::enum_range<E>`.

* Aliased values use the first matching enumerator in the reflected sequence.

`NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` selects compiler-specific reflection, which scans an [enum range](#nameof-enum). Define it before including `nameof.hpp`. Every translation unit must use the same backend, mixing them violates the ODR.

## Nameof

* `NAMEOF` and `NAMEOF_FULL` fail to compile for expressions without a name, such as `static_cast<long>(x)`, `long(x)` and `sizeof(x)`. Operators and C-style casts are skipped: `NAMEOF(*p)` gives `"p"`, `NAMEOF((long)x)` gives `"x"`, `NAMEOF(a + b)` gives `"b"`.

* A temporary `cstring` does not convert to `string_view` or `name_view`, and its `c_str()` and `data()` dangle. Save `NAMEOF(x)` in a variable first.

* `cstring` and `name_view` data members are public for technical reasons. Use `data()` and the string interface.

* A `name_view` template argument requires C++20 and GCC 11, Clang 18 or Visual Studio 2019 and newer (not Apple Clang). It is compared by address, not by characters, so equal names from different calls can be different arguments.

## Nameof Type

* Fundamental types keep the compiler spelling: `long long` is `long long int` on GCC and `__int64` on MSVC.

* With MSVC, type names keep the `class `, `struct `, `union ` and `enum ` prefixes (`class std::vector<...>`). clang-cl keeps them only in RTTI names. Short type names drop them.

* Type names depend on the compiler and the nameof version. Do not store or compare them between builds, use `customize::type_name` for stable names.

* Short type name APIs reject array, pointer, member pointer, and function types.

* `nameof_member` and `nameof_pointer` return an empty name for operators.

* `nameof_pointer` names complete objects and functions. For a pointer to an array element or a subobject, the result is compiler-specific.

* With MSVC, `nameof_member` returns an empty name for virtual member functions and for member functions of local classes declared inside an operator.

* With MSVC, same-type union members at the same offset may resolve to the first member's name.

* With Visual Studio before 2022 17.14, `nameof_member` requires the class of a data member to be fully defined and have external linkage (not local or in an anonymous namespace).

## Nameof Enum

* With compiler-specific reflection, `nameof_enum(value)` finds only values in `[NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]`, others get an empty name. `nameof_enum<V>()` and the flag APIs are not limited by it.

  * By default, `NAMEOF_ENUM_RANGE_MIN = -128`, `NAMEOF_ENUM_RANGE_MAX = 127`.

  * The effective range is also clamped to the limits of the enum's underlying type.

  * To change the default range, define `NAMEOF_ENUM_RANGE_MIN` and `NAMEOF_ENUM_RANGE_MAX` before including `nameof.hpp`.

    ```cpp
    #define NAMEOF_ENUM_RANGE_MIN 0
    #define NAMEOF_ENUM_RANGE_MAX 256
    #include <nameof.hpp>
    ```

    Use the same macro values in every translation unit to avoid ODR violations.

  * For a specific enum, specialize `nameof::customize::enum_range`.

    ```cpp
    #include <nameof.hpp>

    enum class number { one = 100, two = 200, three = 300 };

    template <>
    struct nameof::customize::enum_range<number> {
      static constexpr int min = 100;
      static constexpr int max = 300;
    };
    ```

  * Enum ranges are limited to fewer than `UINT16_MAX` values.

  * For an enum without a custom range, check its largest value: `static_assert(static_cast<int>(E::Max) <= NAMEOF_ENUM_RANGE_MAX);`.

* With compiler-specific reflection, names of aliased enum values are compiler-dependent.

* `customize::enum_name(E)` takes precedence over either backend, including for values outside the range or reflected sequence. Flags use customization for each set bit.

* `nameof_enum(E)` returns custom names without copying. Their storage must outlive the result and provide null termination if needed. `nameof_enum<V>()` copies them into null-terminated static storage.

* With standard reflection, `nameof_enum(E).data()` can be a pointer template argument. With compiler-specific reflection, use `nameof_enum<V>().data()` instead (C++20 and newer).

* Standard reflection requires complete enums.

* Visual Studio IntelliSense may have problems analyzing some `nameof` expressions.

* ReSharper C++ and CLion Nova evaluate enum names from version 2025.3.

## NVCC and NVC++

* Enum names are empty for enums declared inside functions.

* Unnamed types are spelled `<unnamed>` at compile time. Under nvcc, results computed at run time come from the host compiler.
