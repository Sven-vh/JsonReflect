// ============================================================================
// JsonRefelect - Reflection-based Json Serialization Library
// By Sven van Huessen (https://www.svenvh.nl/)
// Made as self-study project at Breda University of Applied Sciences
// Licensed under the MIT License
// https://github.com/Sven-vh/JsonReflect
// ============================================================================
#pragma once
#include <type_traits>
#include "JsonReflect_entry.hpp"
#include "JsonReflect_helpers.hpp"

namespace JsonReflect {

	namespace Detail {
		/* Is Smart pointers */
		template<typename T>
		struct is_smart_pointer_impl : std::false_type {};
		template<typename T>
		struct is_smart_pointer_impl<std::shared_ptr<T>> : std::true_type {};
		template<typename T>
		struct is_smart_pointer_impl<std::unique_ptr<T>> : std::true_type {};

		template<typename T>
		constexpr bool is_smart_pointer_v = Detail::is_smart_pointer_impl<std::remove_cv_t<T>>::value;

		template<typename T, typename = void>
		struct is_weak_pointer_impl : std::false_type {};
		template<typename T>
		struct is_weak_pointer_impl<T, std::void_t<typename T::element_type>> : std::is_same<std::remove_cv_t<T>, std::weak_ptr<typename T::element_type>> {};


		template<typename T>
		constexpr bool is_weak_pointer_v = is_weak_pointer_impl<std::remove_cv_t<T>>::value;

		template<typename T>
		struct smart_pointer_factory {
			static std::shared_ptr<T> create() {
				return std::make_shared<T>(); // Default implementation
			}
		};

		/*
		Custom smart pointer factory example
		If you have a custom smart pointer type, you can specialize this struct to provide a way to create instances.

		template<>
		struct smart_pointer_factory<CustomObject> {
			static std::shared_ptr<CustomObject> create() {
				return std::make_shared<CustomObject>(0, "default");
			}
		};
		*/

		// Helper to construct smart pointers properly
		template<typename T>
		void initialize_smart_pointer(std::shared_ptr<T>& ptr) {
			ptr = Detail::smart_pointer_factory<T>::create();
		}

		template<typename T, typename Deleter>
		void initialize_smart_pointer(std::unique_ptr<T, Deleter>& ptr) {
			static_assert(std::is_default_constructible_v<T>, "Type must be default constructible for unique_ptr deserialization");

			if constexpr (std::is_same_v<Deleter, std::default_delete<T>>) {
				ptr = std::make_unique<T>();
			} else {
				static_assert(std::is_default_constructible_v<Deleter>, "Custom deleter must be default constructible");
				ptr = std::unique_ptr<T, Deleter>(new T(), Deleter{});
			}
		}
	}

	/* [ Serialize ] Smart Pointers, shared & unique */
	template<typename T>
	std::enable_if_t<Detail::is_smart_pointer_v<T>, json>
		tag_invoke(serialize_default_t, const T& value) {
		if (value) {
			return JsonReflect::to_json(*value);
		}
		return json(nullptr);
	}

	/* [ Deserialize ] Smart Pointers, shared & unique */
	template<typename T>
	std::enable_if_t<Detail::is_smart_pointer_v<T>, void>
		tag_invoke(deserialize_default_t, const json& j, T& value) {
		if (j.is_null()) {
			value.reset();
			return;
		}

#ifdef JSON_REFLECT_INITIALIZE_SMART_POINTERS
		if (!value) {
			Detail::initialize_smart_pointer(value);
		}
#else
		if (!value) {
			using element_type = typename T::element_type;
			throw std::runtime_error(
				std::string("JsonReflect Error: Cannot deserialize to null smart pointer for type '") +
				typeid(element_type).name() +
				"' (enable JSON_REFLECT_INITIALIZE_SMART_POINTERS to auto-initialize)"
			);
		}
#endif
		JsonReflect::from_json(j, *value);
	}

	/* [ Serialize ] Weak Pointers */
	template<typename T>
	std::enable_if_t<Detail::is_weak_pointer_v<T>, json>
		tag_invoke(serialize_default_t, const T& value) {
		auto shared_ptr = value.lock();
		if (shared_ptr) {
			return JsonReflect::to_json(*shared_ptr);
		}
		return json(nullptr);
	}

	/* [ Deserialize ] Weak Pointers - Not supported */
	template<typename T>
	std::enable_if_t<Detail::is_weak_pointer_v<T>, void>
		tag_invoke(deserialize_default_t, const json& j, T& value) {
		static_assert(std::false_type::value, "JsonReflect Error: Deserialization of weak_ptr is not supported");
	}
}
