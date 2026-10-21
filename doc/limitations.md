# Limitations

Type, member, and pointer names come from compiler-specific function signatures. See [compiler compatibility](reference.md#compiler-compatibility).

Complex expressions and operator names in template arguments are not reliably parsed. Use `NAMEOF_RAW` for the original expression text.

## C++26 Standard Reflection

Standard reflection is selected automatically for enum APIs when available.

* Runtime enum lookup covers declared enumerators and ignores `NAMEOF_ENUM_RANGE_MIN`, `NAMEOF_ENUM_RANGE_MAX`, and `customize::enum_range<E>`.

* For aliased values, the first matching enumerator encountered in the reflected sequence supplies the name.

* `customize::enum_name(E)` is checked before reflection and can provide a name for declared or synthetic values.

To use range-based reflection, define `NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` before including `nameof.hpp` in every translation unit. Mixing reflection backends violates the ODR.

## Nameof

* If argument to `NAMEOF` or `NAMEOF_FULL` has no name, compilation fails: `"Expression does not have a name."` `NAMEOF_RAW` returns raw expression text for any valid expression.

* `NAMEOF` and `NAMEOF_FULL` do not support raw string literals. Use `NAMEOF_RAW` instead.

## Nameof Type

* Type-name results are compiler-specific unless customized.

* `nameof::nameof_short_type<T>()`, `NAMEOF_SHORT_TYPE`, `NAMEOF_SHORT_TYPE_EXPR`, and `NAMEOF_SHORT_TYPE_RTTI` do not accept array, pointer, member pointer, or function types.

* RTTI-based APIs follow `typeid` rules: the dynamic type is reported only for polymorphic glvalues.

* `nameof_member` and `nameof_pointer` return an empty name for operators.

* With MSVC, `nameof_member` returns an empty name for virtual member functions and for member functions of local classes declared inside an operator.

## Nameof Enum

* With compiler-specific reflection, runtime reflection of ordinary enum values is limited to `[NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]`. Standard reflection ignores the range. `NAMEOF_ENUM_CONST`, `nameof::nameof_enum<V>()`, `NAMEOF_ENUM_FLAG`, and `nameof::nameof_enum_flag()` are not restricted by it.

  * By default, `NAMEOF_ENUM_RANGE_MIN = -128`, `NAMEOF_ENUM_RANGE_MAX = 127`.

  * The effective range is also clamped to the limits of the enum's underlying type.

  * If another range is needed for all enum types by default, redefine the macro `NAMEOF_ENUM_RANGE_MIN` and `NAMEOF_ENUM_RANGE_MAX`.

    ```cpp
    #define NAMEOF_ENUM_RANGE_MIN 0
    #define NAMEOF_ENUM_RANGE_MAX 256
    #include <nameof.hpp>
    ```

    Use the same macro values in every translation unit to avoid ODR violations.

  * If another range is needed for a specific enum type, specialize `enum_range` for that type in `namespace nameof::customize`.

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

* Names of aliased enum values are compiler-dependent with compiler-specific reflection. Standard reflection uses the first matching enumerator encountered in the reflected sequence.

* `customize::enum_name(E)` is checked before either backend. It can name values outside the compiler-specific range or the standard-reflection enumerator sequence. For flags, customization is checked independently for every set bit before backend lookup.

* `nameof_enum(E)` returns custom names as provided. If null termination is required, `customize::enum_name(E)` must provide it.

* With compiler-specific reflection, a non-empty scan range must contain an enumerator for `nameof_enum(E)`. `nameof_enum_flag` requires a single-bit enumerator. A custom name in the scan range also satisfies these checks.

* `nameof_enum_or` returns a custom or reflected name, otherwise the fallback.

* Standard reflection requires complete enums for runtime lookup.

* Visual Studio IntelliSense may have problems analyzing some `nameof` expressions.
