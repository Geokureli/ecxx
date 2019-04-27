#pragma once

#include <vector>
#include "decl.h"
#include "bit_vector.h"
#include <ecxx/identity_generator.h>
#include <functional>

namespace ecxx {

class component_db {
public:
    component_db() = default;

    ~component_db() {
        for (auto& clean : cleaners_) {
            clean();
        }
    }

    template<typename ComponentStorageType>
    void register_type(ComponentStorageType* storage) {
        const auto ci = identity_generator<ComponentStorageType, component_id_t>::value;
        if (ci >= matrix_.size()) {
            matrix_.resize(ci + 1);
            storage_.resize(ci + 1);
        }
        assert(matrix_[ci] == nullptr);
        assert(storage_[ci] == nullptr);
        storage_[ci] = storage;
        matrix_[ci] = &storage->mask();
        cleaners_.push_back([storage] {
            delete storage;
        });
    }

    template<typename ComponentStorageType>
    const ComponentStorageType& storage() const {
        return storage_impl<ComponentStorageType>();
    }

    template<typename ComponentStorageType>
    ComponentStorageType& storage() {
        return const_cast<ComponentStorageType&>(storage_impl<ComponentStorageType>());
    }

    template<typename ComponentTag>
    const typename component_traits<ComponentTag>::manager_type& manager() const {
        using manager_type = typename component_traits<ComponentTag>::manager_type;
        return storage_impl<manager_type>();
    }

    template<typename ComponentTag>
    typename component_traits<ComponentTag>::manager_type& manager() {
        using manager_type = typename component_traits<ComponentTag>::manager_type;
        return const_cast<manager_type&>(storage_impl<manager_type>());
    }

    template<typename ComponentStorageType>
    const bit_vector& bits() const {
        return bits_impl<ComponentStorageType>();
    }

    template<typename ComponentStorageType>
    bit_vector& bits() {
        return const_cast<bit_vector&>(bits_impl<ComponentStorageType>());
    }

    template<typename ComponentStorageType>
    inline bool has(entity_id_t entity) const {
        return bits<ComponentStorageType>()[entity];
    }

    template<typename ComponentStorageType, typename ...Args>
    inline ComponentStorageType& enable(entity_id_t entity, Args&& ... args) {
        bits<ComponentStorageType>().enable(entity);
        auto& s = storage<ComponentStorageType>();
        s.initialize(entity, args...);
        return s;
    }

    template<typename ComponentStorageType>
    inline void disable(entity_id_t entity) {
        bits<ComponentStorageType>().disable(entity);
        storage<ComponentStorageType>().reset(entity);
    }

    void disable_all(entity_id_t entity) {
        for (size_t i = 0; i < matrix_.size(); ++i) {
            auto* mask = matrix_[i];
            if (mask != nullptr && mask->get(entity)) {
                // TODO: could be implemented as pre-registred `disablers` map of callbacks to manager
                //storage_[i]->destroy();
                mask->disable(entity);
            }
        }
    }

private:

    template<typename ComponentStorageType>
    const ComponentStorageType& storage_impl() const {
        const auto component_id = identity_generator<ComponentStorageType, component_id_t>::value;
        assert(component_id < storage_.size());
        const void* ptr = storage_[component_id];
        assert(ptr != nullptr);
        return *static_cast<const ComponentStorageType*>(ptr);
    }

    template<typename ComponentStorageType>
    const bit_vector& bits_impl() const {
        const auto component_id = identity_generator<ComponentStorageType, component_id_t>::value;
        assert(component_id < matrix_.size());
        const bit_vector* ptr = matrix_[component_id];
        assert(ptr != nullptr);
        return *ptr;
    }

    std::vector<bit_vector*> matrix_;
    std::vector<void*> storage_;
    std::vector<std::function<void()>> cleaners_;
    //std::vector<std::function<void()>> disablers_;
};
}