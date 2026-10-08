#pragma once

#include "IDictionary.hpp"
#include "DynamicArray.hpp"
#include "HashSlot.hpp"
#include "PolynomialHasher.hpp"
#include "Hash.hpp"

template<typename TKey, typename TElement, typename THash = Hash<TKey>>
class HashTable : public IDictionary<TKey, TElement> {
private:
    static constexpr int DEFAULT_CAPACITY = 32;
    static constexpr double MAX_LOAD_FACTOR = 0.5;

    int table_capacity;
    int count;
    int deleted_count;
    DynamicArray<HashSlot<TKey, TElement>> slots;

    THash key_hasher;
    PolynomialHasher poly_hasher;

    int get_hash(const TKey& key) const {
        uint64_t raw_key = key_hasher(key);
        return static_cast<int>(poly_hasher(raw_key) % table_capacity);
    }

    using Slot = HashSlot<TKey, TElement>;
    using State = typename Slot::State;

    int find(const TKey& key, bool& found) const {
        int base_index = get_hash(key);
        int first_deleted_index = -1;

        for (int i = 0; i < table_capacity; ++i) {
            int current_index = (base_index + i) % table_capacity;
            const auto& slot = slots[current_index];

            if (slot.state == State::empty) {
                found = false;
                return (first_deleted_index != -1) ? first_deleted_index: current_index;
            }

            if (slot.state == State::deleted) {
                if (first_deleted_index == -1) {
                    first_deleted_index = current_index;
                }
            }
            else if (slot.state == State::occupied && slot.key == key) {
                found = true;
                return current_index;
            }
        }
        found = false;
        return first_deleted_index;
    }

    void rehash() {
        int old_capacity = table_capacity;
        DynamicArray<Slot> old_slots = std::move(slots);
        table_capacity *= 2;
        slots = DynamicArray<Slot>(table_capacity);
        for (int i = 0; i < table_capacity; ++i) {
            slots[i] = Slot();
        }

        count = 0;
        deleted_count = 0;

        for (int i = 0; i < old_capacity; ++i) {
            if (old_slots[i].state == State::occupied) {
                insert(old_slots[i].key, old_slots[i].value);
            }
        }
    }

public:
    ~HashTable() override = default;

    explicit HashTable(int initial_capacity = DEFAULT_CAPACITY)
        : table_capacity(initial_capacity > 0 ? initial_capacity : DEFAULT_CAPACITY),
          count(0), deleted_count(0), slots(table_capacity) {}

    HashTable(const HashTable& other) = default;
    HashTable& operator=(const HashTable& other) = default;

    HashTable(HashTable&& other) noexcept :
        table_capacity(other.table_capacity),
        count(other.count),
        deleted_count(other.deleted_count),
        slots(std::move(other.slots)),
        key_hasher (other.key_hasher),
        poly_hasher(other.poly_hasher)
    {
        other.table_capacity = DEFAULT_CAPACITY;
        other.count = 0;
        other.deleted_count = 0;
        other.slots = DynamicArray<Slot>(DEFAULT_CAPACITY);
    }

    HashTable& operator=(HashTable&& other) noexcept {
        if (this != &other) {
            table_capacity = other.table_capacity;
            count = other.count;
            deleted_count = other.deleted_count;
            slots = std::move(other.slots);
            key_hasher = other.key_hasher;
            poly_hasher = other.poly_hasher;

            other.table_capacity = DEFAULT_CAPACITY;
            other.count = 0;
            other.deleted_count = 0;
            other.slots = DynamicArray<Slot>(DEFAULT_CAPACITY);

        }
        return *this;
    }



    void insert(const TKey& key, const TElement& element) override {
        if (static_cast<double>(count + deleted_count + 1) / table_capacity >= MAX_LOAD_FACTOR) {
            rehash();
        }

        bool found = false;
        int index = find(key,found);

        if (found) {
            slots[index].value = element;
        } else {
            if (slots[index].state == State::deleted) {
                --deleted_count;
            }
            slots[index].key = key;
            slots[index].value = element;
            slots[index].state = State::occupied;
            ++count;
        }
    }

    void erase(const TKey& key) override {
        bool found = false;
        int index = find(key, found);

        if (found) {
            slots[index].state = State::deleted;
            --count;
            ++deleted_count;
        }
    }

    bool contains(const TKey& key) const override {
        bool found = false;
        find(key, found);
        return found;
    }

    const TElement& get(const TKey& key) const override {
        bool found = false;
        int index = find(key, found);

        if (!found) {
            throw std::out_of_range("Key not found");
        }
        return slots[index].value;
    }

    TElement& get(const TKey& key) override {
        bool found = false;
        int index = find(key, found);

        if (!found) {
            throw std::out_of_range("Key not found");
        }
        return slots[index].value;
    }


    int size() const override {
        return count;
    }

    int capacity() const override {
        return table_capacity;
    }

    class HashTableIterator : public IIterator<std::pair<TKey, TElement>> {
    private:
        const HashTable& table;
        int current_index;

        void advance() {
            while (current_index < table.table_capacity &&
                   table.slots[current_index].state != State::occupied ) {
                ++current_index;
            }
        }

    public:
        explicit HashTableIterator(const HashTable& t)
            : table(t), current_index(0) {
            advance();
        }

        bool has_next() const override {
            return current_index < table.table_capacity;
        }

        std::pair<TKey, TElement> current() const override {
            if (!has_next()) {
                throw std::out_of_range("Iterator is out of range");
            }
            return {table.slots[current_index].key, table.slots[current_index].value};
        }

        bool next() override {
            if (!has_next()) {
                return false;
            }
            ++current_index;
            advance();
            return has_next();
        }

        bool try_get(std::pair<TKey, TElement>& value) const override {
            if (!has_next()) {
                return false;
            }
            value = {table.slots[current_index].key, table.slots[current_index].value};
            return true;
        }
    };

    std::unique_ptr<IIterator<std::pair<TKey, TElement>>> get_iterator() const override {
        return std::make_unique<HashTableIterator>(*this);
    }
};