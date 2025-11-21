#pragma once
#include <type_traits>
#include "JsonReflect_entry.hpp"
#include "JsonReflect_helpers.hpp"
#include <magic_enum/magic_enum.hpp>

namespace JsonReflect {

	/* [ Serialize ] nlohmann defaults */
	template<typename T>
	std::enable_if_t<Detail::is_json_compatible_v<T>, json> tag_invoke(serialize_default_t, const T& value) {
		return json(value);
	}

	/* [ Deserialize ] nlohmann defaults */
	template<typename T>
	std::enable_if_t<Detail::is_json_compatible_v<T>, void> tag_invoke(deserialize_default_t, const json& j, T& value) {
		value = j.get<T>();
	}

	/* [ Serialize ] Enum types - serialize as string */
	template<typename T>
	std::enable_if_t<std::is_enum_v<T>, json> tag_invoke(serialize_lib_t, const T& value) {
		return json(magic_enum::enum_name(value));
	}

	/* [ Deserialize ] Enum types - deserialize from string */
	template<typename T>
	std::enable_if_t<std::is_enum_v<T>, void> tag_invoke(deserialize_lib_t, const json& j, T& value) {
		auto name = j.get<std::string>();
		auto enum_value = magic_enum::enum_cast<T>(name);
		if (enum_value.has_value()) {
			value = enum_value.value();
		} else {
			throw std::runtime_error("JsonReflect Error: Invalid enum name '" + name + "' for enum type");
		}
	}
}

static_assert(JsonReflect::Detail::is_json_compatible_v<int>, "Trait check failed");
static_assert(svh::is_tag_invocable_v<JsonReflect::serialize_default_t, const int&>, "Tag invocable check failed");
static_assert(svh::is_tag_invocable_v<JsonReflect::deserialize_default_t, const JsonReflect::json&, int&>, "Tag invocable check failed");