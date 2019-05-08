#pragma once

#include <cstddef>
#include "entity_spec.h"

namespace ecxx {

template<typename T>
class entity_value {
public:
    using spec = entity_spec<T>;
    using value_type = typename spec::value_type;
    using index_type = typename spec::index_type;
    using version_type = typename spec::version_type;

    inline static value_type null_value;
    inline static entity_value<T> null{};

    inline constexpr entity_value() noexcept
            : value_{null_value} {
    }

    inline constexpr explicit entity_value(value_type v) noexcept
            : value_{v} {

    }

    inline constexpr entity_value(index_type i, version_type v) noexcept
            : value_{i | (v << spec::index_bits)} {

    }

    inline constexpr version_type version() const noexcept {
        return (value_ >> spec::index_bits) & spec::version_mask;
    }

    inline constexpr void version(version_type v) noexcept {
        value_ = (value_ & spec::index_mask) | (v << spec::index_bits);
    }

    inline constexpr index_type index() const noexcept {
        return value_ & spec::index_mask;
    }

    inline constexpr void index(index_type v) noexcept {
        value_ = (value_ & ~spec::index_mask) | v;
    }

    inline bool operator==(entity_value<T> other) const {
        return value_ == other.value_;
    }

    inline bool operator!=(entity_value<T> other) const {
        return value_ != other.value_;
    }

    inline bool operator==(std::nullptr_t) const {
        return value_ == null_value;
    }

    inline bool operator!=(std::nullptr_t) const {
        return value_ != null_value;
    }

    inline explicit operator bool() const {
        return value_ != null_value;
    }

private:
    value_type value_;
};

}