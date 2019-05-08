#pragma once

#include "entity_wrapper.h"
#include "world.h"

namespace ecxx {

template<typename T>
inline bool entity_wrapper<T>::is_valid() const {
    return world_.pool().current(entity_.index()) == entity_.version();
}

template<typename T>
inline void entity_wrapper<T>::destroy() {
    world_.destroy(entity_);
    entity_ = entity_value<T>::null;
}

template<typename T>
template<typename Component, typename ...Args>
inline Component& entity_wrapper<T>::set(Args&& ... args) {
    return world_.template assign<Component>(entity_, args...);
}

template<typename T>
template<typename Component>
inline bool entity_wrapper<T>::has() const {
    return world_.template has<Component>(entity_);
}

template<typename T>
template<typename Component>
inline Component& entity_wrapper<T>::get() {
    return world_.template get<Component>(entity_);
}

template<typename T>
template<typename Component>
inline const Component& entity_wrapper<T>::get() const {
    return world_.template get<Component>(entity_);
}

template<typename T>
template<typename Component>
inline void entity_wrapper<T>::unset() {
    world_.template remove<Component>(entity_);
}

}