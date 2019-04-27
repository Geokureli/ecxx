#pragma once

#include "entity.h"

namespace ecxx {

inline bool entity_t::is_valid() const {
    return world().is_valid(entity_id_, version_tag_);
}

inline void entity_t::destroy() {
    world().deallocate(entity_id_);
}

template<typename Component, typename ...Args>
entity_t::component_reference<Component> entity_t::set(Args&& ... args) {
    using manager_type = typename component_traits<Component>::manager_type;
    auto& storage = world().components().enable<manager_type>(entity_id_, args...);
    return storage.view(entity_id_);
}

template<typename Component>
bool entity_t::has() const {
    using manager_type = typename component_traits<Component>::manager_type;
    return world().components().has<manager_type>(entity_id_);
}

template<typename Component>
entity_t::component_reference<Component> entity_t::get() {
    using manager_type = typename component_traits<Component>::manager_type;
    return world().components().storage<manager_type>().view(entity_id_);
}

template<typename Component>
entity_t::component_const_reference<Component> entity_t::get() const {
    using manager_type = typename component_traits<Component>::manager_type;
    return world().components().storage<manager_type>().view(entity_id_);
}

template<typename Component>
void entity_t::unset() {
    using manager_type = typename component_traits<Component>::manager_type;
    world().components().disable<manager_type>(entity_id_);
}

inline void entity_t::commit() {
    world().commit(id());
}

}