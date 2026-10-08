#pragma once

#include<cstdint>
#include<random>
#include"DynamicArray.hpp"

class PolynomialHasher {
private:
    DynamicArray<uint64_t> coefficients;
    uint64_t prime_mod;

public:
    explicit PolynomialHasher(int degree = 4, uint64_t prime = 2147483647ULL)
        : coefficients(degree + 1), prime_mod(prime) {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist(1, prime_mod - 1);

        for (int i = 0; i <= degree; ++i) {
            coefficients[i] = dist(gen);
        }
    }

    uint64_t operator()(uint64_t x) const {
        int n = coefficients.get_size();
        __uint128_t hash_val = coefficients[n-1] % prime_mod;

        for (int i = n - 2; i >= 0; --i) {
            hash_val = (hash_val * (x % prime_mod) + coefficients[i]) % prime_mod;
        }

        return static_cast<uint64_t>(hash_val);
    }
};
