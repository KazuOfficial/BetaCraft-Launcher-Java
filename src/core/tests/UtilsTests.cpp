#include <gtest/gtest.h>
#include "../Utils.hpp"

TEST(Utils, GenerateAUniqueUUID) {
    std::string uuid1 = core::Utils::GenerateUUID();
    std::string uuid2 = core::Utils::GenerateUUID();

    EXPECT_NE(uuid1, uuid2);
}
