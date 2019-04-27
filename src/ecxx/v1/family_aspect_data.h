#pragma once

#include "component_db.h"
#include "decl.h"
#include "bit_vector.h"
#include <array>

namespace ecxx {

class family_aspect_data {
public:
    explicit family_aspect_data(const component_db& db)
            : db_{db} {

    }

    inline bool operator()(entity_id_t entity) const {
        return check(entity);
    }

    bool check(entity_id_t entity) const {
        for (auto* v : inclusion_) {
            if (v->is_false(entity)) {
                return false;
            }
        }
        for (auto* v : exclusion_) {
            if (v->is_true(entity)) {
                return false;
            }
        }
        return true;
    }

    template<typename ComponentTag>
    family_aspect_data& require() {
        using manager_type = typename component_traits<ComponentTag>::manager_type;
        inclusion_.push_back(&db_.bits<manager_type>());
        return *this;
    }

    template<typename ComponentTag>
    family_aspect_data& exclude() {
        using manager_type = typename component_traits<ComponentTag>::manager_type;
        exclusion_.push_back(&db_.bits<manager_type>());
        return *this;
    }

private:
    const component_db& db_;
    std::vector<const bit_vector*> inclusion_;
    std::vector<const bit_vector*> exclusion_;
};

template<typename... Component>
class family_aspect {
public:
    static_assert(sizeof...(Component) > 0);

    using masks_array_type = std::array<const bit_vector*, sizeof...(Component)>;

    explicit family_aspect(const component_db& db) {
        // generate static array of masks
        typename masks_array_type::size_type pos{};
        ((masks_[pos++] = &db.bits<typename component_traits<Component>::manager_type>()), ...);
    }

    inline bool operator()(entity_id_t entity) const {
        return test(entity);
    }

    inline bool test(entity_id_t entity) const {
        for (auto* mask : masks_) {
            if (mask->is_false(entity)) {
                return false;
            }
        }
        return true;
    }

private:
    masks_array_type masks_;
};


}