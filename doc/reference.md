# Reference

* [`NAMEOF` obtains simple name of variable, function, macro.](#nameof)
* [`NAMEOF_FULL` obtains full name of variable, function, macro.](#nameof_full)
* [`NAMEOF_RAW` obtains raw expression text.](#nameof_raw)
* [`NAMEOF_ENUM` obtains name of enum value.](#nameof_enum)
* [`NAMEOF_ENUM_OR` obtains name of enum value or default value if no name is available.](#nameof_enum_or)
* [`NAMEOF_ENUM_CONST` obtains name of enum value at compile time.](#nameof_enum_const)
* [`NAMEOF_ENUM_FLAG` obtains name of enum flag value.](#nameof_enum_flag)
* [`NAMEOF_TYPE` obtains type name.](#nameof_type)
* [`NAMEOF_FULL_TYPE` obtains full type name.](#nameof_full_type)
* [`NAMEOF_SHORT_TYPE` obtains short type name.](#nameof_short_type)
* [`NAMEOF_TYPE_EXPR` obtains type name of expression.](#nameof_type_expr)
* [`NAMEOF_FULL_TYPE_EXPR` obtains full type name of expression.](#nameof_full_type_expr)
* [`NAMEOF_SHORT_TYPE_EXPR` obtains short type name of expression.](#nameof_short_type_expr)
* [`NAMEOF_TYPE_RTTI` obtains type name using RTTI.](#nameof_type_rtti)
* [`NAMEOF_FULL_TYPE_RTTI` obtains full type name using RTTI.](#nameof_full_type_rtti)
* [`NAMEOF_SHORT_TYPE_RTTI` obtains short type name using RTTI.](#nameof_short_type_rtti)
* [`NAMEOF_MEMBER` obtains name of member.](#nameof_member)
* [`NAMEOF_POINTER` obtains name of a function, a global or class static variable.](#nameof_pointer)

## Synopsis

* Before use, read the [limitations](limitations.md).

* Check compiler support with a macro or constexpr constant:

  | API | Macro | Constant in `nameof` |
  | --- | --- | --- |
  | Enum | `NAMEOF_ENUM_SUPPORTED` | `is_nameof_enum_supported` |
  | Type | `NAMEOF_TYPE_SUPPORTED` | `is_nameof_type_supported` |
  | RTTI type | `NAMEOF_TYPE_RTTI_SUPPORTED` | `is_nameof_type_rtti_supported` |
  | Member | `NAMEOF_MEMBER_SUPPORTED` | `is_nameof_member_supported` |
  | Pointer | `NAMEOF_POINTER_SUPPORTED` | `is_nameof_pointer_supported` |

  Unsupported APIs fail to compile when used. Define `NAMEOF_ENUM_NO_CHECK_SUPPORT` to disable enum support checks, or `NAMEOF_TYPE_NO_CHECK_SUPPORT` for the other APIs above.

* `nameof::cstring<N>` is a constexpr string of `N` characters plus a null terminator.

* Type names are compiler-specific unless customized.

* To customize a name, specialize the corresponding function in `namespace nameof::customize`: `enum_name`, `type_name`, `member_name`, or `pointer_name`. See the [enum and type example](../example/example_custom_name.cpp).

* C++26 standard reflection is selected automatically for enum APIs when available. Define `NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` before including `nameof.hpp` to retain range-based reflection. See [limitations](limitations.md#c26-standard-reflection).

* To change the string types, use these macros:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define NAMEOF_USING_ALIAS_STRING using string = my_lib::String;
  #define NAMEOF_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #include <nameof.hpp>
  ```

  The aliases must provide the subset of `std::string` and `std::string_view` operations used by nameof. See the tested [minimal interface](../test/test_aliases.cpp).

* `std::format` support for `nameof::cstring` is enabled when the standard library supports it. For `fmt`, include `<fmt/format.h>` before `<nameof.hpp>`.

* RTTI-based APIs follow `typeid` rules: the dynamic type is reported only for polymorphic glvalues.

## Compiler compatibility

nameof requires C++17 or newer and supports:

* Clang/LLVM >= 5
* GCC >= 9
* Visual Studio >= 2019

RTTI-based APIs require RTTI to be enabled. `NAMEOF_MEMBER` and `NAMEOF_POINTER` require Visual Studio >= 2019 and C++ >= 20. On Clang >= 5 and GCC >= 9, C++17 is sufficient.

## `NAMEOF`

* Obtains name of variable, function, macro.

* Returns `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* If argument has no name, compilation fails: `"Expression does not have a name."`

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
  ```

## `NAMEOF_FULL`

* Obtains full (with template suffix) name of variable, function, macro.

* Returns `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* If argument has no name, compilation fails: `"Expression does not have a name."`

* Examples

  ```cpp
  // Full name of template function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Full name of template member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

## `NAMEOF_RAW`

* Obtains raw expression text.

* Returns `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  NAMEOF_RAW(::somevar.somefield) -> "::somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```

## `NAMEOF_ENUM`

* Obtains name of enum value.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns a custom or reflected name, otherwise an empty `string_view`.

* With compiler-specific reflection, non-empty scan ranges require an enumerator. Use `NAMEOF_ENUM_OR` for fallback lookup.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"
  ```

## `NAMEOF_ENUM_OR`

* Obtains name of enum value or default value if no name is available.

* Returns `string`.

* Returns a custom or reflected name, otherwise `default_value`.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM_OR(color, "none") -> "RED"
  NAMEOF_ENUM_OR((Color)-1, "none") -> "none"
  nameof::nameof_enum_or(color, "none") -> "RED"
  nameof::nameof_enum_or((Color)-1, "none") -> "none"
  ```

## `NAMEOF_ENUM_CONST`

* Obtains name of enum value known at compile time.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* This version has a lower compile-time cost and is not restricted by the [`enum_range`](limitations.md#nameof-enum).

* If value has no name, returns empty `nameof::cstring`.

* Examples

  ```cpp
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"
  ```

## `NAMEOF_ENUM_FLAG`

* Obtains name of enum flag value.

* Returns `string`.

* Composite enumerator names are not used. The value is decomposed into named single-bit values.

* If value is zero or contains unnamed bit, returns empty `string`.

* With compiler-specific reflection, compilation fails if the enum has no single-bit enumerator, for example a forward-declared or empty enum.

* Examples

  ```cpp
  enum AnimalFlags { HasClaws = 1 << 0, CanFly = 1 << 1, EatsFish = 1 << 2, Endangered = 1 << 3 };
  auto flag = Endangered;

  NAMEOF_ENUM_FLAG(flag) -> "Endangered"
  nameof::nameof_enum_flag(flag) -> "Endangered"

  flag = static_cast<AnimalFlags>(CanFly | Endangered);
  NAMEOF_ENUM_FLAG(flag) -> "CanFly|Endangered"
  nameof::nameof_enum_flag(flag) -> "CanFly|Endangered"
  nameof::nameof_enum_flag(flag, '$') -> "CanFly$Endangered"

  NAMEOF_ENUM(static_cast<AnimalFlags>(HasClaws | CanFly)) -> ""
  nameof::nameof_enum(static_cast<AnimalFlags>(HasClaws | CanFly)) -> ""
  ```

## `NAMEOF_TYPE`

* Obtains type name. Reference and cv-qualifiers are ignored (`NAMEOF_TYPE(const T&) == NAMEOF_TYPE(T)`).

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "int"
  nameof::nameof_type<T>() -> "int"
  ```

## `NAMEOF_FULL_TYPE`

* Obtains full type name with reference and cv-qualifiers.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_FULL_TYPE(T) -> "const int&"
  nameof::nameof_full_type<T>() -> "const int&"
  ```

## `NAMEOF_SHORT_TYPE`

* Obtains short type name.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Array, pointer, member pointer, and function types are not supported.

* If type has no short name, compilation fails: `"Type does not have a short name."`

* Examples

  ```cpp
  using T = const my::detail::SomeClass<int>&;
  NAMEOF_SHORT_TYPE(T) -> "SomeClass"
  nameof::nameof_short_type<T>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_EXPR`

* Obtains type name of expression. Reference and cv-qualifiers are ignored.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_TYPE_EXPR(var) -> "int"
  nameof::nameof_type<decltype(var)>() -> "int"
  ```

## `NAMEOF_FULL_TYPE_EXPR`

* Obtains full type name of expression with reference and cv-qualifiers.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_FULL_TYPE_EXPR(var) -> "const int&"
  nameof::nameof_full_type<decltype(var)>() -> "const int&"
  ```

## `NAMEOF_SHORT_TYPE_EXPR`

* Obtains short type name of expression.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* Returns compiler-specific type name.

* Array, pointer, member pointer, and function types are not supported.

* If type has no short name, compilation fails: `"Type does not have a short name."`

* Examples

  ```cpp
  const my::detail::SomeClass<int> var;
  NAMEOF_SHORT_TYPE_EXPR(var) -> "SomeClass"
  nameof::nameof_short_type<decltype(var)>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_RTTI`

* Obtains type name using RTTI. Reference and cv-qualifiers are ignored.

* Returns `string`.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base* ptr = &derived;
  NAMEOF_TYPE_RTTI(*ptr) -> "my::detail::Derived"
  ```

## `NAMEOF_FULL_TYPE_RTTI`

* Obtains type name using RTTI and preserves reference and cv-qualifiers.

* Pointer, array, function, and member pointer types are named like `NAMEOF_FULL_TYPE`, because `typeid` reports their static type.

* Returns `string`.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base& cv_ref = derived;
  NAMEOF_FULL_TYPE_RTTI(cv_ref) -> "const volatile my::detail::Derived&"
  ```

## `NAMEOF_SHORT_TYPE_RTTI`

* Obtains short type name using RTTI. Namespace qualification, template arguments, references, and cv-qualifiers are omitted.

* Returns `string`.

* Array, pointer, member pointer, and function types are not supported.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base* ptr = &derived;
  NAMEOF_SHORT_TYPE_RTTI(*ptr) -> "Derived"
  ```

## `NAMEOF_MEMBER`

* Obtains name of member.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* If member pointer is null, returns `"nullptr"` by default.

* On older Visual Studio versions, the containing class must be fully defined and have external linkage.

* Examples

  ```cpp
  struct A {
    int this_is_the_name;
  };
  // ...
  NAMEOF_MEMBER(&A::this_is_the_name) -> "this_is_the_name"
  nameof::nameof_member<&A::this_is_the_name>() -> "this_is_the_name"
  ```

## `NAMEOF_POINTER`

* Obtains name of a function, a global or class static variable.

* Returns a reference to `nameof::cstring`, a constexpr null-terminated string type. Marked `constexpr` and `noexcept`.

* If pointer is null, returns `"nullptr"` by default.

* Examples

  ```cpp
  int someglobalvariable = 0;
  const int someglobalconstvariable = 42;
  // ...
  NAMEOF_POINTER(&someglobalconstvariable) -> "someglobalconstvariable"
  nameof::nameof_pointer<&someglobalconstvariable>() -> "someglobalconstvariable"

  constexpr auto global_ptr = &someglobalvariable;
  NAMEOF_POINTER(global_ptr) -> "someglobalvariable"
  nameof::nameof_pointer<global_ptr>() -> "someglobalvariable"
  ```
