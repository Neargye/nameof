# Reference

* [`NAMEOF` macro that obtains simple (unqualified) string name of variable, function, macro.](#nameof)
* [`NAMEOF_FULL` macro that obtains simple (unqualified) full (with template suffix) string name of variable, function, macro.](#nameof_full)
* [`NAMEOF_RAW` macro that obtains raw string name of variable, function, macro.](#nameof_raw)
* [`nameof_enum` function that obtains simple (unqualified) string name of enum variable.](#nameof_enum)
* [`NAMEOF_ENUM` macro that obtains simple (unqualified) string name of enum variable.](#nameof_enum-1)
* [`NAMEOF_ENUM_CONST` macro that obtains simple (unqualified) string name of static storage enum variable.](#nameof_enum_const)
* [`nameof_enum_flag` function that obtains simple (unqualified) string name of enum variable.](#nameof_enum_flag)
* [`NAMEOF_ENUM_FLAG` function that obtains simple (unqualified) string name of enum variable.](#nameof_enum_flag-1)
* [`nameof_type` function that obtains string name of type, reference and cv-qualifiers are ignored.](#nameof_type)
* [`nameof_full_type` function that obtains string name of full type, with reference and cv-qualifiers.](#nameof_full_type)
* [`NAMEOF_TYPE` macro that obtains string name of type, reference and cv-qualifiers are ignored.](#nameof_type-1)
* [`NAMEOF_FULL_TYPE` macro that obtains string name of full type, with reference and cv-qualifiers.](#nameof_full_type-1)
* [`NAMEOF_TYPE_EXPR` macro that obtains string name type of expression, reference and cv-qualifiers are ignored.](#nameof_type_expr)
* [`NAMEOF_FULL_TYPE_EXPR` macro that obtains string name full type of expression, with reference and cv-qualifiers.](#nameof_full_type_expr)
* [`NAMEOF_TYPE_RTTI` macro that obtains string name type, using RTTI.](#nameof_type_rtti)

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

* Function that obtains simple (unqualified) string name of enum variable.

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

* Macro that obtains simple (unqualified) string name of enum variable.

* Returns `std::string_view`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `std::string_view`.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  ```

## `NAMEOF_ENUM_CONST`

* Macro that obtains simple (unqualified) string name of static storage enum variable.

* Returns `std::string_view`.

* This version is much lighter on the compile times and is not restricted to the enum_range [limitation](limitations.md#nameof-enum).

* If argument does not have name, occurs the compilation error `"Enum value does not have a name."`.

* Examples

  ```cpp
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  ```

## `nameof_enum_flag`

* Function that obtains simple (unqualified) string name of enum flag variable.

* Returns `std::string`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `std::string`.

* Examples

  ```cpp
  enum class AnimalFlags { HasClaws = 1 << 0, CanFly = 1 << 1, EatsFish = 1 << 2, Endangered = 1 << 3 };
  auto flag = AnimalFlags::Endangered;
  nameof_enum_flag(flag) -> "Endangered"
  flag = AnimalFlags::Endangered | AnimalFlags::CanFly;
  nameof_enum_flag(flag) -> "CanFly|Endangered"
  nameof_enum_flag(flag) -> "HasClaws|CanFly"

  nameof_enum(HasClaws | CanFly) -> ""
  ```

## `NAMEOF_ENUM_FLAG`

* Macro that obtains simple (unqualified) string name of enum flag variable.

* Returns `std::string`.

* If argument does not have name or [out of range](limitations.md#nameof-enum), returns empty `std::string`.

* Examples

  ```cpp
  enum class AnimalFlags { HasClaws = 1 << 0, CanFly = 1 << 1, EatsFish = 1 << 2, Endangered = 1 << 3 };
  auto flag = AnimalFlags::Endangered;
  NAMEOF_ENUM_FLAG(flag) -> "Endangered"
  flag = AnimalFlags::Endangered | AnimalFlags::CanFly;
  NAMEOF_ENUM_FLAG(flag) -> "CanFly|Endangered"
  NAMEOF_ENUM_FLAG(flag) -> "HasClaws|CanFly"

  NAMEOF_ENUM(HasClaws | CanFly) -> ""
  ```

## `nameof_type`

* Function that obtains string name of type, reference and cv-qualifiers are ignored.

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

* Macro that obtains string name of type, reference and cv-qualifiers are ignored.

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

## `NAMEOF_TYPE_RTTI`

* Macro that obtains string name of type, using RTTI.

* Returns demangled RTTI type name.

* Examples

  ```cpp
  struct Base { virtual void foo() {} };
  struct Derived : Base {};

  Base* ptr = new Derived();
  NAMEOF_TYPE_RTTI(ptr)  -> "Base *"
  NAMEOF_TYPE_RTTI(*ptr) -> "Derived"
  ```
