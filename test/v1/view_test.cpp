#include <ecxx/v1/ecxx1.h>
#include <gtest/gtest.h>
#include "../common/components.h"

using namespace ecxx;

int count(world_v1_t& world) {
    int count = 0;
    for (auto e : world.view<value_t, position_t, motion_t>()) {
        ++count;
    }
    return count;
}

TEST(view_test, templated) {
    world_v1_t world{0xFFFFu};

    world.make_component_type<value_t>();
    world.make_component_type<position_t>();
    world.make_component_type<motion_t>();

    ASSERT_EQ(count(world), 0);

    auto e = world.create();
    e.set<position_t>();
    e.set<value_t>();
    e.set<motion_t>();

    ASSERT_EQ(count(world), 1);

    e.unset<motion_t>();

    ASSERT_EQ(count(world), 0);

    e.destroy();

    ASSERT_EQ(count(world), 0);
}
