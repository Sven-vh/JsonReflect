// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once
#include <type_traits>
#include <nlohmann/json.hpp>

#define BEFRIEND_JSON_REFLECT()      \
    template <typename, typename> \
    friend struct ::visit_struct::traits::visitable;

/* Helpers */
namespace JsonReflect::Detail {

	// Remove const/volatile and reference from type
	template<typename T>
	using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	// Check if type is json-compatible (using internal trait)
	//!nlohmann::detail::is_basic_json<uncvref_t<T>>::value&&


	// Check if type T has equality operator defined
	template<typename T, typename = void>
	struct has_equality_operator : std::false_type {};

	template<typename T>
	struct has_equality_operator<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>> : std::true_type {};

	template<typename T>
	inline constexpr bool has_equality_operator_v = has_equality_operator<T>::value;
}