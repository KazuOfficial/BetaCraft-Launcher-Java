#include <gtest/gtest.h>
#include <string>
#include "../Instances.hpp"

TEST(InstanceConstructor, ValidName) {
    const std::string instanceName = "TooManyItems Instance";

    core::Instance instance(instanceName, "b1.7.3");

    EXPECT_EQ(instance.GetName(), instanceName);
}
