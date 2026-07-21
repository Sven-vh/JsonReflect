// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once

/* Defines / Settings */
#include "JsonReflect_defines.hpp"

/* Include nlohmann json */
#include JSON_REFLECT_NLOHMANN_JSON_HPP
/* Include visit_struct */
#include JSON_REFLECT_VISIT_STRUCT_HPP
/* Include svh tag_invoke */
#include JSON_REFLECT_SVH_TAG_INVOKE_HPP

#include <variant>

namespace JsonReflect {
	using json = nlohmann::ordered_json;

	/* Forward declare */
	template <typename T, typename... Args>
	static json to_json(const T& value, Args&&... args);

	template <typename T, typename... Args>
	static void from_json(const json& j, T& value, Args&&... args);

	namespace Detail {
		template<typename T, typename CONTEXT> /* Has a visitable struct implementation */
		constexpr bool is_visitable_v = visit_struct::traits::is_visitable<T, CONTEXT>::value;

		/* Check if type can be serialized */
		template <typename T, typename... Args>
		inline constexpr bool has_to_json_v =
			svh::is_tag_invocable_v<serialize_t, const T&, Args...> ||			/* WITH arguments */
			svh::is_tag_invocable_v<serialize_t, const T&> ||					/* WITHOUT arguments */
			svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...> ||		/* WITH arguments */
			svh::is_tag_invocable_v<serialize_lib_t, const T&> ||				/* WITHOUT arguments */
			svh::is_tag_invocable_v<serialize_default_t, const T&, Args...> ||	/* WITH arguments */
			svh::is_tag_invocable_v<serialize_default_t, const T&> ||			/* WITHOUT arguments */
			is_visitable_v<T, serialize_lib_t>;

		template <typename T, typename... Args>
		inline constexpr bool has_custom_to_json_v =
			svh::is_tag_invocable_v<serialize_t, const T&, Args...> ||			/* WITH arguments */
			svh::is_tag_invocable_v<serialize_t, const T&> ||					/* WITHOUT arguments */
			svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...> || 		/* WITH arguments */
			svh::is_tag_invocable_v<serialize_lib_t, const T&> ||				/* WITHOUT arguments */
			is_visitable_v<T, serialize_lib_t>;

		/* Check if type can be deserialized */
		template <typename T, typename... Args>
		inline constexpr bool has_from_json_v =
			svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...> ||			/* WITH arguments */
			svh::is_tag_invocable_v<deserialize_t, const json&, T&> ||                  /* WITHOUT arguments */
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...> ||		/* WITH arguments */
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&> ||              /* WITHOUT arguments */
			svh::is_tag_invocable_v<deserialize_default_t, const json&, T&, Args...> ||	/* WITH arguments */
			svh::is_tag_invocable_v<deserialize_default_t, const json&, T&> ||          /* WITHOUT arguments */
			is_visitable_v<T, deserialize_lib_t>;

		template <typename T, typename... Args>
		inline constexpr bool has_custom_from_json_v =
			svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...> ||		/* WITH arguments */
			svh::is_tag_invocable_v<deserialize_t, const json&, T&> ||				/* WITHOUT arguments */
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...> || /* WITH arguments */
			svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&> ||          /* WITHOUT arguments */
			is_visitable_v<T, deserialize_lib_t>;

		/* Check if type is nlohmann json compatible and has no custom (de)serialize implementation */
		template<typename T>
		inline constexpr bool is_json_compatible_v =
			nlohmann::detail::is_compatible_type<nlohmann::json, uncvref_t<T>>::value &&
			!has_custom_to_json_v<uncvref_t<T>> &&
			!has_custom_from_json_v<uncvref_t<T>>;

		/* == */
		template<typename T, typename = void>
		struct is_equality_comparable : std::false_type {};

		template<typename T>
		struct is_equality_comparable<T, std::void_t<
			decltype(std::declval<const T&>() == std::declval<const T&>())
			>> : std::true_type {};

		/* != */
		template<typename T, typename = void>
		struct is_inequality_comparable : std::false_type {};

		template<typename T>
		struct is_inequality_comparable<T, std::void_t<
			decltype(std::declval<const T&>() != std::declval<const T&>())
			>> : std::true_type {};

		// std::vector: only comparable if element type is
		template <typename T, typename Alloc>
		struct is_equality_comparable<std::vector<T, Alloc>> : is_equality_comparable<T> {};
		template <typename T, typename Alloc>
		struct is_inequality_comparable<std::vector<T, Alloc>> : is_inequality_comparable<T> {};

		// std::variant: only comparable if ALL alternatives are
		template <typename... Ts>
		struct is_equality_comparable<std::variant<Ts...>> : std::conjunction<is_equality_comparable<Ts>...> {};
		template <typename... Ts>
		struct is_inequality_comparable<std::variant<Ts...>> : std::conjunction<is_inequality_comparable<Ts>...> {};

