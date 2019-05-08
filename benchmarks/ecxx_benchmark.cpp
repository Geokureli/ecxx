#include <iostream>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <iterator>
#include <gtest/gtest.h>
#include <ecxx/ecxx.h>

using namespace ecxx;

struct position {
    std::uint64_t x;
    std::uint64_t y;
};

struct velocity {
    std::uint64_t x;
    std::uint64_t y;
};

template<std::size_t>
struct comp {
    int x;
};

template<typename Func>
void pathological(Func func) {
    world_t registry;

    for (std::uint64_t i = 0; i < 500000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
    }

    for (auto i = 0; i < 10; ++i) {
        registry.each([i = 0, &registry](const auto entity) mutable {
            // note: reset is better?
            if (!(++i % 7)) { registry.remove<position>(entity); }
            if (!(++i % 11)) { registry.remove<velocity>(entity); }
            if (!(++i % 13)) { registry.remove<comp<0>>(entity); }
            if (!(++i % 17)) { registry.destroy(entity); }
        });

        for (std::uint64_t j = 0; j < 50000L; j++) {
            const auto entity = registry.create();
            registry.assign<position>(entity);
            registry.assign<velocity>(entity);
            registry.assign<comp<0>>(entity);
        }
    }

    func(registry, [](auto& ... comp) {
        ((comp.x = {}), ...);
    });
}

struct timer final {
    timer() : start{std::chrono::system_clock::now()} {}

