#include "pch.h"
//#include <nlohmann/json.hpp>

template<typename T>
static void serialize_test(const T input) {
	auto result = JsonReflect::to_json(input);

	const std::string json_str = result.dump(2);

	T output{};
	JsonReflect::from_json(result, output);

	//if constexpr (JsonReflect::Detail::has_equality_operator_v<T>) {
	//	EXPECT_EQ(input, output);
	//} else {
	auto ohmannn_diff = nlohmann::json::diff(result, JsonReflect::to_json(output));
	EXPECT_TRUE(ohmannn_diff.empty());
	//}
}

TEST(JsonReflect, numerics) {
	serialize_test<bool>(true);
	serialize_test<char>('a');
	serialize_test<signed char>(-1);
	serialize_test<unsigned char>(255);
	serialize_test<short>(-1);
	serialize_test<unsigned short>(65535);
	serialize_test<int>(-1);
	serialize_test<unsigned int>(4294967295);
	serialize_test<long>(-1);
	serialize_test<unsigned long>(4294967295);
	serialize_test<long long>(-1);
	serialize_test<unsigned long long>(4294967295);
	serialize_test<float>(-1.0f);
	serialize_test<double>(-1.0);
	serialize_test<long double>(-1.0);
}

TEST(JsonReflect, string) {
	serialize_test<std::string>("Hello, World!");
}

/* Pointers */
TEST(JsonReflect, raw_pointer) {
	int value = 42;
	auto result = JsonReflect::to_json(&value);

	const std::string json_str = result.dump(2);

	int* output = new int;
	JsonReflect::from_json(result, output);

	auto ohmannn_diff = nlohmann::json::diff(result, JsonReflect::to_json(output));
	EXPECT_TRUE(ohmannn_diff.empty());
	delete output;
}

/* Pointer reference */
TEST(JsonReflect, pointer_reference) {
	int value = 42;
	int* ptr = &value;
	auto result = JsonReflect::to_json(ptr);
	const std::string json_str = result.dump(2);
	int* output = new int;
	JsonReflect::from_json(result, output);
	auto ohmannn_diff = nlohmann::json::diff(result, JsonReflect::to_json(output));
	EXPECT_TRUE(ohmannn_diff.empty());
	delete output;
}

/* STL Containers */
TEST(JsonReflect, vector) {
	serialize_test<std::vector<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::vector<std::string>>({ "one", "two", "three" });
	serialize_test<std::vector<bool>>({ true, false, true });
}

TEST(JsonReflect, array) {
	serialize_test<std::array<int, 5>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::array<std::string, 3>>({ "one", "two", "three" });
	serialize_test<std::array<bool, 3>>({ true, false, true });
}

TEST(JsonReflect, list) {
	serialize_test<std::list<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::list<std::string>>({ "one", "two", "three" });
	serialize_test<std::list<bool>>({ true, false, true });
}

TEST(JsonReflect, forward_list) {
	serialize_test<std::forward_list<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::forward_list<std::string>>({ "one", "two", "three" });
	serialize_test<std::forward_list<bool>>({ true, false, true });
}

#include <deque>

TEST(JsonReflect, deque) {
	serialize_test<std::deque<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::deque<std::string>>({ "one", "two", "three" });
	serialize_test<std::deque<bool>>({ true, false, true });
}

#include <set>

TEST(JsonReflect, set) {
	serialize_test<std::set<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::set<std::string>>({ "one", "two", "three" });
	serialize_test<std::set<bool>>({ true, false, true });
}

#include <unordered_set>

TEST(JsonReflect, unordered_set) {
	serialize_test<std::unordered_set<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::unordered_set<std::string>>({ "one", "two", "three" });
	serialize_test<std::unordered_set<bool>>({ true, false, true });
}

TEST(JsonReflect, multi_set) {
	serialize_test<std::multiset<int>>({ 1, 2, 2, 3, 4, 5 });
	serialize_test<std::multiset<std::string>>({ "one", "two", "two", "three" });
	serialize_test<std::multiset<bool>>({ true, false, true, true });
}

TEST(JsonReflect, unordered_multiset) {
	serialize_test<std::unordered_multiset<int>>({ 1, 2, 2, 3, 4, 5 });
	serialize_test<std::unordered_multiset<std::string>>({ "one", "two", "two", "three" });
	serialize_test<std::unordered_multiset<bool>>({ true, false, true, true });
}

