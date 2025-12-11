module forge.digest;

import xxhash;

ulong xxhash64(inout(char) *ptr, size_t len, size_t seed = 0) nothrow
{
    return XXH64(ptr, len, seed);
}

ulong xxhash64(inout(byte)[] bytes, size_t seed = 0) nothrow
{
    return XXH64(bytes.ptr, bytes.length, seed);
}

ulong xxhash64(string s, size_t seed = 0) nothrow
{
    return XXH64(s.ptr, s.length, seed);
}
