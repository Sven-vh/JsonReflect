#pragma once

/* Disable implicit nlohmann conversion */
/* this is to avoid confusion with templates that are exidently converted to json objects */
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>

#include <svh/tag_invoke.hpp>
#include <visit_struct/visit_struct.hpp>

#include "JsonReflect_helpers.hpp"
#include "JsonReflect_entry.hpp"
#include "JsonReflect_primitives.hpp"