/* Maps */
TEST(JsonReflect, map) {
	serialize_test<std::map<std::string, int>>({ {"one", 1}, {"two", 2}, {"three", 3} });
}

TEST(JsonReflect, unordered_map) {
	serialize_test<std::unordered_map<std::string, int>>({ {"one", 1}, {"two", 2}, {"three", 3} });
}

TEST(JsonReflect, multi_map) {
	serialize_test<std::multimap<std::string, int>>({ {"one", 1}, {"two", 2}, {"two", 22}, {"three", 3} });
}

TEST(JsonReflect, unordered_multi_map) {
	serialize_test<std::unordered_multimap<std::string, int>>({ {"one", 1}, {"two", 2}, {"two", 22}, {"three", 3} });
}

/* Other STL Types */
TEST(JsonReflect, pair) {
	serialize_test<std::pair<int, std::string>>({ 1, "one" });
}

TEST(JsonReflect, tuple) {
	serialize_test<std::tuple<int, std::string, bool>>({ 1, "one", true });
}

TEST(JsonReflect, optional) {
	serialize_test<std::optional<int>>(42);
	serialize_test<std::optional<int>>(std::nullopt);
}

/* smart pointers */
TEST(JsonReflect, unique_ptr) {
	serialize_test<std::unique_ptr<int>>(std::make_unique<int>(42));
}

TEST(JsonReflect, shared_ptr) {
	serialize_test<std::shared_ptr<int>>(std::make_shared<int>(42));
}

TEST(JsonReflect, weak_ptr) {
	std::shared_ptr<int> sp = std::make_shared<int>(42);
	std::weak_ptr<int> wp = sp;

	/* Serialization, works! */
	auto result = JsonReflect::to_json(wp);

	const std::string json_str = result.dump(2);

	/* Deserialization, not supported */
	//std::weak_ptr<int> output;
	//JsonReflect::from_json(result, output);
}

#include <variant>

/* variants */
TEST(JsonReflect, variant) {
	std::variant<int, std::string, bool> value = 42;
	serialize_test(value);

	value = std::string("Hello, Variant!");
	serialize_test(value);

	value = true;
	serialize_test(value);
}

/* Custom Types */
enum class Difficulty {
	Easy,
	Medium,
	Hard
};

TEST(JsonReflect, enums) {
	serialize_test<Difficulty>(Difficulty::Easy);
	serialize_test<Difficulty>(Difficulty::Medium);
	serialize_test<Difficulty>(Difficulty::Hard);
}

namespace ns {
	struct GameSettings {
		int			volume = 50;
		float		sensitivity = 1.0f;
		bool		fullscreen = true;
		Difficulty	difficulty = Difficulty::Medium;
	};
}
JSON_REFLECT(ns::GameSettings, volume, sensitivity, fullscreen, difficulty);

TEST(JsonReflect, custom_struct) {
	ns::GameSettings settings;
	serialize_test(settings);
}

namespace ns {
	struct NestedSettings {
		GameSettings	game_settings;
		std::string		player_name = "Player1";
	};
}
JSON_REFLECT(ns::NestedSettings, game_settings, player_name);

TEST(JsonReflect, nested_struct) {
	ns::NestedSettings settings;
	serialize_test(settings);
}

namespace ns {
	struct Foo {
		int			a = 50;
		float		b = 1.0f;
		bool		c = true;
		Difficulty	d = Difficulty::Medium;
	};
}
JSON_REFLECT_SERIALIZE(ns::Foo, a, b, c); /* only serialize a, b, and c */
JSON_REFLECT_DESERIALIZE(ns::Foo, a, b); /* only deserialize a and b */
JSON_REFLECT_COMPARE(ns::Foo, a); /* only compare a */

TEST(JsonReflect, custom_struct_partial) {
	ns::Foo foo;
	foo.a = 100;
	foo.b = 2.0f;
	foo.c = false;
	foo.d = Difficulty::Hard;

	//serialize_test(foo);
	/* custom test */
	{
		auto json = JsonReflect::to_json(foo);
		ns::Foo foo2{};
		JsonReflect::from_json(json, foo2);

		EXPECT_EQ(foo.a, foo2.a);
		EXPECT_EQ(foo.b, foo2.b);
		EXPECT_EQ(foo2.c, true); /* c should be default value */
		EXPECT_EQ(foo2.d, Difficulty::Medium); /* d should be default value */
	}
}

