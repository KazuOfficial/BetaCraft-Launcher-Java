#include <gtest/gtest.h>
#include "../../instances/InstanceList.hpp"

class InstanceListTest : public ::testing::Test {
protected:
    virtual void SetUp() override {
        core::InstanceList::Init();
    }

    virtual void TearDown() override {
        std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
    }
};

TEST_F(InstanceListTest, Initialize_WhenCalled_CreatesDefaultInstancesFile) {
    std::ifstream f(core::constants::INSTANCE_LIST_FILE_NAME);

    ASSERT_TRUE(f.good());
}

TEST_F(InstanceListTest, GetStandalone_WhenCalled_FetchesInstancesFromTheInstancesFile) {
}

TEST_F(InstanceListTest, GetGrouped_WhenCalled_FetchesGroupsFromTheInstancesFile) {

}
