#include <ecxx/impl/entity_pool.h>
#include <gtest/gtest.h>

using namespace ecxx;

using entity_allocator = basic_entity_pool<>;

uint32_t count_entities(entity_allocator& pool) {
    uint32_t count = 0u;
    pool.each([&count](auto) {
        ++count;
    });
    return count;
}

TEST(v2_entity_allocator, allocate) {
    entity_allocator allocator;

    std::vector<entity_allocator::value_type> active;
    active.push_back(allocator.allocate()); // 1
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    for (int i = 0; i < active.size(); ++i) {
        ASSERT_EQ(active[i].index(), i + 1);
    }

    ASSERT_EQ(count_entities(allocator), active.size());
    ASSERT_EQ(allocator.size(), active.size());
    ASSERT_EQ(allocator.available_for_recycling(), 0u);
}

TEST(v2_entity_allocator, deallocate) {
    entity_allocator allocator;

    std::vector<entity_allocator::value_type> active;
    active.push_back(allocator.allocate()); // 1
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    for (auto i : active) {
        allocator.deallocate(i);
    }
    ASSERT_EQ(allocator.available_for_recycling(), active.size());
    ASSERT_EQ(allocator.size(), 0);
}

TEST(v2_entity_allocator, recycle) {
    entity_allocator allocator;

    std::vector<entity_allocator::value_type> active;
    active.push_back(allocator.allocate()); // 1
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());

    allocator.deallocate(active[0]);
    allocator.deallocate(active[1]);
    allocator.deallocate(active[2]);

    auto version_before = active[0].version();

    ASSERT_EQ(allocator.available_for_recycling(), 3);
    ASSERT_EQ(count_entities(allocator), 2);
    ASSERT_EQ(allocator.size(), 2);

    active[0] = allocator.allocate();
    active[1] = allocator.allocate();
    active[2] = allocator.allocate();

    ASSERT_EQ(allocator.available_for_recycling(), 0);
    ASSERT_EQ(count_entities(allocator), 5);
    ASSERT_EQ(allocator.size(), 5);

    ASSERT_EQ(active[0].index(), 3);
    ASSERT_EQ(active[1].index(), 2);
    ASSERT_EQ(active[2].index(), 1);

    ASSERT_EQ(allocator.current(active[2].index()), active[2].version());
    ASSERT_NE(allocator.current(active[2].index()), version_before);
    ASSERT_NE(active[2].version(), version_before);

    active.push_back(allocator.allocate()); // 6
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());
    active.push_back(allocator.allocate());



    for (int i = 5; i < active.size(); ++i) {
        ASSERT_EQ(active[i].index(), i + 1);
    }
}