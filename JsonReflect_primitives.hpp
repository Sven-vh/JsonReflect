// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once
#include <type_traits>
#include "JsonReflect_entry.hpp"
#include "JsonReflect_helpers.hpp"
#include <magic_enum/magic_enum.hpp>
#include <utility>

namespace JsonReflect {

	/* [ Serialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
	template<typename T, typename... Args>
	std::enable_if_t<Detail::is_json_compatible_v<T>, json> tag_invoke(serialize_default_t, const T& value, Args&&... args) {
		/* Lose args */
		/* If you know a way to fix this, let me know by making issue/PR */
		return json(value);
	}

	/* [ Deserialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
	template<typename T, typename... Args>
	std::enable_if_t<Detail::is_json_compatible_v<T>, void> tag_invoke(deserialize_default_t, const json& j, T& value, Args&&... args) {
		/* Lose args */
		/* If you know a way to fix this, let me know by making issue/PR */
		j.get_to(value);
		//value = j.get<T>();
	}

	/* [ Serialize ] Json type */
	template <typename T>
	std::enable_if_t<nlohmann::detail::is_basic_json<T>::value, json>
		tag_invoke(serialize_default_t, const T& value) {
		return value;
	}

	/* [ Deserialize ] Json type */
	template <typename T>
	std::enable_if_t<nlohmann::detail::is_basic_json<T>::value, void>
		tag_invoke(deserialize_default_t, const json& j, T& value) {
		value = j;
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

/* Entry point for type nlohmann doesn't know */
namespace nlohmann {
	template <typename T>
	struct adl_serializer<T, std::enable_if_t<JsonReflect::Detail::has_custom_to_json_v<T> || JsonReflect::Detail::has_custom_from_json_v<T>>> {
		template <typename U = T, typename = std::enable_if_t<JsonReflect::Detail::has_custom_to_json_v<U>>>
		static void to_json(json& j, const U& value) {
			j = JsonReflect::to_json(value);
		}

		template <typename U = T, typename = std::enable_if_t<JsonReflect::Detail::has_custom_to_json_v<U>>>
		static void to_json(ordered_json& j, const U& value) {
			j = JsonReflect::to_json(value);
		}

		template <typename U = T, typename = std::enable_if_t<JsonReflect::Detail::has_custom_from_json_v<U>>>
		static void from_json(const json& j, U& opt) {
			JsonReflect::from_json(j, opt);
		}

		template <typename U = T, typename = std::enable_if_t<JsonReflect::Detail::has_custom_from_json_v<U>>>
		static void from_json(const ordered_json& j, U& opt) {
			JsonReflect::from_json(j, opt);
		}
	};
}

static_assert(JsonReflect::Detail::is_json_compatible_v<int>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<float>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<double>, "Trait check failed");

static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<int>>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<float>>, "Trait check failed");
static_assert(JsonReflect::Detail::is_json_compatible_v<std::vector<double>>, "Trait check failed");

static_assert(svh::is_tag_invocable_v<JsonReflect::serialize_default_t, const int&>, "Tag invocable check failed");
static_assert(svh::is_tag_invocable_v<JsonReflect::deserialize_default_t, const JsonReflect::json&, int&>, "Tag invocable check failed");