TEST(JsonReflect, custom_struct_vector) {
	std::vector<ns::GameSettings> settings_vec = {
		{50, 1.0f, true, Difficulty::Medium},
		{75, 1.5f, false, Difficulty::Hard},
		{ 25, 0.5f, true, Difficulty::Easy}
	};
	serialize_test(settings_vec);
}

TEST(JsonReflect, custom_struct_map) {
	std::map<std::string, ns::GameSettings> settings_map = {
		{"Player1", {50, 1.0f, true, Difficulty::Medium}},
		{"Player2", {75, 1.5f, false, Difficulty::Hard}},
		{"Player3", {25, 0.5f, true, Difficulty::Easy}}
	};
	serialize_test(settings_map);
}

/* private members */
namespace ns {
	struct PrivateSettings {
	private:
		int			volume = 50;
		float		sensitivity = 1.0f;
		bool		fullscreen = true;
		Difficulty	difficulty = Difficulty::Medium;
		BEFRIEND_JSON_REFLECT()
	};
}
JSON_REFLECT(ns::PrivateSettings, volume, sensitivity, fullscreen, difficulty);

TEST(JsonReflect, private_members) {
	ns::PrivateSettings settings;
	serialize_test(settings);
}

/* Tag invoke test */
namespace ns {
	struct CustomObject {
		int id = 0;
		std::string name;
		float value = 0.0f;

		void init() {};
	};
}
JSON_REFLECT(ns::CustomObject, id, name, value);

#if 0

inline JsonReflect::json tag_invoke(JsonReflect::serialize_t, const ns::CustomObject& object) {
	JsonReflect::json j;
	j["id"] = JsonReflect::to_json(object.id);
	j["name"] = JsonReflect::to_json(object.name);
	j["value"] = JsonReflect::to_json(object.value);
	return j;
}

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, ns::CustomObject& object) {
	if (j.contains("id")) JsonReflect::from_json(j["id"], object.id);
	if (j.contains("name")) JsonReflect::from_json(j["name"], object.name);
	if (j.contains("value")) JsonReflect::from_json(j["value"], object.value);

	// Call init after deserialization
	object.init();
}

#else

inline void tag_invoke(JsonReflect::deserialize_t, const JsonReflect::json& j, ns::CustomObject& object) {
	// Use reflection deserialization
	JsonReflect::Detail::from_json_visitable(j, object);

	// Call init after deserialization
	object.init();
}

#endif

TEST(JsonReflect, tag_invoke_custom_object) {
	ns::CustomObject obj;
	obj.id = 1;
	obj.name = "Test Object";
	obj.value = 3.14f;
	serialize_test(obj);
}

/* Json type */
TEST(JsonReflect, json_type) {
	JsonReflect::json j;
	j["number"] = 42;
	j["string"] = "Hello, JSON!";
	j["array"] = { 1, 2, 3 };
	j["object"] = { {"key", "value"} };
	serialize_test(j);

	JsonReflect::json j2 = nullptr;
	serialize_test(j2);

	JsonReflect::json j3 = JsonReflect::json::object();
	serialize_test(j3);
}

/* Json Diff */
TEST(JsonReflect, get_changes) {
	ns::NestedSettings settings1;
	settings1.player_name = "PlayerOne";
	settings1.game_settings.volume = 75;
	settings1.game_settings.fullscreen = false;
	settings1.game_settings.difficulty = Difficulty::Hard;

	ns::NestedSettings settings2;
	settings2.player_name = "PlayerTwo";
	settings2.game_settings.volume = 50;
	settings2.game_settings.fullscreen = true;
	settings2.game_settings.difficulty = Difficulty::Medium;

	auto json_settings1 = JsonReflect::to_json(settings1);
	auto json_settings2 = JsonReflect::to_json(settings2);
	auto diff = nlohmann::json::diff(json_settings1, json_settings2);

	serialize_test(diff);
}

struct JsonHolder {
	nlohmann::json data{};
};
JSON_REFLECT(JsonHolder, data);

