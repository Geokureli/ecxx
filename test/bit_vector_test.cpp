#include <ecxx/impl/bit_vector.h>

#include <gtest/gtest.h>

using namespace ecxx;

TEST(bit_vector_test, basic) {
    bit_vector vec{100};

    ASSERT_TRUE(vec.data() != nullptr);
    ASSERT_TRUE(vec.size() == 100);

    ASSERT_EQ(vec[33], false);
    ASSERT_EQ(vec[34], false);
    vec[91] = true;
    ASSERT_EQ(vec[91], true);

    ASSERT_EQ(vec.get(94), false);
    vec.set(94, true);
    ASSERT_EQ(vec.get(94), true);

    ASSERT_EQ(vec[99], false);
    vec.enable(99);
    ASSERT_EQ(vec[99], true);
    ASSERT_EQ(vec.is_false(99), false);
    vec.disable(99);
    ASSERT_EQ(vec[99], false);
    ASSERT_EQ(vec.is_false(99), true);

    vec.enable_if_not(99);
    ASSERT_EQ(vec[99], true);
    vec.enable_if_not(99);
    ASSERT_EQ(vec[99], true);
}
