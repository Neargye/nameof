# Reference

* [`NAMEOF` obtains simple name of variable, function, macro.](#nameof)
* [`NAMEOF_FULL` obtains full name of variable, function, macro.](#nameof_full)
* [`NAMEOF_RAW` obtains raw expression text.](#nameof_raw)
* [`NAMEOF_ENUM` obtains name of enum value.](#nameof_enum)
* [`NAMEOF_ENUM_OR` obtains enum name or fallback.](#nameof_enum_or)
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
* [`NAMEOF_POINTER` obtains name of a function, global variable, or static data member.](#nameof_pointer)

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

* `NAMEOF`, `NAMEOF_FULL` and `NAMEOF_RAW` return `nameof::cstring<N>`, a constexpr null-terminated string. Save it before taking a view or `c_str()`: `constexpr auto name = NAMEOF(x);`. Compile-time names of enum values, types, members and pointers are `nameof::name_view`, a view of a static name. Both have `c_str()`, `str()`, `find`, `rfind`, `starts_with`, `ends_with`, comparisons and `operator<<`, and convert to `string_view`. `name_view::substr` returns a `string_view` and requires `pos <= size()` (checked with `NAMEOF_ASSERT`). `cstring` has no `substr`.

* In C++20 a `name_view` can be a template argument: `template <nameof::name_view Name> struct Tag` and `Tag<NAMEOF_TYPE(int)>`. See the [limitations](limitations.md#nameof).

* Specialize `enum_name`, `type_name`, `member_name`, or `pointer_name` in `nameof::customize` to provide custom names. An empty name selects the default. See the [enum and type example](../example/example_custom_name.cpp).

* `type_name` uses the type without cv/ref qualifiers, except `NAMEOF_FULL_TYPE`, which uses the exact type. `NAMEOF_SHORT_TYPE` keeps the custom name's last identifier. RTTI APIs ignore custom names.

* Specialize `member_name` and `pointer_name` for the exact pointer value, including typed null pointers.

  ```cpp
  struct Item { int value; };
  inline int global_value = 0;

  template <>
  constexpr nameof::string_view nameof::customize::member_name<&Item::value>() noexcept {
    return "Value";
  }

  template <>
  constexpr nameof::string_view nameof::customize::pointer_name<&global_value>() noexcept {
    return "Global";
  }
  ```

* Enum APIs use [C++26 standard reflection](limitations.md#c26-standard-reflection) when available. Define `NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION` before including `nameof.hpp` to keep compiler-specific reflection, or `NAMEOF_REQUIRE_STD_REFLECTION` to fail compilation without standard reflection.

* To change the string types, use these macros:

  ```cpp
  #include <my_lib/string.hpp>
  #include <my_lib/string_view.hpp>
  #define NAMEOF_USING_ALIAS_STRING using string = my_lib::String;
  #define NAMEOF_USING_ALIAS_STRING_VIEW using string_view = my_lib::StringView;
  #include <nameof.hpp>
  ```

  Aliases must support the `std::string` and `std::string_view` operations used by nameof. See the [minimal interface](../test/test_aliases.cpp).

* `cstring` and `name_view` support `std::format` (C++20 and newer) and `fmt`. With `NAMEOF_NO_STD_FORMAT`, `<format>` is not included: pass a `string_view` to `std::format`. With `fmt` 9, include `<fmt/format.h>` before `<nameof.hpp>` to format `cstring`.

* `NAMEOF_ASSERT` checks preconditions, such as the index of `operator[]`, and defaults to `assert`. Define your own `NAMEOF_ASSERT(...)` (an expression valid in a `constexpr` function) or disable checks with `NAMEOF_NO_ASSERT`.

* Use `NAMEOF_CONFIG_FILE` to keep options in one header. It is included before any option is read:

  ```cpp
  #define NAMEOF_CONFIG_FILE "my_nameof_config.hpp"
  #include <nameof.hpp>
  ```

  Use the same options in every translation unit to avoid ODR violations.

* `NAMEOF_VERSION_MAJOR`, `NAMEOF_VERSION_MINOR` and `NAMEOF_VERSION_PATCH` hold the library version.

* `NAMEOF`, `NAMEOF_FULL`, `NAMEOF_RAW`, and `*_TYPE_EXPR` do not evaluate their arguments. RTTI APIs follow `typeid` rules and evaluate polymorphic glvalues to obtain the dynamic type.

## Compiler compatibility

nameof requires C++17 or newer and supports:

* Clang/LLVM >= 9
* GCC >= 9
* Visual Studio >= 2019

`NAMEOF_TYPE_RTTI`, `NAMEOF_FULL_TYPE_RTTI`, and `NAMEOF_SHORT_TYPE_RTTI` require RTTI to be enabled, so they are unavailable with `-fno-rtti` or `/GR-`. `NAMEOF_MEMBER` and `NAMEOF_POINTER` require C++20 on Visual Studio before 2022 17.14.

## `NAMEOF`

* Obtains name of variable, function, macro.

* Returns `nameof::cstring`. Marked `constexpr` and `noexcept`.

* Fails to compile for some expressions without a name: `"NAMEOF requires a named expression."` See the [limitations](limitations.md#nameof).

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

* Obtains name of variable, function, or macro, including template arguments.

* Returns `nameof::cstring`. Marked `constexpr` and `noexcept`.

* Fails to compile for some expressions without a name: `"NAMEOF_FULL requires a named expression."` See the [limitations](limitations.md#nameof).

* Examples

  ```cpp
  // Full name of template function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Full name of template member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

## `NAMEOF_RAW`

* Obtains expression text by preprocessor stringization. Whitespace outside literals is normalized.

* Returns `nameof::cstring`. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  NAMEOF_RAW(::somevar.somefield) -> "::somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```

## `NAMEOF_ENUM`

* Obtains name of enum value.

* Returns `string_view`. Marked `constexpr` and `noexcept`.

* Returns a custom or reflected name, otherwise an empty `string_view`.

* Automatic lookup with compiler-specific reflection is limited to the [enum range](limitations.md#nameof-enum). A non-empty range requires an enumerator or a custom name, otherwise compilation fails. Use `NAMEOF_ENUM_OR` for fallback lookup.

* Examples

  ```cpp
  auto color = Color::RED;
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"
  ```

## `NAMEOF_ENUM_OR`

* Obtains name of enum value or a fallback.

* Returns a `string` with the custom or reflected enum name, or `default_value`.

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

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* Has a lower compile-time cost than `NAMEOF_ENUM` and ignores [`enum_range`](limitations.md#nameof-enum).

* Returns a custom or reflected name, otherwise an empty `nameof::name_view`.

* Examples

  ```cpp
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"
  ```

## `NAMEOF_ENUM_FLAG`

* Obtains name of enum flag value.

* Returns `string`.

* Joins custom or reflected names of individual set bits with `sep` (`'|'` by default). Composite enumerator names are ignored.

* If value is zero or contains unnamed bit, returns empty `string`.

* Compiler-specific reflection requires a single-bit enumerator or custom flag name, otherwise compilation fails.

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

Exact type spellings in the examples are compiler-specific. See the [type limitations](limitations.md#nameof-type).

* Obtains type name. Reference and cv-qualifiers are ignored (`NAMEOF_TYPE(const T&) == NAMEOF_TYPE(T)`).

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_TYPE(T) -> "int"
  nameof::nameof_type<T>() -> "int"
  ```

## `NAMEOF_FULL_TYPE`

* Obtains full type name with reference and cv-qualifiers.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  using T = const int&;
  NAMEOF_FULL_TYPE(T) -> "const int&"
  nameof::nameof_full_type<T>() -> "const int&"
  ```

## `NAMEOF_SHORT_TYPE`

* Obtains short type name.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* If type has no short name, compilation fails: `"nameof::nameof_short_type requires a type with a short name."`

* Examples

  ```cpp
  using T = const my::detail::SomeClass<int>&;
  NAMEOF_SHORT_TYPE(T) -> "SomeClass"
  nameof::nameof_short_type<T>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_EXPR`

* Obtains type name of expression. Reference and cv-qualifiers are ignored.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_TYPE_EXPR(var) -> "int"
  nameof::nameof_type<decltype(var)>() -> "int"
  ```

## `NAMEOF_FULL_TYPE_EXPR`

* Obtains the name of `decltype(expr)`, preserving reference and cv-qualifiers.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* Examples

  ```cpp
  using T = const int&;
  T var = 42;
  NAMEOF_FULL_TYPE_EXPR(var) -> "const int&"
  nameof::nameof_full_type<decltype(var)>() -> "const int&"

  int x = 0;
  NAMEOF_FULL_TYPE_EXPR(x) -> "int"
  NAMEOF_FULL_TYPE_EXPR((x)) -> "int&"
  ```

## `NAMEOF_SHORT_TYPE_EXPR`

* Obtains short type name of expression.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* If type has no short name, compilation fails: `"nameof::nameof_short_type requires a type with a short name."`

* Examples

  ```cpp
  const my::detail::SomeClass<int> var;
  NAMEOF_SHORT_TYPE_EXPR(var) -> "SomeClass"
  nameof::nameof_short_type<decltype(var)>() -> "SomeClass"
  ```

## `NAMEOF_TYPE_RTTI`

* Obtains type name using RTTI. Reference and cv-qualifiers are ignored.

* Takes an expression, not a type.

* Returns `string`.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base* ptr = &derived;
  NAMEOF_TYPE_RTTI(*ptr) -> "my::detail::Derived"
  ```

## `NAMEOF_FULL_TYPE_RTTI`

* Obtains type name using RTTI and preserves reference and cv-qualifiers.

* Takes an expression, not a type.

* Pointer, array, function, and member pointer types use compile-time names.

* Returns `string`.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base& cv_ref = derived;
  NAMEOF_FULL_TYPE_RTTI(cv_ref) -> "const volatile my::detail::Derived&"
  ```

## `NAMEOF_SHORT_TYPE_RTTI`

* Obtains short type name using RTTI. Namespace qualification, template arguments, references, and cv-qualifiers are omitted.

* Takes an expression, not a type.

* Returns `string`.

* Examples

  ```cpp
  my::detail::Derived derived;
  const volatile my::detail::Base* ptr = &derived;
  NAMEOF_SHORT_TYPE_RTTI(*ptr) -> "Derived"
  ```

## `NAMEOF_MEMBER`

* Obtains name of member.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

* If member pointer is null, returns `"nullptr"` by default.

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

* Obtains name of a function, global variable, or static data member.

* Returns `nameof::name_view`. Marked `constexpr` and `noexcept`.

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
