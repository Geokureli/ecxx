#pragma once

#include "decl.h"
#include <vector>

namespace ecxx {

class worlds_map {
    static std::vector<world_v1_t*> lookup;

public:
    static world_v1_t& get(world_id_t id);

    static world_id_t add(world_v1_t* world);

    static void remove(world_v1_t* world);
};

}