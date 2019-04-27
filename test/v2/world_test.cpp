#include <ecxx/v2/world.h>
#include <gtest/gtest.h>

using namespace ecxx;

TEST(world, basic) {
    // TODO:
    world_t w;
    auto e = w.create();
    w.destroy(e);
}
