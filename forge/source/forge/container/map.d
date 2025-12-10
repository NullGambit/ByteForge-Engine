module forge.container.map;

import forge.mem.allocators;
import forge.container.list;
import std.typecons;
import forge.container.pair;
import core.lifetime;
import forge.fmt;

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

mixin template LinearProbeBucket(float BucketLoadFactor = 0.65)
{
    enum LoadFactor = BucketLoadFactor;

    private
    {
        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            auto hash = getMixedHash(k);
    		auto index = getIndex(hash);

            foreach (_; 0..m_capacity)
            {
                auto entry = &m_bucket[index];

                auto isSame = entry.hash == hash;

                if (!entry.isOccupied || isSame)
          		{
                    if (!isSame)
                    {
                        m_length++;
                    }

                    auto newEntry = makeEntry(k, v, hash);
         			emplaceEntry(newEntry, index);

                    static if (!is (V == void))
                    {
                        return &m_bucket[index].value;
                    }
                    else
                    {
                        return;
                    }
          		}

          		index = getIndex(index + 1);
            }

            static if (!is (V == void))
            {
                return null;
            }
    	}

        inout(Entry)* probeEntry(A)(const auto ref A key) inout
    	{
            const hash = getMixedHash(key);
           	auto index = getIndex(hash);

            foreach (_; 0..m_capacity)
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

        destroyEntry(entry);
        m_length--;
   	}
}

