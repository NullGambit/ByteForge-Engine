module forge.container.string;

import forge.mem.allocators;

// a dynamically sized string struct that can take an allocator
struct BaseString(T, Allocator = DefaultAllocator!T)
{
    T* data;

    private
    {
        uint m_length;
        uint m_capacity;
    }

    this(string str)
    {

    }

    @property
    length()
    {
        return m_length;
    }

    @property
    capacity()
    {
        return m_capacity;
    }

    @property
    void capacity(uint capacity)
    {
        m_capacity = capacity;
        reserve(m_capacity);
    }

    void reserve(uint newCapacity)
    {
        m_capacity = newCapacity;
    }

    void resize(uint newSize)
    {

    }

}

import forge.mem.allocators : DefaultAllocator;

// a dynamically sized string struct
alias String = BaseString!(DefaultAllocator!char);
