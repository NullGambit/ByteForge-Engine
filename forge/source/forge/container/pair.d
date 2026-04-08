module forge.container.pair;

@nogc:

struct Pair(A, B, string AName = "a", string BName = "b")
{
    mixin("A " ~ AName ~ ";");
    mixin("B " ~ BName ~ ";");
}
