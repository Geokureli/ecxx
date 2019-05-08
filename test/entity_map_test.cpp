#include <ecxx/impl/entity_map.h>
#include <gtest/gtest.h>

using namespace ecxx;

TEST(v2_entity_map, simple) {
    entity_map<uint32_t, int> m;
    m.emplace({1u, 0u}, 1);

    ASSERT_TRUE(m.has({1u, 0u}));
    ASSERT_FALSE(m.has({5u, 0u}));

    m.emplace({5u, 0u}, 1);
    ASSERT_TRUE(m.has({5u, 0u}));
}
