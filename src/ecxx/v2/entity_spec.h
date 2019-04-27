#pragma once

#include <cstdint>

namespace ecxx {

template<typename Tag>
struct entity_spec {
};

template<>
struct entity_spec<uint32_t> {
    using value_type = uint32_t;
    using index_type = uint32_t;
    using version_type = uint16_t;

    static constexpr uint32_t index_bits = 20u;
    static constexpr uint32_t index_cap = (1u << index_bits);
    static constexpr uint32_t index_mask = index_cap - 1u;

    static constexpr uint32_t version_bits = 12u;
    static constexpr uint32_t version_cap = (1u << version_bits);
    static constexpr uint32_t version_mask = version_cap - 1u;
};

}