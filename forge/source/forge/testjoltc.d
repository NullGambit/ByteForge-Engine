module forge.testjoltc;

// Copyright (c) Amer Koleci and Contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

import joltc;

enum Layers : JPH_ObjectLayer
{
    NON_MOVING,
    MOVING,
    NUM_LAYERS = 2,
}

enum BroadPhaseLayers : JPH_BroadPhaseLayer
{
    NON_MOVING,
    MOVING,
    NUM_LAYERS,
}

import forge.mem.virtual_mem;
import std.stdio;

size_t totalMemory;
extern (C)
{
    byte* jphAlloc(size_t size)
    {
        writefln("allocating %s bytes", size);
        totalMemory += size;
        return virtualAlloc(size);
    }

    void jphFree(byte* ptr)
    {
        // writeln("freeing");
        virtualFree(ptr);
    }
}

int runTest()
{
    gJphAllocator = IAllocator(Alloc : &jphAlloc, Free:
        &jphFree);

    if (!JPH_Init())
    {
        return 1;
    }

    //JPH_SetAssertFailureHandler(JPH_AssertFailureFunc handler);

    JPH_JobSystem* jobSystem = JPH_JobSystemThreadPool_Create(null);

    // We use only 2 layers: one for non-moving objects and one for moving objects
    JPH_ObjectLayerPairFilter* objectLayerPairFilterTable = JPH_ObjectLayerPairFilterTable_Create(2);

    JPH_ObjectLayerPairFilterTable_EnableCollision(objectLayerPairFilterTable,
        Layers.NON_MOVING, Layers.MOVING);

    JPH_ObjectLayerPairFilterTable_EnableCollision(objectLayerPairFilterTable,
        Layers.MOVING, Layers.NON_MOVING);

    JPH_BroadPhaseLayerInterface* broadPhaseLayerInterfaceTable = JPH_BroadPhaseLayerInterfaceTable_Create(2, 2);

    JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
        broadPhaseLayerInterfaceTable, Layers.NON_MOVING, BroadPhaseLayers.NON_MOVING);

    JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(
        broadPhaseLayerInterfaceTable, Layers.MOVING, BroadPhaseLayers.MOVING);

    JPH_ObjectVsBroadPhaseLayerFilter* objectVsBroadPhaseLayerFilter = JPH_ObjectVsBroadPhaseLayerFilterTable_Create(
        broadPhaseLayerInterfaceTable, 2, objectLayerPairFilterTable, 2);

    JPH_PhysicsSystemSettings settings = {};
    settings.maxBodies = 65536;
    settings.numBodyMutexes = 0;
    settings.maxBodyPairs = 65536;
    settings.maxContactConstraints = 65536;
    settings.broadPhaseLayerInterface = broadPhaseLayerInterfaceTable;
    settings.objectLayerPairFilter = objectLayerPairFilterTable;
    settings.objectVsBroadPhaseLayerFilter = objectVsBroadPhaseLayerFilter;
    JPH_PhysicsSystem* system = JPH_PhysicsSystem_Create(&settings);
    JPH_BodyInterface* bodyInterface = JPH_PhysicsSystem_GetBodyInterface(system);

    JPH_BodyID floorId;
    {
        // Next we can create a rigid body to serve as the floor, we make a large box
        // Create the settings for the collision volume (the shape).
        // Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
        JPH_Vec3 boxHalfExtents = {100.0f, 1.0f, 100.0f};
        JPH_BoxShape* floorShape = JPH_BoxShape_Create(&boxHalfExtents, JPH_DEFAULT_CONVEX_RADIUS);

        JPH_Vec3 floorPosition = {0.0f, -1.0f, 0.0f};
        JPH_BodyCreationSettings* floorSettings = JPH_BodyCreationSettings_Create3(
            cast(const(JPH_Shape*)) floorShape,

            &floorPosition,
            null, // Identity,
            JPH_MotionType_Static,
            Layers.NON_MOVING);

        // Create the actual rigid body
        floorId = JPH_BodyInterface_CreateAndAddBody(bodyInterface, floorSettings, JPH_Activation_DontActivate);
        JPH_BodyCreationSettings_Destroy(floorSettings);
    }

    // Sphere
    JPH_BodyID sphereId;
    {
        JPH_SphereShape* sphereShape = JPH_SphereShape_Create(50.0f);
        JPH_Vec3 spherePosition = {0.0f, 2.0f, 0.0f};
        JPH_BodyCreationSettings* sphereSettings = JPH_BodyCreationSettings_Create3(
            cast(const(JPH_Shape*)) sphereShape,

            &spherePosition,
            null, // Identity,
            JPH_MotionType_Dynamic,
            Layers.MOVING);

        sphereId = JPH_BodyInterface_CreateAndAddBody(bodyInterface, sphereSettings, JPH_Activation_Activate);
        JPH_BodyCreationSettings_Destroy(sphereSettings);
    }

    // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
    // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
    JPH_Vec3 sphereLinearVelocity = {0.0f, -5.0f, 0.0f};
    JPH_BodyInterface_SetLinearVelocity(bodyInterface, sphereId, &sphereLinearVelocity);

    static float cCharacterHeightStanding = 1.35f;
    static float cCharacterRadiusStanding = 0.3f;
    static float cCharacterHeightCrouching = 0.8f;
    static float cCharacterRadiusCrouching = 0.3f;
    static float cInnerShapeFraction = 0.9f;

    JPH_CapsuleShape* capsuleShape = JPH_CapsuleShape_Create(
        0.5f * cCharacterHeightStanding, cCharacterRadiusStanding);

    JPH_Vec3 position = JPH_Vec3(0, 0.5f * cCharacterHeightStanding + cCharacterRadiusStanding);

    auto mStandingShape = JPH_RotatedTranslatedShape_Create(&position, null, cast(JPH_Shape*) capsuleShape);

    JPH_CharacterVirtualSettings characterSettings;

    JPH_CharacterVirtualSettings_Init(&characterSettings);
    characterSettings.base.shape = cast(const JPH_Shape*) mStandingShape;
    characterSettings.base.supportingVolume.normal = JPH_Vec3(0, 1, 0);
    characterSettings.base.supportingVolume.distance = -cCharacterRadiusStanding;

    static const JPH_RVec3 characterVirtualPosition = {-5.0f, 0, 3.0f};

    auto mAnimatedCharacterVirtual = JPH_CharacterVirtual_Create(
        &characterSettings, &characterVirtualPosition, null, 0, system);

    JPH_SixDOFConstraintSettings jointSettings;
    JPH_SixDOFConstraintSettings_Init(&jointSettings);

    // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
    const float cDeltaTime = 1.0f / 60.0f;

    // Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
    // You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
    // Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
    JPH_PhysicsSystem_OptimizeBroadPhase(system);

    // Now we're ready to simulate the body, keep simulating until it goes to sleep
    uint step = 0;
    while (JPH_BodyInterface_IsActive(bodyInterface, sphereId))
    {
        // Next step
        ++step;

        JPH_RVec3 currPosition;
        JPH_Vec3 currVelocity;

        JPH_BodyInterface_GetCenterOfMassPosition(bodyInterface, sphereId, &currPosition);
        JPH_BodyInterface_GetLinearVelocity(bodyInterface, sphereId, &currVelocity);

        import std.stdio;

        writefln("Step %s: Position %s, Velocity %s", step, currPosition, currVelocity);

        // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
        const int cCollisionSteps = 1;

        // Step the world
        JPH_PhysicsSystem_Update(system, cDeltaTime, cCollisionSteps, jobSystem);
    }

    // Remove the destroy sphere from the physics system. Note that the sphere itself keeps all of its state and can be re-added at any time.
    JPH_BodyInterface_RemoveAndDestroyBody(bodyInterface, sphereId);

    // Remove and destroy the floor
    JPH_BodyInterface_RemoveAndDestroyBody(bodyInterface, floorId);

    JPH_JobSystem_Destroy(jobSystem);

    JPH_PhysicsSystem_Destroy(system);
    JPH_Shutdown();

    writefln("Allocated %s bytes in total", totalMemory);

    return 0;
}
