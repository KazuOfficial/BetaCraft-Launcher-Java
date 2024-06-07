#pragma once

#include "Instance.hpp"
#include <string>
#include <vector>

namespace core {
class InstanceGroup {
public:
    InstanceGroup(const std::string& name);

    void AddInstance(const core::Instance& instance);
    void RemoveInstance(const core::Instance& instance);
    void ChangeName(const std::string& name);
    void Delete();

    std::string GetId() { return id; };
    std::string GetName() { return name; };
    std::vector<core::Instance> GetInstances() { return instances; };
private:
    const std::string id;
    std::string name;
    std::vector<core::Instance> instances;
};
}
