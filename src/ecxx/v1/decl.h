#pragma once

#include <cstdint>

namespace ecxx {

using world_id_t = uint8_t;
//using entity_id_t = uint16_t;
using entity_id_t = uint32_t;
using version_tag_t = uint16_t;
inline constexpr entity_id_t null_entity = 0u;
using component_id_t = uint16_t;

template<typename ComponentTag>
class generic_component_manager;

template<typename ComponentTag>
class component_traits {
public:
    using manager_type = generic_component_manager<ComponentTag>;
    using reference = ComponentTag&;
    using const_reference = const ComponentTag&;
};


class world_v1_t;

class entity_t;

class component_db;

class worlds_map;

template<typename F>
class basic_view;

class managed_family;

}