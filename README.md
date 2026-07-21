<picture>
	<img alt="JsonReflect" src="https://github.com/user-attachments/assets/56c14418-c899-42d8-98be-3f72f7a72349" />
</picture>

---

# JsonReflect

**Header-only, reflection-based JSON serializer for C++17.**

Automatically serializes and deserializes any C++ object to and from JSON with a single macro — no boilerplate `to_json`/`from_json` per field.

```cpp
#include "JsonReflect.hpp"

enum class Difficulty { Easy, Medium, Hard };

struct GameSettings {
	int         volume      = 50;
	float       sensitivity = 1.0f;
	bool        fullscreen  = true;
	Difficulty  difficulty  = Difficulty::Medium;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);

GameSettings input{};
JsonReflect::json j = JsonReflect::to_json(input);
```

Output:

```json
{
    "volume": 50,
    "sensitivity": 1.0,
    "fullscreen": true,
    "difficulty": "Medium"
}
```

## Quick Start

**1. Install** — grab the [single header](single_header/JsonReflect.hpp), then:
```cpp
#include "JsonReflect.hpp"
```

**2. Reflect your struct/class**
```cpp
struct GameSettings {
	int         volume      = 50;
	float       sensitivity = 1.0f;
	bool        fullscreen  = true;
	Difficulty  difficulty  = Difficulty::Medium;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);
```

**3. Serialize**
```cpp
GameSettings input;
input.volume      = 75;
input.sensitivity = 1.5f;
input.fullscreen  = false;
input.difficulty  = Difficulty::Hard;

JsonReflect::json serialized = JsonReflect::to_json(input);
std::ofstream("settings.json") << serialized.dump();
```

**4. Deserialize**
```cpp
JsonReflect::json parsed = JsonReflect::json::parse(std::ifstream("settings.json"));

GameSettings output{};
JsonReflect::from_json(parsed, output);

assert(output.volume      == 75);
assert(output.sensitivity == 1.5f);
assert(output.fullscreen  == false);
assert(output.difficulty  == Difficulty::Hard);
```

## Installation

JsonReflect requires **C++17 or later**.

### Option A: Single Header (recommended)

Download [`single_header/JsonReflect.hpp`](single_header/JsonReflect.hpp). It bundles the library and all of its dependencies into one file.

```cpp
#include "JsonReflect.hpp"
```

### Option B: Separate Headers

Download every header from the repository root plus the `extern/` folder, keeping them side by side:

```
JsonReflect.hpp            // umbrella header — include this
JsonReflect_defines.hpp    // configuration + dependency include paths
JsonReflect_macro.hpp      // JSON_REFLECT macros + tags (lightweight, no heavy deps)
JsonReflect_helpers.hpp
JsonReflect_entry.hpp      // to_json / from_json / get_changes
JsonReflect_primitives.hpp // enums + nlohmann fallbacks
JsonReflect_stl.hpp        // containers, smart pointers, variant, ...
extern/                    // nlohmann/json, visit_struct, magic_enum, svh/tag_invoke
```

```cpp
#include "JsonReflect.hpp"
```

> [!TIP]
> **Annotate types without the full include.** `JSON_REFLECT(...)` and `BEFRIEND_JSON_REFLECT()` live in the lightweight `JsonReflect_macro.hpp`, which does **not** pull in nlohmann/json. Include just that header where you declare your types, and include the full `JsonReflect.hpp` only in the translation units that actually serialize:
> ```cpp
> // my_struct.hpp — cheap to include everywhere
> #include "JsonReflect_macro.hpp"
>
> namespace MyProject {
>     struct MyStruct { int a = 0; float b = 0.0f; double c = 0.0; };
> }
> JSON_REFLECT(MyProject::MyStruct, a, b, c);
> ```

## Supported Types

Out of the box, JsonReflect handles:

