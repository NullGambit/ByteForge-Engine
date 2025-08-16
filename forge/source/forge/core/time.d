module forge.core.time;

static struct Time
{
    package static float _delta;
    package static float _unscaledDelta;
    static float scale;

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
