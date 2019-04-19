#include <ecxx/ecxx.h>
#include <ecxx/ecs_node.h>

#include <string>
#include <iostream>

using namespace ecxx;

struct position_t {
    float x = 0.0f;
    float y = 0.0f;
};

struct name_t {
    std::string value;
};

std::ostream& operator<<(std::ostream& os, const name_t& obj) {
    return os << obj.value;
}

void make_entity() {

};

int main() {
    world_t world{0xFFFFu};
    world.make_component_type<ecs_node>();
    world.make_component_type<position_t>();
    world.make_component_type<name_t>();

    auto e = world.create();
    e.set<ecs_node>();
    e.set<position_t>();
    e.set<name_t>("old");

    std::cout << "ecs_node: " << e.has<ecs_node>() << '\n';
    std::cout << "position: " << e.has<position_t>() << " " << e.get<position_t>().x << " " << e.get<position_t>().y
              << '\n';
    std::cout << "name: " << e.get<name_t>() << '\n';

    e.set<ecs_node>();
    e.set<position_t>(1.0f, 1.0f);
    e.set<name_t>().value = "olk";

    const auto ce = e;

    std::cout << "ecs_node: " << ce.has<ecs_node>() << '\n';
    std::cout << "position: " << ce.has<position_t>()
              << " " << ce.get<position_t>().x
              << " " << ce.get<position_t>().y
              << '\n';
    std::cout << "name: " << ce.get<name_t>() << '\n';

    for (auto i : world.active_entities()) {
        std::cout << "entity: " << i.id() << '\n';
    }
}