| Category            | Types                                                                                          |
| ------------------- | ---------------------------------------------------------------------------------------------- |
| Booleans & numbers  | `bool`, all `char` types, all integer types, `float`, `double`, `long double`                  |
| Strings             | `std::string` (and other string-like types)                                                    |
| Enums               | any `enum` / `enum class` — serialized **by name** via magic_enum                              |
| Sequence containers | `std::vector`, `std::array`, `std::list`, `std::forward_list`, `std::deque`                    |
| Sets                | `std::set`, `std::multiset`, `std::unordered_set`, `std::unordered_multiset`                   |
| Maps                | `std::map`, `std::multimap`, `std::unordered_map`, `std::unordered_multimap` (as JSON objects) |
| Tuple-likes         | `std::pair`, `std::tuple`, `std::optional`                                                     |
| Sum types           | `std::variant` (stored as `{ "index": N, "value": ... }`)                                      |
| Pointers            | raw pointers, `std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr` (serialize only)           |
| Passthrough         | `nlohmann::json` values are stored/loaded as-is                                                |
| Your types          | any struct/class marked with `JSON_REFLECT`, including nested and containers of them           |

Enums serialize to a readable string and error on an unknown name:

```json
{ "difficulty": "Hard" }
```

## Reflecting Your Types

### The `JSON_REFLECT` macro

`JSON_REFLECT(Type, field1, field2, ...)` registers a type for serialization, deserialization, **and** diffing in one line. Place it at namespace scope (outside any namespace), naming the type fully qualified:

```cpp
namespace ns {
    struct GameSettings { int volume = 50; float sensitivity = 1.0f; };
}
JSON_REFLECT(ns::GameSettings, volume, sensitivity);
```

### Private members

Add `BEFRIEND_JSON_REFLECT()` inside a class to let JsonReflect read and write private fields:

```cpp
struct PrivateSettings {
private:
    int   volume     = 50;
    float sensitivity = 1.0f;
    BEFRIEND_JSON_REFLECT()
};
JSON_REFLECT(PrivateSettings, volume, sensitivity);
```

### Partial reflection

`JSON_REFLECT` is shorthand for three independent registrations. Use them separately when you want serialize, deserialize, and compare to cover **different** fields:

```cpp
struct Foo { int a = 50; float b = 1.0f; bool c = true; Difficulty d = Difficulty::Medium; };

JSON_REFLECT_SERIALIZE(Foo, a, b, c); // write a, b, c
JSON_REFLECT_DESERIALIZE(Foo, a, b);  // read only a, b
JSON_REFLECT_COMPARE(Foo, a);         // diff only a
```

Fields left out of the deserialize list keep their default/constructed values when loading.

### Templated types

Reflect a class template with `JSON_REFLECT_TEMPLATE`. Wrap the template parameters and arguments in parentheses:

```cpp
template <typename T>
struct Boxed { 
    T value{};
    int tag = 0;
};

JSON_REFLECT_TEMPLATE((typename T), Boxed, (T), value, tag);
```

`JSON_REFLECT_SERIALIZE_TEMPLATE`, `JSON_REFLECT_DESERIALIZE_TEMPLATE`, and `JSON_REFLECT_COMPARE_TEMPLATE` are available for partial template reflection.

## Custom Serialization

When reflection isn't enough, implement a `tag_invoke` overload. JsonReflect finds it automatically (see [Resolution Order](#resolution-order)).

### Full custom behavior

```cpp
struct CustomObject {
    int id = 0;
    std::string name;
    float value = 0.0f;
    void init();
};

inline JsonReflect::json tag_invoke(JsonReflect::serialize_t, const CustomObject& o) {
    JsonReflect::json j;
    j["id"]    = JsonReflect::to_json(o.id);
    j["name"]  = JsonReflect::to_json(o.name);
    j["value"] = JsonReflect::to_json(o.value);
    return j;
}

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, CustomObject& o) {
    if (j.contains("id"))    JsonReflect::from_json(j["id"],    o.id);
    if (j.contains("name"))  JsonReflect::from_json(j["name"],  o.name);
    if (j.contains("value")) JsonReflect::from_json(j["value"], o.value);
    o.init(); // run custom logic after loading
}
```

### Reflection + a custom step

Keep the generated reflection but hook in extra logic — for example, calling `init()` after deserialization:

```cpp
JSON_REFLECT(CustomObject, id, name, value);

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, CustomObject& o) {
    JsonReflect::Detail::from_json_visitable(j, o); // default reflected deserialization
    o.init();
}
```

### Passing context arguments

Both `to_json` and `from_json` forward any extra arguments through to your `tag_invoke` overloads and into the elements of containers. This lets you thread context (an allocator, a version number, a flag) through a whole object graph:

