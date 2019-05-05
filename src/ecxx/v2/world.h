#pragma once

#include <cstdint>
#include "entity_pool.h"
#include "components_db.h"
#include "entity_wrapper.h"
#include "view.h"
#include "runtime_view.h"
#include "mutable_view.h"

namespace ecxx {

template<typename EntityType>
class base_world {
public:
    using entity_type = entity_value<EntityType>;
    using entity_pool = basic_entity_pool<EntityType>;
    using component_typeid = uint32_t;
    using component_database = components_db<EntityType>;

    base_world() = default;

    base_world(const base_world&) = delete;

    inline void reserve(size_t size) {
        pool_.reserve(size);
    }

    template<typename ...Component>
    inline entity_type create() {
        auto e = pool_.allocate();
        (assign<Component>(e), ...);
        return e;
    }

    template<typename ...Component, typename It>
    void create(It begin, It end) {
        pool_.allocate(begin, end);
        if constexpr (sizeof...(Component) > 0) {
            for (auto it = begin; it != end; ++it) {
                (assign<Component>(*it), ...);
            }
        }
    }

    inline void destroy(entity_type entity) {
        components_.remove_all_c(entity);
        pool_.deallocate(entity);
    }

    template<typename Func>
    inline void each(Func func) const {
        pool_.each(func);
    }

    template<typename Component, typename ...Args>
    inline Component& assign(entity_type entity, Args&& ... args) {
        auto& pool = components_.template ensure<Component>();
        return pool.emplace(entity, args...);
    }

    template<typename Component, typename ...Args>
    inline Component& replace_or_assign(entity_type entity, Args&& ... args) {
        auto& pool = components_.template ensure<Component>();
        if (pool.has(entity)) {
            auto& data = pool.get(entity);
            data = {args...};
            return data;
        }
        return pool.emplace(entity, args...);
    }

    template<typename Component>
    inline bool has(entity_type entity) const {
        const auto* pool = components_.template try_get<Component>();
        return pool && pool->has(entity);
    }

    template<typename Component>
    inline const Component& get(entity_type entity) const {
        const auto* pool = components_.template try_get<Component>();
        assert(pool);
        return pool->get(entity);
    }

    template<typename Component>
    inline Component& get(entity_type entity) {
        auto* pool = components_.template try_get<Component>();
        assert(pool);
        return pool->get(entity);
    }

    template<typename Component>
    inline Component& get_or_create(entity_type entity) {
        auto& pool = components_.template ensure<Component>();
        return pool.get_or_create(entity);
    }

    template<typename Component>
    inline const Component& get_or_default(entity_type entity) const {
        const auto& pool = const_cast<component_database&>(components_).template ensure<Component>();
        return pool.get_or_default(entity);
    }

    template<typename Component>
    inline void remove(entity_type entity) {
        auto* pool = components_.template try_get<Component>();
        assert(pool);
        return pool->erase(entity);
    }

    template<typename ...Component>
    inline auto view() {
        return view_t<EntityType, Component...>{components_};
    }

    /** special view provide back-to-front iteration
        and allow modify primary component map during iteration
     **/
    template<typename ...Component>
    inline auto mutable_view() {
        return basic_mutable_view<EntityType, Component...>{components_};
    }

    template<typename It>
    inline runtime_view_t<EntityType> runtime_view(It begin, It end) {
        std::vector<entity_map_base<EntityType>*> table;
        for (auto it = begin; it != end; ++it) {
            auto* set = components_.try_get(*it);
            if (set != nullptr) {
                table.emplace_back(set);
            }
        }
        return runtime_view_t(table);
    }

    inline const auto& pool() const {
        return pool_;
    }

    inline auto& pool() {
        return pool_;
    }

    template<typename Component>
    constexpr inline component_typeid type() noexcept {
        return identity_generator<Component, component_typeid>::value;
    }

    inline auto create_wrapper() {
        return wrap(create());
    }

    inline entity_wrapper<EntityType> wrap(entity_type entity) {
        return {*this, entity};
    }

    inline bool valid(entity_type entity) const {
        return entity != entity_type::null
               && pool_.current(entity.index()) == entity.version();
    }

private:
    entity_pool pool_;
    component_database components_;
};

using world_t = base_world<uint32_t>;

}