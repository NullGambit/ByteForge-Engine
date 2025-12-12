module forge.container.map;

import forge.mem.allocators;
import forge.container.list;
import std.typecons;
import forge.container.pair;
import core.lifetime;
import forge.fmt;

struct HashEntry(K, V)
{
    private ulong hash;

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

struct Map(K, V, float LoadFactor = 0.65, alias Allocator = DefaultAllocator)
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
        ulong getHash(T)(const ref T value) const
        {
            static if (__traits(hasMember, T, "toHash"))
            {
                return value.toHash();
            }
            else
            {
                import forge.digest;

                static if (__traits(compiles, xxhash64(value)))
                {
                    return xxhash64(value);
                }
                else static if (__traits(compiles, xxhash64(value.ptr, value.length)))
                {
                    return xxhash64(value.ptr, value.length);
                }
                else
                {
                    return xxhash64(&value, value.sizeof);
                }
            }
        }

        pragma(inline, true)
   	    ulong getDistance(ulong h, ulong i) const pure
  		{
            return (i + m_capacity - h) & mask;
  		}

        auto putImpl(A, B)(auto ref A k, auto ref B v)
       	{
            const hash = getHash(k);
      		auto index = getIndex(hash);
            auto distance = 0UL;

            auto current = makeEntry(k, v, hash);

      		while (true)
      		{
                auto entry = &m_bucket[index];

                auto isSame = entry.hash == hash && k == entry.key;

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

                const entryDistance = getDistance(hash, index);

                if (entryDistance > distance)
                {
                    import std.algorithm.mutation;

                    swap(current, *entry);
                    distance = entryDistance;
                }

     			index = getIndex(index + 1);
                distance++;
      		}
       	}

        inout(Entry*) probeEntry(A)(const auto ref A key) inout
       	{
            const hash = getHash(key);
           	auto index = getIndex(hash);
            auto distance = 0UL;

            foreach (_; 0..m_capacity)
            {
                auto entry = &m_bucket[index];

                if (!entry.isOccupied)
                {
                    return null;
                }

                if (entry.hash == hash && key == entry.key)
                {
                    return entry;
                }

                const entryDistance = getDistance(hash, index);

                if (entryDistance < distance)
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

            const distance = getDistance(currentEntry.hash, index);

            if (distance == 0)
            {
                return;
            }

            emplace(&m_bucket[index], move(m_bucket[next]));

            index = next;
        }
    }

    void validateBucket()
   	{
  		if (m_length >= m_capacity * LoadFactor)
  		{
  		    const newCapacity = m_capacity > 0 ? m_capacity * 2 : 16;
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

template Set(K, float LoadFactor = 0.65, alias Allocator = DefaultAllocator)
{
    alias Set = Map!(K, void, LoadFactor, Allocator);
}

unittest
{
    import forge.container.string;

    Map!(String, int) map;

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

unittest
{
    import forge.container.string;

    Set!(String) set;

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
