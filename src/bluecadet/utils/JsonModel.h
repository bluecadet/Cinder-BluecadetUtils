#pragma once

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <Windows.h>


namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class JsonModel> JsonModelRef;

class JsonModel {

public:
	JsonModel();
	virtual ~JsonModel();

protected:
	// This templated struct is required to have templated std::functions that
	// we can reuse for parameters below.
	template <typename T> struct Typedefs {
		typedef std::function<T(const ci::JsonTree & json)> ValueFactory;
		typedef std::function<std::shared_ptr<T>(const ci::JsonTree & json)> SharedPtrFactory;
	};


	// The default method used to create values in merge() calls.
	template <typename T> static T defaultValueFactory(const ci::JsonTree & json);


	// The default method used to create shared pointers in merge() calls.
	template <typename T> static std::shared_ptr<T> defaultSharedPtrFactory(const ci::JsonTree & json);


	// Merges in a JSON value if it exists.
	// An exception is thrown if required is true and the key can't be found or
	// parsed.
	//
	// *ptr = json[key]
	template <typename T>
	bool merge(const ci::JsonTree & json, const std::string & key, T * ptr, bool required = false,
			   typename Typedefs<T>::ValueFactory factory = &JsonModel::defaultValueFactory<T>);

	// Uses a factory method that accepts the JSON value and returns an
	// shared_ptr of type T.
	//
	// *ptr = factory(json[key])
	template <typename T>
	bool merge(const ci::JsonTree & json, const std::string & key, std::shared_ptr<T> * ptr, bool required = false,
			   typename Typedefs<T>::SharedPtrFactory factory = &JsonModel::defaultSharedPtrFactory<T>);

	// Uses a factory method that accepts the JSON value and returns a
	// value of type T, and then stores the result in a vector.
	//
	// ptr->push_back(factory(json[key]))
	template <typename T>
	bool merge(const ci::JsonTree & json, const std::string & key, std::vector<T> * ptr, bool required = false,
			   typename Typedefs<T>::ValueFactory factory = &JsonModel::defaultValueFactory<T>);

	// Uses a factory method that accepts the JSON value and returns a
	// shared_ptr of type T, and then stores the result in a vector.
	//
	// ptr->push_back(factory(json[key]))
	template <typename T>
	bool merge(const ci::JsonTree & json, const std::string & key, std::vector<std::shared_ptr<T>> * ptr,
			   bool required									  = false,
			   typename Typedefs<T>::SharedPtrFactory factory = &JsonModel::defaultSharedPtrFactory<T>);

	// Uses a conversion map to map the value of json[key] to any arbitrary
	// type (e.g. string -> enum).
	//
	// *ptr = map[json[key]]
	template <typename KeyType, typename ValueType>
	bool merge(const ci::JsonTree & json, const std::string & key, ValueType * ptr, bool required = false,
			   const std::map<KeyType, ValueType> & itemsByKey = {});

	// Uses a conversion map to map the value of json[key] to any arbitrary
	// type (e.g. string -> enum) and then stores the result in a vector.
	//
	// ptr->push_back(map[json[key]])
	template <typename KeyType, typename ValueType>
	bool merge(const ci::JsonTree & json, const std::string & key, std::vector<ValueType> * ptr, bool required = false,
			   const std::map<KeyType, ValueType> & itemsByKey = {});

	// Uses a conversion map to map the value of json[key] to any arbitrary
	// type (e.g. string -> enum) and then stores the result in a set.
	//
	// ptr->insert(map[json[key]])
	template <typename KeyType, typename ValueType>
	bool merge(const ci::JsonTree & json, const std::string & key, std::set<ValueType> * ptr, bool required = false,
			   const std::map<KeyType, ValueType> & itemsByKey = {});

	// Safely checks if a key exists and is not null without throwing an exception.
	bool contains(const ci::JsonTree & json, const std::string & key);

	// Safely checks if a key exists with the correct type without throwing an
	// exception.
	bool contains(const ci::JsonTree & json, const std::string & key,
				  const ci::JsonTree::NodeType type);

	// Throws an exception if size of items is not equal to the required size.
	// template <typename ContainerType>
	template <typename ContainerType> void assertSize(const ContainerType & items, int size);

	// Throws an exception if size of items is smaller than the required min.
	template <typename ContainerType> void assertMin(const ContainerType & items, int min);

	//// Throws an exception if size of items is larger than the required max.
	template <typename ContainerType> void assertMax(const ContainerType & items, int max);

	//// Throws an exception if size of items is smaller than the required min
	/// or / larger than the required max.
	template <typename ContainerType> void assertRange(const ContainerType & items, int min, int max);

	friend std::ostream & operator<<(std::ostream & os, JsonModel * model);
};

// String to wide string utlity function to support default factory function conversion to wide string
std::wstring string_to_wide_string(const std::string& string)
{
	if (string.empty()) { return L""; }

	const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), nullptr, 0);
	if (size_needed <= 0)
	{
		throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
	}

	std::wstring result(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &string.at(0), (int)string.size(), &result.at(0), size_needed);
	return result;
}



