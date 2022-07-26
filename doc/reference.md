# Reference

* [`NAMEOF` obtains simple name of variable, function, macro.](#nameof)
* [`NAMEOF_FULL` obtains full name of variable, function, macro.](#nameof_full)
* [`NAMEOF_RAW` obtains raw name of variable, function, macro.](#nameof_raw)
* [`NAMEOF_ENUM` obtains name of enum variable.](#nameof_enum)
* [`NAMEOF_ENUM_OR` Obtains name of enum variable or default value if enum variable out of range.](#nameof_enum_or)
* [`NAMEOF_ENUM_CONST` obtains name of static storage enum variable.](#nameof_enum_const)
* [`NAMEOF_ENUM_FLAG` obtains name of enum-flags variable.](#nameof_enum_flag)
* [`NAMEOF_TYPE` obtains type name.](#nameof_type)
* [`NAMEOF_FULL_TYPE` obtains full type name.](#nameof_full_type)
* [`NAMEOF_SHORT_TYPE` obtains short type name.](#nameof_short_type)
* [`NAMEOF_TYPE_EXPR` obtains type name of expression.](#nameof_type_expr)
* [`NAMEOF_FULL_TYPE_EXPR` obtains full type name of expression.](#nameof_full_type_expr)
* [`NAMEOF_SHORT_TYPE_EXPR` obtains short type name of expression.](#nameof_short_type_expr)
* [`NAMEOF_TYPE_RTTI` obtains type name, using RTTI.](#nameof_type_rtti)
* [`NAMEOF_FULL_TYPE_RTTI` obtains short type name, using RTTI.](#nameof_full_type_rtti)
* [`NAMEOF_SHORT_TYPE_RTTI` obtains short type name, using RTTI.](#nameof_short_type_rtti)

## Synopsis

* Before use, read the [limitations](limitations.md) of functionality.

* To check is nameof_type supported compiler use macro `NAMEOF_TYPE_SUPPORTED` or constexpr constant `nameof::is_nameof_type_supported`.</br>
  If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

* To check is nameof_type_rtti supported compiler use macro `NAMEOF_TYPE_RTTI_SUPPORTED` or constexpr constant `nameof::is_nameof_type_rtti_supported`.</br>
  If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

* To check is nameof_enum supported compiler use macro `NAMEOF_ENUM_SUPPORTED` or constexpr constant `nameof::is_nameof_enum_supported`.</br>
  If nameof_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_ENUM_NO_CHECK_SUPPORT`.

* To add custom enum or type names see the [example](../example/example_custom_name.cpp).

* To change the type of strings, use special macros:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define NAMEOF_USING_ALIAS_STRING using string = my_lib::String;
  #define NAMEOF_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #include <nameof.hpp>
  ```

## `NAMEOF`

* Obtains name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string. Marked `constexpr` and `noexcept`.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

    ```cpp
    // Name of variable.
    NAMEOF(somevar) -> "somevar"

    // Name of member variable.
    NAMEOF(person.address.zip_code) -> "zip_code"

    // Name of function.
    NAMEOF(foo<int, float>()) -> "foo"

    // Name of member function.
    NAMEOF(somevar.some_method()) -> "some_method"
    NAMEOF(somevar.some_method<int>()) -> "some_method"

    // Name of macro.
    NAMEOF(__LINE__) -> "__LINE__"
    NAMEOF(NAMEOF(structvar)) -> "NAMEOF"
    ```

## `NAMEOF_FULL`

* Obtains full (with template suffix) name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string. Marked `constexpr` and `noexcept`.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  // Full name of template function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Full name of template member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

## `NAMEOF_RAW`

* Obtains raw name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string. Marked `constexpr` and `noexcept`.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  NAMEOF_RAW(::somevar.somefield) -> "::somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```


## `NAMEOF_ENUM`

* Obtains name of enum variable.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `string_view`, in debug occurs assert.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"
  ```

# `NAMEOF_ENUM_OR`

* Obtains name of enum variable or default value if enum variable out of range.

* Returns `string`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns `default_value`.

## `NAMEOF_ENUM_CONST`

* Obtains name of static storage enum variable.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md#nameof-enum).

* If argument does not have name, occurs the compilation error `"Enum value does not have a name."`.

* Examples

  ```cpp
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"
  ```

## `NAMEOF_ENUM_FLAG`

* Obtains name of enum flag variable.

* Returns `string`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `string`, in debug occurs assert.

* Examples

  ```cpp
  enum class AnimalFlags { HasClaws = 1 << 0, CanFly = 1 << 1, EatsFish = 1 << 2, Endangered = 1 << 3 };
  auto flag = AnimalFlags::Endangered;

  NAMEOF_ENUM_FLAG(flag) -> "Endangered"
  nameof_enum_flag(flag) -> "Endangered"

  flag = AnimalFlags::CanFly | AnimalFlags::Endangered;
  NAMEOF_ENUM_FLAG(flag) -> "CanFly|Endangered"
  nameof_enum_flag(flag) -> "CanFly|Endangered"

  NAMEOF_ENUM(HasClaws | CanFly) -> ""
  nameof_enum(HasClaws | CanFly) -> ""
  ```

## `NAMEOF_TYPE`

* Obtains type name, reference and cv-qualifiers are ignored.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* In all cases, reference and cv-qualifiers are ignored by `NAMEOF_TYPE` (that is, `NAMEOF_TYPE(const T&) == NAMEOF_TYPE(T)`).

* Returns compiler-specific type name.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.


* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "int"
  nameof::nameof_type<T>() -> "int"
  ```

## `NAMEOF_FULL_TYPE`

* Obtains full type name, with reference and cv-qualifiers.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "const int&"
  nameof::nameof_full_type<T>() -> "const int&"
  ```

## `NAMEOF_SHORT_TYPE`

* Obtains short type name.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.

* Examples

  ```cpp
  using T = const my::detail::SomeClass<int>&;
  NAMEOF_SHORT_TYPE(T) -> "SomeClass"
  nameof::nameof_short_type<T>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_EXPR`

* Obtains string name type of expression, reference and cv-qualifiers are ignored.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* In all cases, reference and cv-qualifiers are ignored.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_TYPE_EXPR(var) -> "int"
  nameof::nameof_type<decltype(var)>() -> "int"
  ```

## `NAMEOF_FULL_TYPE_EXPR`

* Obtains full type name of expression, with reference and cv-qualifiers.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_FULL_TYPE_EXPR(var) -> "const int&"
  nameof::nameof_full_type<decltype(var)>() -> "const int&"
  ```

## `NAMEOF_SHORT_TYPE_EXPR`

* Obtains short type name of expression.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* If type does not have name, occurs the compilation error `"Type does not have a name."`.

* Examples

  ```cpp
  const my::detail::SomeClass<int> var;
  NAMEOF_SHORT_TYPE_EXPR(var) -> "SomeClass"
  nameof::nameof_short_type<decltype(var)>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_RTTI`

* Obtains type name, using RTTI.

* Returns `string`.

* Examples

  ```cpp
  volatile const my::detail::Base* ptr = new my::detail::Derived();
  NAMEOF_TYPE_RTTI(*ptr) -> "my::detail::Derived"
  ```

## `NAMEOF_FULL_TYPE_RTTI`

* Obtains full type name, using RTTI.

* Returns `string`.

* Examples

  ```cpp
  volatile const my::detail::Base* ptr = new my::detail::Derived();
  NAMEOF_FULL_TYPE_RTTI(cv_ref) -> "volatile const my::detail::Derived&"
  ``

## `NAMEOF_SHORT_TYPE_RTTI`

* Obtains short type name, using RTTI.

* Returns `string`.

* Examples

  ```cpp
  volatile const my::detail::Base* ptr = new my::detail::Derived();
  NAMEOF_SHORT_TYPE_RTTI(*ptr) -> "Derived"
  ```
