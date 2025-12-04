module forge.container.pair;

@nogc:

struct Pair(K, V)
{
	K key;
	V value;
}
