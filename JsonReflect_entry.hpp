#pragma once

/* Disable implicit nlohmann conversion */
/* this is to avoid confusion with templates that are exidently converted to json objects */
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <svh/tag_invoke.hpp>
#include <visit_struct/visit_struct.hpp>

#define JSON_REFLECT(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::serialize_lib_t, T, __VA_ARGS__);\
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::deserialize_lib_t, T, __VA_ARGS__);\
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::compare_lib_t, T, __VA_ARGS__)

/* Needs to be defined outside of any namespaces */
struct json_reflect_global_tag {};

namespace JsonReflect {
	using json = nlohmann::ordered_json;

	/* Tags */
	struct serialize_t : json_reflect_global_tag { /* Public Tag */ };
	inline constexpr serialize_t serialize{};
	struct serialize_lib_t : json_reflect_global_tag { /* Library only */ };
	inline constexpr serialize_lib_t serialize_lib{};
	struct serialize_default_t : json_reflect_global_tag { /* Library only - nlohmann Default Implementation */ };
	inline constexpr serialize_default_t serialize_default{};

	struct deserialize_t : json_reflect_global_tag { /* Public Tag */ };
	inline constexpr deserialize_t deserialize{};
	struct deserialize_lib_t : json_reflect_global_tag { /* Library only */ };
	inline constexpr deserialize_lib_t deserialize_lib{};
	struct deserialize_default_t : json_reflect_global_tag { /* Library only - nlohmann Default Implementation */ };
	inline constexpr deserialize_default_t deserialize_default{};

	struct compare_t : json_reflect_global_tag { /* Public Tag */ };
	inline constexpr compare_t compare{};
	struct compare_lib_t : json_reflect_global_tag { /* Library only */ };
	inline constexpr compare_lib_t compare_lib{};

	namespace Detail {
		template<typename T, typename CONTEXT> /* Has a visitable struct implementation */
		constexpr bool is_visitable_v = visit_struct::traits::is_visitable<T, CONTEXT>::value;

		/* Check if type can be serialized */
		template <typename T, typename... Args>
		inline constexpr bool has_to_json_v =
			svh::is_tag_invocable_v<serialize_t, const T&, Args...> ||
			svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...> ||
			svh::is_tag_invocable_v<serialize_default_t, const T&, Args...> ||
			is_visitable_v<T, serialize_lib_t>;

		template <typename T, typename... Args>
		inline constexpr bool has_custom_to_json_v =
			svh::is_tag_invocable_v<serialize_t, const T&, Args...> ||
			svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...> ||
			is_visitable_v<T, serialize_lib_t>;

		/* Check if type can be deserialized */
		template <typename T, typename... Args>
		inline constexpr bool has_from_json_v =
			svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...> ||
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...> ||
			svh::is_tag_invocable_v<deserialize_default_t, const json&, T&, Args...> ||
			is_visitable_v<T, deserialize_lib_t>;

		template <typename T, typename... Args>
		inline constexpr bool has_custom_from_json_v =
			svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...> ||
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...> ||
			is_visitable_v<T, deserialize_lib_t>;

		template<typename T>
		inline constexpr bool is_json_compatible_v =
			nlohmann::detail::is_compatible_type<nlohmann::json, uncvref_t<T>>::value &&
			!has_custom_to_json_v<uncvref_t<T>> &&
			!has_custom_from_json_v<uncvref_t<T>>;
	}

	template<typename T, typename... Args>
	static json to_json(const T& value, Args&&... args) {
		/* 1) Check for user defined serialize funciton */
		if constexpr (svh::is_tag_invocable_v<serialize_t, const T&, Args...>) {
			return tag_invoke(serialize, value, std::forward<Args>(args)...);
		}
		/* 2) Check for library defined serialize function */
		else if constexpr (svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...>) {
			return tag_invoke(serialize_lib, value, std::forward<Args>(args)...);
		}
		/* 3) Check for nlohmann default serialize function */
		else if constexpr (svh::is_tag_invocable_v<serialize_default_t, const T&, Args...>) {
			return tag_invoke(serialize_default, value, std::forward<Args>(args)...);
		}
		/* 4) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, serialize_lib_t>) {
			json j;
			visit_struct::context<serialize_lib_t>::for_each(value, [&](const char* name, const auto& field) {
				j[name] = to_json(field, std::forward<Args>(args)...);
				});
			return j;
		}
		/* 5) No suitable serialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable serialize implementation found for type T");
			return {};
		}
	}

	template<typename T, typename... Args>
	static void from_json(const json& j, T& value, Args&&... args) {
		static_assert(std::is_const_v<T> == false, "JsonSerializer Error: Cannot deserialize a const object of type T");
		/* 1) Check for user defined deserialize funciton */
		if constexpr (svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...>) {
			return tag_invoke(deserialize, j, value, std::forward<Args>(args)...);
		}
		/* 2) Check for library defined deserialize function */
		else if constexpr (svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...>) {
			return tag_invoke(deserialize_lib, j, value, std::forward<Args>(args)...);
		}
		/* 3) Check for nlohmann default deserialize function */
		else if constexpr (svh::is_tag_invocable_v<deserialize_default_t, const json&, T&, Args...>) {
			return tag_invoke(deserialize_default, j, value, std::forward<Args>(args)...);
		}
		/* 4) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, deserialize_lib_t>) {
			visit_struct::context<deserialize_lib_t>::for_each(value, [&](const char* name, auto& field) {
				auto it = j.find(name);
				if (it != j.end()) {
					from_json(it.value(), field, std::forward<Args>(args)...);
				}
				});
		}
		/* 5) No suitable deserialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable deserialize implementation found for type T");
			return;
		}
	}

	/* (ORDER MATTERS) Get changes between two objects as a json diff */
	/* If right object has different values than left, those values are stored in the resulting json */
	template<typename T, typename... Args>
	static json get_changes(const T& lhs, const T& rhs, Args&&... args) {
		/* 1) Check for user defined compare funciton */
		if constexpr (svh::is_tag_invocable_v<compare_t, const T&, const T&, Args...>) {
			return tag_invoke(compare, lhs, rhs, std::forward<Args>(args)...);
		}
		/* 2) Check for library defined compare function */
		else if constexpr (svh::is_tag_invocable_v<compare_lib_t, const T&, const T&, Args...>) {
			return tag_invoke(compare_lib, lhs, rhs, std::forward<Args>(args)...);
		}
		/* 3) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, compare_lib_t>) {
			json j;
			visit_struct::context<compare_lib_t>::for_each(lhs, rhs, [&](const char* name, const auto& field_lhs, const auto& field_rhs) {
				json diff = get_changes(field_lhs, field_rhs, std::forward<Args>(args)...);
				if (diff.empty() == false) {
					j[name] = diff;
				}
				});
			return j;
		}
		/* 4) Fallback to equality operator if available */
		else if constexpr (Detail::has_equality_operator_v<T>) {
			if (lhs == rhs) {
				return {};
			} else {
				return to_json(rhs, std::forward<Args>(args)...);
			}
		}
		/* 6) No suitable compare implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable serialize implementation found for type T");
			return {};
		}
	}
}