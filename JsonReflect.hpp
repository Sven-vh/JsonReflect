#pragma once

/* Disable implicit nlohmann conversion */
/* this is to avoid confusion with templates that are exidently converted to json objects */
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <svh/tag_invoke.hpp>
#include <visit_struct/visit_struct.hpp>

namespace JsonReflect {
	using json = nlohmann::json;

	/* Tags */
	struct serialize_t { /* Public Tag */ };
	inline constexpr serialize_t serialize{};
	struct serialize_lib_t { /* Library only */ };
	inline constexpr serialize_lib_t serialize_lib{};

	struct deserialize_t { /* Public Tag */ };
	inline constexpr deserialize_t deserialize{};
	struct deserialize_lib_t { /* Library only */ };
	inline constexpr deserialize_lib_t deserialize_lib{};

	struct compare_t { /* Public Tag */ };
	inline constexpr compare_t compare{};
	struct compare_lib_t { /* Library only */ };
	inline constexpr compare_lib_t compare_lib{};

	namespace Detail {
		template<typename T, typename CONTEXT> /* Has a visitable struct implementation */
		constexpr bool is_visitable_v = visit_struct::traits::is_visitable<T, CONTEXT>::value;
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
		/* 3) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, serialize_lib_t>) {
			json j;
			visit_struct::context<serialize_lib_t>::for_each(value, [&](const char* name, const auto& field) {
				j[name] = to_json(field, std::forward<Args>(args)...);
				});
			return j;
		} 
		/* 4) No suitable serialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable serialize implementation found for type T");
			return {};
		}
	}

	template<typename T, typename... Args>
	static void from_json(const json& j, T& value, Args&&... args) {
		/* 1) Check for user defined deserialize funciton */
		if constexpr (svh::is_tag_invocable_v<deserialize_t, const T&, Args...>) {
			return tag_invoke(deserialize, j, value, std::forward<Args>(args)...);
		}
		/* 2) Check for library defined deserialize function */
		else if constexpr (svh::is_tag_invocable_v<deserialize_lib_t, const T&, Args...>) {
			return tag_invoke(deserialize_lib, j, value, std::forward<Args>(args)...);
		}
		/* 3) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, deserialize_lib_t>) {
			visit_struct::context<deserialize_lib_t>::for_each(value, [&](const char* name, auto& field) {
				auto it = j.find(name);
				if (it != j.end()) {
					from_json(it.value(), field, std::forward<Args>(args)...);
				}
				});
		}
		/* 4) No suitable deserialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable deserialize implementation found for type T");
			return;
		}
	}

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
		/* 4) No suitable comapre implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonSerializer Error: No suitable serialize implementation found for type T");
			return {};
		}
	}
}