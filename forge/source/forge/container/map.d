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

mixin template LinearProbeBucket()
{
    enum LoadFactor = 0.65;

    private
    {
        Entry *m_bucket;
    	uint m_capacity;
    }

    private
    {
        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            validateBucket();

            auto hash = getMixedHash(k);
    		auto index = getIndex(hash);

    		while (m_bucket[index].isOccupied && m_bucket[index].hash != hash)
    		{
    			index = getIndex(index + 1);
    		}

    		auto entry = &m_bucket[index];

            emplace(&entry.key, move(k));

            entry.hash = hash;

            m_length++;

            static if (!is(V == void))
            {
                return emplace(&entry.value, move(v));
            }
    	}

        inout(Entry)* probeEntry(A)(const auto ref A key) inout
    	{
            const hash = getMixedHash(key);
           	auto index = getIndex(hash);

            auto checked = index;

           	while (checked++ <= m_capacity && m_bucket[index].isOccupied)
           	{
          		auto entry = &m_bucket[index];

                if (entry.hash == hash)
          		{
         			return entry;
          		}

          		index = getIndex(index + 1);
           	}

           	return null;
    	}

    }
    void remove(A)(const auto ref A key)
   	{
        auto entry = probeEntry(key);

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
        m_length--;
   	}
}

mixin template RobbinHoodProbing()
{
    enum LoadFactor = 0.95;

    private
    {
        Entry *m_bucket;
    	uint m_capacity;
    }

    private
    {
        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            validateBucket();

            auto hash = getMixedHash(k);
    		auto index = getIndex(hash);
            auto distance = 0UL;
            auto mask = m_capacity - 1;
            Entry current;

            emplace(&current.key, move(k));
            emplace(&current.value, move(v));
            current.hash = hash;

    		while (true)
    		{
                auto entry = &m_bucket[index];

                if (!entry.isOccupied || entry.hash == hash)
                {
                    emplace(entry, move(current));
                    m_length++;
                    return &entry.value;
                }

                auto currentDistance = (index - (entry.hash & mask)) & mask;

                if (currentDistance < distance)
                {
                    import std.algorithm.mutation;

                    swap(current, *entry);
                    distance = currentDistance;
                }

    			index = getIndex(index + 1);
                distance++;
    		}
    	}

        inout(Entry*) probeEntry(A)(const auto ref A key, out ulong hash) inout
    	{
            hash = getMixedHash(key);
           	auto index = getIndex(hash);
            auto mask = m_capacity - 1;
            auto distance = 0UL;

            while (true)
            {
                auto entry = &m_bucket[index];

                if (!entry.isOccupied)
                {
                    return entry;
                }

                if (entry.hash == hash)
                {
                    return entry;
                }

                auto currentDistance = (index - (entry.hash & mask)) & mask;

                if (currentDistance < distance)
                {
                    return entry;
                }

    			index = getIndex(index + 1);
                distance++;
            }
    	}
    }

    void remove(A)(const auto ref A key)
   	{
        auto entry = probeEntry(key);

       	if (entry == null)
       	{
       	    return;
       	}

        auto index = entry - m_bucket;
        auto mask = m_capacity - 1;

        entry.hash = 0;
        m_length--;

        destroy!false(entry.key);

        static if (!is(V == void))
        {
            destroy!false(entry.value);
        }

        while (true)
        {
            auto next = getIndex(index + 1);

            auto currentEntry = &m_bucket[next];

            if (!currentEntry.isOccupied)
            {
                return;
            }

            auto home = currentEntry.hash & mask;
            auto distance = (next - home) & mask;

            if (distance == 0)
            {
                return;
            }

            emplace(&m_bucket[index], move(m_bucket[next]));

            index = next;
        }
   	}
}

struct Map(K, V, alias Bucket = LinearProbeBucket, alias Allocator = DefaultAllocator)
{
	alias Entry = HashEntry!(K, V);

	mixin Allocator allocator;

	@disable this(this);
    @disable void opAssign(ref typeof(this) rhs);

    mixin Bucket;

	private uint m_length;

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

	static if (!is(V == void))
	{
    	inout(V)* get(A)(const auto ref A key) inout
    	{
            ulong hash;
    		auto entry = probeEntry(key, hash);

    		if (hash == 0)
    		{
                return null;
    		}

    		return &entry.value;
    	}

        ref V getOrInit(A)(auto ref A key)
    	{
   		    ulong hash;
            auto entry = probeEntry(key, hash);

    		if (hash == 0)
    		{
                emplace(&entry.key, move(key));
                emplace(&entry.value, V.init);
                entry.hash = hash;
    		}

    		return entry.value;
    	}

        ref inout(V) get(A)(const auto ref A key, ref inout(V) fallback) inout
    	{
   		    ulong hash;
            auto entry = probeEntry(key, hash);

            if (hash == 0)
            {
                return fallback;
            }

            return &entry.value;
    	}

        ref V opIndex(A)(const auto ref A key) const
        {
            return *get(key);
        }

        ref V opIndex(A)(auto ref A key)
        {
            return getOrInit(key);
        }

        alias put = putImpl;
	}
	else
	{
	    auto put(A)(auto ref A k)
		{
            return putImpl(k, 0);
		}
	}

	bool has(A)(const auto ref A key) const
	{
		return probeEntry(key) != null;
	}

	void rehash(const uint newSize)
   	{
  		auto oldBuckets = m_bucket;
  		const oldCapacity = m_capacity;

   	    m_bucket = allocator.alloc!Entry(newSize);
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

template Set(K, alias Bucket = LinearProbeBucket, Allocator = DefaultAllocator!(HashEntry!(K, void)))
{
    alias Set = Map!(K, void, Bucket, Allocator);
}
