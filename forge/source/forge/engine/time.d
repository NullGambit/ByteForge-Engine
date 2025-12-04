module forge.engine.time;

static struct Time
{
    package static float _delta;
    package static float _unscaledDelta;
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
