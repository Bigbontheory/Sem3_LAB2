#pragma once

template<typename T>
class IIterator {
public:
    virtual ~IIterator() = default;

    virtual bool has_next() const  = 0;
    virtual T current() const = 0;
    virtual bool next() = 0;
    virtual bool try_get(T& value) const = 0;

    T operator*() const {
        return current();
    }

    IIterator& operator++() {
        next();
        return *this;
    }

    explicit operator bool() const {
        return has_next();
    }

};