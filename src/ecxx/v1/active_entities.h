#pragma once

#include "world.h"
#include "bit_vector.h"

namespace ecxx {

template<typename F>
class basic_view {
    class iterator {
    public:
        iterator(const world_v1_t& world, const F& filter, entity_id_t id)
                : world_{world},
                  filter_{filter},
                  id_{id} {
            skip_els();
        }

        iterator& operator++() {
            ++id_;
            skip_els();
//            while (id_ < world_.end_ && !filter_(id_)) {
//                ++id_;
//            }
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

        inline void skip_els() {
            while (id_ < world_.end_ && !filter_(id_)) {
                ++id_;
            }
        }

    private:
        const world_v1_t& world_;
        const F& filter_;
        entity_id_t id_{1};
    };

public:
    iterator begin() {
        return {world_, filter_, 1u};
    }

    iterator end() {
        return {world_, filter_, world_.end_};
    }

    explicit basic_view(const world_v1_t& world, F filter)
            : world_{world},
              filter_{filter} {

    }

//    inline entity_id_t size() const {
//        return world_.used_ - 1u;
//    }

private:
    const world_v1_t& world_;
    F filter_;
};

}