		// std::optional: only comparable if value type is
		template <typename T>
		struct is_equality_comparable<std::optional<T>> : is_equality_comparable<T> {};
		template <typename T>
		struct is_inequality_comparable<std::optional<T>> : is_inequality_comparable<T> {};

		// std::array: only comparable if element type is
		template <typename T, std::size_t N>
		struct is_equality_comparable<std::array<T, N>> : is_equality_comparable<T> {};
		template <typename T, std::size_t N>
		struct is_inequality_comparable<std::array<T, N>> : is_inequality_comparable<T> {};

		// std::pair: only comparable if both types are
		template <typename T1, typename T2>
		struct is_equality_comparable<std::pair<T1, T2>> : std::conjunction<is_equality_comparable<T1>, is_equality_comparable<T2>> {};
		template <typename T1, typename T2>
		struct is_inequality_comparable<std::pair<T1, T2>> : std::conjunction<is_inequality_comparable<T1>, is_inequality_comparable<T2>> {};

		// std::map and std::unordered_map: only comparable if key and value types are
		template <typename Key, typename T, typename Compare, typename Alloc>
		struct is_equality_comparable<std::map<Key, T, Compare, Alloc>> : std::conjunction<is_equality_comparable<Key>, is_equality_comparable<T>> {};
		template <typename Key, typename T, typename Compare, typename Alloc>
		struct is_inequality_comparable<std::map<Key, T, Compare, Alloc>> : std::conjunction<is_inequality_comparable<Key>, is_inequality_comparable<T>> {};
		template <typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
		struct is_equality_comparable<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> : std::conjunction<is_equality_comparable<Key>, is_equality_comparable<T>> {};
		template <typename Key, typename T, typename Hash, typename KeyEqual, typename Alloc>
		struct is_inequality_comparable<std::unordered_map<Key, T, Hash, KeyEqual, Alloc>> : std::conjunction<is_inequality_comparable<Key>, is_inequality_comparable<T>> {};

		template <typename T>
		constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;
		template <typename T>
		constexpr bool is_inequality_comparable_v = is_inequality_comparable<T>::value;

		template <typename Tag = serialize_lib_t, typename T, typename... Args>
		static json to_json_visitable(const T& value, Args&&... args) {
			json j;

			constexpr bool delta_serialize_v = delta_serialize<T>::value;
			if constexpr (delta_serialize_v) {
				/* Delta serialize, only save member variables changed */
#if JSON_REFLECT_STATIC_FOR_DELTA
				static T compare = delta_default<T>::make();
#else
				const T compare = delta_default<T>::make();
#endif
				visit_struct::context<Tag>::for_each(value, compare, [&](const char* name, const auto& field_value, const auto& field_compare) {
					using Field_T = std::decay_t<decltype(field_value)>;
					if constexpr (is_equality_comparable_v<Field_T>) {
						if (field_value == field_compare) return;
						j[name] = to_json(field_value, std::forward<Args>(args)...);
					} else if constexpr (is_inequality_comparable_v<Field_T>) {
						if (field_value != field_compare)
							j[name] = to_json(field_value, std::forward<Args>(args)...);
					} else {
#if JSON_REFLECT_ALLOW_JSON_COMPARE
						/* ! EXPENSIVE ! */
						/* No equality operator, serialize both and compare json */
						auto field_json = to_json(field_value, std::forward<Args>(args)...);
						auto compare_json = to_json(field_compare, std::forward<Args>(args)...);
						if (field_json != compare_json) {
							j[name] = std::move(field_json);
							return;
						} else {
							return; /* No change, skip */
						}
#else
						static_assert(svh::always_false<Field_T>::value, "JsonReflect Error: Type T is set to delta serialize but has no equality operator, inequality operator and macro JSON_REFLECT_ALLOW_JSON_COMPARE is disabled.");
#endif
					}
					});
			} else {
				/* Default serialize */
				visit_struct::context<Tag>::for_each(value, [&](const char* name, const auto& field) {
					j[name] = to_json(field, std::forward<Args>(args)...);
					});
			}
			return j;
		}

		template <typename Tag = typename deserialize_lib_t, typename T, typename... Args>
		static void from_json_visitable(const json& j, T& value, Args&&... args) {
			visit_struct::context<Tag>::for_each(value, [&](const char* name, auto& field) {
				auto it = j.find(name);
				if (it != j.end()) {
					from_json(it.value(), field, std::forward<Args>(args)...);
				}
				});
		}
	}

