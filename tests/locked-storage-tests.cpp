#include "../src//utilities//locked-storage/locked-storage.hpp"
#include <gtest/gtest.h>

TEST(LockedStorage, GetingAndSetting)
{
    LockedStorage& storage = LockedStorage::GetInstance();
    storage["rua"] = "Rua 5";
    EXPECT_EQ(storage["rua"], "Rua 5");
}
