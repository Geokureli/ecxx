#pragma once

#include <cstdint>
#include <vector>
#include "entity_value.h"

namespace ecxx {

template<typename Tag = uint32_t>
class entity_pool_t {
public:

    using spec = entity_spec<Tag>;
    using value_type = entity_value<Tag>;
    using index_type = typename spec::index_type;
    using version_type = typename spec::version_type;

    entity_pool_t() {
        list_.emplace_back(value_type::null);
    }

    value_type allocate() {
        if (available_) {
            value_type node = list_[next_];
            value_type e{next_, node.version()};
            next_ = node.index();
            list_[e.index()] = e;
            --available_;
            return e;
        } else {
            value_type e{static_cast<index_type>(list_.size())};
            list_.emplace_back(e);
            return e;
        }
    }

    template<typename It>
    void allocate(It begin, const It end) {
        while (begin != end) {
            // TODO: batch
            *begin = allocate();
            ++begin;
        }
    }

    void deallocate(value_type entity) {
        const auto i = entity.index();
        list_[i] = {next_, static_cast<version_type>(entity.version() + 1u)};
        next_ = i;
        ++available_;
    }

    inline void reserve(size_t size) {
        list_.reserve(size + 1);
    }

    inline index_type size() const {
        return static_cast<index_type>(list_.size() - available_ - 1u);
    }

    /*
     * query current version for entity index
     * index could be:
     *  - alive (index match #slot)
     *  - dead (index doesn't match #slot)
     *  - null
     */
    inline version_type current(index_type i) const {
        //assert(is_alive(i));
        return list_[i].version();
    }

    inline bool is_alive(index_type i) const {
        return i < list_.size() && i == list_[i].index();
    }


    inline index_type available_for_recycling() const {
        return available_;
    }

    template<typename Func>
    void each(Func func) const {
        static_assert(std::is_invocable_v<Func, value_type>);

        if (available_) {
            const index_type end = list_.size();
            for (index_type i = 1u; i != end; ++i) {
                const value_type e = list_[i];
                if (e.index() == i) {
                    func(e);
                }
            }
        } else {
            const index_type end = list_.size();
            for (index_type i = 1u; i != end; ++i) {
                func(list_[i]);
            }
        }
    }

private:
    index_type available_{0u};
    index_type next_{0u};
    std::vector<value_type> list_;
};

}