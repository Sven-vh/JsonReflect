#include "pch.h"
#include <nlohmann/json.hpp>

template<typename T>
static void serialize_test(const T input) {
	auto result = JsonReflect::to_json(input);

	const std::string json_str = result.dump(2);

	T output{};
	JsonReflect::from_json(result, output);

	EXPECT_EQ(input, output);
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

TEST(JsonReflect, vector) {
	serialize_test<std::vector<int>>({ 1, 2, 3, 4, 5 });
	serialize_test<std::vector<std::string>>({ "one", "two", "three" });
}

TEST(JsonReflect, map) {
	serialize_test<std::map<std::string, int>>({ {"one", 1}, {"two", 2}, {"three", 3} });
}

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

struct GameSettings {
	int			volume = 50;
	float		sensitivity = 1.0f;
	bool		fullscreen = true;
	Difficulty	difficulty = Difficulty::Medium;

	/* Required for unit test compare */
	bool operator==(const GameSettings& other) const {
		return volume == other.volume &&
			sensitivity == other.sensitivity &&
			fullscreen == other.fullscreen &&
			difficulty == other.difficulty;
	}
};
JSON_REFLECT(GameSettings, volume, sensitivity, fullscreen, difficulty);

using json = nlohmann::ordered_json;
TEST(JsonReflect, custom_struct) {
	GameSettings settings;
	serialize_test(settings);
}