	template<typename T, typename... Args>
	static json to_json(const T& value, [[maybe_unused]] Args&&... args) {

		/* 0) Pointer unwrapping */
		if constexpr (std::is_pointer_v<T>) {
			if (value == nullptr) return nullptr;
			return to_json(*value, std::forward<Args>(args)...);
		}
		/* 1) Check for user defined serialize funciton */
		else if constexpr (svh::is_tag_invocable_v<serialize_t, const T&, Args...>) { /* WITH arguments */
			return tag_invoke(serialize, value, std::forward<Args>(args)...);
		} else if constexpr (svh::is_tag_invocable_v<serialize_t, const T&>) { /* WITHOUT arguments */
			return tag_invoke(serialize, value);
		}
		/* 2) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, serialize_lib_t>) {
			return Detail::to_json_visitable(value, std::forward<Args>(args)...);
		}
		/* 3) Check for library defined serialize function */
		else if constexpr (svh::is_tag_invocable_v<serialize_lib_t, const T&, Args...>) { /* WITH arguments */
			return tag_invoke(serialize_lib, value, std::forward<Args>(args)...);
		} else if constexpr (svh::is_tag_invocable_v<serialize_lib_t, const T&>) { /* WITHOUT arguments */
			return tag_invoke(serialize_lib, value);
		}
		/* 4) Check for nlohmann default serialize function */
		else if constexpr (svh::is_tag_invocable_v<serialize_default_t, const T&, Args...>) {
			return tag_invoke(serialize_default, value, std::forward<Args>(args)...);
		}
		/* 5) No suitable serialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonReflect Error: No suitable serialize implementation found for type T");
			return {};
		}
	}

	template<typename T, typename... Args>
	static void from_json(const json& j, T& value, [[maybe_unused]] Args&&... args) {
		static_assert(std::is_const_v<T> == false, "JsonReflect Error: Cannot deserialize a const object of type T");
		/* 0) Pointer unwrapping */
		if constexpr (std::is_pointer_v<T>) {
			static_assert(!std::is_const_v<std::remove_pointer_t<T>>, "JsonReflect Error: Cannot deserialize into a pointer-to-const");
			//assert(value != nullptr && "JsonReflect Error: null pointer passed to from_json, allocate before deserializing");
			if (value == nullptr) {
#if JSON_REFLECT_ALLOW_THROW
				throw std::runtime_error("JsonReflect Error: null pointer passed to from_json, allocate before deserializing.");
#else
				std::cerr << "JsonReflect Error: null pointer passed to from_json, allocate before deserializing. function will return without modifying the pointer." << std::endl;
#endif
			}
			return from_json(j, *value, std::forward<Args>(args)...);
		}
		/* 1) Check for user defined deserialize funciton */
		else if constexpr (svh::is_tag_invocable_v<deserialize_t, const json&, T&, Args...>) { /* WITH arguments */
			return tag_invoke(deserialize, j, value, std::forward<Args>(args)...);
		} else if constexpr (svh::is_tag_invocable_v<deserialize_t, const json&, T&>) { /* WITHOUT arguments */
			return tag_invoke(deserialize, j, value);
		}
		/* 2) Check if type is reflected */
		else if constexpr (Detail::is_visitable_v<T, deserialize_lib_t>) {
			return Detail::from_json_visitable(j, value, std::forward<Args>(args)...);
		}
		/* 3) Check for library defined deserialize function */
		else if constexpr (svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&, Args...>) { /* WITH arguments */
			return tag_invoke(deserialize_lib, j, value, std::forward<Args>(args)...);
		} else if constexpr (svh::is_tag_invocable_v<deserialize_lib_t, const json&, T&>) { /* WITHOUT arguments */
			return tag_invoke(deserialize_lib, j, value);
		}
		/* 4) Check for nlohmann default deserialize function */
		else if constexpr (svh::is_tag_invocable_v<deserialize_default_t, const json&, T&, Args...>) {
			return tag_invoke(deserialize_default, j, value, std::forward<Args>(args)...);
		}
		/* 5) No suitable deserialize implementation found, compile assert */
		else {
			static_assert(svh::always_false<T>::value, "JsonReflect Error: No suitable deserialize implementation found for type T");
			return;
		}
	}

	/* (ORDER MATTERS) Get changes between two objects as a json diff */
	/* If right object has different values than left, those values are stored in the resulting json */
	template<typename T, typename... Args>
	static json get_changes(const T& lhs, const T& rhs, Args&&... args) {
		/* 0) Pointer unwrapping */
		if constexpr (std::is_pointer_v<T>) {
			if (lhs == nullptr && rhs == nullptr) return {};
			if (lhs == nullptr) return to_json(*rhs, std::forward<Args>(args)...);
			if (rhs == nullptr) {
#if JSON_REFLECT_ALLOW_THROW
				throw std::runtime_error("JsonReflect Error: provided right object is nullptr in JsonReflect::get_changes().");
#else
				std::cerr << "JsonReflect Error: provided right object is nullptr in JsonReflect::get_changes(). function will return empty json object.";
#endif
				return {};
			}
			return get_changes(*lhs, *rhs, std::forward<Args>(args)...);
		}
		/* 1) Check for user defined compare funciton */
		else if constexpr (svh::is_tag_invocable_v<compare_t, const T&, const T&, Args...>) {
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
			static_assert(svh::always_false<T>::value, "JsonReflect Error: No suitable serialize implementation found for type T");
			return {};
		}
	}
}