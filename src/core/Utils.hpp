#pragma once

#include <string>
#include <uuid/uuid.h>

namespace core {
class Utils {
public:
    static std::string GenerateUUID() {
        char uuid[37];

        uuid_t binuuid;
        uuid_generate_random(binuuid);
        uuid_unparse_lower(binuuid, uuid);

        return std::string(uuid);
    }
};
}
