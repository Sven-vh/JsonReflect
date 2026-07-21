#pragma once
#include "JsonReflect_macro.hpp"

namespace MyNamespace {
	struct MyStruct {
		int a = 0;
		float b = 0.0f;
		double c = 0.0;
		BEFRIEND_JSON_REFLECT();
	};
}
JSON_REFLECT(MyNamespace::MyStruct, a, b, c);