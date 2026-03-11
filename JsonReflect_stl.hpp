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

#include <variant>
#include <string>
#include <filesystem>

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

		// Detect associative containers (map, unordered_map, multimap, etc.)
		template<typename T, typename = void>
		struct is_associative_container : std::false_type {};

		template<typename T>
		struct is_associative_container<T, std::void_t<
			typename T::key_type,
			typename T::mapped_type
			>> : std::true_type {};

		template<typename T>
		inline constexpr bool is_associative_container_v = is_associative_container<T>::value;

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

		// Helper to deserialize variant at runtime index
		template <std::size_t I = 0, typename... Types, typename... Args>
		void deserialize_variant_at_index(std::size_t index, const json& j, std::variant<Types...>& value, [[maybe_unused]] Args... args) {
			if constexpr (I < sizeof...(Types)) {
				if (I == index) {
					using T = std::variant_alternative_t<I, std::variant<Types...>>;
					T temp;
					JsonReflect::from_json(j, temp, std::forward<Args>(args)...);
					value = std::move(temp);
				} else {
					deserialize_variant_at_index<I + 1>(index, j, value, std::forward<Args>(args)...);
				}
			} else {
				throw std::runtime_error(
					"JsonReflect Error: Invalid variant index " + std::to_string(index) +
					" during deserialization (max index: " + std::to_string(sizeof...(Types) - 1) + ")"
				);
			}
		}

		template<typename T, typename = void>
		struct is_container : std::false_type {};

		template<typename T>
		inline constexpr bool is_string_v =
			std::is_same_v<T, std::string> ||
			std::is_same_v<T, std::wstring> ||
			std::is_same_v<T, std::string_view> ||
			std::is_same_v<T, std::wstring_view> ||
			std::is_same_v<T, std::filesystem::path>;

		template<typename T>
		struct is_container<T, std::void_t<
			decltype(std::declval<T>().begin()),
			decltype(std::declval<T>().end()),
			typename T::value_type>
		> : std::bool_constant<!is_string_v<T>> {
		};

		template<typename T>
		inline constexpr bool is_container_v = is_container<T>::value;
	}

	/* [ Serialize ] Any container with arguments */
	template<typename Container, typename... Args>
	std::enable_if_t<
		Detail::is_container_v<Container>
		&& (sizeof...(Args) > 0),
		json>
		tag_invoke(serialize_lib_t, const Container& container, Args&&... args) {
		json j = json::array();
		for (const auto& element : container) {
			j.push_back(to_json(element, std::forward<Args>(args)...));
		}
		return j;
	}

	/* [ Deserialize ] Any container with arguments */
	template<typename Container, typename... Args>
	std::enable_if_t<
		Detail::is_container_v<Container>
		&& (sizeof...(Args) > 0),
		void
		>
		tag_invoke(deserialize_lib_t, const json& j, Container& container, Args&&... args) {
        container.clear();

        if constexpr (Detail::is_associative_container_v<Container>) {
            // For maps: JSON is an object {"key": value}
            for (const auto& [key_str, value_json] : j.items()) {
                using key_type = typename Container::key_type;
                using mapped_type = typename Container::mapped_type;

                // Handle key conversion from JSON string
                key_type key;
                if constexpr (std::is_same_v<key_type, std::string>) {
                    // String keys: use directly
                    key = key_str;
                } else if constexpr (std::is_arithmetic_v<key_type>) {
                    // Numeric keys: parse from string
                    if constexpr (std::is_integral_v<key_type>) {
                        if constexpr (std::is_unsigned_v<key_type>) {
                            key = static_cast<key_type>(std::stoull(key_str));
                        } else {
                            key = static_cast<key_type>(std::stoll(key_str));
                        }
                    } else {
                        key = static_cast<key_type>(std::stod(key_str));
                    }
                } else {
                    // Complex keys: deserialize from JSON string representation
                    json key_json = json::parse(key_str);
                    from_json(key_json, key, std::forward<Args>(args)...);
                }

                // Deserialize the value
                mapped_type value;
                from_json(value_json, value, std::forward<Args>(args)...);

                container.emplace(std::move(key), std::move(value));
            }
        } else {
            // For vectors/lists/etc: JSON is an array
            for (const auto& element_json : j) {
                typename Container::value_type element;
                from_json(element_json, element, std::forward<Args>(args)...);
                container.insert(container.end(), std::move(element));
            }
        }
	}

	/* [ Deserialize ] std::array - fixed size, no clear/insert */
    template <typename T, std::size_t N, typename... Args>
    std::enable_if_t<(sizeof...(Args) > 0), void> 
		tag_invoke(deserialize_lib_t, const json& j, std::array<T, N>& container, Args&&... args) {
        for (std::size_t i = 0; i < N && i < j.size(); ++i) {
            from_json(j[i], container[i], std::forward<Args>(args)...);
        }
    }

    /* [ Serialize ] std::array - same as container but explicit */
    template <typename T, std::size_t N, typename... Args>
    std::enable_if_t<(sizeof...(Args) > 0), json> 
		tag_invoke(serialize_lib_t, const std::array<T, N>& container, Args&&... args) {
        json j = json::array();
        for (const auto& element : container) {
            j.push_back(to_json(element, std::forward<Args>(args)...));
        }
        return j;
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
		tag_invoke(serialize_lib_t, const T& value) {
		auto shared_ptr = value.lock();
		if (shared_ptr) {
			return JsonReflect::to_json(*shared_ptr);
		}
		return json(nullptr);
	}

	/* [ Deserialize ] Weak Pointers - Not supported */
	template<typename T>
	std::enable_if_t<Detail::is_weak_pointer_v<T>, void>
		tag_invoke(deserialize_lib_t, const json& j, T& value) {
		//static_assert(std::false_type::value, "JsonReflect Error: Deserialization of weak_ptr is not supported.");
		throw std::runtime_error("JsonReflect Error: Deserialization of weak_ptr is not supported.");
	}

	/* [ Serialize ] std::variant */
	template <typename... Types, typename... Args>
    json tag_invoke(serialize_lib_t, const std::variant<Types...>& value, Args... args) {
		json result;
		result["index"] = value.index();
		std::visit([&result, &args...](const auto& v) {
			result["value"] = JsonReflect::to_json(v, std::forward<Args>(args)...);
			}, value);
		return result;
	}

	/* [ Deserialize ] std::variant */
	template <typename... Types, typename... Args>
    void tag_invoke(deserialize_lib_t, const json& j, std::variant<Types...>& value, Args... args) {
		if (!j.is_object()) {
			throw std::runtime_error("JsonReflect Error: Expected JSON object for std::variant deserialization");
		}

		if (!j.contains("index") || !j.contains("value")) {
			throw std::runtime_error("JsonReflect Error: Missing 'index' or 'value' field in variant JSON");
		}

		std::size_t index = j["index"].get<std::size_t>();

		if (index >= sizeof...(Types)) {
			throw std::runtime_error(
				"JsonReflect Error: Variant index " + std::to_string(index) +
				" out of range (variant has " + std::to_string(sizeof...(Types)) + " alternatives)"
			);
		}

		Detail::deserialize_variant_at_index(index, j["value"], value, std::forward<Args>(args)...);
	}
}
