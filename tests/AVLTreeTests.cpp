#include <gtest/gtest.h>
#include <string>
#include "DynamicArray.hpp"
#include "AVLTree.hpp"

TEST(AVLTreeTest, EmptyTreeState) {
    AVLTree<int, std::string> tree;

    EXPECT_EQ(tree.size(), 0);
    EXPECT_EQ(tree.capacity(), 0);
    EXPECT_FALSE(tree.contains(10));

    EXPECT_THROW(tree.get(10), std::out_of_range);
}

TEST(AVLTreeTest, InsertAndGet) {
    AVLTree<int, std::string> tree;

    tree.insert(10, "Ten");
    tree.insert(20, "Twenty");
    tree.insert(5, "Five");

    EXPECT_EQ(tree.size(), 3);
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_FALSE(tree.contains(100));

    EXPECT_EQ(tree.get(10), "Ten");
    EXPECT_EQ(tree.get(20), "Twenty");
    EXPECT_EQ(tree.get(5), "Five");
}

TEST(AVLTreeTest, ModifyViaGet) {
    AVLTree<int, std::string> tree;
    tree.insert(1, "One");

    tree.get(1) = "Uno";
    EXPECT_EQ(tree.get(1), "Uno");
}

TEST(AVLTreeTest, EraseLeafNode) {
    AVLTree<int, std::string> tree;
    tree.insert(10, "10");
    tree.insert(5, "5");
    tree.insert(15, "15");

    tree.erase(5);

    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(10));
    EXPECT_TRUE(tree.contains(15));
}

TEST(AVLTreeTest, EraseNodeWithOneChild) {
    AVLTree<int, std::string> tree;
    tree.insert(10, "10");
    tree.insert(5, "5");
    tree.insert(3, "3");

    tree.erase(5);

    EXPECT_EQ(tree.size(), 2);
    EXPECT_FALSE(tree.contains(5));
    EXPECT_TRUE(tree.contains(3));
    EXPECT_EQ(tree.get(3), "3");
}

TEST(AVLTreeTest, EraseNodeWithTwoChildren) {
    AVLTree<int, std::string> tree;
    tree.insert(20, "20");
    tree.insert(10, "10");
    tree.insert(30, "30");
    tree.insert(5, "5");
    tree.insert(15, "15");

    tree.erase(10);

    EXPECT_EQ(tree.size(), 4);
    EXPECT_FALSE(tree.contains(10));
    EXPECT_TRUE(tree.contains(5));
    EXPECT_TRUE(tree.contains(15));
    EXPECT_TRUE(tree.contains(20));
    EXPECT_TRUE(tree.contains(30));
}

TEST(AVLTreeTest, SequentialInsertMaintainsBalance) {
    AVLTree<int, int> tree;

    for (int i = 1; i <= 100; ++i) {
        tree.insert(i, i * 10);
    }

    EXPECT_EQ(tree.size(), 100);
    for (int i = 1; i <= 100; ++i) {
        EXPECT_TRUE(tree.contains(i));
        EXPECT_EQ(tree.get(i), i * 10);
    }
}

TEST(AVLTreeTest, IteratorInOrderTraversal) {
    AVLTree<int, std::string> tree;

    tree.insert(50, "50");
    tree.insert(30, "30");
    tree.insert(70, "70");
    tree.insert(20, "20");
    tree.insert(40, "40");
    tree.insert(60, "60");
    tree.insert(80, "80");

    auto it = tree.get_iterator();
    DynamicArray<int> sorted_keys;

    while (it->has_next()) {
        int sz = sorted_keys.get_size();
        sorted_keys.resize(sz + 1);
        sorted_keys[sz] = it->current().first;
        it->next();
    }

    int expected_data[] = {20, 30, 40, 50, 60, 70, 80};
    DynamicArray<int> expected(expected_data, 7);

    EXPECT_EQ(sorted_keys, expected);
}

TEST(AVLTreeTest, IteratorTryGet) {
    AVLTree<int, std::string> tree;
    tree.insert(1, "One");
    tree.insert(2, "Two");

    auto it = tree.get_iterator();
    std::pair<int, std::string> current_val;

    EXPECT_TRUE(it->try_get(current_val));
    EXPECT_EQ(current_val.first, 1);
    EXPECT_EQ(current_val.second, "One");
    it->next();

    EXPECT_TRUE(it->try_get(current_val));
    EXPECT_EQ(current_val.first, 2);
    EXPECT_EQ(current_val.second, "Two");
    it->next();

    EXPECT_FALSE(it->try_get(current_val));
}

TEST(AVLTreeTest, IteratorEmptyExceptions) {
    AVLTree<int, std::string> tree;
    auto it = tree.get_iterator();

    EXPECT_FALSE(it->has_next());
    EXPECT_THROW(it->current(), std::out_of_range);
}

TEST(AVLTreeTest, CopyConstructorCreatesDeepCopy) {
    AVLTree<int, std::string> tree1;
    tree1.insert(10, "A");
    tree1.insert(20, "B");

    AVLTree<int, std::string> tree2(tree1);

    EXPECT_EQ(tree2.size(), 2);
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(20));

    tree2.get(10) = "A_modified";
    tree2.insert(30, "C");

    EXPECT_EQ(tree1.get(10), "A");
    EXPECT_FALSE(tree1.contains(30));
    EXPECT_EQ(tree1.size(), 2);
    EXPECT_EQ(tree2.size(), 3);
}

TEST(AVLTreeTest, CopyAssignmentCreatesDeepCopy) {
    AVLTree<int, std::string> tree1;
    tree1.insert(1, "One");

    AVLTree<int, std::string> tree2;
    tree2.insert(99, "Ninety-Nine");

    tree2 = tree1;

    EXPECT_EQ(tree2.size(), 1);
    EXPECT_TRUE(tree2.contains(1));
    EXPECT_FALSE(tree2.contains(99));

    tree2.get(1) = "Uno";
    EXPECT_EQ(tree1.get(1), "One");
}

TEST(AVLTreeTest, MoveConstructorTransfersOwnership) {
    AVLTree<int, std::string> tree1;
    tree1.insert(10, "Ten");
    tree1.insert(20, "Twenty");

    AVLTree<int, std::string> tree2(std::move(tree1));

    EXPECT_EQ(tree2.size(), 2);
    EXPECT_TRUE(tree2.contains(10));
    EXPECT_TRUE(tree2.contains(20));

    EXPECT_EQ(tree1.size(), 0);
    EXPECT_FALSE(tree1.contains(10));
}

TEST(AVLTreeTest, MoveAssignmentTransfersOwnership) {
    AVLTree<int, std::string> tree1;
    tree1.insert(5, "Five");

    AVLTree<int, std::string> tree2;
    tree2.insert(100, "Hundred");

    tree2 = std::move(tree1);

    EXPECT_EQ(tree2.size(), 1);
    EXPECT_TRUE(tree2.contains(5));
    EXPECT_FALSE(tree2.contains(100));

    EXPECT_EQ(tree1.size(), 0);
}