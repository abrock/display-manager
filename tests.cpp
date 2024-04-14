#include <glog/logging.h>
#include <gtest/gtest.h>

#include "misc.h"

TEST(Misc, prune_fn) {
    ASSERT_EQ("", Misc::prune_fn("file://"));
    ASSERT_EQ("blah", Misc::prune_fn("blah"));
    ASSERT_EQ("blub", Misc::prune_fn("file://blub"));
}

TEST(Misc, trim) {
    ASSERT_EQ("", Misc::trim(""));
    ASSERT_EQ("asdf", Misc::trim("asdf"));
    ASSERT_EQ("asdf", Misc::trim("asdf\f\r\n\t   "));
    ASSERT_EQ("as  df", Misc::trim("\t   as  df\f\r\n\t   "));
}

int main(int argc, char** argv) {
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    testing::InitGoogleTest(&argc, argv);
    int const return_val = RUN_ALL_TESTS();
    std::cout << "RUN_ALL_TESTS return value: " << return_val << std::endl;
    return return_val;
}
