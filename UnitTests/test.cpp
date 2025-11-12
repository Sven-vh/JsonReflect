#include "pch.h"
#include "JsonReflect.hpp"

TEST(JsonReflect, integer) {
	int input = 42;
	auto j = JsonReflect::to_json(input);

	int output = 0;
	JsonReflect::from_json(j, output);

	EXPECT_EQ(input, output);
}