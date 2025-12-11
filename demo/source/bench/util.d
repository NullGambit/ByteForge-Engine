module bench.util;

import forge.container;

List!String makeKeys(int count, int min, int max)
{
    List!String keys;

    keys.reserve(count);

    foreach (i; 0..count)
    {
        import std.random;
        import core.lifetime;

        auto keySize = uniform(min, max);

        String key;

        key.reserve(keySize);

        foreach (_; 0..keySize)
        {
            auto c = uniform('/', '~');
            key.append(c);
        }

        keys.append(key.move());
    }

    return keys;
}
