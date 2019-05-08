#include <ecxx/ecxx.h>
#include <gtest/gtest.h>
#include "common/components.h"

using namespace ecxx;

TEST(view, each) {
    world_t w;
    w.create<position_t, motion_t>();
    int i = 0;
    w.view<position_t, motion_t>().each([&i](auto& pos, auto& mot) {
        ++i;
    });
    ASSERT_EQ(i, 1);
}

TEST(view, min_to_max) {
    world_t w;
    uint32_t values_count = 0u;
    for (uint32_t i = 0; i < 100; ++i) {
        auto e = w.create();
        w.assign<position_t>(e);
        if ((i & 1u) == 1u) {
            w.assign<motion_t>(e);
        }
        if ((i & 3u) == 3u) {
            w.assign<value_t>(e);
            values_count++;
        }
    }

    uint32_t view_count = 0u;
    w.view<position_t, motion_t, value_t>().each([&view_count](auto&& ...args) {
        ++view_count;
    });

    ASSERT_EQ(view_count, values_count);
}

TEST(runtime_view, min_to_max) {
    world_t w;
    uint32_t values_count = 0u;
    for (uint32_t i = 0; i < 100; ++i) {
        auto e = w.create();
        w.assign<position_t>(e);
        if ((i & 1u) == 1u) {
            w.assign<motion_t>(e);
        }
        if ((i & 3u) == 3u) {
            w.assign<value_t>(e);
            values_count++;
        }
    }

    uint32_t view_count = 0u;
    world_t::component_typeid types[] = {
            w.type<position_t>(),
            w.type<motion_t>(),
            w.type<value_t>()
    };
    w.runtime_view(std::begin(types), std::end(types)).each([&view_count](auto e) {
        ++view_count;
    });

    ASSERT_EQ(view_count, values_count);
}
