#include <ecxx/impl/entity_value.h>
#include <gtest/gtest.h>

using namespace ecxx;

TEST(entity_value, basic) {
    using entity_t = entity_value<uint32_t>;

    ASSERT_EQ(entity_t::null.index(), 0u);
    ASSERT_EQ(entity_t::null.version(), 0u);

    entity_t e{1, 1};
    ASSERT_EQ(e.index(), 1u);
    ASSERT_EQ(e.version(), 1u);

    e.index(23);
    e.version(99);
    ASSERT_EQ(e.index(), 23u);
    ASSERT_EQ(e.version(), 99u);
}