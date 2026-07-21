// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once

/* Configuration */
/* Whether to use a static instance when comparing for delta (using static causes less allocations) */
/* See Detail::to_json_visitable for usage */
#ifndef JSON_REFLECT_STATIC_FOR_DELTA
#define JSON_REFLECT_STATIC_FOR_DELTA 1
#endif

/* Whether to allow json compare when calculating delta (serializes object twice and compares, can be more expensive) */
/* See Detail::to_json_visitable for usage */
#ifndef JSON_REFLECT_ALLOW_JSON_COMPARE
#define JSON_REFLECT_ALLOW_JSON_COMPARE 1
#endif

/* Whether to auto-initialize smart pointers during deserialization (if null, will be initialized with default constructor) */
#ifndef JSON_REFLECT_INITIALIZE_SMART_POINTERS
#define JSON_REFLECT_INITIALIZE_SMART_POINTERS 1
#endif

/* Whether to allow throwing exceptions from JsonReflect (if disabled, will return null json or do nothing on error instead) */
#ifndef JSON_REFLECT_ALLOW_THROW
#define JSON_REFLECT_ALLOW_THROW 1
#endif

/* Disable implicit nlohmann conversion */
/* this is to avoid confusion with templates that are exidently converted to json objects */
#ifndef JSON_USE_IMPLICIT_CONVERSIONS
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#endif

/* Includes */
/* feel free to define your own include paths */
/* nlohmann json */
#ifndef JSON_REFLECT_INCLUDE_NLOHMANN_JSON_HPP
#define JSON_REFLECT_NLOHMANN_JSON_HPP <nlohmann/json.hpp>
#endif

/* visit_struct */
#ifndef JSON_REFLECT_VISIT_STRUCT_HPP
#define JSON_REFLECT_VISIT_STRUCT_HPP <visit_struct/visit_struct.hpp>
#endif

/* svh tag_invoke */
#ifndef JSON_REFLECT_SVH_TAG_INVOKE_HPP
#define JSON_REFLECT_SVH_TAG_INVOKE_HPP <svh/tag_invoke.hpp>
#endif

/* magic_enum */
#ifndef JSON_REFLECT_MAGIC_ENUM_HPP
#define JSON_REFLECT_MAGIC_ENUM_HPP <magic_enum/magic_enum.hpp>
#endif