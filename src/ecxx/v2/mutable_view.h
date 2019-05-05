#pragma once

#include <array>

#include "components_db.h"

namespace ecxx {

template<typename T, typename ...Component>
class basic_mutable_view {
public:
    using entity_type = entity_value<T>;
    using index_type = typename entity_type::index_type;
    static constexpr auto components_num = sizeof ... (Component);
    using map_type = entity_map_base<T>;

    using table_type = std::array<map_type*, components_num>;
    using table_index_type = uint32_t;

    using indices_type = std::array<table_index_type, components_num>;

    using entity_vector_iterator = typename std::vector<entity_type>::iterator;

    class iterator {
    public:
        iterator(table_type& table, uint32_t it)
                : table_{table},
                  it_{it} {
            skips();
        }

        inline iterator& operator++() noexcept {
            --it_;
            skips();
            return *this;
        }

        inline bool operator==(const iterator& other) const {
            return ent_ == other.ent_;
        }

        inline bool operator!=(const iterator& other) const {
            return ent_ != other.ent_;
        }

        inline void skips() {
            // todo: size recovery (case we remove entities before *it)
            assert(it_ < (first_map().size() + 1));

            while (it_ != 0 && !valid(first_map().at(it_))) {
                --it_;
            }
            ent_ = first_map().at(it_);
        }

        inline bool valid(entity_type entity) const {
            for (uint32_t i = 1u; i < components_num; ++i) {
                if (!table_[i]->has(entity)) {
                    return false;
                }
            }
            return true;
        }

        inline const entity_type operator*() const {
            return ent_;
        }

        inline entity_type operator*() {
            return ent_;
        }

        inline const map_type& first_map() const {
            return *table_[0];
        }

    private:
        uint32_t it_;
        entity_type ent_;
        table_type& table_;
    };

    explicit basic_mutable_view(components_db <T>& db) {
        table_index_type i{};
        ((access_[i] = table_[i] = &db.template ensure<Component>(), ++i), ...);

        std::sort(table_.begin(), table_.end(), [this](auto a, auto b) -> bool {
            return a->size() < b->size();
        });
    }

    iterator begin() {
        return {table_, table_[0]->size()};
    }

    iterator end() {
        return {table_, 0};
    }

    template<typename Comp>
    constexpr inline Comp& unsafe_get(table_index_type i, entity_type e) {
        return static_cast<entity_map <T, Comp>*>(access_[i])->get(e);
    }

    template<typename Func>
    void each(Func func) {
        for (auto e : *this) {
            table_index_type i{0u};
            func(unsafe_get<Component>(i++, e)...);
        }
    }

private:
    table_type access_;
    table_type table_;
};

}