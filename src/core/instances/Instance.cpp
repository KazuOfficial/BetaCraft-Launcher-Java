#include "Instance.hpp"
#include "../Utils.hpp"

namespace core {
Instance::Instance(const std::string& name, const std::string& versionId) : id(Utils::GenerateUUID()), name(name), versionId(versionId) { }

void Instance::ChangeName(const std::string& name) {

}

void Instance::Delete() {

}
}
