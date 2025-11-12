#pragma once
#include <type_traits>
#include <nlohmann/json.hpp>

/* Helpers */
namespace JsonReflect::Detail {
	
	template<typename T>
	using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

	// Check if type is json-compatible (using internal trait)
	template<typename T>
	inline constexpr bool is_json_compatible_v =
		!nlohmann::detail::is_basic_json<uncvref_t<T>>::value &&
		nlohmann::detail::is_compatible_type<nlohmann::json, uncvref_t<T>>::value;
}