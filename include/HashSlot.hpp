#pragma once
#include <cstdint>

template<typename TKey, typename TElement>
struct HashSlot {
    enum class State: uint8_t {
        empty,
        occupied,
        deleted,
    };

    TKey key{};
    TElement value{};
    State state = State::empty;

    HashSlot() = default;

    HashSlot(const TKey& k, const TElement& v, State s ) :
        key(k), value(v), state(s) {}
};