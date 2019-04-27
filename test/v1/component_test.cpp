#include <ecxx/v1/ecxx1.h>
#include <gtest/gtest.h>
#include "../common/components.h"

using namespace ecxx;

TEST(components_v1, test) {
    world_v1_t world{0xFFFFu};
    world.make_component_type<value_t>();

    auto e = world.create();
    auto& v = e.set<value_t>();
    ASSERT_EQ(v.value, 0);

    v.value = 10;
    ASSERT_EQ(10, v.value);
    ASSERT_EQ((e.get<value_t>().value), 10);
}

TEST(components_v1, remove) {
    world_v1_t world{0xFFFFu};
    world.make_component_type<value_t>();
    auto& component_manager = world.components().manager<value_t>();

    auto e = world.create();
    e.set<value_t>(1);
    ASSERT_EQ(e.get<value_t>().value, 1);
    ASSERT_TRUE(e.has<value_t>());
    ASSERT_TRUE(component_manager.has(e.id()));

    e.unset<value_t>();
    ASSERT_FALSE(e.has<value_t>());
    ASSERT_FALSE(component_manager.has(e.id()));

    // todo: get should abort

    world.destroy(e);
}
