#pragma once

#include <vector>
#include <memory>
#include <type_traits>
#include "entity_spec.h"
#include "entity_value.h"
#include "sparse_vector.h"

namespace ecxx {

template<typename EntityType>
class entity_map_base {
public:

    using entity_type = entity_value<EntityType>;
    using index_type = typename entity_type::index_type;

    entity_map_base() {
        entity_.emplace_back();
    }

    virtual ~entity_map_base() = default;

    virtual void emplace_dyn(entity_type) = 0;

    virtual void erase_dyn(entity_type) = 0;
    //virtual void get()

    inline bool has(entity_type e) const {
        return table_.has(e.index());
    }

    inline auto begin() {
        return ++(entity_.begin());
    }

    inline const auto begin() const {
        return ++(entity_.begin());
    }

    inline auto end() {
        return entity_.end();
    }

    inline const auto end() const {
        return entity_.end();
    }

    inline index_type size() const {
        return static_cast<index_type>(entity_.size() - 1);
    }

protected:
    sparse_vector<index_type, 0u> table_;
    std::vector<entity_type> entity_;
};

template<typename EntityType, typename DataType>
class entity_map : public entity_map_base<EntityType> {
public:
    using base_type = entity_map_base<EntityType>;
    using entity_type = entity_value<EntityType>;
    using index_type = typename entity_spec<EntityType>::index_type;
    using data_type = DataType;

    constexpr static bool is_empty_data = std::is_empty<data_type>::value;

    entity_map() {
        // null data
        data_.emplace_back();
    }

    ~entity_map() override = default;

    template<typename ...Args>
    DataType& emplace(entity_type e, Args&& ...args) {

        assert(!base_type::has(e));

        index_type di = base_type::entity_.size();
        base_type::entity_.emplace_back(e);
        base_type::table_.insert(e.index(), di);

        if constexpr (is_empty_data) {
            return data_[0];
        } else {
            if constexpr (std::is_aggregate_v<data_type>) {
                return data_.emplace_back(data_type{args...});
            } else {
                return data_.emplace_back(args...);
            }
        }
    }

    void erase(entity_type e) {
        assert(base_type::has(e));

        const auto index = base_type::table_.get_and_remove(e.index());
        const bool swap_with_back = index < base_type::entity_.size() - 1u;

        if (swap_with_back) {
            const entity_type back_entity = base_type::entity_.back();
            base_type::table_.replace(back_entity.index(), index);
            std::swap(base_type::entity_.back(), base_type::entity_[index]);

            if constexpr (!is_empty_data) {
                std::swap(data_.back(), data_[index]);
            }
        }
        base_type::entity_.pop_back();
        if constexpr (!is_empty_data) {
            data_.pop_back();
        }
    }

    DataType& get(entity_type e) {
        assert(base_type::has(e));
        const index_type index = is_empty_data ? 0u : base_type::table_.at(e.index());
        return data_[index];
    }

    const DataType& get(entity_type e) const {
        assert(base_type::has(e));
        const index_type index = is_empty_data ? 0u : base_type::table_.at(e.index());
        return data_[index];
    }

    void emplace_dyn(entity_type e) override {
        emplace(e);
    }

    void erase_dyn(entity_type e) override {
        erase(e);
    }

private:
    std::vector<data_type> data_;
};

}