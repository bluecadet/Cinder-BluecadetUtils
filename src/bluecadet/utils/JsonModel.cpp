#include "JsonModel.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

JsonModel::JsonModel(const ci::JsonTree & json) {
	merge(json, "nid", &mId);
}

JsonModel::~JsonModel() {}

bool JsonModel::contains(const ci::JsonTree & json, const std::string & key, const ci::JsonTree::NodeType type) {
	try {
		if (!json.hasChild(key)) {
			return false;
		}

		const auto childType = json.getChild(key).getNodeType();

		if (type != JsonTree::NODE_NULL && childType == JsonTree::NODE_NULL) {
			return false;
		}

		return childType == type;

	} catch (const std::exception & e) {
		CI_LOG_EXCEPTION("Could not check if json contains '" << key << "'", e);
		return false;
	}
}

std::ostream& operator<<(std::ostream& os, JsonModel * model) {
	os << "[JsonModel id: " << toString(model->mId) << "]";
	return os;
}

} // namespace utils
} // namespace bluecadet
