#pragma once

#include "world.h"
#include "active_entities.h"

namespace ecxx {

basic_view <mask_filter> world_v1_t::active_entities() const {
    return basic_view<mask_filter>(*this, mask_filter{active_mask_});
}

template<typename Aspect>
inline basic_view <component_mask_filter_ref<Aspect>> world_v1_t::aspect_entities_ref(const Aspect& aspect) const {
    return basic_view<component_mask_filter_ref<Aspect>>{*this, component_mask_filter_ref{active_mask_, aspect}};
}

template<typename Aspect>
inline basic_view <component_mask_filter<Aspect>> world_v1_t::aspect_entities(Aspect aspect) const {
    return basic_view<component_mask_filter<Aspect>>{*this, component_mask_filter{active_mask_, aspect}};
}

template<typename ...Component>
inline basic_view <component_mask_filter<family_aspect < Component...>>> world_v1_t::view() const {
    return aspect_entities(family_aspect<Component...>{components()});
}


#if defined(ECXX_DEBUG)

inline void world_v1_t::guard_entity(entity_id_t entity) const {
    // Invalid entity
    assert(entity != null_entity);
    // Dead entity
    assert(active_mask_.is_true(entity));
}

inline void world_v1_t::guard_families() const {
    for (auto* family : families_) {
        if (family != nullptr) {
            for (auto entity : family->list()) {
                // FAMILY GUARD: Invalid entity id: ${entity.id}
                assert(entity != null_entity);
                // FAMILY GUARD: ${entity.id} is dead, but in family ${family.entities.length}}
                assert(active_mask_.is_true(entity));
            }
        }
    }
}

inline void world_v1_t::make_families_mutable() {
    for (auto* family : families_) {
        if (family != nullptr) {
            family->debug_make_mutable();
        }
    }
}

inline void world_v1_t::make_families_immutable() {
    for (auto* family : families_) {
        if (family != nullptr) {
            family->debug_make_immutable();
        }
    }
}

#endif

inline void world_v1_t::change_entities() {
    auto& entities = changed_list_;
    auto familiesTotal = families_.size();
    size_t i = 0;
    while (i < entities.size()) {
        size_t tail = entities.size();
        while (i < tail) {
            auto entity = entities[i];
            bool alive = alive_mask_.get(entity);
            if (alive) {
                auto active = active_mask_.get(entity);
                if (active) {
                    for (size_t j = 0; j < familiesTotal; ++j) {
                        families_[j]->change(entity);
                    }
                } else {
                    for (size_t j = 0; j < familiesTotal; ++j) {
                        families_[j]->remove_entity(entity);
                    }
                }
            }
            changed_mask_.disable(entity);
            ++i;
        }
    }
    entities.resize(0);
}

inline void world_v1_t::destroy_entities() {
    auto& entities = removed_list_;
    size_t i = 0;
    while (i < entities.size()) {
        size_t tail = entities.size();
        while (i < tail) {
            auto entity = entities[i];

            // Need to remove entities from families before deletion and notify systems
            for (auto* family : families_) {
                family->remove_entity(entity);
            }

            components().disable_all(entity);
            active_mask_.disable(entity);
            alive_mask_.disable(entity);
            removed_mask_.disable(entity);
            pool.emplace_back(entity);
            ++i;
        }
    }

    size_t count = entities.size();
    used_ -= count;
#if defined(ECXX_DEBUG)
    // no way!
    assert(used_ > 0);
#endif
    entities.resize(0);
}

inline void world_v1_t::update_families() {
    for (auto* family : families_) {
        if (family != nullptr && family->changed_) {
            family->invalidate();
        }
    }
}

inline void world_v1_t::invalidate() {
    make_families_mutable();

    if (removed_list_.empty() && changed_list_.empty()) {
    } else {
        destroy_entities();
        change_entities();
        update_families();
    }

    guard_families();
    make_families_immutable();
}

}