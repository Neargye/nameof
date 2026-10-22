# Limitations

Type, member, and pointer names come from compiler-specific function signatures. See [compiler compatibility](reference.md#compiler-compatibility).

Complex expressions and operator names in template arguments may not parse correctly. Use `NAMEOF_RAW` for expression text.

## C++26 Standard Reflection

Standard reflection is selected automatically for enum APIs when available.

* Runtime enum lookup covers declared enumerators and ignores `NAMEOF_ENUM_RANGE_MIN`, `NAMEOF_ENUM_RANGE_MAX`, and `customize::enum_range<E>`.

* Aliased values use the first matching enumerator in the reflected sequence.

* `customize::enum_name(E)` overrides reflection for declared or synthetic values.

To use range-based reflection, define `NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` before including `nameof.hpp` in every translation unit. Mixing reflection backends violates the ODR.

## Nameof

* `NAMEOF` and `NAMEOF_FULL` fail to compile if the expression has no name. Use `NAMEOF_RAW` for expression text.

* `NAMEOF` and `NAMEOF_FULL` do not support raw string literals. Use `NAMEOF_RAW` instead.

## Nameof Type

* Type-name results are compiler-specific unless customized.

* Short type name APIs reject array, pointer, member pointer, and function types.

* RTTI-based APIs follow `typeid` rules: the dynamic type is reported only for polymorphic glvalues.

* `nameof_member` and `nameof_pointer` return an empty name for operators.

* With MSVC, `nameof_member` returns an empty name for virtual member functions and for member functions of local classes declared inside an operator.

* With MSVC, same-type union members at the same offset may resolve to the first member's name.

* With Visual Studio before 2022 17.14, `nameof_member` does not compile for data members of local or anonymous-namespace classes.

## Nameof Enum

* With compiler-specific reflection, automatic runtime lookup of ordinary enum values is limited to `[NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]`. Standard reflection ignores the range. `NAMEOF_ENUM_CONST`, `nameof::nameof_enum<V>()`, `NAMEOF_ENUM_FLAG`, and `nameof::nameof_enum_flag()` are not restricted by it.

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

* With compiler-specific reflection, names of aliased enum values are compiler-dependent.

* `customize::enum_name(E)` takes precedence over either backend, including for values outside the range or reflected sequence. Flags use customization for each set bit.

* `nameof_enum(E)` returns custom names without copying. Their storage must outlive the result and provide null termination if needed. `nameof_enum<V>()` copies them into a null-terminated `cstring`.

* With compiler-specific reflection, a non-empty scan range must contain an enumerator for `nameof_enum(E)`. `nameof_enum_flag` requires a single-bit enumerator. A custom name in the scan range also satisfies these checks.

* Standard reflection requires complete enums for runtime lookup.

* Visual Studio IntelliSense may have problems analyzing some `nameof` expressions.
