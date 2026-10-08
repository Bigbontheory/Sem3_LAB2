#include <gtest/gtest.h>
#include <string>
#include <set>
#include <stdexcept>
#include "HashTable.hpp"


TEST(HashTableDetailedTest, EmptyTableState) {
    HashTable<int, std::string> table;
    EXPECT_EQ(table.size(), 0);
    EXPECT_GE(table.capacity(), 32);
    EXPECT_FALSE(table.contains(42));
    EXPECT_THROW(table.get(42), std::out_of_range);
}

TEST(HashTableDetailedTest, InsertAndOverwrite) {
    HashTable<int, std::string> table;

    table.insert(1, "initial");
    EXPECT_EQ(table.size(), 1);
    EXPECT_TRUE(table.contains(1));
    EXPECT_EQ(table.get(1), "initial");

    table.insert(1, "updated");
    EXPECT_EQ(table.size(), 1);
    EXPECT_EQ(table.get(1), "updated");
}

TEST(HashTableDetailedTest, EraseNonExistentDoesNotAffectSize) {
    HashTable<int, int> table;
    table.insert(1, 10);
    table.insert(2, 20);

    table.erase(999);
    EXPECT_EQ(table.size(), 2);
    EXPECT_TRUE(table.contains(1));
    EXPECT_TRUE(table.contains(2));
}

struct ConstantZeroHasher {
    uint64_t operator()(int) const {
        return 0;
    }
};

TEST(HashTableDetailedTest, HandlesForcedCollisions) {
    HashTable<int, int, ConstantZeroHasher> table(16);

    for (int i = 1; i <= 8; ++i) {
        table.insert(i, i * 100);
    }

    EXPECT_EQ(table.size(), 8);

    for (int i = 1; i <= 8; ++i) {
        EXPECT_TRUE(table.contains(i));
        EXPECT_EQ(table.get(i), i * 100);
    }
}

TEST(HashTableDetailedTest, DeletedSlotDoesNotBreakSearchChain) {
    HashTable<int, std::string, ConstantZeroHasher> table(16);

    table.insert(1, "A");
    table.insert(2, "B");
    table.insert(3, "C");

    table.erase(2);

    EXPECT_EQ(table.size(), 2);
    EXPECT_TRUE(table.contains(1));
    EXPECT_FALSE(table.contains(2));
    EXPECT_TRUE(table.contains(3));
    EXPECT_EQ(table.get(3), "C");
}

TEST(HashTableDetailedTest, ReuseDeletedSlotOnNewInsert) {
    HashTable<int, std::string, ConstantZeroHasher> table(16);

    table.insert(1, "first");
    table.insert(2, "second");

    table.erase(1);
    EXPECT_FALSE(table.contains(1));

    table.insert(3, "third");
    EXPECT_EQ(table.size(), 2);
    EXPECT_TRUE(table.contains(3));
    EXPECT_TRUE(table.contains(2));
    EXPECT_EQ(table.get(3), "third");
}

TEST(HashTableDetailedTest, StressInsertAndMultipleRehashes) {
    HashTable<int, int> table(8);
    const int N = 1000;

    for (int i = 0; i < N; ++i) {
        table.insert(i, i * 2);
    }

    EXPECT_EQ(table.size(), N);
    EXPECT_GE(table.capacity(), N);

    for (int i = 0; i < N; ++i) {
        ASSERT_TRUE(table.contains(i)) << "Key missing: " << i;
        ASSERT_EQ(table.get(i), i * 2);
    }

    for (int i = 0; i < N; i += 2) {
        table.erase(i);
    }

    EXPECT_EQ(table.size(), N / 2);

    for (int i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            EXPECT_FALSE(table.contains(i));
        } else {
            EXPECT_TRUE(table.contains(i));
            EXPECT_EQ(table.get(i), i * 2);
        }
    }
}

TEST(HashTableDetailedTest, IteratorOnEmptyTable) {
    HashTable<int, int> table;
    auto it = table.get_iterator();

    EXPECT_FALSE(it->has_next());
    EXPECT_THROW(it->current(), std::out_of_range);

    std::pair<int, int> val;
    EXPECT_FALSE(it->try_get(val));
    EXPECT_FALSE(it->next());
}

TEST(HashTableDetailedTest, IteratorTraversesAllOccupiedSlots) {
    HashTable<std::string, int> table;
    table.insert("one", 1);
    table.insert("two", 2);
    table.insert("three", 3);
    table.insert("four", 4);

    table.erase("three");

    std::set<std::string> expected_keys = {"one", "two", "four"};
    std::set<std::string> iterated_keys;

    auto it = table.get_iterator();
    while (it->has_next()) {
        std::pair<std::string, int> pair;
        EXPECT_TRUE(it->try_get(pair));
        iterated_keys.insert(pair.first);
        it->next();
    }

    EXPECT_EQ(iterated_keys, expected_keys);
}

