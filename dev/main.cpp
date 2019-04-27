#include <ecxx/ecxx.h>
//#include <ecxx/ecs_node.h>

#include <string>
#include <iostream>

using namespace ecxx;

struct position_t {
    float x = 0.0f;
    float y = 0.0f;
};

struct velocity_t {
    float vx = 0.0f;
    float vy = 0.0f;
};

struct name_t {
    std::string value;
};

std::ostream& operator<<(std::ostream& os, const name_t& obj) {
    return os << obj.value;
}

void update(world_t& world) {
    auto view = world.view<position_t, velocity_t>();
    for (auto entity: view) {
        auto& vel = world.get<velocity_t>(entity);
        vel.vx = 0.0f;
        vel.vy = 0.0f;
    }
}
//
void update(float dt, world_t& world) {
//    auto view = world.view<position_t, velocity_t>();

//    for (auto entity: view) {
    world.view<position_t, velocity_t>().each([dt](auto& pos, auto& vel) {

//        auto& pos = world.get<position_t>(entity);
//        auto& vel = world.get<velocity_t>(entity);

        // gets all the components of the view at once ...

        pos.x += vel.vx * dt;
        pos.y += vel.vy * dt;

        // ...
    });
//    }
}

int main() {
//    world_t world{0xFFFFu};
//    world.make_component_type<ecs_node>();
//    world.make_component_type<position_t>();
//    world.make_component_type<velocity_t>();
//    world.make_component_type<name_t>();
    world_t world;


    auto e = world.create_wrapper();
//    e.set<ecs_node>();
    e.set<position_t>();
    e.set<name_t>("old");

//    std::cout << "ecs_node: " << e.has<ecs_node>() << '\n';
    std::cout << "position: " << e.has<position_t>() << " " << e.get<position_t>().x << " " << e.get<position_t>().y
              << '\n';
    std::cout << "name: " << e.get<name_t>() << '\n';

//    e.set<ecs_node>();
    e.get<position_t>() = {1.0f, 1.0f};
    e.get<name_t>().value = "olk";

    const auto ce = e;

//    std::cout << "ecs_node: " << ce.has<ecs_node>() << '\n';
    std::cout << "position: " << ce.has<position_t>()
              << " " << ce.get<position_t>().x
              << " " << ce.get<position_t>().y
              << '\n';
    std::cout << "name: " << ce.get<name_t>() << '\n';

    world.each([](auto e) {
        std::cout << "entity: " << e.index() << '\n';
    });

    //////

    auto dt = 0.016f;

    for (auto i = 0; i < 10; ++i) {
        auto entity = world.create_wrapper();
        entity.set<position_t>(i * 1.0f, i * 1.0f);
        if (i % 2 == 0) {
            entity.set<velocity_t>(i * 0.1f, i * 0.1f);
        }
    }


    update(dt, world);
    update(dt, world);
    update(world);
}