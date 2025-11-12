#pragma once
#include <type_traits>
#include "JsonReflect_entry.hpp"
#include "JsonReflect_helpers.hpp"

namespace JsonReflect {

	template<typename T>
	std::enable_if_t<Detail::is_json_compatible_v<T>, json> tag_invoke(serialize_lib_t, const T& value) {
		return json(value);
	}

	template<typename T>
	std::enable_if_t<Detail::is_json_compatible_v<T>, void> tag_invoke(deserialize_lib_t, const json& j, T& value) {
		value = j.get<T>();
	}
}

static_assert(JsonReflect::Detail::is_json_compatible_v<int>, "Trait check failed");
static_assert(svh::is_tag_invocable_v<JsonReflect::serialize_lib_t, const int&>, "Tag invocable check failed");