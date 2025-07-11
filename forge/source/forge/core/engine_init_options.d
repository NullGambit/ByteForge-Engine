module forge.core.engine_init_options;
import dlib.math.vector;

// import dlib.math.vector;

public struct EngineInitOptions
{
    string windowName;
    Vector2i windowSize;
}

void setDelta()
{
    import forge.core.time;

    Time._delta = 20;
}