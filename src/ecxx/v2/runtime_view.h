#pragma once

#include <array>

#include "entity_map.h"

namespace ecxx {

template<typename T>
class runtime_view_t {
public:
    using entity_type = entity_value<T>;
    using index_type = typename entity_type::index_type;
    using map_type = entity_map_base<T>;

    using table_type = std::vector<map_type*>;
    using table_index_type = typename table_type::size_type;

    using indices_type = std::vector<table_index_type>;

    using entity_vector_iterator = typename std::vector<entity_type>::iterator;

    class iterator {
    public:
        iterator(table_type& table, entity_vector_iterator it)
                : table_{table},
                  it_{it} {
            skips();
        }

        inline iterator& operator++() noexcept {
            if (it_ != first_map().end() && *it_ == ent_) {
                ++it_;
            }
            skips();
            return *this;
        }

        inline bool operator!=(const iterator& other) const noexcept {
            return ent_ != other.ent_;
        }

        inline void skips() {
            const auto end = first_map().end();
            while (it_ != end && !valid(*it_)) {
                ++it_;
            }
            ent_ = it_ == end ? entity_type::null : *it_;
        }

        inline bool valid(entity_type entity) const {
            for (uint32_t i = 1u; i < table_.size(); ++i) {
                if (!table_[i]->has(entity)) {
                    return false;
                }
            }
            return true;
        }

        inline const entity_type operator*() const noexcept {
            return ent_;
        }

        inline entity_type operator*() noexcept {
            return ent_;
        }

        inline const entity_map_base<T>& first_map() const {
            return *table_[0];
        }
    private:
        entity_vector_iterator it_;
        entity_type ent_;
        table_type& table_;
    };

    explicit runtime_view_t(table_type& table)
            : access_{table},
              table_{table} {

        std::sort(table_.begin(), table_.end(), [this](auto a, auto b) -> bool {
            return a->size() < b->size();
        });
    }

    iterator begin() {
        return {table_, table_[0]->begin()};
    }

    iterator end() {
        return {table_, table_[0]->end()};
    }

    template<typename Func>
    void each(Func func) {
        for (auto e : *this) {
            func(e);
        }
    }

private:
    table_type access_;
    table_type table_;
};

}