#pragma once

namespace ecxx {

class basic_component_manager {
public:
    explicit basic_component_manager(world_v1_t& world)
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
    world_v1_t& world_;
    bit_vector mask_;
};


template<typename ComponentTag>
class generic_component_manager : public basic_component_manager {
public:
    explicit generic_component_manager(world_v1_t& world)
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

}