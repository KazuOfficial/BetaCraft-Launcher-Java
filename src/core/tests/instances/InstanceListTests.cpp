#include <gtest/gtest.h>
#include "../../instances/InstanceList.hpp"
#include "../../Constants.hpp"
#include <nlohmann/json.hpp>
#include <vector>

class InstanceListTest : public ::testing::Test {
protected:
    virtual void SetUp() override {
        std::ifstream instanceListFile(core::constants::INSTANCE_LIST_FILE_NAME);
        if (instanceListFile.good()) {
            std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
        }

        core::InstanceList::Init();
    }

    virtual void TearDown() override {
        std::remove(core::constants::INSTANCE_LIST_FILE_NAME);
    }
};

TEST_F(InstanceListTest, Initialize_CreatesDefaultInstancesFile) {
    nlohmann::json expectedInstanceListJson = {
        { "standalone", nlohmann::json::array() },
        { "groups", nlohmann::json::array() }
    };
    std::ifstream instanceListFile(core::constants::INSTANCE_LIST_FILE_NAME);
    std::string actualInstanceListJson((std::istreambuf_iterator<char>(instanceListFile)), (std::istreambuf_iterator<char>()));

    EXPECT_EQ(expectedInstanceListJson.dump(), actualInstanceListJson);
}

TEST_F(InstanceListTest, Initialize_LoadsInstancesFromTheInstancesFile) {
}

TEST_F(InstanceListTest, Add_GivenValidInput_AddsInstanceToInstanceList) {
    // core::Instance instance("Test Instance", "b1.7.3");
    // core::Instance instance2("Test Instance 2", "1.8.9");

    // core::InstanceList::Add(instance);
    // core::InstanceList::Add(instance2);

    // std::vector<core::Instance> standaloneInstances = core::InstanceList::GetStandalone();
    // EXPECT_EQ(standaloneInstances[0].GetId(), instance.GetId());
    // EXPECT_EQ(standaloneInstances[0].GetName(), instance.GetName());
    // EXPECT_EQ(standaloneInstances[0].GetVersionId(), instance.GetVersionId());

    // EXPECT_EQ(standaloneInstances[1].GetId(), instance2.GetId());
    // EXPECT_EQ(standaloneInstances[1].GetName(), instance2.GetName());
    // EXPECT_EQ(standaloneInstances[1].GetVersionId(), instance2.GetVersionId());
}

TEST_F(InstanceListTest, Add_GivenValidInput_AddsInstanceToInstanceListFile) {
}

TEST_F(InstanceListTest, Add_GivenValidInput_AddsGroupToInstanceList) {
}

TEST_F(InstanceListTest, Add_GivenValidInput_AddsGroupToInstanceListFile) {
}

TEST_F(InstanceListTest, Remove_GivenValidInput_RemovesInstanceFromInstanceList) {
}

TEST_F(InstanceListTest, Remove_GivenValidInput_RemovesInstanceFromInstanceListFile) {
}

TEST_F(InstanceListTest, Remove_GivenValidInput_RemovesGroupFromInstanceList) {
}

TEST_F(InstanceListTest, Remove_GivenValidInput_RemovesGroupFromInstanceListFile) {
}

TEST_F(InstanceListTest, GetStandalone_GivenValidInput_FetchesInstancesFromTheInstancesFile) {
}

TEST_F(InstanceListTest, GetGrouped_GivenValidInput_FetchesGroupsFromTheInstancesFile) {

}
