<img width="1251" height="526" alt="Json Reflect" src="https://github.com/user-attachments/assets/206f7879-9bcd-47ae-b284-6abe427742e1" />

---

# JsonReflect

**Reflection-based JSON serializer.**

Automatically serializes and deserializes any C++ object to and from JSON.

```cpp
struct GameSettings {
	int			volume = 50;
	float		sensitivity = 1.0f;
	bool		fullscreen = true;
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen);

/* Serialize to JSON */
GameSettings input{};
json j = JsonReflect::to_json(input);

/* Deserialize from JSON */
GameSettings output{};
JsonReflect::from_json(j, output);
```

Output:

```json
{
    "volume": 75,
    "sensitivity": 1.0,
    "fullscreen": false
}
```

> [!WARNING]
>
> **(WIP)** This library is part of my self-study project for university and is still a work in progress.
