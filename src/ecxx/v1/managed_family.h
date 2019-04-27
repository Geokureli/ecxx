#pragma once

#include <cmath>
#include "bit_vector.h"
#include "world.h"

namespace ecxx {

template<typename T>
void fill_enabled_bits(std::vector<T>& dst, const bit_vector& src, uint32_t begin_index = 0u, uint32_t end_index = 0u) {
    auto* data = src.data();
    uint32_t begin = begin_index / bit_vector::bits_per_element;
    if (end_index == 0) {
        end_index = src.size();
    }
    uint32_t end = ceil(end_index / float(bit_vector::bits_per_element));
    uint32_t at = 0;
    for (uint32_t i = begin; i < end; ++i) {
        auto value = data[i];
        if (value != 0u) {
            int index = i << bit_vector::bit_shift;
            for (uint8_t j = 0u; j < bit_vector::bits_per_element; ++j) {
                if ((value & (1u << j)) != 0u) {
                    dst[at++] = index + j;
                }
            }
        }
    }
    dst.resize(at);
}

class managed_family {
public:
    explicit managed_family(world_v1_t& world)
            : aspect_{world.components()},
              mask_{world.capacity()} {

    }

    const std::vector<entity_id_t>& list() const {
        return list_;
    }

private:

    void invalidate() {
        list_.resize(total_);
        fill_enabled_bits(list_, mask_, 0, 0);
        changed_ = false;
    }

    void change(entity_id_t entity) {
        const auto e = mask_.get(entity);
        const auto c = aspect_.check(entity);
        if (c != e) {
            if (c) {
                add_entity(entity);
            } else {
                remove_entity(entity);
            }
        }
    }

    void add_entity(entity_id_t entity) {
#if defined(ECXX_DEBUG)
        assert(debug_mutable_);
#endif
        // todo: apect is checked in `change`
        if (!mask_.get(entity) && aspect_.check(entity)) {
            mask_.enable(entity);

            //_system.onEntityAdded(entity, this);
            ++total_;
            changed_ = true;
        }
    }

    void remove_entity(entity_id_t entity) {
#if defined(ECXX_DEBUG)
        assert(debug_mutable_);
#endif
        if (mask_.get(entity)) {
            mask_.disable(entity);

            //_system.onEntityRemoved(entity, this);
            --total_;
            changed_ = true;
        }
    }

    family_aspect_data aspect_;
    bit_vector mask_;
    std::vector<entity_id_t> list_;
    entity_id_t total_ = 0;
    bool changed_ = false;

#if defined(ECXX_DEBUG)
    std::vector<entity_id_t> debug_entities_snapshot_;
    bool debug_mutable_ = false;

    void debug_make_mutable() {
        assert(!debug_mutable_);
        debug_mutable_ = true;

        // Family assert: entity list access violation
        assert(debug_entities_snapshot_.size() == list_.size());

        for (size_t i = 0u; i < debug_entities_snapshot_.size(); ++i) {
            // Family assert: entity list access violation (bad element)
            assert(debug_entities_snapshot_[i] == list_[i]);
        }
    }

    void debug_make_immutable() {
        assert(debug_mutable_);
        debug_mutable_ = false;
        // create immutable copy for checking
        debug_entities_snapshot_.resize(0);
        for (auto e : list_) {
            debug_entities_snapshot_.push_back(e);
        }
    }

#endif

    friend class world_v1_t;
};

}