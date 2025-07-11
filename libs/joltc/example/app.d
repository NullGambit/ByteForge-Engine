import std.stdio;
import joltc;

enum Layers : JPH_ObjectLayer
{
    Moving,
    Static
}

void main()
{
    if (!joltc.JPH_Init())
    {
        writeln("could not init jolt");
        return;
    }

    JPH_JobSystem* jobSystem = JPH_JobSystemThreadPool_Create(null);

    JPH_PhysicsSystemSettings settings = {};
    settings.maxBodies = 65536;
    settings.numBodyMutexes = 0;
    settings.maxBodyPairs = 65536;
    settings.maxContactConstraints = 65536;

    JPH_PhysicsSystem* system = JPH_PhysicsSystem_Create(&settings);
    JPH_BodyInterface* bodyInterface = JPH_PhysicsSystem_GetBodyInterface(system);

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    const float cDeltaTime = 1.0f / 60.0f;

    // Now we're ready to simulate the body, keep simulating until it goes to sleep
    uint step = 0;
    foreach (i; 0 .. 60)
    {
        // Next step
        ++step;

        // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
        const cCollisionSteps = 1;

        // Step the world
        JPH_PhysicsSystem_Update(system, cDeltaTime, cCollisionSteps, jobSystem);
    }

    // Remove the destroy sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.

    JPH_JobSystem_Destroy(jobSystem);

    JPH_PhysicsSystem_Destroy(system);
    JPH_Shutdown();
}
