#include "InstanceGroup.hpp"
#include "../Utils.hpp"

namespace core {
InstanceGroup::InstanceGroup(const std::string& name) : id(Utils::GenerateUUID()), name(name) {

}

void InstanceGroup::AddInstance(const core::Instance& instance) {

}

void InstanceGroup::RemoveInstance(const core::Instance& instance) {

}

void InstanceGroup::ChangeName(const std::string& name) {
}

void InstanceGroup::Delete() {

}
}
