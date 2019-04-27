#pragma once

#include "decl.h"
#include "worlds_map.h"
#include "world.h"

namespace ecxx {

class entity_t {
public:

    entity_t(entity_id_t entity_id, version_tag_t version_tag, world_id_t world_id)
            : entity_id_{entity_id},
              version_tag_{version_tag},
              world_id_{world_id} {

    }

    inline const world_v1_t& world() const {
        return worlds_map::get(world_id_);
    }

    inline world_v1_t& world() {
        return worlds_map::get(world_id_);
    }

    inline bool is_valid() const;

    inline void destroy();

    template<typename Component>
    using component_reference = typename component_traits<Component>::reference;

    template<typename Component>
    using component_const_reference = typename component_traits<Component>::const_reference;

    inline version_tag_t version() const {
        return version_tag_;
    }

    inline entity_id_t id() const {
        return entity_id_;
    }

    template<typename Component, typename ...Args>
    component_reference<Component> set(Args&& ... args);

    template<typename Component>
    bool has() const;

    template<typename Component>
    component_reference<Component> get();

    template<typename Component>
    component_const_reference<Component> get() const;

    template<typename Component>
    void unset();

    void commit();

private:
    entity_id_t entity_id_;
    version_tag_t version_tag_;
    world_id_t world_id_;
};

}