TEST(JsonReflect, json_holder) {
	JsonHolder holder{};
	holder.data["key1"] = "value1";
	holder.data["key2"] = 42;
	holder.data["nested"] = { {"nkey", "nvalue"} };
	serialize_test(holder);
}

/* diffing */
TEST(JsonReflect, json_holder_diff) {
	JsonHolder holder1{};
	holder1.data["key1"] = "value1";
	holder1.data["key2"] = 42;
	holder1.data["nested"] = { {"nkey", "nvalue"} };
	JsonHolder holder2{};
	holder2.data["key1"] = "value1_modified";
	holder2.data["key2"] = 43;
	holder2.data["nested"] = { {"nkey", "nvalue_modified"} };
	auto json_holder1 = JsonReflect::to_json(holder1);
	auto json_holder2 = JsonReflect::to_json(holder2);
	auto diff = nlohmann::json::diff(json_holder1, json_holder2);
	serialize_test(diff);
}

struct MyStruct {
	int a = 42;
	float n = 3.14f;
	bool c = true;
};

JsonReflect::json tag_invoke(JsonReflect::serialize_t, const MyStruct& value, bool prop) {
	return {};
}

JsonReflect::json tag_invoke(JsonReflect::serialize_t, const MyStruct& value) {
	return {};
}

TEST(JsonReflect, tag_invoke_only_serialize) {
	MyStruct input;
	auto result = JsonReflect::to_json(input, true);

	std::vector<MyStruct> input_1;
	input_1.push_back(MyStruct{});
	auto result_1 = JsonReflect::to_json(input_1);

	std::vector<MyStruct> input_2;
	input_2.push_back(MyStruct{});
	auto result_2 = JsonReflect::to_json(input_2, true, true);
}

/* Maps with custom structs */
struct MyValue {
	int a = 42;
	float b = 3.14f;
	std::string c = "Hello";
};
JSON_REFLECT(MyValue, a, b, c);

TEST(JsonReflect, map_with_custom_struct) {
	std::unordered_map<std::string, MyValue> my_map = { {"first", {42, 3.14f, "Hello"}}, {"second", {24, 2.71f, "World"}} };

	auto result = JsonReflect::to_json(my_map);

	const std::string json_str = result.dump(2);

	std::unordered_map<std::string, MyValue> output{};
	JsonReflect::from_json(result, output, 5);
}

/* Struct with delta serialization */
struct DeltaStruct {
	int a = 42;
	float b = 3.14f;
	bool c = true;
	std::string d = "Hello";

	std::vector<int> vec = { 1, 2, 3, 4, 5 };
	std::variant<int, std::string> var = 42;
};
JSON_REFLECT(DeltaStruct, a, b, c, d, vec, var);

template<>
struct JsonReflect::Detail::delta_serialize<DeltaStruct> : std::true_type {};

TEST(JsonReflect, delta_serialization) {
	DeltaStruct obj{};
	/* only change b and c */
	obj.b = 2.71f;
	obj.c = false;

	serialize_test(obj);
}

/* Struct with delta serialization NOT default constructible */
struct NonDefaultConstructible {
	int a;
	float b;
	bool c;
	std::string d;
	std::vector<int> vec;
	std::variant<int, std::string> var;
	NonDefaultConstructible(int a, float b, bool c, std::string d, std::vector<int> vec, std::variant<int, std::string> var)
		: a(a), b(b), c(c), d(d), vec(vec), var(var) {
	}
};
JSON_REFLECT(NonDefaultConstructible, a, b, c, d, vec, var);

template<>
struct JsonReflect::Detail::delta_serialize<NonDefaultConstructible> : std::true_type {};

template<>
struct JsonReflect::Detail::delta_default<NonDefaultConstructible> {
	static NonDefaultConstructible make() {
		return NonDefaultConstructible{ 0, 0.0f, false, "", {}, 0 };
	}
};

TEST(JsonReflect, delta_serialization_non_default_constructible) {
	NonDefaultConstructible obj{ 0, 0.0f, false, "", {}, 0 };
	/* only change b and c */
	obj.b = 2.71f;
	obj.c = true;

	auto result = JsonReflect::to_json(obj);
	auto string_result = result.dump(4);
}