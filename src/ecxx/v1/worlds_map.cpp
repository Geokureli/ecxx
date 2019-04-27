#include "worlds_map.h"
#include "world.h"

namespace ecxx {

std::vector<world_v1_t*> worlds_map::lookup;

world_id_t worlds_map::add(world_v1_t* world) {
    world_id_t id;
    auto it = std::find(lookup.begin(), lookup.end(), nullptr);
    if (it == lookup.end()) {
        id = lookup.size();
        lookup.push_back(world);
    } else {
        id = it - lookup.begin();
        lookup[id] = world;
    }
    return id;
}

world_v1_t& worlds_map::get(world_id_t id) {
    assert(id < lookup.size());
    auto* res = lookup[id];
    assert(res != nullptr);
    return *res;
}

void worlds_map::remove(world_v1_t* world) {
    assert(world != nullptr);
    assert(world->id() < lookup.size());
    assert(lookup[world->id()] == world);
    lookup[world->id()] = nullptr;
}

}