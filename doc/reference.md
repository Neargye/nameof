# Reference

* [`NAMEOF` macro that obtains simple (unqualified) string name of variable, function, macro.](#NAMEOF)
* [`NAMEOF_FULL` macro that obtains simple (unqualified) full (with template suffix) string name of variable, function, macro.](#NAMEOF_FULL)
* [`NAMEOF_RAW` macro that obtains raw string name of variable, function, macro.](#NAMEOF_RAW)
* [`nameof_enum(E value)` function that obtains simple (unqualified) string enum name of enum variable.](#nameof_enum(value))
* [`nameof_enum<auto V>()` function that obtains simple (unqualified) string enum name of static storage enum variable.](#nameof_enum<Value>())
* [`NAMEOF_ENUM` macro that obtains simple (unqualified) string enum name of enum variable.](#NAMEOF_ENUM)
* [`NAMEOF_CONST_ENUM` macro that obtains simple (unqualified) string enum name of static storage enum variable.](#NAMEOF_CONST_ENUM)
* [`nameof_type<T>()` function that obtains string name of type, reference and cv-qualifiers are ignored.](#nameof_type<T>())
* [`nameof_full_type<T>()` function that obtains string name of full type, with reference and cv-qualifiers.](#nameof_full_type<T>())
* [`NAMEOF_TYPE` macro that obtains string name of type, reference and cv-qualifiers are ignored.](#NAMEOF_TYPE)
* [`NAMEOF_FULL_TYPE` macro that obtains string name of full type, with reference and cv-qualifiers.](#NAMEOF_FULL_TYPE)
* [`NAMEOF_TYPE_EXPR` macro that obtains string name type of expression, reference and cv-qualifiers are ignored.](#NAMEOF_TYPE_EXPR)
* [`NAMEOF_FULL_TYPE_EXPR` macro that obtains string name full type of expression, with reference and cv-qualifiers.](#NAMEOF_FULL_TYPE_EXPR)

# Synopsis

* Before use, read the [limitations](limitations.md) of functionality.

* All functions `constexpr` and `noexcept`.

* To check is nameof_type supported compiler use macro `NAMEOF_TYPE_SUPPORTED` or constexpr constant `nameof::is_nameof_type_supported`.
If nameof_type used on unsupported compiler, occurs the compilation error. To suppress error define macro `NAMEOF_TYPE_NO_CHECK_SUPPORT`.

* To check is nameof_enum supported compiler use macro `NAMEOF_ENUM_SUPPORTED` or constexpr constant `nameof::is_nameof_enum_supported`.
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

## `nameof_enum(value)`

* Function that obtains simple (unqualified) string enum name of enum variable.

* Returns `std::string_view`.

* If argument does not have name, returns empty `std::string_view`.

* Examples
  ```cpp
  auto color = Color::RED;
  nameof::nameof_enum(color) -> "RED"
  ```

## `nameof_enum<Value>()`

* Function that obtains simple (unqualified) string enum name of static storage enum variable.

* Returns `std::string_view`.

* This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md).

* If argument does not have name, returns empty `std::string_view`.

* Examples
  ```cpp
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"
  ```

## `NAMEOF_ENUM`

* Macro that obtains simple (unqualified) string enum name of enum variable.

* Returns `std::string_view`.

* If argument does not have name, returns empty `std::string_view`.

* Examples
  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  ```

## `NAMEOF_CONST_ENUM`

* Macro macro that obtains simple (unqualified) string enum name of static storage enum variable.

* Returns `std::string_view`.

* This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md).

* If argument does not have name, returns empty `std::string_view`.

* Examples
  ```cpp
  NAMEOF_CONST_ENUM(Color::GREEN) -> "GREEN"
  ```

## `nameof_type<T>()`

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

## `nameof_full_type<T>()`

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

## `NAMEOF_TYPE_EXPR`

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
