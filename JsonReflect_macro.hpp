// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once
#include "JsonReflect_defines.hpp"

/* Include visit struct */
#include JSON_REFLECT_VISIT_STRUCT_HPP

/* Needs to be defined outside of any namespaces */
struct json_reflect_global_tag {};

namespace JsonReflect {
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
		/* Whether or not a type should only serialize it's changes */
		template<typename T>
		struct delta_serialize : std::false_type {};


		template<typename T>
		struct delta_default {
			/* Default, assumse default-constructible */
			static T make() {
				static_assert(std::is_default_constructible_v<T>, "JsonReflect: T is not default-constructible. Implement/specialize delta_default<T> to provide a baseline instance.");
				return T{};
			}
		};
	}
}

/* allow JsonReflect access to private members to be serialized/deserialized */
#define BEFRIEND_JSON_REFLECT()      \
    template <typename, typename> \
    friend struct ::visit_struct::traits::visitable;

/* [Advanced Usage] Only use if you want different serialization behavior */
#define JSON_REFLECT_SERIALIZE(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::serialize_lib_t, T, __VA_ARGS__)

/* [Advanced Usage] Only use if you want different deserialization behavior */
#define JSON_REFLECT_DESERIALIZE(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::deserialize_lib_t, T, __VA_ARGS__)

/* [Advanced Usage] Only use if you want different compare behavior */
#define JSON_REFLECT_COMPARE(T, ...) \
VISITABLE_STRUCT_IN_CONTEXT(JsonReflect::compare_lib_t, T, __VA_ARGS__)

#define JSON_REFLECT_EXPAND(x) x

// MAIN MACRO:
/*
	[Recommended Usage] Use this macro to reflect your struct/class
	Example usage:
	JSON_REFLECT(MyStruct, field1, field2, field3);
*/
#define JSON_REFLECT(T, ...) \
JSON_REFLECT_EXPAND(JSON_REFLECT_SERIALIZE(T, __VA_ARGS__)); \
JSON_REFLECT_EXPAND(JSON_REFLECT_DESERIALIZE(T, __VA_ARGS__)); \
JSON_REFLECT_EXPAND(JSON_REFLECT_COMPARE(T, __VA_ARGS__))

/* [Advanced Usage] Use this macro to reflect your templated struct/class */
#define JSON_REFLECT_SERIALIZE_TEMPLATE(TPARAMS, T, TARGS, ...) \
VISITABLE_TEMPLATE_STRUCT_IN_CONTEXT(JsonReflect::serialize_lib_t, TPARAMS, T, TARGS, __VA_ARGS__)

/* [Advanced Usage] Use this macro to reflect your templated struct/class */
#define JSON_REFLECT_DESERIALIZE_TEMPLATE(TPARAMS, T, TARGS, ...) \
VISITABLE_TEMPLATE_STRUCT_IN_CONTEXT(JsonReflect::deserialize_lib_t, TPARAMS, T, TARGS, __VA_ARGS__)

/* [Advanced Usage] Use this macro to reflect your templated struct/class */
#define JSON_REFLECT_COMPARE_TEMPLATE(TPARAMS, T, TARGS, ...) \
VISITABLE_TEMPLATE_STRUCT_IN_CONTEXT(JsonReflect::compare_lib_t, TPARAMS, T, TARGS, __VA_ARGS__)

/*
	[Advanced Usage] Use this macro to reflect your templated struct/class
	Example usage:
	JSON_REFLECT_TEMPLATE((typename T), MyTemplateStruct, (T), field1, field2, field3);
*/
#define JSON_REFLECT_TEMPLATE(TPARAMS, T, TARGS, ...) \
JSON_REFLECT_EXPAND(JSON_REFLECT_SERIALIZE_TEMPLATE(TPARAMS, T, TARGS, __VA_ARGS__)) \
JSON_REFLECT_EXPAND(JSON_REFLECT_DESERIALIZE_TEMPLATE(TPARAMS, T, TARGS, __VA_ARGS__)) \
JSON_REFLECT_EXPAND(JSON_REFLECT_COMPARE_TEMPLATE(TPARAMS, T, TARGS, __VA_ARGS__));