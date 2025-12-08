module forge.container.map;

import forge.mem.allocators;
import forge.container.list;
import std.typecons;
import forge.container.pair;
import core.lifetime;
import forge.fmt;

@nogc:

struct HashEntry(K, V)
{
    package ulong hash;
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

	void toString(W)(auto ref W w) const
	{
	    stringifyWrite(w, key);
	    w.write(": ");
	    stringifyWrite(w, value);
	}
}

mixin template LinearProbeBucket(float BucketLoadFactor = 0.60)
{
    enum LoadFactor = BucketLoadFactor;

    private
    {
        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            auto hash = getMixedHash(k);
    		auto index = getIndex(hash);

    		while (m_bucket[index].isOccupied && m_bucket[index].hash != hash)
    		{
    			index = getIndex(index + 1);
    		}

            auto entry = makeEntry(k, v, hash);

            emplaceEntry(entry, index);

            m_length++;
    	}

        inout(Entry)* probeEntry(A)(const auto ref A key, out ulong hash) inout
    	{
            hash = getMixedHash(key);
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

           	return &m_bucket[index];
    	}

    }
    void remove(A)(const auto ref A key)
   	{
        auto entry = probeEntry(key);

       	if (entry == null)
       	{
       	    return;
       	}

        destroyEntru(entry);
        m_length--;
   	}
}

