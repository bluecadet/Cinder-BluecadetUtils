#pragma once

#include "cinder/Json.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class JsonModel> JsonModelRef;

class JsonModel {

public:
	JsonModel(const ci::JsonTree & json);
	virtual ~JsonModel();

	int mId = -1;

protected:
	// This templated struct is required to have templated std::functions that
	// we can reuse for parameters below.
	template <typename T> struct Typedefs {
		typedef std::function<std::shared_ptr<T>(const ci::JsonTree & json)>
			Factory;
	};

	// The default method used to create objects in merge() calls.
	template <typename T>
	static std::shared_ptr<T> defaultFactory(const ci::JsonTree & json);


	// Merges in a JSON value if it exists.
	// An exception is thrown if required is true and the key can't be found or
	// parsed.
	//
	// *ptr = json[key]
	template <typename T>
	bool merge(const ci::JsonTree & json, const std::string & key, T * ptr,
			   bool required = false);

	// Uses a factory method that accepts the JSON value and returns an
	// shared_ptr of type T.
	//
	// *ptr = factory(json[key])
	template <typename T>
	bool merge(
		const ci::JsonTree & json, const std::string & key,
		std::shared_ptr<T> * ptr, bool required = false,
		typename Typedefs<T>::Factory factory = &JsonModel::defaultFactory<T>);

	// Uses a factory method that accepts the JSON value and returns a
	// shared_ptr of type T, and then stores the result in a vector.
	//
	// ptr->push_back(factory(json[key]))
	template <typename T>
	bool merge(
		const ci::JsonTree & json, const std::string & key,
		std::vector<std::shared_ptr<T>> * ptr, bool required = false,
		typename Typedefs<T>::Factory factory = &JsonModel::defaultFactory<T>);

	// Uses a converstion map to map the value of json[key] to any arbitrary
	// type (e.g. string -> enum).
	//
	// *ptr = map[json[key]]
	template <typename KeyType, typename ValueType>
	bool merge(const ci::JsonTree & json, const std::string & key,
			   ValueType * ptr, bool required = false,
			   const std::map<KeyType, ValueType> & itemsByKey = {});

	// Uses a converstion map to map the value of json[key] to any arbitrary
	// type (e.g. string -> enum) and then stores the result in a vector.
	//
	// ptr->push_back(map[json[key]])
	template <typename KeyType, typename ValueType>
	bool merge(const ci::JsonTree & json, const std::string & key,
			   std::vector<ValueType> * ptr, bool required = false,
			   const std::map<KeyType, ValueType> & itemsByKey = {});


	// Safely checks if a key exists with the correct type without throwing an
	// exception.
	bool contains(
		const ci::JsonTree & json, const std::string & key,
		const ci::JsonTree::NodeType type = ci::JsonTree::NodeType::NODE_VALUE);


	// Throws an exception if size of items is not equal to the required size.
	template <typename T>
	void assertSize(const std::vector<T> & items, int size);
	// Throws an exception if size of items is smaller than the required min.
	template <typename T> void assertMin(const std::vector<T> & items, int min);
	// Throws an exception if size of items is larger than the required max.
	template <typename T> void assertMax(const std::vector<T> & items, int max);
	// Throws an exception if size of items is smaller than the required min or
	// larger than the required max.
	template <typename T>
	void assertRange(const std::vector<T> & items, int min, int max);

	friend std::ostream & operator<<(std::ostream & os, JsonModel * model);
};

template <typename T>
inline std::shared_ptr<T> JsonModel::defaultFactory(const ci::JsonTree & json) {
	return std::make_shared<T>(json);
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 T * ptr, bool required) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_VALUE)) {
			*ptr = json.getValueForKey<T>(key);
			return true;

		} else {
			CI_LOG_D("Value for '"
					 << key << "' not found or not set correctly in json.");
		}

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key to value '" << key << "'",
						 e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

