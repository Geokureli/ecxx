#pragma once

#include "ecxx1.h"

namespace ecxx {

class ecs_node;

class ecs_node_storage : public basic_component_manager {
public:

    explicit ecs_node_storage(world_v1_t& world)
            : basic_component_manager{world} {
        reserve(world.capacity());
        initialize(0);
    }

    ecs_node view(entity_id_t entity);

    const ecs_node view(entity_id_t entity) const;

    void reserve(uint32_t size) {
        parent_.resize(size);
        sibling_next_.resize(size);
        sibling_prev_.resize(size);
        child_first_.resize(size);
        child_last_.resize(size);
    }

    // initialize component data
    inline void initialize(entity_id_t entity) {
        parent_[entity] = null_entity;
        child_first_[entity] = null_entity;
        child_last_[entity] = null_entity;
        sibling_next_[entity] = null_entity;
        sibling_prev_[entity] = null_entity;
    }

    // deinitialize component data
    inline void reset(entity_id_t) {}

    inline void destroy(entity_id_t entity) {
        remove_from_parent(entity);
        destroy_children(entity);
    }

    /**
		Insert `childAfter` next to the `entity`.
		Throws exception if `entity` has no parent.
		`childAfter` will be removed from it's current parent.
		If `childAfter` has not Node component, it will be added.
	**/
    void insert_after(entity_id_t entity, entity_id_t child_after) {
        auto par = parent_[entity];
        assert (par != null_entity);
        if (!has(child_after)) {
            world_enable_node(child_after);
        }
        remove_from_parent(child_after);
        auto next = sibling_next_[entity];
        sibling_next_[entity] = child_after;
        sibling_prev_[child_after] = entity;
        if (next != null_entity) {
            sibling_prev_[next] = child_after;
            sibling_next_[child_after] = next;
        } else {
            child_last_[par] = child_after;
        }
        parent_[child_after] = parent_[entity];
    }

    inline void world_enable_node(entity_id_t entity) {
        world_.components().enable<ecs_node_storage>(entity);
    }

    inline void world_disable_node(entity_id_t entity) {
        world_.components().disable<ecs_node_storage>(entity);
    }

    /**
        Insert `childBefore` back to the `entity`.
        Throws exception if `entity` has no parent.
        `childBefore` will be removed from it's current parent.
        If `childBefore` has not Node component, it will be added.
    **/
    void insert_before(entity_id_t entity, entity_id_t child_before) {
        auto par = parent_[entity];
        assert(par != null_entity);
        if (!has(child_before)) {
            world_enable_node(child_before);
        }
        remove_from_parent(child_before);
        auto prev = sibling_prev_[entity];
        sibling_prev_[entity] = child_before;
        sibling_next_[child_before] = entity;
        if (prev != null_entity) {
            sibling_next_[prev] = child_before;
            sibling_prev_[child_before] = prev;
        } else {
            child_first_[par] = child_before;
        }
        parent_[child_before] = parent_[entity];
    }

    /**
        Add `child` to `entity` to the end.
        If `child` or `entity` have no Node component, it will be created.
        `child` will be removed from it's current parent.
    **/
    void append(entity_id_t entity, entity_id_t child) {
        if (!has(entity)) {
            world_enable_node(entity);
        }
        if (!has(child)) {
            world_enable_node(child);
        }

        if (parent_[child] != null_entity) {
            remove_from_parent(child);
        }

        auto tail = child_last_[entity];
        if (tail != null_entity) {
            sibling_next_[tail] = child;
            sibling_prev_[child] = tail;
            child_last_[entity] = child;
        } else {
            child_first_[entity] = child;
            child_last_[entity] = child;
        }
        parent_[child] = entity;
    }

    /**
        Add `child` to `entity` to the beginning.
        If `child` or `entity` have no Node component, it will be created.
        `child` will be removed from it's current parent.
    **/
    void prepend(entity_id_t entity, entity_id_t child) {
        if (!has(entity)) {
            world_enable_node(entity);
        }
        if (!has(child)) {
            world_enable_node(child);
        }

        if (parent_[child] != null_entity) {
            remove_from_parent(child);
        }

        auto head = child_first_[entity];
        if (head != null_entity) {
            sibling_next_[child] = head;
            sibling_prev_[head] = child;
            child_first_[entity] = child;
        } else {
            child_first_[entity] = child;
            child_last_[entity] = child;
        }
        parent_[child] = entity;
    }

    /**
        Number of children of `entity`.
        Returns 0 if `entity` has no Node component.

        Note: children will be counted in fast-traversing
        from the first to the last child of `entity`
    **/
    uint32_t children_count(entity_id_t entity) {
        uint32_t num = 0u;
        if (has(entity)) {
            auto child = child_first_[entity];
            while (child != null_entity) {
                ++num;
                child = sibling_next_[child];
            }
        }
        return num;
    }


