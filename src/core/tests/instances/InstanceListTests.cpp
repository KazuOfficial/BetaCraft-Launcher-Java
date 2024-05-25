#include <gtest/gtest.h>
#include "../../instances/InstanceList.hpp"

TEST(InstanceList, InitCreatesDefaultInstancesFile) {
    core::InstanceList::Init();

    std::ifstream f(core::constants::INSTANCE_LIST_FILE_NAME);

    ASSERT_TRUE(f.good());
    
    std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
}

TEST(InstanceList, GetStandalone) {
    core::InstanceList::Init();
    std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
}

TEST(InstanceList, GetGrouped) {
    core::InstanceList::Init();
    std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
}