mixin template RobbinHoodProbing(float BucketLoadFactor = 0.95)
{
    enum LoadFactor = BucketLoadFactor;

    private
    {
        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            auto hash = getMixedHash(k);
    		auto index = getIndex(hash);
            auto distance = 0UL;

            auto current = makeEntry(k, v, hash);

    		while (true)
    		{
                auto entry = &m_bucket[index];

                if (!entry.isOccupied || entry.hash == hash)
                {
                    emplaceEntry(current, index);
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
            auto distance = 0UL;

            while (true)
            {
                auto entry = &m_bucket[index];

                if (!entry.isOccupied || entry.hash == hash)
                {
                    return entry;
                }

                auto currentDistance = (index - (entry.hash & mask)) & mask;

                if (currentDistance < distance && !entry.isOccupied)
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

        m_length--;

        destroyEntry(entry);

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

mixin template SwissTableProbbing(float BucketLoadFactor = 0.75)
{
    enum LoadFactor = BucketLoadFactor;

    private
    {
        ubyte *m_meta;

        enum ubyte META_EMPTY = 0x80;
        enum ubyte META_TOMBSTONE = 0xFE;
        enum GROUP_SIZE = 16;

        void onDestroy()
        {
            allocator.dealloc(m_meta);
            m_meta = null;
        }

        pragma(inline, true)
        static byte fingerprint(ulong hash)
        {
            return hash & 0x7F;
        }

        void onRehash(uint newSize)
        {
            import core.stdc.string;

            auto temp = allocator.alloc!ubyte(newSize, 64);

            auto diff = newSize - m_capacity;

            memset(temp + m_capacity, META_EMPTY, diff);

            if (m_meta)
            {
                memcpy(temp, m_meta, m_capacity);
                allocator.dealloc(m_meta);
            }

            m_meta = temp;
        }

        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            auto hash = getMixedHash(k);
            auto fp = fingerprint(hash);
    		auto index = getIndex(hash);

            auto entry = makeEntry(k, v, hash);

    		while (true)
    		{
                auto meta = m_meta[index];

                if
                (
                    (meta == META_EMPTY || meta == META_TOMBSTONE)
                    ||
                    (meta == fp && m_bucket[index].hash == hash)
                )
                {
                    m_meta[index] = fp;
                    emplaceEntry(entry, index);
                    m_length++;
                    return;
                }

                index = getIndex(index + 1);
    		}
    	}

        inout(Entry*) probeEntry(A)(const auto ref A key, out ulong hash) inout
    	{
            hash = getMixedHash(key);
            auto fp = fingerprint(hash);
           	auto index = getIndex(hash);
            auto base = index & ~(GROUP_SIZE - 1);

            import inteli;
            import core.bitop;

            auto fpv = _mm_set1_epi8(fp);
            auto emptyv = _mm_set1_epi8(cast(byte)META_EMPTY);

            while (true)
            {
                auto m = _mm_loadu_si128(cast(__m128i*)(m_meta + base));

                auto eq = _mm_cmpeq_epi8(m, fpv);
                auto eqmask = _mm_movemask_epi8(eq);

                while (eqmask)
                {
                    auto bit = bsf(eqmask);
                    auto pos = (base + bit) & mask;

                    if (m_bucket[pos].hash == hash)
                    {
                        return &m_bucket[pos];
                    }

                    eqmask &= eqmask - 1;
                }

                auto emp = _mm_cmpeq_epi8(m, emptyv);

                if (_mm_movemask_epi8(emp))
                {
                    return null;
                }

                base = (base + GROUP_SIZE) & mask;
            }
    	}
    }

    void remove(A)(auto ref A key)
   	{
        ulong hash;
        auto entry = probeEntry(key, hash);

       	if (entry == null)
       	{
       	    return;
       	}

        auto index = entry - m_bucket;

        m_length--;

        destroyEntry(entry);

        m_meta[index] = META_TOMBSTONE;
   	}
}

struct Map(K, V, alias Bucket = LinearProbeBucket, alias Allocator = DefaultAllocator)
{
	alias Entry = HashEntry!(K, V);

	mixin Allocator allocator;

	@disable this(this);
    @disable void opAssign(ref typeof(this) rhs);

    ~this()
    {
        allocator.dealloc(m_bucket);
        m_bucket = null;
        m_length = 0;
        m_capacity = 0;

        static if (__traits(hasMember, typeof(this), "onDestroy"))
        {
            onDestroy();
        }
    }

    mixin Bucket;

    private
    {
        Entry *m_bucket;
    	uint m_capacity;
        uint m_length;
    }

	@property
	auto capacity() const pure
	{
	    return m_capacity;
	}

	@property
	auto length() const pure
	{
	    return m_length;
	}

	private
	{
	    @property uint mask() const pure
		{
		    return m_capacity - 1;
		}

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

        Entry makeEntry(A, B)(ref A k, ref B v, ulong hash)
        {
            Entry entry;

            emplace(&entry.key, move(k));

            static if (!is(V == void))
            {
                emplace(&entry.value, move(v));
            }

            entry.hash = hash;

            return entry;
        }

        void emplaceEntry(ref Entry entry, ulong index)
        {
            emplace(&m_bucket[index], move(entry));
        }

        // take pointer because its always used after probeEntry
        void destroyEntry(Entry *entry)
        {
            entry.hash = 0;

            destroy!false(entry.key);

            static if (!is(V == void))
            {
                destroy!false(entry.value);
            }
        }
	}

	static if (!is(V == void))
	{
    	inout(V)* get(A)(const auto ref A key) inout
    	{
            ulong hash;
    		auto entry = probeEntry(key, hash);

    		if (hash == 0 || entry == null)
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

        auto put(A, B)(auto ref A k, auto ref B v)
        {
            validateBucket();
            return putImpl(k, v);
        }
	}
	else
	{
	    auto put(A)(auto ref A k)
		{
		    validateBucket();
            return put(k, 0);
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

        static if (__traits(hasMember, typeof(this), "onRehash"))
        {
            onRehash(newSize);
        }

   	    m_bucket = allocator.alloc!Entry(newSize);

        static if (__traits(hasMember, Allocator, "getTotal"))
		{
			m_capacity = cast(uint)allocator.getTotal();
		}
		else
		{
			m_capacity = newSize;
		}

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

    typeof(this) clone()
    {
	    typeof(this) newSelf;

		newSelf.m_length = m_length;
		newSelf.m_capacity = m_capacity;
		// newSelf.allocator = allocator;

		newSelf.m_bucket = allocator.alloc!Entry(m_capacity);

		static if (__traits(isPOD, Entry))
		{
		    import core.stdc.string;

		    memcpy(newSelf.m_bucket, m_bucket, m_capacity);
		}
		else
		{
		    import memutil = forge.mem.utils;

			auto i = 0;

		    foreach (ref entry; this)
			{
                emplace(&newSelf.m_bucket[i++], memutil.clone(entry));
			}
		}

		return newSelf;
    }

    int opApply(scope int delegate(ref Entry) dg)
	{
		foreach (ref entry; m_bucket[0..m_capacity])
		{
		    if (!entry.isOccupied)
			{
			    continue;
			}

			auto result = dg(entry);

			if (result)
			{
				return result;
			}
		}

		return 0;
	}

	int opApply(scope int delegate(ref K, ref V) dg)
	{
    	foreach (ref entry; m_bucket[0..m_capacity])
    	{
    	    if (!entry.isOccupied)
    		{
    		    continue;
    		}

    		auto result = dg(entry.key, entry.value);

    		if (result)
    		{
    			return result;
    		}
    	}

		return 0;
	}

	int opApply(scope int delegate(const ref Entry) dg) const
    {
        foreach (ref entry; m_bucket[0..m_capacity])
        {
            if (!entry.isOccupied)
            {
                continue;
            }

            auto result = dg(entry);

            if (result)
            {
           	    return result;
            }
        }

        return 0;
    }

    int opApply(scope int delegate(const ref K, const ref V) dg) const
    {
       	foreach (ref entry; m_bucket[0..m_capacity])
       	{
       	    if (!entry.isOccupied)
      		{
      		    continue;
      		}

      		auto result = dg(entry.key, entry.value);

      		if (result)
      		{
     			return result;
      		}
       	}

        return 0;
    }

	void toString(W)(auto ref W w) const
	{
	    w.write("{ ");
	    foreach (ref item; this)
		{
		    w.write(item);
		}
		w.write(" }");
	}
}

template Set(K, alias Bucket = LinearProbeBucket, Allocator = DefaultAllocator!(HashEntry!(K, void)))
{
    alias Set = Map!(K, void, Bucket, Allocator);
}
