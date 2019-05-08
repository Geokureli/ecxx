#pragma once

#include "entity_value.h"

namespace ecxx {

template<typename>
class base_world;

template<typename EntityKind>
class entity_wrapper {
public:

    using world_type = base_world<EntityKind>;
    using entity_type = entity_value<EntityKind>;

    entity_wrapper(world_type& world, entity_type entity) noexcept
            : world_{world},
              entity_{entity} {

    }

    inline const world_type& world() const {
        return world_;
    }

    inline world_type& world() {
        return world_;
    }

    inline bool is_valid() const;

    inline void destroy();

    inline auto version() const {
        return entity_.version();
    }

    inline auto index() const {
        return entity_.index();
    }

    template<typename Component, typename ...Args>
    inline Component& set(Args&& ... args);

    template<typename Component>
    inline bool has() const;

    template<typename Component>
    inline Component& get();

    template<typename Component>
    inline const Component& get() const;

    template<typename Component>
    inline void unset();

    inline entity_type value() const {
        return entity_;
    }

private:
    world_type& world_;
    entity_type entity_;
};

}