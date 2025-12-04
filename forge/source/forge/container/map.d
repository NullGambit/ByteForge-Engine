module forge.container.map;

import forge.mem.allocators;
import forge.container.list;
import std.typecons;
import forge.container.pair;
import core.lifetime;

@nogc:

struct HashEntry(K, V)
{
    ulong hash;
    K key;

    static if (!is(V == void))
    {
        V value;
    }

	@property
	bool isOccupied() const pure
	{
	    return hash != 0;
	}
}

struct Map(K, V, Allocator = DefaultAllocator!(HashEntry!(K, V)))
{
	enum LoadFactor = 0.65;

	alias Entry = HashEntry!(K, V);

	Allocator allocator;

	@disable this(this);
    @disable void opAssign(ref typeof(this) rhs);

	private
	{
	    HashEntry!(K, V) *m_buckets;
		uint m_capacity;
		uint m_length;
	}

	@property
	auto capacity()
	{
	    return m_capacity;
	}

	@property
	auto length()
	{
	    return m_length;
	}

	private
	{
        pragma(inline, true)
	    ulong getIndex(ulong h) const pure
		{
            return h & (m_capacity - 1);
		}

		pragma(inline, true)
        ulong getMixedHash(T)(const ref T value) const
        {
            import std.traits;
            auto h = typeid(Unqual!T).getHash(&value);

            return mix64(h);
        }

		pragma(inline, true)
		static ulong mix64(ulong x) pure
        {
            x ^= x >> 33;
            x *= 0xff51afd7ed558ccd;
            x ^= x >> 33;
            x *= 0xc4ceb9fe1a85ec53;
            x ^= x >> 33;
            return x;
        }

        void validateBucket()
    	{
    		if (m_length + 1 > m_capacity * LoadFactor)
    		{
    		    const newCapacity = m_capacity > 0 ? m_capacity * 2 : 2;
    		    rehash(newCapacity);
    		}
    	}
	}

    private void putImpl(A, B)(auto ref A k, auto ref B v)
	{
        validateBucket();

        auto hash = getMixedHash(k);
		auto index = getIndex(hash);

		while (index < m_capacity && m_buckets[index].isOccupied)
		{
			index = getIndex(index + 1);
		}

		auto entry = &m_buckets[index];

        emplace(&entry.key, move(k));

        static if (!is(V == void))
        {
            emplace(&entry.value, move(v));
        }

        entry.hash = hash;

        m_length++;
	}

	static if (!is(V == void))
	{
	    alias put = putImpl;
	}
	else
	{
	    void put(A)(auto ref A k)
		{
            putImpl(k, 0);
		}
	}

	inout(Entry)* findEntry(A)(const auto ref A key) inout
	{
        const hash = getMixedHash(key);
    	auto index = getIndex(hash);

    	while (index < m_capacity && m_buckets[index].isOccupied)
    	{
    		auto entry = &m_buckets[index];

    		if (entry.hash == hash)
    		{
    			return entry;
    		}

    		index = getIndex(index + 1);
    	}

    	return null;
	}

	static if (!is(V == void))
	{
    	inout(V)* get(A)(const auto ref A key) inout
    	{
    		auto entry = findEntry(key);

    		if (entry == null)
    		{
    		    return null;
    		}

    		return &entry.value;
    	}
	}
	else
	{
        bool has(A)(const auto ref A key) const
    	{
    		return findEntry(key) != null;
    	}
	}

	void remove(A)(const auto ref A key)
	{
        auto entry = findEntry(key);

       	if (entry == null)
       	{
       	    return;
       	}

        destroy!false(entry.key);

        static if (!is(V == void))
        {
            destroy!false(entry.value);
        }

        entry.hash = 0;
	}

	void rehash(const uint newSize)
	{
		auto oldBuckets = m_buckets;
		const oldCapacity = m_capacity;

	    m_buckets = allocator.alloc(newSize);
		m_capacity = newSize;

		if (oldBuckets == null)
		{
		    return;
		}

		foreach (ref bucket; oldBuckets[0..oldCapacity])
		{
		    if (bucket.isOccupied)
			{
                static if (!is(V == void))
				{
			        put(bucket.key, bucket.value);
				}
				else
				{
				    put(bucket.key);
				}
			}
		}

		allocator.dealloc(oldBuckets);
	}
}

template Set(K, Allocator = DefaultAllocator!(HashEntry!(K, void)))
{
    alias Set = Map!(K, void);
}
