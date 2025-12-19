#pragma once
#include <type_traits>
#include "JsonReflect_entry.hpp"
#include "JsonReflect_helpers.hpp"
#include <magic_enum/magic_enum.hpp>

namespace JsonReflect {

	/* [ Serialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
	template<typename T>
	std::enable_if_t<Detail::is_json_compatible_v<T>, json> tag_invoke(serialize_default_t, const T& value) {
		return json(value);
	}

	/* [ Deserialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
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

template<typename T>
static std::enable_if_t<JsonReflect::Detail::has_custom_to_json_v<T>, void> to_json(nlohmann::json& j, const T& value) {
	j = JsonReflect::to_json(value);
}
template<typename T>
static std::enable_if_t<JsonReflect::Detail::has_custom_to_json_v<T>, void> to_json(nlohmann::ordered_json& j, const T& value) {
	j = JsonReflect::to_json(value);
}

template<typename T>
static std::enable_if_t<JsonReflect::Detail::has_custom_from_json_v<T>, void> from_json(const nlohmann::json& j, T& value) {
	JsonReflect::from_json(j, value);
}
template<typename T>
static std::enable_if_t<JsonReflect::Detail::has_custom_from_json_v<T>, void> from_json(const nlohmann::ordered_json& j, T& value) {
	JsonReflect::from_json(j, value);
}

//namespace nlohmann {
//	template <typename T>
//	struct adl_serializer<T, std::enable_if_t<JsonReflect::Detail::is_json_compatible_v<T>>> {
//		static void to_json(json& j, const T& value) {
//			j = JsonReflect::to_json(value);
//		}
//
//		static void to_json(ordered_json& j, const T& value) {
//			j = JsonReflect::to_json(value);
//		}
//
//		static void from_json(const json& j, T& opt) {
//			JsonReflect::from_json(j, opt);
//		}
//
//		static void from_json(const ordered_json& j, T& opt) {
//			JsonReflect::from_json(j, opt);
//		}
//	};
//}

static_assert(JsonReflect::Detail::is_json_compatible_v<int>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<float>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<double>, "Trait check failed");

static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<int>>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<float>>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<double>>, "Trait check failed");


//namespace JsonReflect::Detail::Tests {
//	struct CustomType {
//		int a;
//		float b;
//	};
//}
//JSON_REFLECT(JsonReflect::Detail::Tests::CustomType, a, b);
//static_assert(nlohmann::detail::is_compatible_type<nlohmann::json, std::vector<JsonReflect::Detail::Tests::CustomType>>::value, "Trait check failed");
//static_assert(nlohmann::detail::is_compatible_type<nlohmann::json, std::vector<GameSettings>>::value, "Trait check failed");
//static_assert(nlohmann::detail::is_compatible_type<nlohmann::ordered_json, std::vector<GameSettings>>::value, "Trait check failed");

static_assert(svh::is_tag_invocable_v<JsonReflect::serialize_default_t, const int&>, "Tag invocable check failed");
static_assert(svh::is_tag_invocable_v<JsonReflect::deserialize_default_t, const JsonReflect::json&, int&>, "Tag invocable check failed");