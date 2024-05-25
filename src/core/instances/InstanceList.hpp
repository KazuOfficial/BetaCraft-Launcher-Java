#pragma once

#include "InstanceGroup.hpp"
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "../Constants.hpp"

namespace core {
class InstanceList {
public:
    static void Init() {
        std::ifstream f(constants::INSTANCE_LIST_FILE_NAME);

        if (!f.good()) {
            std::ofstream{constants::INSTANCE_LIST_FILE_NAME};
        }
    };
    static std::vector<Instance> GetStandalone() { return standalone; };
    static std::vector<InstanceGroup> GetGroups() { return groups; };

private:
    static std::vector<Instance> standalone;
    static std::vector<InstanceGroup> groups;
};
}
