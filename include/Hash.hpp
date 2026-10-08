#pragma once

#include<cstdint>
#include<string>

template <typename T>
struct Hash;

template <>
struct Hash<int> {
    uint64_t operator()(int value) const {
        return static_cast<uint64_t>(value);
    }
};

template <>
struct Hash<uint64_t> {
    uint64_t operator()(uint64_t value) const {
        return value;
    }
};

template<>
struct Hash<std::string> {
    uint64_t operator()(const std::string& str) const {
        uint64_t hash = 5381;
        for (unsigned char c: str) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
};