```cpp
JsonReflect::json tag_invoke(JsonReflect::serialize_t, const MyStruct& v, bool verbose) { /* ... */ }

auto j = JsonReflect::to_json(my_struct, /*verbose=*/true);

// forwarded to every element as well:
std::vector<MyStruct> items = /* ... */;
auto arr = JsonReflect::to_json(items, true);
```

## Resolution Order

When (de)serializing a type, JsonReflect picks an implementation at compile time in this order:

1. **Your `tag_invoke`** — `serialize_t` / `deserialize_t` / `compare_t` overloads you defined.
2. **Reflection** — types registered with `JSON_REFLECT`.
3. **Library `tag_invoke`** — JsonReflect's built-in support (enums, containers, smart pointers, variant, ...).
4. **nlohmann::json defaults** — anything nlohmann already knows how to handle.

Because your overloads come first, you can always override library or reflection behavior. If nothing matches, compilation fails with:

```
JsonReflect Error: No suitable serialize implementation found for type T
```

## Pointers & Smart Pointers

**Raw pointers** are dereferenced automatically. A null pointer serializes to `null`. When deserializing, the pointer must already point at valid storage:

```cpp
int value = 0;
int* p = &value;
JsonReflect::from_json(j, p); // writes into *p — allocate before calling
```

