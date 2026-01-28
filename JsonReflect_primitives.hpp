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

	namespace Detail {
		template<typename T, typename... Args>
		struct serialize_argument_wrapper {
			const T& value;
			std::tuple<Args&&...> args;

			serialize_argument_wrapper(const T& v, Args&&... a)
				: value(v)
				, args(std::forward<Args>(a)...) {
			}
		};

		template<typename T, typename... Args>
		struct deserialize_argument_wrapper {
			T& value;
			std::tuple<Args&&...> args;

			deserialize_argument_wrapper(T& v, Args&&... a)
				: value(v)
				, args(std::forward<Args>(a)...) {
			}
		};
	}

	/* [ Serialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
	template<typename T, typename... Args>
	std::enable_if_t<Detail::is_json_compatible_v<T>, json> tag_invoke(serialize_default_t, const T& value, Args&&... args) {
		constexpr bool no_args = sizeof...(Args) == 0;
		constexpr bool tag_invocable = Detail::has_custom_to_json_v<T, Args...>;
		if constexpr (no_args && !tag_invocable) {
			return json(value);
		}
		/* Wrap value + arguments */
		return json(Detail::serialize_argument_wrapper(value, std::forward<Args>(args)...));
	}

	/* [ Deserialize ] nlohmann defaults */
	/* Includes most of std types and primitives */
	template<typename T, typename... Args>
	std::enable_if_t<Detail::is_json_compatible_v<T>, void> tag_invoke(deserialize_default_t, const json& j, T& value, Args&&... args) {
		constexpr bool no_args = sizeof...(Args) == 0;
		constexpr bool tag_invocable = Detail::has_custom_from_json_v<T, Args...>;
		if constexpr (no_args && !tag_invocable) {
			value = j.get<T>();
			return;
		}
		/* Wrap value + arguments */
		using wrapper = Detail::deserialize_argument_wrapper<T, Args...>;
		wrapper w(value, std::forward<Args>(args)...);
		j.get_to(w);
	}

	/* [ Serialize ] argument wrapper */
	template<typename T, typename... Args>
	json tag_invoke(serialize_lib_t, const Detail::serialize_argument_wrapper<T, Args...>& wrapper) {
		/* Unwrap value + arguments */
		auto& mutable_wrapper = const_cast<Detail::serialize_argument_wrapper<T, Args...>&>(wrapper);
		return std::apply([&](auto&&... args) {
			return JsonReflect::to_json(mutable_wrapper.value, std::forward<decltype(args)>(args)...);
			}, std::move(mutable_wrapper.args));
	}

	/* [ Deserialize ] argument wrapper */
	template<typename T, typename... Args>
	void tag_invoke(deserialize_lib_t, const json& j, Detail::deserialize_argument_wrapper<T, Args...>& wrapper) {
		/* Unwrap value + arguments */
		return std::apply([&](auto&&... args) {
			return JsonReflect::from_json(j, wrapper.value, std::forward<decltype(args)>(args)...);
			}, std::move(wrapper.args));
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

using example_serialize_wrapper = JsonReflect::Detail::serialize_argument_wrapper<int>;

static_assert(JsonReflect::Detail::has_custom_to_json_v<example_serialize_wrapper>, "no to json"); // succes
static_assert(svh::is_tag_invocable_v<JsonReflect::serialize_lib_t, const example_serialize_wrapper&>, "no to json"); // succes

using example_deserialize_wrapper = JsonReflect::Detail::deserialize_argument_wrapper<int>;

static_assert(JsonReflect::Detail::has_custom_from_json_v<example_deserialize_wrapper>, "no from json"); // succes
static_assert(svh::is_tag_invocable_v<JsonReflect::deserialize_lib_t, const JsonReflect::json&, example_deserialize_wrapper&>, "no from json"); // succes
