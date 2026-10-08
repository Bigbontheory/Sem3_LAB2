#pragma once
#include <stdexcept>

template <typename TKey, typename TElement>
int AVLTree<TKey, TElement>::get_height(AVLNode* node) const noexcept {
    return node ? node-> height : 0;
}

template <typename TKey, typename TElement>
void AVLTree<TKey, TElement>:: update_height(AVLNode* node) noexcept {
    if (node) {
        node->height = 1 + std::max(get_height(node->left.get()), get_height(node->right.get()));
    }
}

template<typename TKey, typename TElement>
int AVLTree<TKey, TElement>::get_balance_factor(AVLNode* node) const noexcept {
    if (!node) return 0;
    return get_height(node->right.get()) - get_height(node->left.get());
}


template <typename TKey, typename TElement>
void AVLTree<TKey, TElement>::rotate_right(NodePtr& node){
    NodePtr pivot = std::move(node->left);
    node->left = std::move(pivot->right);

    AVLNode* old_root = node.get();
    pivot->right = std::move(node);

    node = std::move(pivot);

    update_height(old_root);
    update_height(node.get());

}

template<typename TKey, typename TElement>
void AVLTree<TKey, TElement>::rotate_left(NodePtr& node) {
    NodePtr pivot = std::move(node->right);
    node->right = std::move(pivot->left);

    AVLNode* old_root = node.get();
    pivot->left = std::move(node);
    node = std::move(pivot);

    update_height(old_root);
    update_height(node.get());
}


template<typename TKey, typename TElement>
void AVLTree<TKey, TElement>::balance(NodePtr& node) {
    if (!node) {
        return;
    }

    update_height(node.get());
    int bf = get_balance_factor(node.get());

    if (bf == -2) {
        if (get_balance_factor(node->left.get()) > 0) {
            rotate_left(node->left);
        }
        rotate_right(node);
    }
    else if (bf == 2) {
        if (get_balance_factor(node->right.get()) < 0) {
            rotate_right(node->right);
        }
        rotate_left(node);
    }
}

template <typename TKey,typename TElement>
void AVLTree<TKey, TElement>::insert(NodePtr& node, const TKey& key, const TElement& value) {
    if (!node) {
        node = std::make_unique<AVLNode>(key, value);
        ++count;
        return;
    }

    if (key < node->key) {
        insert(node->left, key, value);
    } else if (key > node->key) {
        insert(node->right, key, value);
    } else if (key == node->key) {
        node->value = value;
        return;
    }
    balance(node);
}

template<typename TKey, typename TElement>
auto AVLTree<TKey, TElement>::find_min(AVLNode* node ) const -> AVLNode* {
    if (!node) return nullptr;
    while (node->left) {
        node = node->left.get();
    }
    return node;
}

template<typename TKey, typename TElement>
void AVLTree<TKey, TElement>::erase(NodePtr& node, const TKey& key) {
    if (!node) return;

    if (key < node->key) {
        erase(node->left, key);
    } else if (key > node->key) {
        erase(node->right, key);
    } else {

        if (!node->left) {
            node = std::move(node->left);
            --count;
        }
        else if (!node->right) {
            node = std::move(node->left);
            --count;
        }
        else {
            AVLNode* min_node = find_min(node->right.get());
            node->key = min_node->key;
            node->value = min_node->value;
            erase(node->right, min_node->key);

        }
    }
    balance(node);
}

template<typename TKey, typename TElement>
auto AVLTree<TKey, TElement>::find_node(const TKey& key) const -> AVLNode* {
    AVLNode* curr = root.get();
    while (curr) {
        if (key < curr->key) {
            curr = curr->left.get();
        } else if (key > curr-> key) {
            curr = curr->right.get();
        } else if (key == curr->key) {
            return curr;
        }
    }
    return nullptr;
}

template <typename TKey, typename TElement>
auto AVLTree<TKey, TElement>::clone(const NodePtr& node) const-> NodePtr {
    if (!node) return nullptr;
    NodePtr new_node(new AVLNode(node->key, node->value));
    new_node->height = node->height;
    new_node->left = clone(node->left);
    new_node->right = clone(node->right);
    return new_node;
}