TEST(HashTableDetailedTest, IteratorThrowAfterEnd) {
    HashTable<int, int> table;
    table.insert(10, 100);

    auto it = table.get_iterator();
    EXPECT_TRUE(it->has_next());
    EXPECT_EQ(it->current().first, 10);

    it->next();
    EXPECT_FALSE(it->has_next());
    EXPECT_THROW(it->current(), std::out_of_range);
}

TEST(HashTableLifecycleTest, DefaultConstructor) {
    HashTable<int, std::string> table;

    EXPECT_EQ(table.size(), 0);
    EXPECT_GT(table.capacity(), 0);
    EXPECT_FALSE(table.contains(1));
    EXPECT_THROW(table.get(1), std::out_of_range);
}

TEST(HashTableLifecycleTest, CopyConstructor) {
    HashTable<int, std::string> original;
    original.insert(1, "Alice");
    original.insert(2, "Bob");
    original.insert(3, "Charlie");


    HashTable<int, std::string> copy(original);

    EXPECT_EQ(copy.size(), original.size());
    EXPECT_TRUE(copy.contains(1));
    EXPECT_TRUE(copy.contains(2));
    EXPECT_TRUE(copy.contains(3));
    EXPECT_EQ(copy.get(1), "Alice");
    EXPECT_EQ(copy.get(2), "Bob");

    copy.insert(4, "David");
    copy.erase(1);


    EXPECT_EQ(copy.size(), 3);
    EXPECT_FALSE(copy.contains(1));
    EXPECT_TRUE(copy.contains(4));

    EXPECT_EQ(original.size(), 3);
    EXPECT_TRUE(original.contains(1));
    EXPECT_FALSE(original.contains(4));
    EXPECT_EQ(original.get(1), "Alice");
}

TEST(HashTableLifecycleTest, CopyAssignmentOperator) {
    HashTable<int, std::string> table1;
    table1.insert(10, "Ten");
    table1.insert(20, "Twenty");

    HashTable<int, std::string> table2;
    table2.insert(99, "Ninety-Nine");


    table2 = table1;

    EXPECT_EQ(table2.size(), 2);
    EXPECT_TRUE(table2.contains(10));
    EXPECT_TRUE(table2.contains(20));
    EXPECT_FALSE(table2.contains(99));
    EXPECT_EQ(table2.get(10), "Ten");

    table1 = table1;
    EXPECT_EQ(table1.size(), 2);
    EXPECT_TRUE(table1.contains(10));
    EXPECT_TRUE(table1.contains(20));
}

TEST(HashTableLifecycleTest, MoveConstructor) {
    HashTable<int, std::string> source;
    source.insert(100, "One Hundred");
    source.insert(200, "Two Hundred");

    int source_prev_size = source.size();

    HashTable<int, std::string> destination(std::move(source));

    EXPECT_EQ(destination.size(), source_prev_size);
    EXPECT_TRUE(destination.contains(100));
    EXPECT_TRUE(destination.contains(200));
    EXPECT_EQ(destination.get(100), "One Hundred");


    EXPECT_EQ(source.size(), 0);
    EXPECT_FALSE(source.contains(100));
    EXPECT_THROW(source.get(100), std::out_of_range);

    source.insert(300, "Three Hundred");
    EXPECT_EQ(source.size(), 1);
    EXPECT_TRUE(source.contains(300));
    EXPECT_EQ(source.get(300), "Three Hundred");
}

TEST(HashTableLifecycleTest, MoveAssignmentOperator) {
    HashTable<int, std::string> source;
    source.insert(5, "Five");
    source.insert(6, "Six");

    HashTable<int, std::string> destination;
    destination.insert(1, "One");


    destination = std::move(source);

    // Проверяем данные в destination
    EXPECT_EQ(destination.size(), 2);
    EXPECT_TRUE(destination.contains(5));
    EXPECT_TRUE(destination.contains(6));
    EXPECT_FALSE(destination.contains(1));
    EXPECT_EQ(destination.get(5), "Five");

    EXPECT_EQ(source.size(), 0);
    EXPECT_FALSE(source.contains(5));

    destination = std::move(destination);
    EXPECT_EQ(destination.size(), 2);
    EXPECT_TRUE(destination.contains(5));
    EXPECT_TRUE(destination.contains(6));
}
