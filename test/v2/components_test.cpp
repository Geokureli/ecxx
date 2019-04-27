#include <ecxx/v2/ecxx2.h>

#include <gtest/gtest.h>
#include "../common/components.h"

using namespace ecxx;

TEST(components, add) {
    world_t world;

    auto e = world.create();
    auto& v = world.assign<value_t>(e);
    ASSERT_EQ(v.value, 0);

    v.value = 10;
    ASSERT_EQ(10, v.value);
    ASSERT_EQ((world.get<value_t>(e).value), 10);

    // const access
    const auto& cw = world;
    ASSERT_EQ((cw.get<value_t>(e).value), 10);
}

TEST(components, remove) {
    world_t world;
    auto e = world.create();
    world.assign<value_t>(e, 1);
    ASSERT_EQ(world.get<value_t>(e).value, 1);
    ASSERT_TRUE(world.has<value_t>(e));
    world.remove<value_t>(e);
    ASSERT_FALSE(world.has<value_t>(e));
    world.destroy(e);
}

TEST(entity_wrapper, components_add) {
    world_t world;

    auto e = world.create_wrapper();
    auto& v = e.set<value_t>();
    ASSERT_EQ(v.value, 0);

    v.value = 10;
    ASSERT_EQ(10, v.value);
    ASSERT_EQ((e.get<value_t>().value), 10);

    // const access
    const auto ec = e;
    ASSERT_EQ((ec.get<value_t>().value), 10);
}

TEST(entity_wrapper, components_remove) {
    world_t world;

    auto e = world.create_wrapper();
    e.set<value_t>(1);
    ASSERT_EQ(e.get<value_t>().value, 1);
    ASSERT_TRUE(e.has<value_t>());

    e.unset<value_t>();
    ASSERT_FALSE(e.has<value_t>());

    // todo: get should abort
    e.destroy();
}