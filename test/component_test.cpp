#include <ecxx/ecxx.h>
#include <gtest/gtest.h>
#include "components.h"

using namespace ecxx;

TEST(components, test) {
    world_t world{0xFFFFu};
    world.make_component_type<value_t>();

    auto e = world.create();
    auto& v = e.set<value_t>();
    ASSERT_EQ(v.value, 0);

    v.value = 10;
    ASSERT_EQ(10, v.value);
    ASSERT_EQ((e.get<value_t>().value), 10);
}
