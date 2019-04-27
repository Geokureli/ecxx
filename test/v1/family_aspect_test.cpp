#include <ecxx/v1/ecxx1.h>
#include <gtest/gtest.h>
#include "../common/components.h"

using namespace ecxx;

TEST(family_aspect_test, managed) {
    world_v1_t world{0xFFFFu};

    world.make_component_type<value_t>();
    world.make_component_type<position_t>();
    world.make_component_type<motion_t>();

    family_aspect_data aspect{world.components()};
    aspect.require<position_t>().require<motion_t>().exclude<value_t>();

    auto e = world.create();
    e.set<position_t>();
    ASSERT_FALSE(aspect(e.id()));
    e.destroy();

    e = world.create();
    e.set<position_t>();
    e.set<motion_t>();
    ASSERT_TRUE(aspect(e.id()));
    e.destroy();

    e = world.create();
    e.set<position_t>();
    e.set<motion_t>();
    e.set<value_t>();
    ASSERT_FALSE(aspect(e.id()));
    e.destroy();
}

TEST(family_aspect_test, templated) {
    world_v1_t world{0xFFFFu};

    world.make_component_type<value_t>();
    world.make_component_type<position_t>();
    world.make_component_type<motion_t>();

    family_aspect<position_t, motion_t> aspect{world.components()};

    auto e = world.create();
    e.set<position_t>();
    ASSERT_FALSE(aspect(e.id()));
    e.destroy();

    e = world.create();
    e.set<position_t>();
    e.set<motion_t>();
    ASSERT_TRUE(aspect(e.id()));
    e.destroy();

    // exclude case
//    e = world.create();
//    e.set<position_t>();
//    e.set<motion_t>();
//    e.set<value_t>();
//    ASSERT_FALSE(aspect(e.id()));
//    e.destroy();
}