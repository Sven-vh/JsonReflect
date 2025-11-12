#include "pch.h"
#include <nlohmann/json.hpp>

template<typename T>
static void serialize_test(const T input = {}) {
	auto result = JsonReflect::to_json(input);

	T output{};
	JsonReflect::from_json(result, output);

	EXPECT_EQ(input, output);
}

TEST(JsonReflect, numerics) {
	serialize_test<bool>();
	serialize_test<char>();
	serialize_test<signed char>();
	serialize_test<unsigned char>();
	serialize_test<short>();
	serialize_test<unsigned short>();
	serialize_test<int>();
	serialize_test<unsigned int>();
	serialize_test<long>();
	serialize_test<unsigned long>();
	serialize_test<long long>();
	serialize_test<unsigned long long>();
	serialize_test<float>();
	serialize_test<double>();
	serialize_test<long double>();
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

struct MyStruct {
	int a;
	float b;
	bool c;
	std::string d;

	bool operator==(const MyStruct& other) const {
		return a == other.a && b == other.b && c == other.c && d == other.d;
	}
};
JSON_REFLECT(MyStruct, a, b, c, d);

TEST(JsonReflect, struct_reflect) {
	MyStruct input{ 42, 3.14f, true, "Test String" };
	serialize_test(input);


	MyStruct left{ 1, 2.0f, false, "Left" };
	MyStruct right{ 3, 4.0f, true, "Right" };

	auto json_left = JsonReflect::to_json(left);
	auto json_right = JsonReflect::to_json(right);

	printf("JSON Left: %s\n", json_left.dump(2).c_str());
	printf("JSON Right: %s\n", json_right.dump(2).c_str());

	auto patch = nlohmann::json::diff(json_left, json_right);

	printf("JSON Patch: %s\n", patch.dump(2).c_str());
}









//TEST(JsonReflect, diff) {
//	nlohmann::json a = { 1, 2, 3 };
//	nlohmann::json b = { 1, 2, 4 };
//
//	std::cout << "JSON A: " << a.dump(2) << std::endl;
//	std::cout << "JSON B: " << b.dump(2) << std::endl;
//
//	// Compute the difference as a JSON Patch
//	nlohmann::json patch = nlohmann::json::diff(a, b);
//
//	std::cout << patch.dump(2) << std::endl;
//}