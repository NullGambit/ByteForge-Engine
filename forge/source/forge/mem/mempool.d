module forge.mem.mempool;

import dlib.container.array;
import std.typecons : tuple;

import forge.mem.virtual_mem;
import forge.mem.utils;

struct AllocResult(T)
{
    T* data;
    size_t id;
    bool fromFreeList;
}

struct MempoolIterator(T)
{
    byte* memory;
    size_t maxOffset;
    size_t offset;

    bool empty = false;

    @property
    ref T front() const
    {
        return *(cast(T*)(memory + offset));
    }

    void popFront()
    {
        offset += getTypeSize!T();

        empty = offset >= maxOffset;
    }
}

struct Mempool
{
    byte* memory;
    size_t offset;
    size_t length;
    size_t elementSize;
    size_t mapSize;
    Array!size_t freeList;

    void function(byte*) destroyFunc;

    bool initPool(size_t mapSize, size_t elementSize)
    {
        this.mapSize = mapSize;
        this.elementSize = elementSize;

        length = 0;
        offset = 0;

        memory = virtualAlloc(mapSize);

        return memory != null;
    }

    bool initPool(T)(size_t count)
    {
        auto size = getTypeSize!T();

        destroyFunc = (byte* ptr) {
            const auto obj = (cast(T*) ptr);
            object.destroy!false(obj);
        };

        return initPool(size * count, size);
    }

    void destroyPool()
    {
        virtualFree(memory);
    }

    MempoolIterator!T getIterator(T)()
    {
        return MempoolIterator!T(memory, offset);
    }

    AllocResult!T alloc(T = byte)()
    {
        AllocResult!T result;

        result.id = offset;

        if (freeList.length != 0)
        {
            result.id = freeList[$];
            result.fromFreeList = true;
            freeList.removeBack(1);
        }

        result.data = cast(T*)(memory + offset);

        length++;
        offset += elementSize;

        return result;
    }

    AllocResult!T emplace(T = byte, Args...)(auto ref Args args)
    {
        import std.conv : emplace;
        import core.memory : GC;

        auto result = alloc!T();

        auto memory = (cast(byte*) result.data)[0 .. elementSize];

        static if (is(T == class))
        {
            *result.data = emplace!T(memory, forward!args);
        }
        else
        {
            emplace!T(memory, forward!args);
        }

        return result;
    }

    void free(size_t id)
    {
        freeList.insertBack(id);

        length--;
    }

    void destroy(T = byte)(T* ptr, size_t id)
    {
        destroyFunc(cast(byte*) ptr);
        free(id);
    }

    void clearPool(bool destroy = true, void function(byte*) onDestroy = null)
    {
        if (destroy && destroyFunc)
        {
            for (size_t i = 0; i < length; i++)
            {
                auto ptr = memory + i * elementSize;

                if (onDestroy)
                {
                    onDestroy(ptr);
                }

                destroyFunc(ptr);
            }
        }

        offset = 0;
        length = 0;
        freeList.resize(0, 0);
    }

}
