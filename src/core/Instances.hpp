#pragma once

#include <string>

namespace core {
class Instance {
public:
    Instance(const std::string& name, const std::string& versionId);

    std::string GetId() { return id; };
    std::string GetName() { return name; };
    std::string GetVersionId() { return versionId; };
private:
    const std::string id;
    std::string name;
    std::string versionId;
};
}
