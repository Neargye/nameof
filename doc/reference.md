# Reference

* [`NAMEOF` macro that obtains simple (unqualified) string name of variable, function, macro.](#nameof)
* [`NAMEOF_FULL` macro that obtains simple (unqualified) full (with template suffix) string name of variable, function, macro.](#nameof_full)
* [`NAMEOF_RAW` macro that obtains raw string name of variable, function, macro.](#nameof_raw)
* [`nameof_enum` function that obtains simple (unqualified) string enum name of enum variable.](#nameof_enum)
* [`NAMEOF_ENUM` macro that obtains simple (unqualified) string enum name of enum variable.](#nameof_enum-1)
* [`NAMEOF_CONST_ENUM` macro that obtains simple (unqualified) string enum name of static storage enum variable.](#NAMEOF_CONST_ENUM)
* [`nameof_type` function that obtains string name of type, reference and cv-qualifiers are ignored.](#nameof_type)
* [`nameof_full_type` function that obtains string name of full type, with reference and cv-qualifiers.](#nameof_full_type)
* [`NAMEOF_TYPE` macro that obtains string name of type, reference and cv-qualifiers are ignored.](#nameof_type-1)
* [`NAMEOF_FULL_TYPE` macro that obtains string name of full type, with reference and cv-qualifiers.](#nameof_full_type-1)
* [`NAMEOF_TYPE_EXPR` macro that obtains string name type of expression, reference and cv-qualifiers are ignored.](#nameof_type_expr)
* [`NAMEOF_FULL_TYPE_EXPR` macro that obtains string name full type of expression, with reference and cv-qualifiers.](#nameof_full_type_expr)

## Synopsis

* Before use, read the [limitations](limitations.md) of functionality.

* All functions `constexpr` and `noexcept`.

* To check is nameof_type supported compiler use macro `NAMEOF_TYPE_SUPPORTED` or constexpr constant `nameof::is_nameof_type_supported`.</br>
  If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

* To check is nameof_enum supported compiler use macro `NAMEOF_ENUM_SUPPORTED` or constexpr constant `nameof::is_nameof_enum_supported`.</br>
  If nameof_enum used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_ENUM_NO_CHECK_SUPPORT`.

## `NAMEOF`

* Macro that obtains simple (unqualified) string name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string.

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

* Macro that obtains simple (unqualified) full (with template suffix) string name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  // Full name of template function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Full name of template member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

## `NAMEOF_RAW`

* Macro that obtains raw string name of variable, function, macro.

* Returns `nameof::cstring` - constexpr implementation of an string.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  NAMEOF_RAW(::somevar.somefield) -> "::somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```

## `nameof_enum`

* Function that obtains simple (unqualified) string enum name of enum variable.

* Returns `std::string_view`.

* Enum variable to string.

  * If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `std::string_view`.

  * Examples

    ```cpp
    auto color = Color::RED;
    nameof::nameof_enum(color) -> "RED"
    ```

* Static storage enum variable to string.

  * This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md#nameof-enum).

  * If argument does not have name, occurs the compilation error `"Enum value does not have a name."`.

  * Examples

    ```cpp
    nameof::nameof_enum<Color::GREEN>() -> "GREEN"
    ```

## `NAMEOF_ENUM`

* Macro that obtains simple (unqualified) string enum name of enum variable.

* Returns `std::string_view`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `std::string_view`.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  ```

## `NAMEOF_CONST_ENUM`

* Macro macro that obtains simple (unqualified) string enum name of static storage enum variable.

* Returns `std::string_view`.

* This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md#nameof-enum).

* If argument does not have name, occurs the compilation error `"Enum value does not have a name."`.

* Examples

  ```cpp
  NAMEOF_CONST_ENUM(Color::GREEN) -> "GREEN"
  ```

## `nameof_type`

* Function macro that obtains string name of type, reference and cv-qualifiers are ignored.

* Returns `nameof::cstring` - constexpr implementation of an string.

* In all cases, reference and cv-qualifiers are ignored by `nameof_type` (that is, `nameof_type<const T&>() == nameof_type<T>()`).

* Returns compiler-specific type name.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  nameof::nameof_type<T>() -> "int"
  ```

## `nameof_full_type`

* Function that obtains string name of full type, with reference and cv-qualifiers.

* Returns `nameof::cstring` - constexpr implementation of an string.

* Returns compiler-specific type name.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  nameof::nameof_full_type<T>() -> "const int&"
  ```

## `NAMEOF_TYPE`

* Macro macro that obtains string name of type, reference and cv-qualifiers are ignored.

* Returns `nameof::cstring` - constexpr implementation of an string.

* In all cases, reference and cv-qualifiers are ignored by `NAMEOF_TYPE` (that is, `NAMEOF_TYPE(const T&) == NAMEOF_TYPE(T)`).

* Returns compiler-specific type name.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "int"
  ```

## `NAMEOF_FULL_TYPE`

* Macro that obtains string name of full type, with reference and cv-qualifiers.

* Returns `nameof::cstring` - constexpr implementation of an string.

* Returns compiler-specific type name.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "const int&"
  ```

## `NAMEOF_TYPE_EXPR`

* Macro that obtains string name type of expression, reference and cv-qualifiers are ignored.

* Returns `nameof::cstring` - constexpr implementation of an string.

* Returns returns compiler-specific type name.

* In all cases, reference and cv-qualifiers are ignored.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_TYPE_EXPR(var) -> "int"
  ```

## `NAMEOF_FULL_TYPE_EXPR`

* Macro that obtains string name full type of expression, with reference and cv-qualifiers.

* Returns `nameof::cstring` - constexpr implementation of an string.

* Returns compiler-specific type name.

* If argument does not have name, occurs the compilation error `"Expression does not have a name."`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_FULL_TYPE_EXPR(var) -> "const int&"
  ```
