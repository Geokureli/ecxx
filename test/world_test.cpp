#include <ecxx/impl/world.h>
#include <gtest/gtest.h>

using namespace ecxx;

TEST(world, basic) {
    // TODO:
    world_t w;
    auto e = w.create();
    w.destroy(e);
}
