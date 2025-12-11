module forge.engine.time;

static struct Time
{
    package __gshared float _delta;
    package __gshared float _unscaledDelta;
    __gshared float scale;

    @property
    static delta()
    {
        return _delta;
    }

    @property
    static unscaledDelta()
    {
        return _unscaledDelta;
    }
}
