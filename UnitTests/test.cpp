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
	{
		auto sp = std::make_shared<int>(42);
		std::weak_ptr<int> wp = sp;
		serialize_test<std::weak_ptr<int>>(wp);
	}
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