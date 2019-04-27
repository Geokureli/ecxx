#include <ecxx/v1/ecxx1.h>
#include <gtest/gtest.h>

using namespace ecxx;

TEST(query, active) {
    world_v1_t world{0xFFFFu};

    int n = 3;
    for (int i = 0; i < n; ++i) {
        world.create();
    }

    int count = 0;
    for (auto i : world.active_entities()) {
        ++count;
    }

    ASSERT_EQ(count, n);
}