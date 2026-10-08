#pragma once

#include <memory>
#include <utility>
#include "IIterator.hpp"

template <typename TKey, typename TElement>
class IDictionary {
public:
    virtual ~IDictionary() = default;

    virtual int size() const = 0;
    virtual int capacity() const = 0;

    virtual const TElement& get(const TKey& key) const = 0;
    virtual TElement& get(const TKey& key) = 0;
    
    virtual bool contains(const TKey& key) const = 0;
    virtual void insert(const TKey& key, const TElement& element) = 0;
    virtual void erase(const TKey&) = 0;
    
    virtual std::unique_ptr<IIterator<std::pair<TKey, TElement>>> get_iterator() const = 0;
};