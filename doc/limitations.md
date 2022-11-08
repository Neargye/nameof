# Limitations

## Nameof

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

## Nameof Type

* This library uses a compiler-specific hack (based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`).

* nameof_type and nameof_type_rtti returns compiler-specific type name.

* To check is nameof_type supported compiler use macro `NAMEOF_TYPE_SUPPORTED` or constexpr constant `nameof::is_nameof_type_supported`.</br>
  If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

* To check is nameof_type_rtti supported compiler use macro `NAMEOF_TYPE_RTTI_SUPPORTED` or constexpr constant `nameof::is_nameof_type_rtti_supported`.</br>
  If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

## Nameof Enum

* This library uses a compiler-specific hack (based on `__PRETTY_FUNCTION__` / `__FUNCSIG__`), which works on Clang >= 5, MSVC >= 15.3 and GCC >= 9.

* Do not use [nameof](https://github.com/Neargye/nameof) and [magic_enum](https://github.com/Neargye/magic_enum) in the same project to get enum name.

* To check is nameof_enum supported compiler use macro `NAMEOF_ENUM_SUPPORTED` or constexpr constant `nameof::is_nameof_enum_supported`.</br>
  If nameof_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_ENUM_NO_CHECK_SUPPORT`.

* Enum value must be in range `[NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]`.

  * By default `NAMEOF_ENUM_RANGE_MIN = -128`, `NAMEOF_ENUM_RANGE_MAX = 128`.

  * `NAMEOF_ENUM_RANGE_MIN` must be less or equals than `0` and must be greater than `INT16_MIN`.

  * `NAMEOF_ENUM_RANGE_MAX` must be greater than `0` and must be less than `INT16_MAX`.

  * If need another range for all enum types by default, redefine the macro `NAMEOF_ENUM_RANGE_MIN` and `NAMEOF_ENUM_RANGE_MAX`.

    ```cpp
    #define NAMEOF_ENUM_RANGE_MIN 0
    #define NAMEOF_ENUM_RANGE_MAX 256
    #include <nameof.hpp>
    ```

  * If need another range for specific enum type, add specialization `enum_range` for necessary enum type. Specialization of `enum_range` must be injected in `namespace nameof::customize`.

    ```cpp
    #include <nameof.hpp>

    enum class number { one = 100, two = 200, three = 300 };

    template <>
    struct nameof::customize::enum_range<number> {
      static constexpr int min = 100;
      static constexpr int max = 300;
    };
    ```

* Won't work if a value is aliased, work with enum-aliases is compiler-implementation-defined.

* Won't work if the enum is a forward declaration.

* Intellisense Visual Studio may have some problems analyzing `nameof`.
