#pragma once

#include "entity_map.h"
#include "../identity_generator.h"

namespace ecxx {

template<typename EntityType>
class components_db {
public:
    using entity_type = entity_value<EntityType>;
    using component_typeid = uint32_t;

    template<typename Component>
    using pool_type = entity_map<EntityType, Component>;

    using pool_base_type = entity_map_base<EntityType>;

    template<typename Component>
    inline static constexpr component_typeid type() noexcept {
        return identity_generator<Component, component_typeid>::value;
    }

    template<typename Component>
    pool_type<Component>& ensure() {
        const auto cid = type<Component>();
        if (cid < pools_.size()) {
        } else {
            pools_.resize(cid + 1u);
        }

        auto* map = pools_[cid];
        if (map != nullptr) {
        } else {
            map = new pool_type<Component>();
            pools_[cid] = map;
        }
        return *static_cast<pool_type<Component>*>(map);
    }

    template<typename Component>
    inline const pool_type<Component>* try_get() const {
        return static_cast<const pool_type<Component>*>(try_get(type<Component>()));
    }

    template<typename Component>
    inline pool_type<Component>* try_get() {
        return static_cast<pool_type<Component>*>(try_get(type<Component>()));
    }

    inline pool_base_type* try_get(component_typeid type) {
        return type < pools_.size() ? pools_[type] : nullptr;
    }

    inline const pool_base_type* try_get(component_typeid type) const {
        return type < pools_.size() ? pools_[type] : nullptr;
    }

    void remove_all_c(entity_type entity) {
        for (auto* pool : pools_) {
            if (pool != nullptr && pool->has(entity)) {
                pool->erase_dyn(entity);
            }
        }
    }

private:
    std::vector<entity_map_base<EntityType>*> pools_;
};

}