// Specialization that's required to allow for wide string support with
// ci::JsonTree::getValueForKey
template <>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 std::wstring * ptr, bool required) {
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>
		wStringConverter;

	try {
		if (contains(json, key, ci::JsonTree::NODE_VALUE)) {
			const std::string narrowStr = json.getValueForKey<std::string>(key);
			if (!narrowStr.empty()) {
				*ptr = wStringConverter.from_bytes(narrowStr);
				return true;
			}
		}

		CI_LOG_D("Value for '" << key
							   << "' not found or not set correctly in json.");

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key to value '" << key << "'",
						 e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 ValueType * ptr, bool required,
							 const std::map<KeyType, ValueType> & itemsByKey) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_VALUE)) {
			auto jsonValue = json.getValueForKey<KeyType>(key);
			auto it = itemsByKey.find(jsonValue);

			if (it == itemsByKey.end()) {
				throw ci::Exception("No value conversion found for key '" +
									ci::toString(key) + "'");
			}

			*ptr = it->second;
			return true;

		} else {
			CI_LOG_D("Value for '"
					 << key << "' not found or not set correctly in json.");
		}

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Exception when merging key to value '" << key << "'",
						 e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename KeyType, typename ValueType>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 std::vector<ValueType> * ptr, bool required,
							 const std::map<KeyType, ValueType> & itemsByKey) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					auto itemKey = child.getValue<KeyType>();
					auto it = itemsByKey.find(itemKey);
					if (it == itemsByKey.end()) {
						CI_LOG_D("Could not find item for key '"
								 << ci::toString(key) << "'");
						continue;
					}
					ptr->push_back(it->second);
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;

		} else {
			CI_LOG_D("Array for key '"
					 << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION(
			"Exception when merging key with item map '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 std::shared_ptr<T> * ptr, bool required,
							 typename Typedefs<T>::Factory factory) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_OBJECT)) {
			*ptr = factory(json.getChild(key));
			return true;

		} else {
			CI_LOG_D("Object for key '"
					 << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION(
			"Exception when merging key with factory '" << key << "'", e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename T>
inline bool JsonModel::merge(const ci::JsonTree & json, const std::string & key,
							 std::vector<std::shared_ptr<T>> * ptr,
							 bool required,
							 typename Typedefs<T>::Factory factory) {
	try {
		if (contains(json, key, ci::JsonTree::NODE_ARRAY)) {
			for (const auto & child : json.getChild(key).getChildren()) {
				try {
					auto item = factory(child);
					ptr->push_back(item);
				} catch (const std::exception & e) {
					CI_LOG_EXCEPTION("Could not create item", e);
				}
			}
			return true;
		} else if (contains(json, key, ci::JsonTree::NODE_OBJECT) ||
				   contains(json, key, ci::JsonTree::NODE_VALUE)) {
			try {
				auto item = factory(json.getChild(key));
				ptr->push_back(item);
				return true;
			} catch (const std::exception & e) {
				CI_LOG_EXCEPTION("Could not create item", e);
			}

		} else {
			CI_LOG_D("Array for key '"
					 << key << "' not found or not set correctly in json.");
		}
	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION(
			"Exception when merging key with factory to list '" << key << "'",
			e);
	}
	if (required) {
		throw ci::Exception("Could not merge required key '" + key + "'");
	}
	return false;
}

template <typename T>
inline void JsonModel::assertSize(const std::vector<T> & items, int size) {
	if (items.size() != size) {
		throw ci::Exception(
			"Items is out of range: " + ci::toString(items.size()) +
			" found, but at exactly " + ci::toString(size) + " are required");
	}
}

template <typename T>
inline void JsonModel::assertMin(const std::vector<T> & items, int min) {
	if (items.size() < min) {
		throw ci::Exception(
			"Not enough items found: " + ci::toString(items.size()) +
			", but at least " + ci::toString(min) + " are required");
	}
}

template <typename T>
inline void JsonModel::assertMax(const std::vector<T> & items, int max) {
	if (items.size() > max) {
		throw ci::Exception(
			"Too many items found: " + ci::toString(items.size()) +
			", but only " + ci::toString(max) + " are allowed");
	}
}

template <typename T>
inline void JsonModel::assertRange(const std::vector<T> & items, int min,
								   int max) {
	assertMin(items, min);
	assertMax(items, max);
}

} // namespace utils
} // namespace bluecadet
