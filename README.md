<img width="1251" height="526" alt="JsonReflect" src="https://github.com/user-attachments/assets/56c14418-c899-42d8-98be-3f72f7a72349" />


---

# JsonReflect

**Header-only reflection-based JSON serializer.**

Automatically serializes and deserializes any C++ object to and from JSON.

```cpp
struct GameSettings {
	int			volume = 50;
	float		sensitivity = 1.0f;
	bool		fullscreen = true;
	Difficulty	difficulty = Difficulty::Medium;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);


GameSettings input{};
json j = JsonReflect::to_json(input);
```

Output:

```json
{
    "volume": 75,
    "sensitivity": 1.0,
    "fullscreen": false,
	"difficulty": "Medium"
}
```

## Quick Start

**1. Installation**

**Option A: Single Header (Recommended)**

Download [`single_header/JsonReflect.hpp`](single_header/JsonReflect.hpp) - includes everything in one file.

```cpp
#include "JsonReflect.hpp"
```

**Option B: Separate Headers**

Download all headers from the root directory + the `extern/` folder, then:

```cpp
#include "JsonReflect.hpp"
```

**2. Reflect Your Struct/Class**
```cpp
struct GameSettings {
	int			volume = 50;
	float		sensitivity = 1.0f;
	bool		fullscreen = true;
	Difficulty	difficulty = Difficulty::Medium;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);
```

**3. Serialize**
```cpp
GameSettings input;
input.volume = 75;
input.sensitivity = 1.5f;
input.fullscreen = false;
input.difficulty = Difficulty::Hard;

JsonReflect::json serialized = JsonReflect::to_json(input);
std::ofstream("settings.json") << serialized.dump();
```

**4. Deserialize**
```cpp
JsonReflect::json parsed_json = JsonReflect::json::parse(std::ifstream("settings.json"));

GameSettings output{};
JsonReflect::from_json(parsed_json, output);
assert(output.volume == 75);
assert(output.sensitivity == 1.5f);
assert(output.fullscreen == false);
assert(output.difficulty = Difficulty::Hard);
```

## Custom Types

There are 2 ways to serialize an object.

### Reflection

```cpp
struct GameSettings {
	int			volume = 50;
	float		sensitivity = 1.0f;
	bool		fullscreen = true;
	Difficulty	difficulty = Difficulty::Medium;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);
```

> [!TIP]
> Use the ``BEFRIEND_JSON_REFLECT()`` macro to let JsonReflect access private variables.

### Tag Invoke

If the macro is not sufficient, you can implement a tag invoke function that will be called when JsonReflect encounters that object.
```cpp
struct CustomObject {
	int id = 0;
	std::string name;
	float value = 0.0f;

	void init();
};

inline JsonReflect::json tag_invoke(JsonReflect::serialize_t, const CustomObject& object) {
	JsonReflect::json j;
	j["id"] = JsonReflect::to_json(object.id);
	j["name"] = JsonReflect::to_json(object.name);
	j["value"] = JsonReflect::to_json(object.value);
	return j;
}

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, CustomObject& object) {
	if (j.contains("id")) JsonReflect::from_json(j["id"], object.id);
	if (j.contains("name")) JsonReflect::from_json(j["name"], object.name);
	if (j.contains("value")) JsonReflect::from_json(j["value"], object.value);

	// Call init after deserialization
	object.init();
}
```

Or, still use the reflection, but also do something custom:
```cpp
struct CustomObject {
	int id = 0;
	std::string name;
	float value = 0.0f;

	void init() {};
};
JSON_REFLECT(CustomObject, id, name, value);

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, CustomObject& object) {
	// Use default visitable deserialization
	JsonReflect::Detail::from_json_visitable(j, object);

	// Call init after deserialization
	object.init();
}
```

## Resolution Order

When serializing a type, JsonReflect searches in this order at compile time:

1. **User implementations** - Your tag_invoke functions
2. **Reflection** - Types with ``JSON_REFLECT`` macro
3. **Library implementations** - Built-in JsonReflect types
4. **[nlohmann::json](https://github.com/nlohmann/json)** - Types supported by nlohmann

This means you can always override library and reflection behavior with your own ``tag_invoke`` implementations.

If no implementation is found:
```
"JsonSerializer Error: No suitable serialize implementation found for type T"
```

## Dependencies

All dependencies are included in the single header file. If using separate headers (or check ``extern/`` folder):

- [nlohmann::json](https://github.com/nlohmann/json) - JSON parsing and serialization
- [visit_struct](https://github.com/cbeck88/visit_struct) - Compile-time struct reflection
- [magic_enum](https://github.com/Neargye/magic_enum) - Enum reflection
- C++17 or later

## Contributing

Feedback, issues, and pull requests are welcome! This project is part of my university work, so everything is a learning experience.

## License

**MIT License** - See [LICENSE](https://github.com/Sven-vh/JsonReflect/blob/main/LICENSE)

As a student, credit or a quick note about what you're using it for is greatly appreciated! It motivates me to keep contributing to open source!
