// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
// DEFAULT EXAMPLE USAGE:
// 
// Mark your struct visitable:
// ```
// #include "JsonReflect_macro.hpp"
// 
// namespace MyProject {
// 	 struct MyStruct {
// 	   int a = 0;
// 	   float b = 0.0f;
// 	   double c = 0.0;
// 	 };
// }
// JSON_REFLECT(MyProject::MyStruct, a, b, c);
// ```
// 
// Serialize and deserialize:
// ```
// #include "JsonReflect.hpp"
// 
// MyProject::MyStruct my_struct;
// // Serialize to json
// nlohmann::json j = JsonReflect::to_json(my_struct);
// // Deserialize from json
// JsonReflect::from_json(j, my_struct);
// ```
// 
// Add the macro `BEFRIEND_JSON_REFLECT()` to your struct/class to allow private members to be serialized/deserialized:
// 
// ```
// #include "JsonReflect_macro.hpp"
// 
// namespace MyProject {
// 	 struct MyStruct {
//    private:
// 	   int a = 0;
// 	   float b = 0.0f;
// 	   double c = 0.0;
// 
// 	   BEFRIEND_JSON_REFLECT();
//   };
// }
// ```
// ============================================================================
#pragma once

#include "JsonReflect_defines.hpp"

/* Include nlohmann json */
#include JSON_REFLECT_NLOHMANN_JSON_HPP
/* svh tag_invoke */
#include JSON_REFLECT_SVH_TAG_INVOKE_HPP
/* visit_struct */
#include JSON_REFLECT_VISIT_STRUCT_HPP

#include "JsonReflect_macro.hpp"
#include "JsonReflect_helpers.hpp"
#include "JsonReflect_entry.hpp"
#include "JsonReflect_primitives.hpp"
#include "JsonReflect_stl.hpp"