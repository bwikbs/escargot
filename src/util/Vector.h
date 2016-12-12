#ifndef __EscargotVector__
#define __EscargotVector__

namespace Escargot {

template <typename T, typename Allocator>
class Vector : public gc {
public:
    Vector()
    {
        m_buffer = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    Vector(size_t siz)
    {
        m_buffer = nullptr;
        m_size = 0;
        m_capacity = 0;
        resize(siz);
    }

    Vector(Vector<T, Allocator>&& other)
    {
        m_size = other.size();
        m_buffer = other.m_buffer;
        m_capacity = other.m_capacity;
        other.m_buffer = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    Vector(const Vector<T, Allocator>& other)
    {
        if (other.size()) {
            m_size = other.size();
            m_capacity = other.m_capacity;
            m_buffer = Allocator().allocate(m_capacity);
            for (size_t i = 0; i < m_size; i++) {
                m_buffer[i] = other[i];
            }
        } else {
            m_buffer = nullptr;
            m_size = 0;
            m_capacity = 0;
        }
    }

    const Vector<T, Allocator>& operator=(const Vector<T, Allocator>& other)
    {
        if (other.size()) {
            m_size = other.size();
            m_capacity = other.m_capacity;
            m_buffer = Allocator().allocate(m_capacity);
            for (size_t i = 0; i < m_size; i++) {
                m_buffer[i] = other[i];
            }
        } else {
            clear();
        }
        return *this;
    }

    Vector(const Vector<T, Allocator>& other, const T& newItem)
    {
        m_size = other.size() + 1;
        m_capacity = other.m_capacity + 1;
        m_buffer = Allocator().allocate(m_size);
        for (size_t i = 0; i < other.size(); i++) {
            m_buffer[i] = other[i];
        }
        m_buffer[other.size()] = newItem;
    }

    ~Vector()
    {
        if (m_buffer)
            Allocator().deallocate(m_buffer, m_size);
    }

    void pushBack(const T& val)
    {
        if (m_capacity <= (m_size + 1)) {
            m_capacity = computeAllocateSize(m_size + 1);
            T* newBuffer = Allocator().allocate(m_capacity);
            for (size_t i = 0; i < m_size; i++) {
                newBuffer[i] = m_buffer[i];
            }
            if (m_buffer)
                Allocator().deallocate(m_buffer, m_size);
            m_buffer = newBuffer;
        }
        m_buffer[m_size] = val;
        m_size++;
    }

    void push_back(const T& val)
    {
        pushBack(val);
    }

    void insert(size_t pos, const T& val)
    {
        ASSERT(pos <= m_size);
        if (m_capacity <= (m_size + 1)) {
            m_capacity = computeAllocateSize(m_size + 1);
            T* newBuffer = Allocator().allocate(m_capacity);
            for (size_t i = 0; i < pos; i++) {
                newBuffer[i] = m_buffer[i];
            }
            newBuffer[pos] = val;
            for (size_t i = pos; i < m_size; i++) {
                newBuffer[i + 1] = m_buffer[i];
            }
            if (m_buffer)
                Allocator().deallocate(m_buffer, m_size);
            m_buffer = newBuffer;
        } else {
            for (size_t i = pos; i < m_size; i++) {
                m_buffer[i + 1] = m_buffer[i];
            }
            m_buffer[pos] = val;
        }

        m_size++;
    }

    void erase(size_t pos)
    {
        erase(pos, pos + 1);
    }

    void erase(size_t start, size_t end)
    {
        ASSERT(start < end);
        ASSERT(start >= 0);
        ASSERT(end <= m_size);

        size_t c = end - start;
        if (m_size - c) {
            T* newBuffer = Allocator().allocate(m_size - c);
            for (size_t i = 0; i < start; i++) {
                newBuffer[i] = m_buffer[i];
            }

            for (size_t i = end + c; i < m_size; i++) {
                newBuffer[i - c] = m_buffer[i];
            }

            if (m_buffer)
                Allocator().deallocate(m_buffer, m_size);
            m_buffer = newBuffer;
            m_size = m_size - c;
            m_capacity = m_size - c;
        } else {
            clear();
        }
    }

    size_t size() const
    {
        return m_size;
    }

    size_t capacity() const
    {
        return m_size;
    }

    bool empty() const
    {
        return m_size == 0;
    }

    void pop_back()
    {
        erase(m_size - 1);
    }

    T& operator[](const size_t& idx)
    {
        ASSERT(idx < m_size);
        return m_buffer[idx];
    }

    const T& operator[](const size_t& idx) const
    {
        ASSERT(idx < m_size);
        return m_buffer[idx];
    }

    T* data()
    {
        return m_buffer;
    }

    T& back()
    {
        return m_buffer[m_size - 1];
    }

    void clear()
    {
        if (m_buffer) {
            Allocator().deallocate(m_buffer, m_size);
        }
        m_buffer = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    void resizeWithUninitializedValues(size_t newSize)
    {
        if (newSize) {
            T* newBuffer = Allocator().allocate(newSize);

            for (size_t i = 0; i < m_size && i < newSize; i++) {
                newBuffer[i] = m_buffer[i];
            }
            m_capacity = newSize;
            m_size = newSize;
            if (m_buffer)
                Allocator().deallocate(m_buffer, m_size);
            m_buffer = newBuffer;
        } else {
            clear();
        }
    }

    void resize(size_t newSize, const T& val = T())
    {
        if (newSize) {
            if (newSize > m_capacity) {
                size_t newCapacity = computeAllocateSize(newSize);
                T* newBuffer = Allocator().allocate(newCapacity);
                for (size_t i = 0; i < m_size && i < newSize; i++) {
                    newBuffer[i] = m_buffer[i];
                }
                for (size_t i = m_size; i < newSize; i++) {
                    newBuffer[i] = val;
                }
                m_size = newSize;
                m_capacity = newCapacity;
                if (m_buffer)
                    Allocator().deallocate(m_buffer, m_size);
                m_buffer = newBuffer;
            } else {
                for (size_t i = m_size; i < newSize; i++) {
                    m_buffer[i] = val;
                }
                m_size = newSize;
            }
        } else {
            clear();
        }
    }

protected:
    size_t computeAllocateSize(size_t newSize)
    {
        const float glowFactor = 1.2f;
        const size_t maxGap = 1024;
        size_t n = newSize * glowFactor;
        if (n == newSize)
            n++;
        if ((n - newSize) > maxGap) {
            n = newSize + maxGap;
        }
        return n;
    }

    T* m_buffer;
    size_t m_capacity;
    size_t m_size;
};


class VectorUtil {
public:
    static const size_t invalidIndex;

    template <typename T, typename Allocator>
    static size_t findInVector(const Vector<T, Allocator>& vector, const T& target)
    {
        size_t len = vector.size();
        for (size_t i = 0; i < len; i++) {
            if (vector[i] == target) {
                return i;
            }
        }

        return invalidIndex;
    }
};
}

#endif