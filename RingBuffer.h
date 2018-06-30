#ifndef RINGBUFFER_RINGBUFFER_H
#define RINGBUFFER_RINGBUFFER_H

#include <cstddef>
#include <iterator>

template<class T>
struct Iterator;

template<class T>
struct RingBuffer;

template<class T>
void swap(RingBuffer<T>& first, RingBuffer<T>& second) {
    std::swap(first.head, second.head);
    std::swap(first.sz, second.sz);
    std::swap(first.capacity, second.capacity);
    std::swap(first.data, second.data);
}

template<class T>
class RingBuffer {
    size_t head{}, sz{}, capacity{};
    T *data;

    void ensure_capacity() {
        if (sz < capacity) { return; }
        RingBuffer nw(capacity * 2 + 1);
        for (const T& element : *this)
            nw.push_back(element);
        swap(nw, *this);
    }


public:
    using iterator = Iterator<T>;
    using const_iterator = Iterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    T& operator[](size_t i) { return *iterator(data, i, head, capacity); }
    T& operator[](size_t i) const { return *iterator(data, i, head, capacity); }

    explicit RingBuffer(size_t capacity = 0) : head(0), sz(0), capacity(capacity), data((T *)malloc(sizeof(T) * capacity)) {}

    RingBuffer(const std::initializer_list<T> init): RingBuffer(init.size()) {
        for (const T& x : init)
            push_back(x);
    }
    RingBuffer(const RingBuffer& second) : RingBuffer(second.capacity) {
        for (const T& x : second)
            push_back(x);
    }
    ~RingBuffer() {
        for(auto &el : *this) {
            el.~T();
        }
        delete (uint8_t*)data;
    }

    RingBuffer& operator=(const RingBuffer &second) {
        RingBuffer nw(second);
        swap(*this, nw);
        return *this;
    }

    inline size_t size() const {
        return sz;
    }
    inline bool empty() const {
        return sz == 0;
    }
    void clear() {
        for(auto& el : *this) {
            el.~T();
        }
        sz = 0;
        head = 0;
        capacity = 4;
    }

    iterator begin() { return iterator(data, 0, head, capacity); }
    const_iterator begin() const { return const_iterator(data, 0, head, capacity); }
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    iterator end() { return iterator(data, sz, head, capacity); }
    const_iterator end() const { return const_iterator(data, sz, head, capacity); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    void push_back(const T& el) {
        ensure_capacity();
        new(&data[ (head+sz) % capacity]) T(el);
        ++sz;
    }

    void push_front(const T& el) {
        ensure_capacity();
        size_t nw = (head + capacity - 1) % capacity;
        new(&data[nw]) T(el);
        head = nw;
        ++sz;
    }

    iterator insert(const_iterator pos, const T& el) {
        if (pos.id > sz / 2) {
            push_back(el);
            for(auto x = end() - 1; x.id != pos.id; x--) {
                *x = *std::prev(x);
            }
            (*this)[pos.id] = el;
        }
        else {
            push_front(el);
            for(auto x = begin(); x.id != pos.id; x++) {
                *x = *std::next(x);
            }
            (*this)[pos.id] = el;
        }
        return iterator(data, pos.id, head, capacity);
    }

    void pop_back() {
        data[(head + sz - 1) % capacity].~T();
        --sz;
    }
    void pop_front() {
        data[head].~T();
        ++head %= capacity;
        --sz;
    }

    iterator erase(const_iterator pos) {
        if (pos.id > sz / 2) {
            for(auto x = iterator(data, pos.id, head, capacity); x != end() - 1; x++) {
                *x = *std::next(x);
            }
            pop_back();
        }
        else {
            for(auto x = iterator(data, pos.id, head, capacity); x != begin(); x--) {
                *x = *std::prev(x);
            }
            pop_front();
        }
        return iterator(data, pos.id, head, capacity);
    }

    T& back() { return *(end() - 1); }
    T& back() const { return (*this)[sz - 1]; }
    T& front() { return *begin(); }
    T& front() const { return (*this)[0]; }

    template<class S>
    friend void swap(RingBuffer<S>& first, RingBuffer<S>& second);
};

template<class T>
struct Iterator {
private:
    template<class B>
    friend class RingBuffer;

    template<class I>
    friend class Iterator;

    T * data;
    size_t id, head, capacity;

    Iterator(T *data, size_t ind, size_t head, size_t capacity) : data(data), id(ind), head(head), capacity(capacity) {}

public:
    typedef std::ptrdiff_t difference_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef std::random_access_iterator_tag iterator_category;

    template<class = std::enable_if_t<std::is_const<T>::value>>
    Iterator(const Iterator<std::decay_t<T>>& second) : data(second.data), id(second.id), head(second.head), capacity(second.capacity){ }


    const Iterator operator++(int) {
        Iterator nw = *this;
        ++id;
        return nw;
    }
    Iterator& operator++() {
        (*this)++;
        return *this;
    }

    const Iterator operator--(int) {
        Iterator nw = *this;
        --id;
        return nw;
    }
    Iterator& operator--() {
        (*this)--;
        return *this;
    }

    pointer operator->() const {
        return (head + id) % capacity + data;
    }

    reference operator*() const {
        return *((head + id) % capacity + data);
    }

    friend Iterator operator+(Iterator const& iter, difference_type add) {
        return Iterator(iter.data, iter.id + add, iter.head, iter.capacity);
    }
    friend Iterator operator-(Iterator const& iter, difference_type add) {
        return iter + (-add);
    }
    friend Iterator operator+=(Iterator& iter, difference_type add) {
        iter = iter + add;
        return iter;
    }
    friend Iterator operator-=(Iterator& iter, difference_type add) {
        return iter += (-add);
    }

    template<class F, class S>
    friend bool operator==(Iterator<F> const& first, Iterator<S> const& second);

    template<class F, class S>
    friend bool operator!=(Iterator<F> const& first, Iterator<S> const& second);
    template<class F, class S>
    friend bool operator<(Iterator<F> const& first, Iterator<S> const& second);

    template<class F, class S>
    friend bool operator>(Iterator<F> const& first, Iterator<S> const& second);
    template<class F, class S>
    friend bool operator<=(Iterator<F> const& first, Iterator<S> const& second);

    template<class F, class S>
    friend bool operator>=(Iterator<F> const& first, Iterator<S> const& second);
};

template<class F, class S>
bool operator==(Iterator<F> const& first, Iterator<S> const& second) {
    return (!(first<second) && !(second<first));
}

template<class F, class S>
bool operator!=(Iterator<F> const& first, Iterator<S> const& second) {
    return !(first == second);
}

template<class F, class S>
bool operator<(Iterator<F> const& first, Iterator<S> const& second) {
    return first.id < second.id;
}

template<class F, class S>
bool operator>(Iterator<F> const& first, Iterator<S> const& second) {
    return second < first;
}


template<class F, class S>
bool operator<=(Iterator<F> const& first, Iterator<S> const& second) {
    return !(second < first);
}


template<class F, class S>
bool operator>=(Iterator<F> const& first, Iterator<S> const& second) {
    return !(first < second);
}

#endif //RINGBUFFER_RINGBUFFER_H