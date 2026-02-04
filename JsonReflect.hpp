// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once

/* Disable implicit nlohmann conversion */
/* this is to avoid confusion with templates that are exidently converted to json objects */
#ifndef JSON_USE_IMPLICIT_CONVERSIONS
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#endif
#include <nlohmann/json.hpp>

#include <svh/tag_invoke.hpp>
#include <visit_struct/visit_struct.hpp>

#include "JsonReflect_helpers.hpp"
#include "JsonReflect_entry.hpp"
#include "JsonReflect_primitives.hpp"
#include "JsonReflect_stl.hpp"