**`std::unique_ptr` and `std::shared_ptr`** serialize to their pointee (or `null`). On deserialize, a `null` JSON resets the pointer; otherwise a null smart pointer is auto-initialized (see [`JSON_REFLECT_INITIALIZE_SMART_POINTERS`](#configuration)) before loading into it.

**`std::weak_ptr`** serializes the locked value (or `null`). Deserialization of `weak_ptr` is intentionally **not** supported and will fail to compile.

For non-default-constructible pointee types, specialize the factory used to auto-initialize `shared_ptr`:

```cpp
template<>
struct JsonReflect::Detail::smart_pointer_factory<CustomObject> {
    static std::shared_ptr<CustomObject> create() {
        return std::make_shared<CustomObject>(/* your args */);
    }
};
```

## Delta Serialization

Delta serialization writes **only the fields that differ from a baseline**, producing smaller JSON. It is opt-in per type:

```cpp
struct MyStruct { int a = 42; float b = 3.14f; bool c = true; std::string d = "Hello"; };
JSON_REFLECT(MyStruct, a, b, c, d);

// enable delta serialization for this type
template<>
struct JsonReflect::Detail::delta_serialize<MyStruct> : std::true_type {};

MyStruct input{};
input.a = 100;
input.b = 2.71f; // c and d unchanged
JsonReflect::json j = JsonReflect::to_json(input);
```

**Result** (unchanged `c` and `d` are omitted):

```json
{
    "a": 100,
    "b": 2.71
}
```

By default the baseline is a default-constructed instance, so delta serialization requires a **default-constructible** type. For types that aren't, specialize `delta_default` to supply the baseline:

```cpp
struct NonDefaultConstructible {
    int a; float b; bool c;
    NonDefaultConstructible(int a, float b, bool c) : a(a), b(b), c(c) {}
};
JSON_REFLECT(NonDefaultConstructible, a, b, c);

template<>
struct JsonReflect::Detail::delta_serialize<NonDefaultConstructible> : std::true_type {};

template<>
struct JsonReflect::Detail::delta_default<NonDefaultConstructible> {
    static NonDefaultConstructible make() { 
        return NonDefaultConstructible{ 0, 0.0f, true }; 
    }
};
```

> [!IMPORTANT]
> Change detection uses each field's `==` / `!=` operator. If a field type has neither, JsonReflect falls back to serializing both values and comparing the resulting JSON — correct, but more expensive. That fallback can be disabled with [`JSON_REFLECT_ALLOW_JSON_COMPARE`](#configuration).

## Diffing with `get_changes`

`get_changes(lhs, rhs)` compares two objects of the same type and returns a JSON object containing **only the fields whose value in `rhs` differs from `lhs`** (the `rhs` value is what gets stored). It recurses into nested reflected types, emitting just the changed leaves.

```cpp
GameSettings a{};            // defaults
GameSettings b{};
b.volume     = 75;
b.fullscreen = false;

JsonReflect::json diff = JsonReflect::get_changes(a, b);
```

**Result:**

```json
{
    "volume": 75,
    "fullscreen": false
}
```

It works for types registered with `JSON_REFLECT` (which registers the compare context) and for types with an `==` operator, and recurses through nested reflected members. Register a custom `tag_invoke(JsonReflect::compare_t, const T&, const T&)` to override the diff for a specific type.

> [!NOTE]
> **`get_changes` vs. delta serialization.** Delta serialization compares an object against a fixed *baseline* (its default, or your `delta_default`) during `to_json`. `get_changes` compares two *explicit* objects you provide. Use delta serialization for "save only what changed from defaults"; use `get_changes` for "what changed between these two snapshots".

## Configuration

Define any of these **before** including JsonReflect to change its behavior. All have safe defaults.

| Macro                                    | Default | Effect                                                                                                                               |
| ---------------------------------------- | ------- | ------------------------------------------------------------------------------------------------------------------------------------ |
| `JSON_REFLECT_STATIC_FOR_DELTA`          | `1`     | Reuse a `static` baseline instance during delta serialization (fewer allocations, but the baseline stays alive).                     |
| `JSON_REFLECT_ALLOW_JSON_COMPARE`        | `1`     | Allow the JSON-compare fallback for delta detection on fields with no `==`/`!=`. If `0`, such fields cause a compile error.          |
| `JSON_REFLECT_INITIALIZE_SMART_POINTERS` | `1`     | Auto-initialize a null `shared_ptr`/`unique_ptr` before deserializing into it. If `0`, a null smart pointer errors instead.          |
| `JSON_REFLECT_ALLOW_THROW`               | `1`     | Throw on runtime errors (null pointers, invalid enum names, ...). If `0`, JsonReflect logs to `stderr` and continues where possible. |
| `JSON_USE_IMPLICIT_CONVERSIONS`          | `0`     | Passed through to nlohmann/json. Disabled by default to avoid accidental implicit conversions to JSON.                               |

Example:

```cpp
#define JSON_REFLECT_ALLOW_THROW 0
#include "JsonReflect.hpp"
```

## Custom Dependency Include Paths

By default JsonReflect includes its dependencies from their conventional paths (`<nlohmann/json.hpp>`, etc.). If your project vendors these under different names or folders, redirect them by defining the matching macro **before** including JsonReflect:

| Macro                             | Default                           |
| --------------------------------- | --------------------------------- |
| `JSON_REFLECT_NLOHMANN_JSON_HPP`  | `<nlohmann/json.hpp>`             |
| `JSON_REFLECT_VISIT_STRUCT_HPP`   | `<visit_struct/visit_struct.hpp>` |
| `JSON_REFLECT_SVH_TAG_INVOKE_HPP` | `<svh/tag_invoke.hpp>`            |
| `JSON_REFLECT_MAGIC_ENUM_HPP`     | `<magic_enum/magic_enum.hpp>`     |

```cpp
#define JSON_REFLECT_NLOHMANN_JSON_HPP <my/vendored/json.hpp>
#define JSON_REFLECT_MAGIC_ENUM_HPP   "third_party/magic_enum.hpp"
#include "JsonReflect.hpp"
```

> [!NOTE]
> This applies to the separate-header build. The single-header build already has every dependency inlined, so these macros aren't needed there.

## Building the Single Header

The single header is generated from the individual headers by a script. Regenerate it after changing any library header:

```bash
python scripts/generate_single_header.py
# or write somewhere else:
python scripts/generate_single_header.py path/to/JsonReflect.hpp
```

The script inlines the `extern/` dependencies and all `JsonReflect_*` headers in dependency order into `single_header/JsonReflect.hpp`.

## Dependencies

All dependencies are bundled in the single header and mirrored in `extern/`:

- [nlohmann::json](https://github.com/nlohmann/json) — JSON parsing and serialization
- [visit_struct](https://github.com/cbeck88/visit_struct) — compile-time struct reflection
- [magic_enum](https://github.com/Neargye/magic_enum) — enum reflection
- svh/tag_invoke — the `tag_invoke` dispatch mechanism
- C++17 or later

## Contributing

Feedback, issues, and pull requests are welcome! This project is part of my university work, so everything is a learning experience.

## License

**MIT License** — see [LICENSE](https://github.com/Sven-vh/JsonReflect/blob/main/LICENSE).

As a student, credit or a quick note about what you're using it for is greatly appreciated. It motivates me to keep contributing to open source!