    void elapsed() {
        auto now = std::chrono::system_clock::now();
        std::cout << std::chrono::duration<double>(now - start).count() << " seconds" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> start;
};

TEST(BenchmarkECXX, Construct) {
    world_t world;

    std::cout << "Constructing 1M entities" << std::endl;

    {
        timer timer;

        for (std::uint64_t i = 0; i < 1000000L; i++) {
            world.create();
        }

        timer.elapsed();
    }
}


TEST(BenchmarkECXX, ConstructMany) {
    world_t world;
    std::vector<world_t::entity_type> entities(1000000);

    std::cout << "Constructing 1000000 entities at once" << std::endl;

    timer timer;
    world.create(entities.begin(), entities.end());
    timer.elapsed();
}

TEST(BenchmarkECXX, ConstructManyAndAssignComponents) {
    world_t world;
    std::vector<world_t::entity_type> entities(1000000);

    std::cout << "Constructing 1000000 entities at once and assign components" << std::endl;

    timer timer;

    world.create(entities.begin(), entities.end());

    for (const auto entity: entities) {
        world.assign<position>(entity);
        world.assign<velocity>(entity);
    }

    timer.elapsed();
}

TEST(BenchmarkECXX, ConstructManyWithComponents) {
    world_t world;
    std::vector<world_t::entity_type> entities(1000000);

    std::cout << "Constructing 1000000 entities at once with components" << std::endl;

    timer timer;
    world.create<position, velocity>(entities.begin(), entities.end());
    timer.elapsed();
}


TEST(BenchmarkECXX, Destroy) {
    world_t world;

    std::cout << "Destroying 1M entities" << std::endl;

    for (std::uint64_t i = 0; i < 1000000L; i++) {
        world.create();
    }

    {
        timer timer;

        world.each([&world](auto entity) {
            world.destroy(entity);
        });

        timer.elapsed();
    }
}

TEST(BenchmarkECXX, IterateSingleComponent1M) {
    world_t world;

    std::cout << "Iterating over 1000000 entities, one component" << std::endl;

    for (std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = world.create();
        world.assign<position>(entity);
    }

    auto test = [&world](auto func) {
        timer timer;
        world.view<position>().each(func);
        timer.elapsed();
    };

    test([](auto& ... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateSingleComponentRuntime1M) {
    world_t world;

    std::cout << "Iterating over 1000000 entities, one component, runtime view" << std::endl;

    for (std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = world.create();
        world.assign<position>(entity);
    }

    auto test = [&world](auto func) {
        using component_type = world_t::component_typeid;
        component_type types[] = {world.type<position>()};

        timer timer;
        world.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&world](auto entity) {
        world.get<position>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateTwoComponents1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateTwoComponents1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateTwoComponents1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

//TEST(BenchmarkECXX, IterateTwoComponentsNonOwningGroup1M) {
//    world_t registry;
//    registry.group<>(entt::get<position, velocity>);
//
//    std::cout << "Iterating over 1000000 entities, two components, non owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<>(entt::get<position, velocity>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateTwoComponentsFullOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity>();
//
//    std::cout << "Iterating over 1000000 entities, two components, full owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity>().each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateTwoComponentsPartialOwningGroup1M) {
//    world_t registry;
//    registry.group<position>(entt::get<velocity>);
//
//    std::cout << "Iterating over 1000000 entities, two components, partial owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position>(entt::get<velocity>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}

TEST(BenchmarkECXX, IterateTwoComponentsRuntime1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateTwoComponentsRuntime1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateTwoComponentsRuntime1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, two components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateThreeComponents1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateThreeComponents1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateThreeComponents1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

//TEST(BenchmarkECXX, IterateThreeComponentsNonOwningGroup1M) {
//    world_t registry;
//    registry.group<>(entt::get<position, velocity, comp<0>>);
//
//    std::cout << "Iterating over 1000000 entities, three components, non owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<>(entt::get<position, velocity, comp<0>>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateThreeComponentsFullOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity, comp<0>>();
//
//    std::cout << "Iterating over 1000000 entities, three components, full owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity, comp<0>>().each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateThreeComponentsPartialOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity>(entt::get<comp<0>>);
//
//    std::cout << "Iterating over 1000000 entities, three components, partial owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity>(entt::get<comp<0>>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}

TEST(BenchmarkECXX, IterateThreeComponentsRuntime1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>(), registry.type<comp<0>>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateThreeComponentsRuntime1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>(), registry.type<comp<0>>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateThreeComponentsRuntime1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, three components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = { registry.type<position>(), registry.type<velocity>(), registry.type<comp<0>>() };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateFiveComponents1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>, comp<1>, comp<2>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateFiveComponents1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components, half of the entities have all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>, comp<1>, comp<2>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

TEST(BenchmarkECXX, IterateFiveComponents1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components, only one entity has all the components" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        timer timer;
        registry.view<position, velocity, comp<0>, comp<1>, comp<2>>().each(func);
        timer.elapsed();
    };

    test([](auto &... comp) {
        ((comp.x = {}), ...);
    });
}

//TEST(BenchmarkECXX, IterateFiveComponentsNonOwningGroup1M) {
//    world_t registry;
//    registry.group<>(entt::get<position, velocity, comp<0>, comp<1>, comp<2>>);
//
//    std::cout << "Iterating over 1000000 entities, five components, non owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//        registry.assign<comp<1>>(entity);
//        registry.assign<comp<2>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<>(entt::get<position, velocity, comp<0>, comp<1>, comp<2>>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateFiveComponentsFullOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity, comp<0>, comp<1>, comp<2>>();
//
//    std::cout << "Iterating over 1000000 entities, five components, full owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//        registry.assign<comp<1>>(entity);
//        registry.assign<comp<2>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity, comp<0>, comp<1>, comp<2>>().each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateFiveComponentsPartialFourOfFiveOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity, comp<0>, comp<1>>(entt::get<comp<2>>);
//
//    std::cout << "Iterating over 1000000 entities, five components, partial (4 of 5) owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//        registry.assign<comp<1>>(entity);
//        registry.assign<comp<2>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity, comp<0>, comp<1>>(entt::get<comp<2>>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}
//
//TEST(BenchmarkECXX, IterateFiveComponentsPartialThreeOfFiveOwningGroup1M) {
//    world_t registry;
//    registry.group<position, velocity, comp<0>>(entt::get<comp<1>, comp<2>>);
//
//    std::cout << "Iterating over 1000000 entities, five components, partial (3 of 5) owning group" << std::endl;
//
//    for(std::uint64_t i = 0; i < 1000000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity);
//        registry.assign<velocity>(entity);
//        registry.assign<comp<0>>(entity);
//        registry.assign<comp<1>>(entity);
//        registry.assign<comp<2>>(entity);
//    }
//
//    auto test = [&registry](auto func) {
//        timer timer;
//        registry.group<position, velocity, comp<0>>(entt::get<comp<1>, comp<2>>).each(func);
//        timer.elapsed();
//    };
//
//    test([](auto &... comp) {
//        ((comp.x = {}), ...);
//    });
//}

TEST(BenchmarkECXX, IterateFiveComponentsRuntime1M) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<position>(entity);
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = {
                registry.type<position>(),
                registry.type<velocity>(),
                registry.type<comp<0>>(),
                registry.type<comp<1>>(),
                registry.type<comp<2>>()
        };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateFiveComponentsRuntime1MHalf) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components, half of the entities have all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);

        if(i % 2) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = {
                registry.type<position>(),
                registry.type<velocity>(),
                registry.type<comp<0>>(),
                registry.type<comp<1>>(),
                registry.type<comp<2>>()
        };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IterateFiveComponentsRuntime1MOne) {
    world_t registry;

    std::cout << "Iterating over 1000000 entities, five components, only one entity has all the components, runtime view" << std::endl;

    for(std::uint64_t i = 0; i < 1000000L; i++) {
        const auto entity = registry.create();
        registry.assign<velocity>(entity);
        registry.assign<comp<0>>(entity);
        registry.assign<comp<1>>(entity);
        registry.assign<comp<2>>(entity);

        if(i == 500000L) {
            registry.assign<position>(entity);
        }
    }

    auto test = [&registry](auto func) {
        using component_type = typename world_t::component_typeid;
        component_type types[] = {
                registry.type<position>(),
                registry.type<velocity>(),
                registry.type<comp<0>>(),
                registry.type<comp<1>>(),
                registry.type<comp<2>>()
        };

        timer timer;
        registry.runtime_view(std::begin(types), std::end(types)).each(func);
        timer.elapsed();
    };

    test([&registry](auto entity) {
        registry.get<position>(entity).x = {};
        registry.get<velocity>(entity).x = {};
        registry.get<comp<0>>(entity).x = {};
        registry.get<comp<1>>(entity).x = {};
        registry.get<comp<2>>(entity).x = {};
    });
}

TEST(BenchmarkECXX, IteratePathological) {
    std::cout << "Pathological case" << std::endl;

    pathological([](auto &registry, auto func) {
        timer timer;
        registry.template view<position, velocity, comp<0>>().each(func);
        timer.elapsed();
    });
}

//TEST(BenchmarkECXX, IteratePathologicalNonOwningGroup) {
//    std::cout << "Pathological case (non-owning group)" << std::endl;
//
//    pathological([](auto &registry, auto func) {
//        auto group = registry.template group<>(entt::get<position, velocity, comp<0>>);
//
//        timer timer;
//        group.each(func);
//        timer.elapsed();
//    });
//}
//
//TEST(BenchmarkECXX, IteratePathologicalFullOwningGroup) {
//    std::cout << "Pathological case (full-owning group)" << std::endl;
//
//    pathological([](auto &registry, auto func) {
//        auto group = registry.template group<position, velocity, comp<0>>();
//
//        timer timer;
//        group.each(func);
//        timer.elapsed();
//    });
//}
//
//TEST(BenchmarkECXX, IteratePathologicalPartialOwningGroup) {
//    std::cout << "Pathological case (partial-owning group)" << std::endl;
//
//    pathological([](auto &registry, auto func) {
//        auto group = registry.template group<position, velocity>(entt::get<comp<0>>);
//
//        timer timer;
//        group.each(func);
//        timer.elapsed();
//    });
//}

//TEST(BenchmarkECXX, SortSingle) {
//    world_t registry;
//
//    std::cout << "Sort 150000 entities, one component" << std::endl;
//
//    for(std::uint64_t i = 0; i < 150000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity, i, i);
//    }
//
//    timer timer;
//
//    registry.sort<position>([](const auto &lhs, const auto &rhs) {
//        return lhs.x < rhs.x && lhs.y < rhs.y;
//    });
//
//    timer.elapsed();
//}

//TEST(BenchmarkECXX, SortMulti) {
//    world_t registry;
//
//    std::cout << "Sort 150000 entities, two components" << std::endl;
//
//    for(std::uint64_t i = 0; i < 150000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity, i, i);
//        registry.assign<velocity>(entity, i, i);
//    }
//
//    registry.sort<position>([](const auto &lhs, const auto &rhs) {
//        return lhs.x < rhs.x && lhs.y < rhs.y;
//    });
//
//    timer timer;
//
//    registry.sort<velocity, position>();
//
//    timer.elapsed();
//}

//TEST(BenchmarkECXX, AlmostSortedStdSort) {
//    world_t registry;
//    entt::entity entities[3];
//
//    std::cout << "Sort 150000 entities, almost sorted, std::sort" << std::endl;
//
//    for(std::uint64_t i = 0; i < 150000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity, i, i);
//
//        if(!(i % 50000)) {
//            entities[i / 50000] = entity;
//        }
//    }
//
//    for(std::uint64_t i = 0; i < 3; ++i) {
//        registry.destroy(entities[i]);
//        const auto entity = registry.create();
//        registry.assign<position>(entity, 50000 * i, 50000 * i);
//    }
//
//    timer timer;
//
//    registry.sort<position>([](const auto &lhs, const auto &rhs) {
//        return lhs.x > rhs.x && lhs.y > rhs.y;
//    });
//
//    timer.elapsed();
//}

//TEST(BenchmarkECXX, AlmostSortedInsertionSort) {
//    world_t registry;
//    entt::entity entities[3];
//
//    std::cout << "Sort 150000 entities, almost sorted, insertion sort" << std::endl;
//
//    for(std::uint64_t i = 0; i < 150000L; i++) {
//        const auto entity = registry.create();
//        registry.assign<position>(entity, i, i);
//
//        if(!(i % 50000)) {
//            entities[i / 50000] = entity;
//        }
//    }
//
//    for(std::uint64_t i = 0; i < 3; ++i) {
//        registry.destroy(entities[i]);
//        const auto entity = registry.create();
//        registry.assign<position>(entity, 50000 * i, 50000 * i);
//    }
//
//    timer timer;
//
//    registry.sort<position>([](const auto &lhs, const auto &rhs) {
//        return lhs.x > rhs.x && lhs.y > rhs.y;
//    }, entt::insertion_sort{});
//
//    timer.elapsed();
//}