mixin template RobbinHoodProbing(float BucketLoadFactor = 0.65)
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

                auto isSame = entry.hash == hash;

                if (!entry.isOccupied || isSame)
                {
                    emplaceEntry(current, index);

                    if (!isSame)
                    {
                        m_length++;
                    }

                    static if (!is (V == void))
                    {
                        return &m_bucket[index].value;
                    }
                    else
                    {
                        return;
                    }
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

        inout(Entry*) probeEntry(A)(const auto ref A key) inout
    	{
            const hash = getMixedHash(key);
           	auto index = getIndex(hash);
            auto distance = 0UL;

            foreach (_; 0..m_capacity)
            {
                auto entry = &m_bucket[index];

                if (!entry.isOccupied)
                {
                    return null;
                }

                if (entry.hash == hash)
                {
                    return entry;
                }

                auto currentDistance = (index - (entry.hash & mask)) & mask;

                if (currentDistance < distance)
                {
                    return null;
                }

    			index = getIndex(index + 1);
                distance++;
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
        enum ALIGNMENT = 64;

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
            import forge.mem.utils;

            auto alignedSize = alignTo(newSize, ALIGNMENT);

            if (alignedSize <= newSize || !m_meta)
            {
                if (m_meta)
                {
                    allocator.dealloc(m_meta);
                }

                m_meta = allocator.alloc!ubyte(alignedSize);
            }

            memset(m_meta, META_EMPTY, alignedSize);
        }

        auto putImpl(A, B)(auto ref A k, auto ref B v)
    	{
            auto hash = getMixedHash(k);
            auto fp = fingerprint(hash);
    		auto index = getIndex(hash);

            while (true)
            {
                auto meta = m_meta[index];
                auto isOld = meta == META_TOMBSTONE;

                if
                (
                    (meta == META_EMPTY || isOld)
                    ||
                    (meta == fp && m_bucket[index].hash == hash)
                )
                {
                    m_meta[index] = fp;

                    auto entry = makeEntry(k, v, hash);

                    emplaceEntry(entry, index);

                    if (!isOld)
                    {
                        m_length++;
                    }

                    static if (!is (V == void))
                    {
                        return &m_bucket[index].value;
                    }
                    else
                    {
                        return;
                    }
                }

                index = getIndex(index + 1);
            }
    	}

        @trusted
        inout(Entry)* probeEntry(A)(const auto ref A key) inout
    	{
            auto hash = getMixedHash(key);
            auto fp = fingerprint(hash);
           	const index = getIndex(hash);
            auto base = index & ~(GROUP_SIZE - 1);

            import inteli;
            import core.bitop;

            auto fpv = _mm_set1_epi8(fp);
            auto emptyv = _mm_set1_epi8(cast(byte)META_EMPTY);

            foreach (_; 0..m_capacity)
            {
                auto meta = _mm_loadu_si128(cast(__m128i*)(m_meta + base));

                auto eq = _mm_cmpeq_epi8(meta, fpv);
                auto eqmask = _mm_movemask_epi8(eq);
                size_t pos;

                while (eqmask)
                {
                    auto bit = bsf(eqmask);
                    pos = (base + bit) & mask;

                    if (m_bucket[pos].hash == hash)
                    {
                        return &m_bucket[pos];
                    }

                    eqmask &= eqmask - 1;
                }

                auto emp = _mm_cmpeq_epi8(meta, emptyv);

                if (_mm_movemask_epi8(emp))
                {
                    return null;
                }

                base = (base + GROUP_SIZE) & mask;
            }

            return null;
    	}
    }

    void remove(A)(auto ref A key)
   	{
        auto entry = probeEntry(key);

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

struct Map(K, V, alias Bucket = RobbinHoodProbing, alias Allocator = DefaultAllocator)
{
	alias Entry = HashEntry!(K, V);

	mixin Allocator allocator;

	@disable this(this);
    @disable void opAssign(ref typeof(this) rhs);

    ~this()
    {
        static if (__traits(hasMember, typeof(this), "onDestroy"))
        {
            onDestroy();
        }

        allocator.dealloc(m_bucket);
        m_bucket = null;
        m_length = 0;
        m_capacity = 0;
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
            return h & mask;
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
    		if (m_length >= m_capacity * LoadFactor)
    		{
    		    const newCapacity = m_capacity > 0 ? m_capacity * 2 : 4;
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
    		auto entry = probeEntry(key);

    		if (entry == null)
    		{
                return null;
    		}

    		return &entry.value;
    	}

        ref V getOrInit(A)(auto ref A key)
    	{
            auto entry = probeEntry(key);

            if (entry == null)
            {
                return *put(key, V.init);
            }

    		return entry.value;
    	}

        ref inout(V) get(A)(const auto ref A key, ref inout(V) fallback) inout
    	{
            auto entry = probeEntry(key);

            if (entry == 0)
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
            return putImpl(k, 0);
		}
	}

	bool has(A)(const auto ref A key) const
	{
		return probeEntry(key) != null;
	}

	void rehash(uint newSize)
   	{
        import forge.mem.utils;

        newSize = toNextPower2(newSize);

  		auto oldBuckets = m_bucket;
  		const oldCapacity = m_capacity;

        static if (__traits(hasMember, typeof(this), "onRehash"))
        {
            onRehash(newSize);
        }

   	    m_bucket = allocator.alloc!Entry(newSize);

        import core.stdc.string;

        memset(m_bucket, 0, newSize * Entry.sizeof);

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

        m_length = 0;

  		foreach (ref entry; oldBuckets[0..oldCapacity])
  		{
  		    if (entry.isOccupied)
 			{
                static if (!is(V == void))
				{
 			        putImpl(entry.key, entry.value);
				}
				else
				{
				    putImpl(entry.key, 0);
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
			    static if (!is (V == void))
				{
                    emplace(&newSelf.m_bucket[i++], memutil.clone(entry));
				}
				else
				{
				    emplace(&newSelf.m_bucket[i++].key, memutil.clone(entry));
				}
			}
		}

		return newSelf;
    }

	static if (!is (V == void))
	{
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
	}
	else
	{
        int opApply(scope int delegate(ref K) dg)
    	{
           	foreach (ref entry; m_bucket[0..m_capacity])
           	{
           	    if (!entry.isOccupied)
          		{
          		    continue;
          		}

          		auto result = dg(entry.key);

          		if (result)
          		{
         			return result;
          		}
           	}

    		return 0;
    	}

        int opApply(scope int delegate(const ref K) dg) const
       	{
               	foreach (ref entry; m_bucket[0..m_capacity])
               	{
               	    if (!entry.isOccupied)
              		{
              		    continue;
              		}

              		auto result = dg(entry.key);

              		if (result)
              		{
             			return result;
              		}
               	}

      		return 0;
       	}
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

template Set(K, alias Bucket = RobbinHoodProbing, alias Allocator = DefaultAllocator)
{
    alias Set = Map!(K, void, Bucket, Allocator);
}

private mixin template MakeTests(alias Bucket)
{
    void runMapTest()
    {
        import forge.container.string;

        Map!(String, int, Bucket) map;

        map["a"] = 1;
        map["b"] = 2;
        map["c"] = 3;
        map["d"] = 4;

        map.put("hello", 1000);

        map.put("remove me", -1);

        map["c"] = 100;

        map.remove("remove me");

        assert(map["a"] == 1);
        assert(map["b"] == 2);
        assert(map["c"] == 100);
        assert(map["d"] == 4);
        assert(*map.get("hello") == 1000);
        assert(!map.has("remove me"));
        assert(map.length == 5);
    }

    void runSetTest()
    {
        import forge.container.string;

        Set!(String, Bucket) set;

        set.put("a");
        set.put("b");
        set.put("c");
        set.put("d");

        set.put("hello");

        set.put("remove me");

        set.remove("remove me");

        assert(set.has("a"));
        assert(set.has("b"));
        assert(set.has("c"));
        assert(set.has("d"));
        assert(set.has("hello"));
        assert(!set.has("remove me"));
        assert(set.length == 5);
    }
}

unittest
{
    mixin MakeTests!LinearProbeBucket;
    runMapTest();
    runSetTest();
}

unittest
{
    mixin MakeTests!RobbinHoodProbing;
    runMapTest();
    runSetTest();
}

unittest
{
    mixin MakeTests!SwissTableProbbing;
    runMapTest();
    runSetTest();
}
