#pragma once

#include "Instance.hpp"
#include "InstanceGroup.hpp"
#include <fstream>
#include <string>
#include <vector>

namespace core {
class InstanceList {
public:
    static void Init();

    static void Add(Instance& instance);
    static void Add(InstanceGroup& group);
    static void Remove(Instance& instance);
    static void Remove(InstanceGroup& group);

    static std::vector<Instance> GetStandalone() { return standalone; };
    static std::vector<InstanceGroup> GetGroups() { return groups; };

private:
    struct StandaloneInstanceListEntry {
        std::string id;
        std::string path;
    };

    struct GroupInstanceListEntry {
        std::string id;
        std::string name;
        std::vector<StandaloneInstanceListEntry> instances;
    };

    static void CreateDefaultInstanceListFile();
    static void LoadInstances(std::ifstream& instanceListFile);

    static std::vector<Instance> standalone;
    static std::vector<InstanceGroup> groups;
};
}
