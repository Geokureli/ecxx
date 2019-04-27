#pragma once

#include "decl.h"
#include "bit_vector.h"
#include "family_aspect_data.h"

namespace ecxx {

class mask_filter {
public:
    explicit mask_filter(const bit_vector& mask)
            : mask_{mask} {

    }

    inline bool operator()(entity_id_t entity) const {
        return mask_.is_true(entity);
    }

private:
    const bit_vector& mask_;
};

template<typename Aspect>
class component_mask_filter_ref {
public:
    explicit component_mask_filter_ref(const bit_vector& active, const Aspect& aspect)
            : active_{active},
              aspect_{aspect} {

    }

    inline bool operator()(entity_id_t entity) const {
        return active_.is_true(entity) && aspect_(entity);
    }

private:
    const bit_vector& active_;
    const Aspect& aspect_;
};

template<typename Aspect>
class component_mask_filter {
public:
    explicit component_mask_filter(const bit_vector& active, const Aspect aspect)
            : active_{active},
              aspect_{aspect} {

    }

    inline bool operator()(entity_id_t entity) const {
        return active_.is_true(entity) && aspect_(entity);
    }

private:
    const bit_vector& active_;
    const Aspect aspect_;
};

}