    /**
        Delete all children and sub-children of `entity`
        if `entity` has Node component.
    **/
    void destroy_children(entity_id_t entity) {
        if (!has(entity)) {
            return;
        }
        auto child = child_first_[entity];
        while (child != null_entity) {
            auto temp = child;
            child = sibling_next_[child];
            destroy_children(temp);
            world_disable_node(temp);
            parent_[temp] = null_entity;
            world_.deallocate(temp);
        }
        child_first_[entity] = null_entity;
        child_last_[entity] = null_entity;
    }

    /**
        Remove `entity` from it's parent
        if `entity` has Node component and is a child.
    **/
    void remove_from_parent(entity_id_t entity) {
        if (!has(entity)) {
            return;
        }
        auto par = parent_[entity];
        if (par == null_entity) {
            return;
        }

        auto prev = sibling_prev_[entity];
        auto next = sibling_next_[entity];
        if (prev != null_entity) {
            sibling_next_[prev] = next;
        } else {
            child_first_[par] = next;
        }
        if (next != null_entity) {
            sibling_prev_[next] = prev;
        } else {
            child_last_[par] = prev;
        }

        parent_[entity] = null_entity;
        sibling_next_[entity] = null_entity;
        sibling_prev_[entity] = null_entity;
    }

    /**
        Remove all children of `entity`
        if `entity` has Node component and is a child.
    **/
    void remove_children(entity_id_t entity) {
        if (!has(entity)) {
            return;
        }
        auto child = child_first_[entity];
        while (child != null_entity) {
            auto temp = child;
            child = sibling_next_[child];
            parent_[temp] = null_entity;
            sibling_next_[temp] = null_entity;
            sibling_prev_[temp] = null_entity;
        }
        child_first_[entity] = null_entity;
        child_last_[entity] = null_entity;
    }

    /**
        Returns true if entity is descendant of ancestor.
    **/
    bool is_descendant(entity_id_t entity, entity_id_t ancestor) const {
        if (!has(entity) || !has(ancestor)) {
            return false;
        }

        while (entity != null_entity) {
            entity = parent_[entity];
            if (entity == ancestor) {
                return true;
            }
        }

        return false;
    }


    /**
		First child of entity.
		NULL will be returned if entity has no children,
		or entity has not Node component
	**/
    inline entity_id_t child_first(entity_id_t entity) const {
        return has(entity) ? child_first_[entity] : null_entity;
    }

    /**
        Last child of entity.
        NULL will be returned if entity has no children,
        or entity has not Node component
    **/
    inline entity_id_t child_last(entity_id_t entity) const {
        return has(entity) ? child_last_[entity] : null_entity;
    }

    /**
        Next sibling of the entity.
        NULL will be returned if entity has no next sibling,
        or entity has not Node component
    **/
    inline entity_id_t sibling_next(entity_id_t entity) const {
        return has(entity) ? sibling_next_[entity] : null_entity;
    }

    /**
        Previous sibling of the entity.
        NULL will be returned if entity has no previous sibling,
        or entity has not Node component
    **/
    inline entity_id_t sibling_prev(entity_id_t entity) const {
        return has(entity) ? sibling_prev_[entity] : null_entity;
    }

    /**
        Parent of the entity.
        NULL will be returned if entity has not been added to any entity,
        or entity has not Node component
    **/
    inline entity_id_t parent(entity_id_t entity) const {
        return has(entity) ? parent_[entity] : null_entity;
    }

    /**
        True if entity has at least one child.
        False if entity has no children or it has not Node component.
    **/
    inline bool has_children(entity_id_t entity) const {
        return child_first(entity) != null_entity;
    }

private:
    std::vector<entity_id_t> parent_;
    std::vector<entity_id_t> sibling_next_;
    std::vector<entity_id_t> sibling_prev_;
    std::vector<entity_id_t> child_first_;
    std::vector<entity_id_t> child_last_;

};

class ecs_node {
public:

    using storage_type = ecs_node_storage;

    ecs_node(entity_id_t id, ecs_node_storage& data)
            : id_{id},
              data_{data} {

    }

    inline entity_id_t child_first() const {
        return data_.child_first(id_);
    }

    inline entity_id_t child_last() const {
        return data_.child_last(id_);
    }

    inline entity_id_t sibling_next() const {
        return data_.sibling_next(id_);
    }

    inline entity_id_t sibling_prev() const {
        return data_.sibling_prev(id_);
    }

    inline entity_id_t parent() const {
        return data_.parent(id_);
    }

    inline bool has_children() const {
        return data_.has_children(id_);
    }

private:
    entity_id_t id_;
    ecs_node_storage& data_;
};

ecs_node ecs_node_storage::view(entity_id_t entity) {
    return {entity, *this};
}

const ecs_node ecs_node_storage::view(entity_id_t entity) const {
    return {entity, *const_cast<ecs_node_storage*>(this)};
}

template<>
class component_traits<ecs_node> {
public:
    using manager_type = ecs_node_storage;
    using reference = ecs_node;
    using const_reference = const ecs_node;
};

}


