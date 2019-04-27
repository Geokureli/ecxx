#pragma once

#include "decl.h"
#include <vector>
#include "worlds_map.h"
#include "component_db.h"
#include "entity.h"
#include "view_filters.h"

namespace ecxx {

class world_v1_t {
public:
    std::vector<entity_id_t> pool;
    std::vector<version_tag_t> versions;
    std::vector<uint8_t> flags;

    explicit world_v1_t(entity_id_t capacity)
            : end_{capacity},
              active_mask_{capacity},
              alive_mask_{capacity},
              removed_mask_{capacity},
              changed_mask_{capacity} {

        assert(capacity > 0u);

        for (entity_id_t id = 1u; id < capacity; ++id) {
            pool.push_back(capacity - id);
        }
        for (entity_id_t id = 0u; id < capacity; ++id) {
            versions.push_back(1u);
        }

        id_ = worlds_map::add(this);
    }

    ~world_v1_t() {
        worlds_map::remove(this);
    }

    entity_id_t allocate() {
        // Out of entities, max allowed `capacity`
        assert(used_ < end_);
        ++used_;

        entity_id_t id = pool.back();
        pool.pop_back();
        active_mask_.enable(id);
        alive_mask_.enable(id);

        mark_entity_as_changed(id);
        return id;
    }

    void deallocate(entity_id_t id) {
        versions[id] = versions[id] + 1;
        pool.push_back(id);
//        active_mask_.disable(id);
        if (removed_mask_.enable_if_not(id)) {
            removed_list_.emplace_back(id);
        }
    }

    template<typename ComponentTag, typename ... Args>
    void make_component_type(Args&& ...args) {
        using manager_type = typename component_traits<ComponentTag>::manager_type;
        auto* d = new manager_type(*this, args...);
        components().register_type(d);
    }

    entity_t create() {
        return ref(allocate());
    }

    void destroy(const entity_t& entity) {
        deallocate(entity.id());
    }

    entity_t ref(entity_id_t entity_id) const {
        return entity_t{entity_id, version(entity_id), id_};
    }

    inline version_tag_t version(entity_id_t entity_id) const {
        return versions[entity_id];
    }

    inline bool is_valid(const entity_t& entity) const {
        return entity.version() == versions[entity.id()];
    }

    inline bool is_valid(entity_id_t entity_id, version_tag_t version) const {
        return version == versions[entity_id];
    }

    inline entity_id_t capacity() const {
        return end_;
    }

    inline world_id_t id() const {
        return id_;
    }

    inline entity_id_t available() const {
        // null = 0
        // used = 1
        // end = 4
        // available = 3

        // 0 1 2 3 4
        //
        // x _ _ _ x

        return end_ - used_;
    }

    inline const component_db& components() const {
        return components_;
    }

    inline component_db& components() {
        return components_;
    }

    template<typename F>
    friend
    class basic_view;

    inline basic_view<mask_filter> active_entities() const;


    inline const bit_vector& active_mask() const {
        return active_mask_;
    }

    template<typename Aspect>
    inline basic_view<component_mask_filter<Aspect>> aspect_entities(Aspect aspect) const;

    template<typename Aspect>
    inline basic_view<component_mask_filter_ref<Aspect>> aspect_entities_ref(const Aspect& aspect) const;

    template<typename ...Component>
    inline basic_view<component_mask_filter<family_aspect<Component...>>> view() const;


#if defined(ECXX_DEBUG)

    void guard_entity(entity_id_t entity) const;

    void guard_families() const;

    void make_families_mutable();

    void make_families_immutable();

#else
    inline void guard_entity(const entity_id_t entity) const {}

    inline void guard_families() const {}

    inline void make_families_mutable() {}

    inline void make_families_immutable() {}
#endif


    /**
		Destroy all components attached to `entity`
	**/
    void destroy_components(entity_id_t entity) {
        components().disable_all(entity);
        mark_entity_as_changed(entity);
    }

    /**
        Mark entity as changed
    **/
    void commit(entity_id_t entity) {
        mark_entity_as_changed(entity);
    }


    /**
		Performs entities destroying, commits and update families
	**/
    void invalidate();

private:
    void mark_entity_as_changed(entity_id_t entity) {
        guard_entity(entity);
        if (changed_mask_.enable_if_not(entity)) {
            changed_list_.emplace_back(entity);
        }
    }

    void destroy_entities();

    void change_entities();

    void update_families();

    world_id_t id_;
    entity_id_t end_;
    entity_id_t used_{1u};
    component_db components_;
    std::vector<managed_family*> families_;

    bit_vector active_mask_;
    bit_vector alive_mask_; // TODO: ??
    bit_vector removed_mask_;
    bit_vector changed_mask_;

    std::vector<entity_id_t> removed_list_;
    std::vector<entity_id_t> changed_list_;

    //::iterator;
};
}