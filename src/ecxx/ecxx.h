#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <vector>
#include <utility>
#include <type_traits>

#include "bit_vector.h"
#include "identity_generator.h"

namespace ecxx {

using world_id_t = uint8_t;
using entity_id_t = uint16_t;
using version_tag_t = uint16_t;
inline constexpr entity_id_t null_entity = 0u;
using component_id_t = uint16_t;

template<typename ComponentTag>
class generic_component_manager;

template<typename ComponentTag>
class component_traits {
public:
    using manager_type = generic_component_manager<ComponentTag>;
    using reference = ComponentTag&;
    using const_reference = const ComponentTag&;
};


class world_t;

class entity_t;

class worlds_map {
    static std::vector<world_t*> lookup;

public:
    static world_t& get(world_id_t id);

    static world_id_t add(world_t* world);

    static void remove(world_t* world);
};

class component_db {
public:
    component_db() = default;

    ~component_db() {
        for (auto& clean : cleaners_) {
            clean();
        }
    }

    template<typename ComponentStorageType>
    void register_type(ComponentStorageType* storage) {
        const auto ci = identity_generator<ComponentStorageType, component_id_t>::value;
        if (ci >= matrix_.size()) {
            matrix_.resize(ci + 1);
            storage_.resize(ci + 1);
        }
        assert(matrix_[ci] == nullptr);
        assert(storage_[ci] == nullptr);
        storage_[ci] = storage;
        matrix_[ci] = &storage->mask();
        cleaners_.push_back([storage] {
            delete storage;
        });
    }

    template<typename ComponentStorageType>
    const ComponentStorageType& storage() const {
        return storage_impl<ComponentStorageType>();
    }

    template<typename ComponentStorageType>
    ComponentStorageType& storage() {
        return const_cast<ComponentStorageType&>(storage_impl<ComponentStorageType>());
    }

    template<typename ComponentStorageType>
    const bit_vector& bits() const {
        return bits_impl<ComponentStorageType>();
    }

    template<typename ComponentStorageType>
    bit_vector& bits() {
        return const_cast<bit_vector&>(bits_impl<ComponentStorageType>());
    }

    template<typename ComponentStorageType>
    inline bool has(entity_id_t entity) const {
        return bits<ComponentStorageType>()[entity];
    }

    template<typename ComponentStorageType, typename ...Args>
    inline ComponentStorageType& enable(entity_id_t entity, Args&& ... args) {
        bits<ComponentStorageType>().enable(entity);
        auto& s = storage<ComponentStorageType>();
        s.initialize(entity, args...);
        return s;
    }

    template<typename ComponentStorageType>
    inline void disable(entity_id_t entity) {
        bits<ComponentStorageType>().disable(entity);
        storage<ComponentStorageType>().reset(entity);
    }

private:

    template<typename ComponentStorageType>
    const ComponentStorageType& storage_impl() const {
        const auto component_id = identity_generator<ComponentStorageType, component_id_t>::value;
        assert(component_id < storage_.size());
        const void* ptr = storage_[component_id];
        assert(ptr != nullptr);
        return *static_cast<const ComponentStorageType*>(ptr);
    }

    template<typename ComponentStorageType>
    const bit_vector& bits_impl() const {
        const auto component_id = identity_generator<ComponentStorageType, component_id_t>::value;
        assert(component_id < matrix_.size());
        const bit_vector* ptr = matrix_[component_id];
        assert(ptr != nullptr);
        return *ptr;
    }

    std::vector<bit_vector*> matrix_;
    std::vector<void*> storage_;
    std::vector<std::function<void()>> cleaners_;
};

class entity_t {
public:

    entity_t(entity_id_t entity_id, version_tag_t version_tag, world_id_t world_id)
            : entity_id_{entity_id},
              version_tag_{version_tag},
              world_id_{world_id} {

    }

    inline const world_t& world() const {
        return worlds_map::get(world_id_);
    }

    inline world_t& world() {
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

private:
    entity_id_t entity_id_;
    version_tag_t version_tag_;
    world_id_t world_id_;
};

class active_entities_view;

class world_t {
public:
    std::vector<entity_id_t> pool;
    std::vector<version_tag_t> versions;
    std::vector<uint8_t> flags;

    explicit world_t(entity_id_t capacity)
            : end_{capacity},
              active_{capacity} {

        assert(capacity > 0u);

        for (entity_id_t id = 1u; id < capacity; ++id) {
            pool.push_back(capacity - id);
        }
        for (entity_id_t id = 0u; id < capacity; ++id) {
            versions.push_back(1u);
        }

        id_ = worlds_map::add(this);
    }

    ~world_t() {
        worlds_map::remove(this);
    }

    entity_id_t allocate() {
        // Out of entities, max allowed `capacity`
        assert(used_ < end_);
        ++used_;

        entity_id_t id = pool.back();
        pool.pop_back();
        active_.enable(id);
        return id;
    }

    void deallocate(entity_id_t id) {
        versions[id] = versions[id] + 1;
        pool.push_back(id);
        active_.disable(id);
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

    friend class active_entities_view;

    inline active_entities_view active_entities();

private:
    world_id_t id_;
    entity_id_t end_;
    entity_id_t used_{1u};
    bit_vector active_;
    component_db components_;


    //::iterator;
};

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

/// component managers
class basic_component_manager {
public:
    explicit basic_component_manager(world_t& world)
            : world_{world},
              mask_{world.capacity()} {
    }

    inline bit_vector& mask() {
        return mask_;
    }

    inline const bit_vector& mask() const {
        return mask_;
    }

    inline bool has(entity_id_t entity) const {
        return mask_[entity];
    }

protected:
    world_t& world_;
    bit_vector mask_;
};


template<typename ComponentTag>
class generic_component_manager : public basic_component_manager {
public:
    explicit generic_component_manager(world_t& world)
            : basic_component_manager{world} {
        data_.resize(world.capacity());
    }

    ComponentTag& view(entity_id_t entity) {
        return data_[entity];
    }

    const ComponentTag& view(entity_id_t entity) const {
        return data_[entity];
    }

    template<typename ...Args>
    inline void initialize(entity_id_t entity, Args&& ... args) {
        data_[entity] = ComponentTag{args...};
    }

    inline void reset(entity_id_t) {}

private:
    std::vector<ComponentTag> data_;
};


// iteration
class active_entities_view {
    class iterator {
    public:
        iterator(world_t& world, entity_id_t id)
                : world_{world},
                  id_{id} {

        }

        iterator operator++() {
            while (++id_ < world_.end_ && world_.active_.is_false(id_)) {}
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return id_ != other.id_;
        }

        const entity_t operator*() const {
            return world_.ref(id_);
        }

        entity_t operator*() {
            return world_.ref(id_);
        }

    private:
        world_t& world_;
        entity_id_t id_{1};
    };
public:
    iterator begin() {
        return {world_, 1u};
    }

    iterator end() {
        return {world_, world_.end_};
    }

    explicit active_entities_view(world_t& world)
            : world_{world} {

    }

private:
    world_t& world_;
};

active_entities_view world_t::active_entities() {
    return active_entities_view(*this);
}

}