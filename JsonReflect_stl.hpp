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

		// Helper to construct smart pointers properly
		template<typename T>
		void initialize_smart_pointer(std::shared_ptr<T>& ptr) {
			ptr = std::make_shared<T>();
		}

		template<typename T, typename Deleter>
		void initialize_smart_pointer(std::unique_ptr<T, Deleter>& ptr) {
			ptr = std::make_unique<T>();
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
			value.reset();  // Explicitly set to null
			return;
		}

#ifdef JSON_REFLECT_INITIALIZE_SMART_POINTERS
		if (!value) {
			Detail::initialize_smart_pointer(value);
		}
#else
		if (!value) {
			throw std::runtime_error("JsonReflect Error: Cannot deserialize to null smart pointer (enable JSON_REFLECT_INITIALIZE_SMART_POINTERS to auto-initialize)");
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

	/* [ Deserialize ] Weak Pointers */
	template<typename T>
	std::enable_if_t<Detail::is_weak_pointer_v<T>, void>
		tag_invoke(deserialize_default_t, const json& j, T& value) {
		if (j.is_null()) {
			return;  // Nothing to do
		}

		auto shared_ptr = value.lock();
		if (shared_ptr) {
			JsonReflect::from_json(j, *shared_ptr);
		} else {
			throw std::runtime_error("JsonReflect Error: Cannot deserialize to expired weak_ptr");
		}
	}
}