template <typename T> inline T JsonModel::defaultValueFactory(const ci::JsonTree & json) { return json.getValue<T>(); }

// Specialization that's required to allow for wide string support
template <> inline std::wstring JsonModel::defaultValueFactory(const ci::JsonTree & json) {
	const std::string narrowStr =  json.getValue<std::string>();
	if (!narrowStr.empty()) {
		return string_to_wide_string(narrowStr);
	}
	return std::wstring();
}


template <typename T> inline std::shared_ptr<T> JsonModel::defaultSharedPtrFactory(const ci::JsonTree & json) {
	return std::make_shared<T>(json);
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, T * ptr, bool required, typename Typedefs<T>::ValueFactory factory) {
	try {
		if (contains(json, key)) {
			*ptr = factory(json.getChild(key));
			return true;

		} else {
			CI_LOG_V("Value for '" << key << "' not found or not set correctly in json.");
		}

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key to value '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, ValueType * ptr, bool required,
							 const std::map<KeyType, ValueType> & itemsByKey) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_VALUE)) {
			auto jsonValue = json.getValueForKey<KeyType>(key);
			auto it		   = itemsByKey.find(jsonValue);

			if (it == itemsByKey.end()) {
				throw ci::Exception("No value conversion found for key '" + ci::toString(key) + "'");
			}

			*ptr = it->second;
			return true;

		} else {
			CI_LOG_V("Value for '" << key << "' not found or not set correctly in json.");
		}

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key to value '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, std::vector<ValueType> * ptr,
							 bool required, const std::map<KeyType, ValueType> & itemsByKey) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					auto itemKey = child.getValue<KeyType>();
					auto it		 = itemsByKey.find(itemKey);
					if (it == itemsByKey.end()) {
						CI_LOG_V("Could not find item for key '" << ci::toString(key) << "'");
						continue;
					}
					ptr->push_back(it->second);
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;

		} else {
			CI_LOG_V("Array for key '" << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key with item map '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, std::set<ValueType> * ptr,
							 bool required, const std::map<KeyType, ValueType> & itemsByKey) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					auto itemKey = child.getValue<KeyType>();
					auto it		 = itemsByKey.find(itemKey);
					if (it == itemsByKey.end()) {
						CI_LOG_V("Could not find item for key '" << ci::toString(key) << "'");
						continue;
					}
					ptr->insert(it->second);
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;

		} else {
			CI_LOG_V("Array for key '" << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key with item map '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, std::shared_ptr<T> * ptr,
							 bool required, typename Typedefs<T>::SharedPtrFactory factory) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_OBJECT)) {
			*ptr = factory(json.getChild(key));
			return true;

		} else {
			CI_LOG_V("Object for key '" << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key with factory '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template<typename T>
inline bool JsonModel::merge(const ci::JsonTree& json, const std::string& key, std::vector<T>* ptr, bool required, typename Typedefs<T>::ValueFactory factory) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					ptr->push_back(std::move(factory(child)));
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;
		} else if (contains(json, key, ci::JsonTree::NODE_OBJECT) || contains(json, key, ci::JsonTree::NODE_VALUE)) {
			try {
				ptr->push_back(std::move(factory(json.getChild(key))));
				return true;
			} catch (const std::exception & e) {
				CI_LOG_EXCEPTION("Could not create item", e);
			}

		} else {
			CI_LOG_V("Array for key '" << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key with factory to list '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key, std::vector<std::shared_ptr<T>> * ptr,
							 bool required, typename Typedefs<T>::SharedPtrFactory factory) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					ptr->push_back(factory(child));
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;
		} else if (contains(json, key, ci::JsonTree::NODE_OBJECT) || contains(json, key, ci::JsonTree::NODE_VALUE)) {
			try {
				ptr->push_back(factory(json.getChild(key)));
				return true;
			} catch (const std::exception & e) {
				CI_LOG_EXCEPTION("Could not create item", e);
			}

		} else {
			CI_LOG_V("Array for key '" << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key with factory to list '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename ContainerType> inline void JsonModel::assertSize(const ContainerType & items, int size) {
	if (items.size() != size) {
		throw ci::Exception("Items is out of range: " + ci::toString(items.size()) + " found, but at exactly " +
							ci::toString(size) + " are required");
	}
}

template <typename ContainerType> inline void JsonModel::assertMin(const ContainerType & items, int min) {
	if (items.size() < min) {
		throw ci::Exception("Not enough items found: " + ci::toString(items.size()) + ", but at least " +
							ci::toString(min) + " are required");
	}
}

template <typename ContainerType> inline void JsonModel::assertMax(const ContainerType & items, int max) {
	if (items.size() > max) {
		throw ci::Exception("Too many items found: " + ci::toString(items.size()) + ", but only " + ci::toString(max) +
							" are allowed");
	}
}

template <typename ContainerType> inline void JsonModel::assertRange(const ContainerType & items, int min, int max) {
	assertMin(items, min);
	assertMax(items, max);
}

}  // namespace utils
}  // namespace bluecadet
