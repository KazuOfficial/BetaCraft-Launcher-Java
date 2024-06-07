#include "InstanceList.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include "../Constants.hpp"
#include "Instance.hpp"

namespace core {
void InstanceList::Init() {
    std::ifstream instanceListFile(constants::INSTANCE_LIST_FILE_NAME);

    if (instanceListFile.bad()) {
        throw std::runtime_error("Error while reading instance list file");
    }

    if (!instanceListFile.good()) {
        CreateDefaultInstanceListFile();
    } else {
        LoadInstances(instanceListFile);
    }
};

void InstanceList::Add(Instance& instance) {
    // Add to file
    std::ifstream instanceListFile(constants::INSTANCE_LIST_FILE_NAME);

    if (instanceListFile.bad() || !instanceListFile.good()) {
        throw std::runtime_error("Can't add instance to instances file");
    }

    nlohmann::json instanceListJson;
    instanceListFile >> instanceListJson;

    StandaloneInstanceListEntry standaloneEntry{};
    standaloneEntry.id = instance.GetId();
    // standaloneEntry.path = instance.GetPath();
    //
    // instanceListJson["standalone"].push_back(standaloneEntry);

    InstanceList::Init();
}

void InstanceList::Remove(Instance& instance) {
    // Remove from file

    InstanceList::Init();
}

void InstanceList::Add(InstanceGroup& group) {
    // Add to file

    InstanceList::Init();
}

void InstanceList::Remove(InstanceGroup& group) {
    // Remove from file

    InstanceList::Init();
}

void InstanceList::CreateDefaultInstanceListFile() {
    std::ofstream instanceListFile(constants::INSTANCE_LIST_FILE_NAME);

    if (instanceListFile.bad()) {
        throw std::runtime_error("Can't create a default instance list file");
    }

    nlohmann::json emptyInstanceListJson = {
        { "standalone", nlohmann::json::array() },
        { "groups", nlohmann::json::array() }
    };

    instanceListFile << emptyInstanceListJson.dump();
}

void InstanceList::LoadInstances(std::ifstream& instanceListFile) {
    nlohmann::json instanceListJson;
    instanceListFile >> instanceListJson;

    for (auto& standaloneInstance : instanceListJson["standalone"]) {
        std::cout << standaloneInstance << '\n';
    }

    std::cout << '\n';

    for (auto& instanceGroup : instanceListJson["groups"]) {
        std::cout << instanceGroup << '\n';
    }
}
}
