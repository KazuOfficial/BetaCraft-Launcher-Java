#include "Instance.hpp"
#include "../Utils.hpp"
#include "../Constants.hpp"
#include <stdexcept>

namespace core {
Instance::Instance(const std::string& name, const std::string& versionId) : id(Utils::GenerateUUID()), name(name), versionId(versionId) {
    ValidateInstanceName(name);
}

void Instance::ChangeName(const std::string& name) {

}

void Instance::Delete() {

}


bool Instance::ValidateInstanceName(const std::string& name) {
    if (name.empty()) {
        throw std::runtime_error("Instance name cannot be empty");
    }

    if (name.length() > constants::MAX_INSTANCE_NAME_LEN) {
        throw std::runtime_error("Instance name is too long");
    }

    return true;
}
}
