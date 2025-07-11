module joltc;

// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

extern (C):

struct IAllocator
{
    byte* function(size_t) Alloc;
    void function(byte*) Free;
}

enum JOLT_C_H_ = 1;

__gshared IAllocator gJphAllocator;

enum JPH_DEFAULT_COLLISION_TOLERANCE = 1.0e-4f; // float cDefaultCollisionTolerance = 1.0e-4f
enum JPH_DEFAULT_PENETRATION_TOLERANCE = 1.0e-4f; // float cDefaultPenetrationTolerance = 1.0e-4f
enum JPH_DEFAULT_CONVEX_RADIUS = 0.05f; // float cDefaultConvexRadius = 0.05f
enum JPH_CAPSULE_PROJECTION_SLOP = 0.02f; // float cCapsuleProjectionSlop = 0.02f
enum JPH_MAX_PHYSICS_JOBS = 2048; // int cMaxPhysicsJobs = 2048
enum JPH_MAX_PHYSICS_BARRIERS = 8; // int cMaxPhysicsBarriers = 8
enum JPH_INVALID_COLLISION_GROUP_ID = ~0U;
enum JPH_INVALID_COLLISION_SUBGROUP_ID = ~0U;

alias JPH_Bool = uint;
alias JPH_BodyID = uint;
alias JPH_SubShapeID = uint;
alias JPH_ObjectLayer = uint;
alias JPH_BroadPhaseLayer = ubyte;
alias JPH_CollisionGroupID = uint;
alias JPH_CollisionSubGroupID = uint;
alias JPH_CharacterID = uint;

/* Forward declarations */
struct JPH_BroadPhaseLayerInterface;
struct JPH_ObjectVsBroadPhaseLayerFilter;
struct JPH_ObjectLayerPairFilter;

struct JPH_BroadPhaseLayerFilter;
struct JPH_ObjectLayerFilter;
struct JPH_BodyFilter;
struct JPH_ShapeFilter;

struct JPH_SimShapeFilter;

struct JPH_PhysicsStepListener;
struct JPH_PhysicsSystem;
struct JPH_PhysicsMaterial;

/* ShapeSettings */
struct JPH_ShapeSettings;
struct JPH_ConvexShapeSettings;
struct JPH_SphereShapeSettings;
struct JPH_BoxShapeSettings;
struct JPH_PlaneShapeSettings;
struct JPH_TriangleShapeSettings;
struct JPH_CapsuleShapeSettings;
struct JPH_TaperedCapsuleShapeSettings;
struct JPH_CylinderShapeSettings;
struct JPH_TaperedCylinderShapeSettings;
struct JPH_ConvexHullShapeSettings;
struct JPH_CompoundShapeSettings;
struct JPH_StaticCompoundShapeSettings;
struct JPH_MutableCompoundShapeSettings;
struct JPH_MeshShapeSettings;
struct JPH_HeightFieldShapeSettings;
struct JPH_RotatedTranslatedShapeSettings;
struct JPH_ScaledShapeSettings;
struct JPH_OffsetCenterOfMassShapeSettings;
struct JPH_EmptyShapeSettings;

/* Shape */
struct JPH_Shape;
struct JPH_ConvexShape;
struct JPH_SphereShape;
struct JPH_BoxShape;
struct JPH_PlaneShape;
struct JPH_CapsuleShape;
struct JPH_CylinderShape;
struct JPH_TaperedCylinderShape;
struct JPH_TriangleShape;
struct JPH_TaperedCapsuleShape;
struct JPH_ConvexHullShape;
struct JPH_CompoundShape;
struct JPH_StaticCompoundShape;
struct JPH_MutableCompoundShape;
struct JPH_MeshShape;
struct JPH_HeightFieldShape;
struct JPH_DecoratedShape;
struct JPH_RotatedTranslatedShape;
struct JPH_ScaledShape;
struct JPH_OffsetCenterOfMassShape;
struct JPH_EmptyShape;

struct JPH_BodyCreationSettings;
struct JPH_SoftBodyCreationSettings;
struct JPH_BodyInterface;
struct JPH_BodyLockInterface;
struct JPH_BroadPhaseQuery;
struct JPH_NarrowPhaseQuery;
struct JPH_MotionProperties;
struct JPH_Body;

struct JPH_ContactListener;
struct JPH_ContactManifold;
struct JPH_ContactSettings;

struct JPH_GroupFilter;
struct JPH_GroupFilterTable; /* Inherits JPH_GroupFilter */

/* Enums */
enum JPH_PhysicsUpdateError
{
    JPH_PhysicsUpdateError_None = 0,
    JPH_PhysicsUpdateError_ManifoldCacheFull = 1 << 0,
    JPH_PhysicsUpdateError_BodyPairCacheFull = 1 << 1,
    JPH_PhysicsUpdateError_ContactConstraintsFull = 1 << 2,

    _JPH_PhysicsUpdateError_Count = 5,
    _JPH_PhysicsUpdateError_Force32 = 0x7fffffff
}

alias JPH_PhysicsUpdateError_None = JPH_PhysicsUpdateError.JPH_PhysicsUpdateError_None;
alias JPH_PhysicsUpdateError_ManifoldCacheFull = JPH_PhysicsUpdateError
    .JPH_PhysicsUpdateError_ManifoldCacheFull;
alias JPH_PhysicsUpdateError_BodyPairCacheFull = JPH_PhysicsUpdateError
    .JPH_PhysicsUpdateError_BodyPairCacheFull;
alias JPH_PhysicsUpdateError_ContactConstraintsFull = JPH_PhysicsUpdateError
    .JPH_PhysicsUpdateError_ContactConstraintsFull;
alias _JPH_PhysicsUpdateError_Count = JPH_PhysicsUpdateError._JPH_PhysicsUpdateError_Count;
alias _JPH_PhysicsUpdateError_Force32 = JPH_PhysicsUpdateError._JPH_PhysicsUpdateError_Force32;

enum JPH_BodyType
{
    JPH_BodyType_Rigid = 0,
    JPH_BodyType_Soft = 1,

    _JPH_BodyType_Count = 2,
    _JPH_BodyType_Force32 = 0x7fffffff
}

alias JPH_BodyType_Rigid = JPH_BodyType.JPH_BodyType_Rigid;
alias JPH_BodyType_Soft = JPH_BodyType.JPH_BodyType_Soft;
alias _JPH_BodyType_Count = JPH_BodyType._JPH_BodyType_Count;
alias _JPH_BodyType_Force32 = JPH_BodyType._JPH_BodyType_Force32;

enum JPH_MotionType
{
    JPH_MotionType_Static = 0,
    JPH_MotionType_Kinematic = 1,
    JPH_MotionType_Dynamic = 2,

    _JPH_MotionType_Count = 3,
    _JPH_MotionType_Force32 = 0x7fffffff
}

alias JPH_MotionType_Static = JPH_MotionType.JPH_MotionType_Static;
alias JPH_MotionType_Kinematic = JPH_MotionType.JPH_MotionType_Kinematic;
alias JPH_MotionType_Dynamic = JPH_MotionType.JPH_MotionType_Dynamic;
alias _JPH_MotionType_Count = JPH_MotionType._JPH_MotionType_Count;
alias _JPH_MotionType_Force32 = JPH_MotionType._JPH_MotionType_Force32;

enum JPH_Activation
{
    JPH_Activation_Activate = 0,
    JPH_Activation_DontActivate = 1,

    _JPH_Activation_Count = 2,
    _JPH_Activation_Force32 = 0x7fffffff
}

alias JPH_Activation_Activate = JPH_Activation.JPH_Activation_Activate;
alias JPH_Activation_DontActivate = JPH_Activation.JPH_Activation_DontActivate;
alias _JPH_Activation_Count = JPH_Activation._JPH_Activation_Count;
alias _JPH_Activation_Force32 = JPH_Activation._JPH_Activation_Force32;

enum JPH_ValidateResult
{
    JPH_ValidateResult_AcceptAllContactsForThisBodyPair = 0,
    JPH_ValidateResult_AcceptContact = 1,
    JPH_ValidateResult_RejectContact = 2,
    JPH_ValidateResult_RejectAllContactsForThisBodyPair = 3,

    _JPH_ValidateResult_Count = 4,
    _JPH_ValidateResult_Force32 = 0x7fffffff
}

alias JPH_ValidateResult_AcceptAllContactsForThisBodyPair = JPH_ValidateResult
    .JPH_ValidateResult_AcceptAllContactsForThisBodyPair;
alias JPH_ValidateResult_AcceptContact = JPH_ValidateResult.JPH_ValidateResult_AcceptContact;
alias JPH_ValidateResult_RejectContact = JPH_ValidateResult.JPH_ValidateResult_RejectContact;
alias JPH_ValidateResult_RejectAllContactsForThisBodyPair = JPH_ValidateResult
    .JPH_ValidateResult_RejectAllContactsForThisBodyPair;
alias _JPH_ValidateResult_Count = JPH_ValidateResult._JPH_ValidateResult_Count;
alias _JPH_ValidateResult_Force32 = JPH_ValidateResult._JPH_ValidateResult_Force32;

enum JPH_ShapeType
{
    JPH_ShapeType_Convex = 0,
    JPH_ShapeType_Compound = 1,
    JPH_ShapeType_Decorated = 2,
    JPH_ShapeType_Mesh = 3,
    JPH_ShapeType_HeightField = 4,
    JPH_ShapeType_SoftBody = 5,

    JPH_ShapeType_User1 = 6,
    JPH_ShapeType_User2 = 7,
    JPH_ShapeType_User3 = 8,
    JPH_ShapeType_User4 = 9,

    _JPH_ShapeType_Count = 10,
    _JPH_ShapeType_Force32 = 0x7fffffff
}

alias JPH_ShapeType_Convex = JPH_ShapeType.JPH_ShapeType_Convex;
alias JPH_ShapeType_Compound = JPH_ShapeType.JPH_ShapeType_Compound;
alias JPH_ShapeType_Decorated = JPH_ShapeType.JPH_ShapeType_Decorated;
alias JPH_ShapeType_Mesh = JPH_ShapeType.JPH_ShapeType_Mesh;
alias JPH_ShapeType_HeightField = JPH_ShapeType.JPH_ShapeType_HeightField;
alias JPH_ShapeType_SoftBody = JPH_ShapeType.JPH_ShapeType_SoftBody;
alias JPH_ShapeType_User1 = JPH_ShapeType.JPH_ShapeType_User1;
alias JPH_ShapeType_User2 = JPH_ShapeType.JPH_ShapeType_User2;
alias JPH_ShapeType_User3 = JPH_ShapeType.JPH_ShapeType_User3;
alias JPH_ShapeType_User4 = JPH_ShapeType.JPH_ShapeType_User4;
alias _JPH_ShapeType_Count = JPH_ShapeType._JPH_ShapeType_Count;
alias _JPH_ShapeType_Force32 = JPH_ShapeType._JPH_ShapeType_Force32;

enum JPH_ShapeSubType
{
    JPH_ShapeSubType_Sphere = 0,
    JPH_ShapeSubType_Box = 1,
    JPH_ShapeSubType_Triangle = 2,
    JPH_ShapeSubType_Capsule = 3,
    JPH_ShapeSubType_TaperedCapsule = 4,
    JPH_ShapeSubType_Cylinder = 5,
    JPH_ShapeSubType_ConvexHull = 6,
    JPH_ShapeSubType_StaticCompound = 7,
    JPH_ShapeSubType_MutableCompound = 8,
    JPH_ShapeSubType_RotatedTranslated = 9,
    JPH_ShapeSubType_Scaled = 10,
    JPH_ShapeSubType_OffsetCenterOfMass = 11,
    JPH_ShapeSubType_Mesh = 12,
    JPH_ShapeSubType_HeightField = 13,
    JPH_ShapeSubType_SoftBody = 14,

    _JPH_ShapeSubType_Count = 15,
    _JPH_ShapeSubType_Force32 = 0x7fffffff
}

alias JPH_ShapeSubType_Sphere = JPH_ShapeSubType.JPH_ShapeSubType_Sphere;
alias JPH_ShapeSubType_Box = JPH_ShapeSubType.JPH_ShapeSubType_Box;
alias JPH_ShapeSubType_Triangle = JPH_ShapeSubType.JPH_ShapeSubType_Triangle;
alias JPH_ShapeSubType_Capsule = JPH_ShapeSubType.JPH_ShapeSubType_Capsule;
alias JPH_ShapeSubType_TaperedCapsule = JPH_ShapeSubType.JPH_ShapeSubType_TaperedCapsule;
alias JPH_ShapeSubType_Cylinder = JPH_ShapeSubType.JPH_ShapeSubType_Cylinder;
alias JPH_ShapeSubType_ConvexHull = JPH_ShapeSubType.JPH_ShapeSubType_ConvexHull;
alias JPH_ShapeSubType_StaticCompound = JPH_ShapeSubType.JPH_ShapeSubType_StaticCompound;
alias JPH_ShapeSubType_MutableCompound = JPH_ShapeSubType.JPH_ShapeSubType_MutableCompound;
alias JPH_ShapeSubType_RotatedTranslated = JPH_ShapeSubType.JPH_ShapeSubType_RotatedTranslated;
alias JPH_ShapeSubType_Scaled = JPH_ShapeSubType.JPH_ShapeSubType_Scaled;
alias JPH_ShapeSubType_OffsetCenterOfMass = JPH_ShapeSubType.JPH_ShapeSubType_OffsetCenterOfMass;
alias JPH_ShapeSubType_Mesh = JPH_ShapeSubType.JPH_ShapeSubType_Mesh;
alias JPH_ShapeSubType_HeightField = JPH_ShapeSubType.JPH_ShapeSubType_HeightField;
alias JPH_ShapeSubType_SoftBody = JPH_ShapeSubType.JPH_ShapeSubType_SoftBody;
alias _JPH_ShapeSubType_Count = JPH_ShapeSubType._JPH_ShapeSubType_Count;
alias _JPH_ShapeSubType_Force32 = JPH_ShapeSubType._JPH_ShapeSubType_Force32;

enum JPH_ConstraintType
{
    JPH_ConstraintType_Constraint = 0,
    JPH_ConstraintType_TwoBodyConstraint = 1,

    _JPH_ConstraintType_Count = 2,
    _JPH_ConstraintType_Force32 = 0x7fffffff
}

alias JPH_ConstraintType_Constraint = JPH_ConstraintType.JPH_ConstraintType_Constraint;
alias JPH_ConstraintType_TwoBodyConstraint = JPH_ConstraintType
    .JPH_ConstraintType_TwoBodyConstraint;
alias _JPH_ConstraintType_Count = JPH_ConstraintType._JPH_ConstraintType_Count;
alias _JPH_ConstraintType_Force32 = JPH_ConstraintType._JPH_ConstraintType_Force32;

enum JPH_ConstraintSubType
{
    JPH_ConstraintSubType_Fixed = 0,
    JPH_ConstraintSubType_Point = 1,
    JPH_ConstraintSubType_Hinge = 2,
    JPH_ConstraintSubType_Slider = 3,
    JPH_ConstraintSubType_Distance = 4,
    JPH_ConstraintSubType_Cone = 5,
    JPH_ConstraintSubType_SwingTwist = 6,
    JPH_ConstraintSubType_SixDOF = 7,
    JPH_ConstraintSubType_Path = 8,
    JPH_ConstraintSubType_Vehicle = 9,
    JPH_ConstraintSubType_RackAndPinion = 10,
    JPH_ConstraintSubType_Gear = 11,
    JPH_ConstraintSubType_Pulley = 12,

    JPH_ConstraintSubType_User1 = 13,
    JPH_ConstraintSubType_User2 = 14,
    JPH_ConstraintSubType_User3 = 15,
    JPH_ConstraintSubType_User4 = 16,

    _JPH_ConstraintSubType_Count = 17,
    _JPH_ConstraintSubType_Force32 = 0x7fffffff
}

alias JPH_ConstraintSubType_Fixed = JPH_ConstraintSubType.JPH_ConstraintSubType_Fixed;
alias JPH_ConstraintSubType_Point = JPH_ConstraintSubType.JPH_ConstraintSubType_Point;
alias JPH_ConstraintSubType_Hinge = JPH_ConstraintSubType.JPH_ConstraintSubType_Hinge;
alias JPH_ConstraintSubType_Slider = JPH_ConstraintSubType.JPH_ConstraintSubType_Slider;
alias JPH_ConstraintSubType_Distance = JPH_ConstraintSubType.JPH_ConstraintSubType_Distance;
alias JPH_ConstraintSubType_Cone = JPH_ConstraintSubType.JPH_ConstraintSubType_Cone;
alias JPH_ConstraintSubType_SwingTwist = JPH_ConstraintSubType.JPH_ConstraintSubType_SwingTwist;
alias JPH_ConstraintSubType_SixDOF = JPH_ConstraintSubType.JPH_ConstraintSubType_SixDOF;
alias JPH_ConstraintSubType_Path = JPH_ConstraintSubType.JPH_ConstraintSubType_Path;
alias JPH_ConstraintSubType_Vehicle = JPH_ConstraintSubType.JPH_ConstraintSubType_Vehicle;
alias JPH_ConstraintSubType_RackAndPinion = JPH_ConstraintSubType
    .JPH_ConstraintSubType_RackAndPinion;
alias JPH_ConstraintSubType_Gear = JPH_ConstraintSubType.JPH_ConstraintSubType_Gear;
alias JPH_ConstraintSubType_Pulley = JPH_ConstraintSubType.JPH_ConstraintSubType_Pulley;
alias JPH_ConstraintSubType_User1 = JPH_ConstraintSubType.JPH_ConstraintSubType_User1;
alias JPH_ConstraintSubType_User2 = JPH_ConstraintSubType.JPH_ConstraintSubType_User2;
alias JPH_ConstraintSubType_User3 = JPH_ConstraintSubType.JPH_ConstraintSubType_User3;
alias JPH_ConstraintSubType_User4 = JPH_ConstraintSubType.JPH_ConstraintSubType_User4;
alias _JPH_ConstraintSubType_Count = JPH_ConstraintSubType._JPH_ConstraintSubType_Count;
alias _JPH_ConstraintSubType_Force32 = JPH_ConstraintSubType._JPH_ConstraintSubType_Force32;

enum JPH_ConstraintSpace
{
    JPH_ConstraintSpace_LocalToBodyCOM = 0,
    JPH_ConstraintSpace_WorldSpace = 1,

    _JPH_ConstraintSpace_Count = 2,
    _JPH_ConstraintSpace_Force32 = 0x7fffffff
}

alias JPH_ConstraintSpace_LocalToBodyCOM = JPH_ConstraintSpace.JPH_ConstraintSpace_LocalToBodyCOM;
alias JPH_ConstraintSpace_WorldSpace = JPH_ConstraintSpace.JPH_ConstraintSpace_WorldSpace;
alias _JPH_ConstraintSpace_Count = JPH_ConstraintSpace._JPH_ConstraintSpace_Count;
alias _JPH_ConstraintSpace_Force32 = JPH_ConstraintSpace._JPH_ConstraintSpace_Force32;

enum JPH_MotionQuality
{
    JPH_MotionQuality_Discrete = 0,
    JPH_MotionQuality_LinearCast = 1,

    _JPH_MotionQuality_Count = 2,
    _JPH_MotionQuality_Force32 = 0x7fffffff
}

alias JPH_MotionQuality_Discrete = JPH_MotionQuality.JPH_MotionQuality_Discrete;
alias JPH_MotionQuality_LinearCast = JPH_MotionQuality.JPH_MotionQuality_LinearCast;
alias _JPH_MotionQuality_Count = JPH_MotionQuality._JPH_MotionQuality_Count;
alias _JPH_MotionQuality_Force32 = JPH_MotionQuality._JPH_MotionQuality_Force32;

enum JPH_OverrideMassProperties
{
    JPH_OverrideMassProperties_CalculateMassAndInertia = 0,
    JPH_OverrideMassProperties_CalculateInertia = 1,
    JPH_OverrideMassProperties_MassAndInertiaProvided = 2,

    _JPH_JPH_OverrideMassProperties_Count = 3,
    _JPH_JPH_OverrideMassProperties_Force32 = 0x7FFFFFFF
}

alias JPH_OverrideMassProperties_CalculateMassAndInertia = JPH_OverrideMassProperties
    .JPH_OverrideMassProperties_CalculateMassAndInertia;
alias JPH_OverrideMassProperties_CalculateInertia = JPH_OverrideMassProperties
    .JPH_OverrideMassProperties_CalculateInertia;
alias JPH_OverrideMassProperties_MassAndInertiaProvided = JPH_OverrideMassProperties
    .JPH_OverrideMassProperties_MassAndInertiaProvided;
alias _JPH_JPH_OverrideMassProperties_Count = JPH_OverrideMassProperties
    ._JPH_JPH_OverrideMassProperties_Count;
alias _JPH_JPH_OverrideMassProperties_Force32 = JPH_OverrideMassProperties
    ._JPH_JPH_OverrideMassProperties_Force32;

enum JPH_AllowedDOFs
{
    JPH_AllowedDOFs_All = 0b111111,
    JPH_AllowedDOFs_TranslationX = 0b000001,
    JPH_AllowedDOFs_TranslationY = 0b000010,
    JPH_AllowedDOFs_TranslationZ = 0b000100,
    JPH_AllowedDOFs_RotationX = 0b001000,
    JPH_AllowedDOFs_RotationY = 0b010000,
    JPH_AllowedDOFs_RotationZ = 0b100000,
    JPH_AllowedDOFs_Plane2D = JPH_AllowedDOFs_TranslationX | JPH_AllowedDOFs_TranslationY | JPH_AllowedDOFs_RotationZ,

    _JPH_AllowedDOFs_Count = 36,
    _JPH_AllowedDOFs_Force32 = 0x7FFFFFFF
}

alias JPH_AllowedDOFs_All = JPH_AllowedDOFs.JPH_AllowedDOFs_All;
alias JPH_AllowedDOFs_TranslationX = JPH_AllowedDOFs.JPH_AllowedDOFs_TranslationX;
alias JPH_AllowedDOFs_TranslationY = JPH_AllowedDOFs.JPH_AllowedDOFs_TranslationY;
alias JPH_AllowedDOFs_TranslationZ = JPH_AllowedDOFs.JPH_AllowedDOFs_TranslationZ;
alias JPH_AllowedDOFs_RotationX = JPH_AllowedDOFs.JPH_AllowedDOFs_RotationX;
alias JPH_AllowedDOFs_RotationY = JPH_AllowedDOFs.JPH_AllowedDOFs_RotationY;
alias JPH_AllowedDOFs_RotationZ = JPH_AllowedDOFs.JPH_AllowedDOFs_RotationZ;
alias JPH_AllowedDOFs_Plane2D = JPH_AllowedDOFs.JPH_AllowedDOFs_Plane2D;
alias _JPH_AllowedDOFs_Count = JPH_AllowedDOFs._JPH_AllowedDOFs_Count;
alias _JPH_AllowedDOFs_Force32 = JPH_AllowedDOFs._JPH_AllowedDOFs_Force32;

enum JPH_GroundState
{
    JPH_GroundState_OnGround = 0,
    JPH_GroundState_OnSteepGround = 1,
    JPH_GroundState_NotSupported = 2,
    JPH_GroundState_InAir = 3,

    _JPH_GroundState_Count = 4,
    _JPH_GroundState_Force32 = 0x7FFFFFFF
}

alias JPH_GroundState_OnGround = JPH_GroundState.JPH_GroundState_OnGround;
alias JPH_GroundState_OnSteepGround = JPH_GroundState.JPH_GroundState_OnSteepGround;
alias JPH_GroundState_NotSupported = JPH_GroundState.JPH_GroundState_NotSupported;
alias JPH_GroundState_InAir = JPH_GroundState.JPH_GroundState_InAir;
alias _JPH_GroundState_Count = JPH_GroundState._JPH_GroundState_Count;
alias _JPH_GroundState_Force32 = JPH_GroundState._JPH_GroundState_Force32;

enum JPH_BackFaceMode
{
    JPH_BackFaceMode_IgnoreBackFaces = 0,
    JPH_BackFaceMode_CollideWithBackFaces = 1,

    _JPH_BackFaceMode_Count = 2,
    _JPH_BackFaceMode_Force32 = 0x7FFFFFFF
}

alias JPH_BackFaceMode_IgnoreBackFaces = JPH_BackFaceMode.JPH_BackFaceMode_IgnoreBackFaces;
alias JPH_BackFaceMode_CollideWithBackFaces = JPH_BackFaceMode
    .JPH_BackFaceMode_CollideWithBackFaces;
alias _JPH_BackFaceMode_Count = JPH_BackFaceMode._JPH_BackFaceMode_Count;
alias _JPH_BackFaceMode_Force32 = JPH_BackFaceMode._JPH_BackFaceMode_Force32;

enum JPH_ActiveEdgeMode
{
    JPH_ActiveEdgeMode_CollideOnlyWithActive = 0,
    JPH_ActiveEdgeMode_CollideWithAll = 1,

    _JPH_ActiveEdgeMode_Count = 2,
    _JPH_ActiveEdgeMode_Force32 = 0x7FFFFFFF
}

alias JPH_ActiveEdgeMode_CollideOnlyWithActive = JPH_ActiveEdgeMode
    .JPH_ActiveEdgeMode_CollideOnlyWithActive;
alias JPH_ActiveEdgeMode_CollideWithAll = JPH_ActiveEdgeMode.JPH_ActiveEdgeMode_CollideWithAll;
alias _JPH_ActiveEdgeMode_Count = JPH_ActiveEdgeMode._JPH_ActiveEdgeMode_Count;
alias _JPH_ActiveEdgeMode_Force32 = JPH_ActiveEdgeMode._JPH_ActiveEdgeMode_Force32;

enum JPH_CollectFacesMode
{
    JPH_CollectFacesMode_CollectFaces = 0,
    JPH_CollectFacesMode_NoFaces = 1,

    _JPH_CollectFacesMode_Count = 2,
    _JPH_CollectFacesMode_Force32 = 0x7FFFFFFF
}

alias JPH_CollectFacesMode_CollectFaces = JPH_CollectFacesMode.JPH_CollectFacesMode_CollectFaces;
alias JPH_CollectFacesMode_NoFaces = JPH_CollectFacesMode.JPH_CollectFacesMode_NoFaces;
alias _JPH_CollectFacesMode_Count = JPH_CollectFacesMode._JPH_CollectFacesMode_Count;
alias _JPH_CollectFacesMode_Force32 = JPH_CollectFacesMode._JPH_CollectFacesMode_Force32;

enum JPH_MotorState
{
    JPH_MotorState_Off = 0,
    JPH_MotorState_Velocity = 1,
    JPH_MotorState_Position = 2,

    _JPH_MotorState_Count = 3,
    _JPH_MotorState_Force32 = 0x7FFFFFFF
}

alias JPH_MotorState_Off = JPH_MotorState.JPH_MotorState_Off;
alias JPH_MotorState_Velocity = JPH_MotorState.JPH_MotorState_Velocity;
alias JPH_MotorState_Position = JPH_MotorState.JPH_MotorState_Position;
alias _JPH_MotorState_Count = JPH_MotorState._JPH_MotorState_Count;
alias _JPH_MotorState_Force32 = JPH_MotorState._JPH_MotorState_Force32;

enum JPH_CollisionCollectorType
{
    JPH_CollisionCollectorType_AllHit = 0,
    JPH_CollisionCollectorType_AllHitSorted = 1,
    JPH_CollisionCollectorType_ClosestHit = 2,
    JPH_CollisionCollectorType_AnyHit = 3,

    _JPH_CollisionCollectorType_Count = 4,
    _JPH_CollisionCollectorType_Force32 = 0x7FFFFFFF
}

alias JPH_CollisionCollectorType_AllHit = JPH_CollisionCollectorType
    .JPH_CollisionCollectorType_AllHit;
alias JPH_CollisionCollectorType_AllHitSorted = JPH_CollisionCollectorType
    .JPH_CollisionCollectorType_AllHitSorted;
alias JPH_CollisionCollectorType_ClosestHit = JPH_CollisionCollectorType
    .JPH_CollisionCollectorType_ClosestHit;
alias JPH_CollisionCollectorType_AnyHit = JPH_CollisionCollectorType
    .JPH_CollisionCollectorType_AnyHit;
alias _JPH_CollisionCollectorType_Count = JPH_CollisionCollectorType
    ._JPH_CollisionCollectorType_Count;
alias _JPH_CollisionCollectorType_Force32 = JPH_CollisionCollectorType
    ._JPH_CollisionCollectorType_Force32;

enum JPH_SwingType
{
    JPH_SwingType_Cone = 0,
    JPH_SwingType_Pyramid = 1,

    _JPH_SwingType_Count = 2,
    _JPH_SwingType_Force32 = 0x7FFFFFFF
}

alias JPH_SwingType_Cone = JPH_SwingType.JPH_SwingType_Cone;
alias JPH_SwingType_Pyramid = JPH_SwingType.JPH_SwingType_Pyramid;
alias _JPH_SwingType_Count = JPH_SwingType._JPH_SwingType_Count;
alias _JPH_SwingType_Force32 = JPH_SwingType._JPH_SwingType_Force32;

enum JPH_SixDOFConstraintAxis
{
    JPH_SixDOFConstraintAxis_TranslationX = 0,
    JPH_SixDOFConstraintAxis_TranslationY = 1,
    JPH_SixDOFConstraintAxis_TranslationZ = 2,

    JPH_SixDOFConstraintAxis_RotationX = 3,
    JPH_SixDOFConstraintAxis_RotationY = 4,
    JPH_SixDOFConstraintAxis_RotationZ = 5,

    _JPH_SixDOFConstraintAxis_Num = 6,
    _JPH_SixDOFConstraintAxis_NumTranslation = JPH_SixDOFConstraintAxis_TranslationZ + 1,
    _JPH_SixDOFConstraintAxis_Force32 = 0x7FFFFFFF
}

alias JPH_SixDOFConstraintAxis_TranslationX = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_TranslationX;
alias JPH_SixDOFConstraintAxis_TranslationY = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_TranslationY;
alias JPH_SixDOFConstraintAxis_TranslationZ = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_TranslationZ;
alias JPH_SixDOFConstraintAxis_RotationX = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_RotationX;
alias JPH_SixDOFConstraintAxis_RotationY = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_RotationY;
alias JPH_SixDOFConstraintAxis_RotationZ = JPH_SixDOFConstraintAxis
    .JPH_SixDOFConstraintAxis_RotationZ;
alias _JPH_SixDOFConstraintAxis_Num = JPH_SixDOFConstraintAxis._JPH_SixDOFConstraintAxis_Num;
alias _JPH_SixDOFConstraintAxis_NumTranslation = JPH_SixDOFConstraintAxis
    ._JPH_SixDOFConstraintAxis_NumTranslation;
alias _JPH_SixDOFConstraintAxis_Force32 = JPH_SixDOFConstraintAxis
    ._JPH_SixDOFConstraintAxis_Force32;

enum JPH_SpringMode
{
    JPH_SpringMode_FrequencyAndDamping = 0,
    JPH_SpringMode_StiffnessAndDamping = 1,

    _JPH_SpringMode_Count = 2,
    _JPH_SpringMode_Force32 = 0x7FFFFFFF
}

alias JPH_SpringMode_FrequencyAndDamping = JPH_SpringMode.JPH_SpringMode_FrequencyAndDamping;
alias JPH_SpringMode_StiffnessAndDamping = JPH_SpringMode.JPH_SpringMode_StiffnessAndDamping;
alias _JPH_SpringMode_Count = JPH_SpringMode._JPH_SpringMode_Count;
alias _JPH_SpringMode_Force32 = JPH_SpringMode._JPH_SpringMode_Force32;

/// Defines how to color soft body constraints
enum JPH_SoftBodyConstraintColor
{
    JPH_SoftBodyConstraintColor_ConstraintType = 0, /// Draw different types of constraints in different colors
    JPH_SoftBodyConstraintColor_ConstraintGroup = 1, /// Draw constraints in the same group in the same color, non-parallel group will be red
    JPH_SoftBodyConstraintColor_ConstraintOrder = 2, /// Draw constraints in the same group in the same color, non-parallel group will be red, and order within each group will be indicated with gradient

    _JPH_SoftBodyConstraintColor_Count = 3,
    _JPH_SoftBodyConstraintColor_Force32 = 0x7FFFFFFF
}

alias JPH_SoftBodyConstraintColor_ConstraintType = JPH_SoftBodyConstraintColor
    .JPH_SoftBodyConstraintColor_ConstraintType;
alias JPH_SoftBodyConstraintColor_ConstraintGroup = JPH_SoftBodyConstraintColor
    .JPH_SoftBodyConstraintColor_ConstraintGroup;
alias JPH_SoftBodyConstraintColor_ConstraintOrder = JPH_SoftBodyConstraintColor
    .JPH_SoftBodyConstraintColor_ConstraintOrder;
alias _JPH_SoftBodyConstraintColor_Count = JPH_SoftBodyConstraintColor
    ._JPH_SoftBodyConstraintColor_Count;
alias _JPH_SoftBodyConstraintColor_Force32 = JPH_SoftBodyConstraintColor
    ._JPH_SoftBodyConstraintColor_Force32;

enum JPH_BodyManager_ShapeColor
{
    JPH_BodyManager_ShapeColor_InstanceColor = 0, ///< Random color per instance
    JPH_BodyManager_ShapeColor_ShapeTypeColor = 1, ///< Convex = green, scaled = yellow, compound = orange, mesh = red
    JPH_BodyManager_ShapeColor_MotionTypeColor = 2, ///< Static = grey, keyframed = green, dynamic = random color per instance
    JPH_BodyManager_ShapeColor_SleepColor = 3, ///< Static = grey, keyframed = green, dynamic = yellow, sleeping = red
    JPH_BodyManager_ShapeColor_IslandColor = 4, ///< Static = grey, active = random color per island, sleeping = light grey
    JPH_BodyManager_ShapeColor_MaterialColor = 5, ///< Color as defined by the PhysicsMaterial of the shape

    _JPH_BodyManager_ShapeColor_Count = 6,
    _JPH_BodyManager_ShapeColor_Force32 = 0x7FFFFFFF
}

alias JPH_BodyManager_ShapeColor_InstanceColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_InstanceColor;
alias JPH_BodyManager_ShapeColor_ShapeTypeColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_ShapeTypeColor;
alias JPH_BodyManager_ShapeColor_MotionTypeColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_MotionTypeColor;
alias JPH_BodyManager_ShapeColor_SleepColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_SleepColor;
alias JPH_BodyManager_ShapeColor_IslandColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_IslandColor;
alias JPH_BodyManager_ShapeColor_MaterialColor = JPH_BodyManager_ShapeColor
    .JPH_BodyManager_ShapeColor_MaterialColor;
alias _JPH_BodyManager_ShapeColor_Count = JPH_BodyManager_ShapeColor
    ._JPH_BodyManager_ShapeColor_Count;
alias _JPH_BodyManager_ShapeColor_Force32 = JPH_BodyManager_ShapeColor
    ._JPH_BodyManager_ShapeColor_Force32;

enum JPH_DebugRenderer_CastShadow
{
    JPH_DebugRenderer_CastShadow_On = 0, ///< This shape should cast a shadow
    JPH_DebugRenderer_CastShadow_Off = 1, ///< This shape should not cast a shadow

    _JPH_DebugRenderer_CastShadow_Count = 2,
    _JPH_DebugRenderer_CastShadow_Force32 = 0x7FFFFFFF
}

alias JPH_DebugRenderer_CastShadow_On = JPH_DebugRenderer_CastShadow
    .JPH_DebugRenderer_CastShadow_On;
alias JPH_DebugRenderer_CastShadow_Off = JPH_DebugRenderer_CastShadow
    .JPH_DebugRenderer_CastShadow_Off;
alias _JPH_DebugRenderer_CastShadow_Count = JPH_DebugRenderer_CastShadow
    ._JPH_DebugRenderer_CastShadow_Count;
alias _JPH_DebugRenderer_CastShadow_Force32 = JPH_DebugRenderer_CastShadow
    ._JPH_DebugRenderer_CastShadow_Force32;

enum JPH_DebugRenderer_DrawMode
{
    JPH_DebugRenderer_DrawMode_Solid = 0, ///< Draw as a solid shape
    JPH_DebugRenderer_DrawMode_Wireframe = 1, ///< Draw as wireframe

    _JPH_DebugRenderer_DrawMode_Count = 2,
    _JPH_DebugRenderer_DrawMode_Force32 = 0x7FFFFFFF
}

alias JPH_DebugRenderer_DrawMode_Solid = JPH_DebugRenderer_DrawMode
    .JPH_DebugRenderer_DrawMode_Solid;
alias JPH_DebugRenderer_DrawMode_Wireframe = JPH_DebugRenderer_DrawMode
    .JPH_DebugRenderer_DrawMode_Wireframe;
alias _JPH_DebugRenderer_DrawMode_Count = JPH_DebugRenderer_DrawMode
    ._JPH_DebugRenderer_DrawMode_Count;
alias _JPH_DebugRenderer_DrawMode_Force32 = JPH_DebugRenderer_DrawMode
    ._JPH_DebugRenderer_DrawMode_Force32;

enum JPH_Mesh_Shape_BuildQuality
{
    JPH_Mesh_Shape_BuildQuality_FavorRuntimePerformance = 0,
    JPH_Mesh_Shape_BuildQuality_FavorBuildSpeed = 1,

    _JPH_Mesh_Shape_BuildQuality_Count = 2,
    _JPH_Mesh_Shape_BuildQuality_Force32 = 0x7FFFFFFF
}

alias JPH_Mesh_Shape_BuildQuality_FavorRuntimePerformance = JPH_Mesh_Shape_BuildQuality
    .JPH_Mesh_Shape_BuildQuality_FavorRuntimePerformance;
alias JPH_Mesh_Shape_BuildQuality_FavorBuildSpeed = JPH_Mesh_Shape_BuildQuality
    .JPH_Mesh_Shape_BuildQuality_FavorBuildSpeed;
alias _JPH_Mesh_Shape_BuildQuality_Count = JPH_Mesh_Shape_BuildQuality
    ._JPH_Mesh_Shape_BuildQuality_Count;
alias _JPH_Mesh_Shape_BuildQuality_Force32 = JPH_Mesh_Shape_BuildQuality
    ._JPH_Mesh_Shape_BuildQuality_Force32;

enum JPH_TransmissionMode
{
    JPH_TransmissionMode_Auto = 0,
    JPH_TransmissionMode_Manual = 1,

    _JPH_TransmissionMode_Count = 2,
    _JPH_TransmissionMode_Force32 = 0x7FFFFFFF
}

alias JPH_TransmissionMode_Auto = JPH_TransmissionMode.JPH_TransmissionMode_Auto;
alias JPH_TransmissionMode_Manual = JPH_TransmissionMode.JPH_TransmissionMode_Manual;
alias _JPH_TransmissionMode_Count = JPH_TransmissionMode._JPH_TransmissionMode_Count;
alias _JPH_TransmissionMode_Force32 = JPH_TransmissionMode._JPH_TransmissionMode_Force32;

struct JPH_Vec3
{
    float x;
    float y;
    float z;
}

struct JPH_Vec4
{
    float x;
    float y;
    float z;
    float w;
}

struct JPH_Quat
{
    float x;
    float y;
    float z;
    float w;
}

struct JPH_Plane
{
    JPH_Vec3 normal;
    float distance;
}

struct JPH_Matrix4x4
{
    float m11;
    float m12;
    float m13;
    float m14;
    float m21;
    float m22;
    float m23;
    float m24;
    float m31;
    float m32;
    float m33;
    float m34;
    float m41;
    float m42;
    float m43;
    float m44;
}

alias JPH_RVec3 = JPH_Vec3;
alias JPH_RMatrix4x4 = JPH_Matrix4x4;

alias JPH_Color = uint;

struct JPH_AABox
{
    JPH_Vec3 min;
    JPH_Vec3 max;
}

struct JPH_Triangle
{
    JPH_Vec3 v1;
    JPH_Vec3 v2;
    JPH_Vec3 v3;
    uint materialIndex;
}

struct JPH_IndexedTriangleNoMaterial
{
    uint i1;
    uint i2;
    uint i3;
}

struct JPH_IndexedTriangle
{
    uint i1;
    uint i2;
    uint i3;
    uint materialIndex;
    uint userData;
}

struct JPH_MassProperties
{
    float mass;
    JPH_Matrix4x4 inertia;
}

struct JPH_CollideSettingsBase
{
    /// How active edges (edges that a moving object should bump into) are handled
    JPH_ActiveEdgeMode activeEdgeMode; /* = JPH_ActiveEdgeMode_CollideOnlyWithActive*/

    /// If colliding faces should be collected or only the collision point
    JPH_CollectFacesMode collectFacesMode; /* = JPH_CollectFacesMode_NoFaces*/

    /// If objects are closer than this distance, they are considered to be colliding (used for GJK) (unit: meter)
    float collisionTolerance; /* = JPH_DEFAULT_COLLISION_TOLERANCE*/

    /// A factor that determines the accuracy of the penetration depth calculation. If the change of the squared distance is less than tolerance * current_penetration_depth^2 the algorithm will terminate. (unit: dimensionless)
    float penetrationTolerance; /* = JPH_DEFAULT_PENETRATION_TOLERANCE*/

    /// When mActiveEdgeMode is CollideOnlyWithActive a movement direction can be provided. When hitting an inactive edge, the system will select the triangle normal as penetration depth only if it impedes the movement less than with the calculated penetration depth.
    JPH_Vec3 activeEdgeMovementDirection; /* = Vec3::sZero()*/
}

/* CollideShapeSettings */
struct JPH_CollideShapeSettings
{
    JPH_CollideSettingsBase base; /* Inherits JPH_CollideSettingsBase */
    /// When > 0 contacts in the vicinity of the query shape can be found. All nearest contacts that are not further away than this distance will be found (unit: meter)
    float maxSeparationDistance; /* = 0.0f*/

    /// How backfacing triangles should be treated
    JPH_BackFaceMode backFaceMode; /* = JPH_BackFaceMode_IgnoreBackFaces*/
}

/* ShapeCastSettings */
struct JPH_ShapeCastSettings
{
    JPH_CollideSettingsBase base; /* Inherits JPH_CollideSettingsBase */

    /// How backfacing triangles should be treated (should we report moving from back to front for triangle based shapes, e.g. for MeshShape/HeightFieldShape?)
    JPH_BackFaceMode backFaceModeTriangles; /* = JPH_BackFaceMode_IgnoreBackFaces*/

    /// How backfacing convex objects should be treated (should we report starting inside an object and moving out?)
    JPH_BackFaceMode backFaceModeConvex; /* = JPH_BackFaceMode_IgnoreBackFaces*/

    /// Indicates if we want to shrink the shape by the convex radius and then expand it again. This speeds up collision detection and gives a more accurate normal at the cost of a more 'rounded' shape.
    bool useShrunkenShapeAndConvexRadius; /* = false*/

    /// When true, and the shape is intersecting at the beginning of the cast (fraction = 0) then this will calculate the deepest penetration point (costing additional CPU time)
    bool returnDeepestPoint; /* = false*/
}

struct JPH_RayCastSettings
{
    /// How backfacing triangles should be treated (should we report back facing hits for triangle based shapes, e.g. MeshShape/HeightFieldShape?)
    JPH_BackFaceMode backFaceModeTriangles; /* = JPH_BackFaceMode_IgnoreBackFaces*/

    /// How backfacing convex objects should be treated (should we report back facing hits for convex shapes?)
    JPH_BackFaceMode backFaceModeConvex; /* = JPH_BackFaceMode_IgnoreBackFaces*/

    /// If convex shapes should be treated as solid. When true, a ray starting inside a convex shape will generate a hit at fraction 0.
    bool treatConvexAsSolid; /* = true*/
}

struct JPH_SpringSettings
{
    JPH_SpringMode mode;
    float frequencyOrStiffness;
    float damping;
}

struct JPH_MotorSettings
{
    JPH_SpringSettings springSettings;
    float minForceLimit;
    float maxForceLimit;
    float minTorqueLimit;
    float maxTorqueLimit;
}

struct JPH_SubShapeIDPair
{
    JPH_BodyID Body1ID;
    JPH_SubShapeID subShapeID1;
    JPH_BodyID Body2ID;
    JPH_SubShapeID subShapeID2;
}

struct JPH_BroadPhaseCastResult
{
    JPH_BodyID bodyID;
    float fraction;
}

struct JPH_RayCastResult
{
    JPH_BodyID bodyID;
    float fraction;
    JPH_SubShapeID subShapeID2;
}

struct JPH_CollidePointResult
{
    JPH_BodyID bodyID;
    JPH_SubShapeID subShapeID2;
}

struct JPH_CollideShapeResult
{
    JPH_Vec3 contactPointOn1;
    JPH_Vec3 contactPointOn2;
    JPH_Vec3 penetrationAxis;
    float penetrationDepth;
    JPH_SubShapeID subShapeID1;
    JPH_SubShapeID subShapeID2;
    JPH_BodyID bodyID2;
    uint shape1FaceCount;
    JPH_Vec3* shape1Faces;
    uint shape2FaceCount;
    JPH_Vec3* shape2Faces;
}

struct JPH_ShapeCastResult
{
    JPH_Vec3 contactPointOn1;
    JPH_Vec3 contactPointOn2;
    JPH_Vec3 penetrationAxis;
    float penetrationDepth;
    JPH_SubShapeID subShapeID1;
    JPH_SubShapeID subShapeID2;
    JPH_BodyID bodyID2;
    float fraction;
    bool isBackFaceHit;
}

struct JPH_DrawSettings
{
    bool drawGetSupportFunction; ///< Draw the GetSupport() function, used for convex collision detection
    bool drawSupportDirection; ///< When drawing the support function, also draw which direction mapped to a specific support point
    bool drawGetSupportingFace; ///< Draw the faces that were found colliding during collision detection
    bool drawShape; ///< Draw the shapes of all bodies
    bool drawShapeWireframe; ///< When mDrawShape is true and this is true, the shapes will be drawn in wireframe instead of solid.
    JPH_BodyManager_ShapeColor drawShapeColor; ///< Coloring scheme to use for shapes
    bool drawBoundingBox; ///< Draw a bounding box per body
    bool drawCenterOfMassTransform; ///< Draw the center of mass for each body
    bool drawWorldTransform; ///< Draw the world transform (which can be different than the center of mass) for each body
    bool drawVelocity; ///< Draw the velocity vector for each body
    bool drawMassAndInertia; ///< Draw the mass and inertia (as the box equivalent) for each body
    bool drawSleepStats; ///< Draw stats regarding the sleeping algorithm of each body
    bool drawSoftBodyVertices; ///< Draw the vertices of soft bodies
    bool drawSoftBodyVertexVelocities; ///< Draw the velocities of the vertices of soft bodies
    bool drawSoftBodyEdgeConstraints; ///< Draw the edge constraints of soft bodies
    bool drawSoftBodyBendConstraints; ///< Draw the bend constraints of soft bodies
    bool drawSoftBodyVolumeConstraints; ///< Draw the volume constraints of soft bodies
    bool drawSoftBodySkinConstraints; ///< Draw the skin constraints of soft bodies
    bool drawSoftBodyLRAConstraints; ///< Draw the LRA constraints of soft bodies
    bool drawSoftBodyPredictedBounds; ///< Draw the predicted bounds of soft bodies
    JPH_SoftBodyConstraintColor drawSoftBodyConstraintColor; ///< Coloring scheme to use for soft body constraints
}

struct JPH_SupportingFace
{
    uint count;
    JPH_Vec3[32] vertices;
}

struct JPH_CollisionGroup
{
    const(JPH_GroupFilter)* groupFilter;
    JPH_CollisionGroupID groupID;
    JPH_CollisionSubGroupID subGroupID;
}

alias JPH_CastRayResultCallback = void function(void* context, const(JPH_RayCastResult)* result);
alias JPH_RayCastBodyResultCallback = void function(void* context, const(JPH_BroadPhaseCastResult)* result);
alias JPH_CollideShapeBodyResultCallback = void function(void* context, const JPH_BodyID result);
alias JPH_CollidePointResultCallback = void function(void* context, const(JPH_CollidePointResult)* result);
alias JPH_CollideShapeResultCallback = void function(void* context, const(JPH_CollideShapeResult)* result);
alias JPH_CastShapeResultCallback = void function(void* context, const(JPH_ShapeCastResult)* result);

alias JPH_CastRayCollectorCallback = float function(void* context, const(JPH_RayCastResult)* result);
alias JPH_RayCastBodyCollectorCallback = float function(void* context, const(
        JPH_BroadPhaseCastResult)* result);
alias JPH_CollideShapeBodyCollectorCallback = float function(void* context, const JPH_BodyID result);
alias JPH_CollidePointCollectorCallback = float function(void* context, const(
        JPH_CollidePointResult)* result);
alias JPH_CollideShapeCollectorCallback = float function(void* context, const(
        JPH_CollideShapeResult)* result);
alias JPH_CastShapeCollectorCallback = float function(void* context, const(JPH_ShapeCastResult)* result);

struct JPH_CollisionEstimationResultImpulse
{
    float contactImpulse;
    float frictionImpulse1;
    float frictionImpulse2;
}

struct JPH_CollisionEstimationResult
{
    JPH_Vec3 linearVelocity1;
    JPH_Vec3 angularVelocity1;
    JPH_Vec3 linearVelocity2;
    JPH_Vec3 angularVelocity2;

    JPH_Vec3 tangent1;
    JPH_Vec3 tangent2;

    uint impulseCount;
    JPH_CollisionEstimationResultImpulse* impulses;
}

struct JPH_BodyActivationListener;
struct JPH_BodyDrawFilter;

struct JPH_SharedMutex;

struct JPH_DebugRenderer;

/* Constraint */
struct JPH_Constraint;
struct JPH_TwoBodyConstraint;
struct JPH_FixedConstraint;
struct JPH_DistanceConstraint;
struct JPH_PointConstraint;
struct JPH_HingeConstraint;
struct JPH_SliderConstraint;
struct JPH_ConeConstraint;
struct JPH_SwingTwistConstraint;
struct JPH_SixDOFConstraint;
struct JPH_GearConstraint;

/* Character, CharacterVirtual */
struct JPH_CharacterBase;
struct JPH_Character; /* Inherits JPH_CharacterBase */
struct JPH_CharacterVirtual; /* Inherits JPH_CharacterBase */
struct JPH_CharacterContactListener;
struct JPH_CharacterVsCharacterCollision;

/* Skeleton/Ragdoll */
struct JPH_Skeleton;
struct JPH_RagdollSettings;
struct JPH_Ragdoll;

struct JPH_ConstraintSettings
{
    bool enabled;
    uint constraintPriority;
    uint numVelocityStepsOverride;
    uint numPositionStepsOverride;
    float drawConstraintSize;
    ulong userData;
}

struct JPH_FixedConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    bool autoDetectPoint;
    JPH_RVec3 point1;
    JPH_Vec3 axisX1;
    JPH_Vec3 axisY1;
    JPH_RVec3 point2;
    JPH_Vec3 axisX2;
    JPH_Vec3 axisY2;
}

struct JPH_DistanceConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 point1;
    JPH_RVec3 point2;
    float minDistance;
    float maxDistance;
    JPH_SpringSettings limitsSpringSettings;
}

struct JPH_PointConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 point1;
    JPH_RVec3 point2;
}

struct JPH_HingeConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 point1;
    JPH_Vec3 hingeAxis1;
    JPH_Vec3 normalAxis1;
    JPH_RVec3 point2;
    JPH_Vec3 hingeAxis2;
    JPH_Vec3 normalAxis2;
    float limitsMin;
    float limitsMax;
    JPH_SpringSettings limitsSpringSettings;
    float maxFrictionTorque;
    JPH_MotorSettings motorSettings;
}

struct JPH_SliderConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    bool autoDetectPoint;
    JPH_RVec3 point1;
    JPH_Vec3 sliderAxis1;
    JPH_Vec3 normalAxis1;
    JPH_RVec3 point2;
    JPH_Vec3 sliderAxis2;
    JPH_Vec3 normalAxis2;
    float limitsMin;
    float limitsMax;
    JPH_SpringSettings limitsSpringSettings;
    float maxFrictionForce;
    JPH_MotorSettings motorSettings;
}

struct JPH_ConeConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 point1;
    JPH_Vec3 twistAxis1;
    JPH_RVec3 point2;
    JPH_Vec3 twistAxis2;
    float halfConeAngle;
}

struct JPH_SwingTwistConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 position1;
    JPH_Vec3 twistAxis1;
    JPH_Vec3 planeAxis1;
    JPH_RVec3 position2;
    JPH_Vec3 twistAxis2;
    JPH_Vec3 planeAxis2;
    JPH_SwingType swingType;
    float normalHalfConeAngle;
    float planeHalfConeAngle;
    float twistMinAngle;
    float twistMaxAngle;
    float maxFrictionTorque;
    JPH_MotorSettings swingMotorSettings;
    JPH_MotorSettings twistMotorSettings;
}

struct JPH_SixDOFConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_RVec3 position1;
    JPH_Vec3 axisX1;
    JPH_Vec3 axisY1;
    JPH_RVec3 position2;
    JPH_Vec3 axisX2;
    JPH_Vec3 axisY2;
    float[_JPH_SixDOFConstraintAxis_Num] maxFriction;
    JPH_SwingType swingType;
    float[_JPH_SixDOFConstraintAxis_Num] limitMin;
    float[_JPH_SixDOFConstraintAxis_Num] limitMax;

    JPH_SpringSettings[_JPH_SixDOFConstraintAxis_NumTranslation] limitsSpringSettings;
    JPH_MotorSettings[_JPH_SixDOFConstraintAxis_Num] motorSettings;
}

struct JPH_GearConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_ConstraintSpace space;
    JPH_Vec3 hingeAxis1;
    JPH_Vec3 hingeAxis2;
    float ratio;
}

struct JPH_BodyLockRead
{
    const(JPH_BodyLockInterface)* lockInterface;
    JPH_SharedMutex* mutex;
    const(JPH_Body)* body_;
}

struct JPH_BodyLockWrite
{
    const(JPH_BodyLockInterface)* lockInterface;
    JPH_SharedMutex* mutex;
    JPH_Body* body_;
}

struct JPH_BodyLockMultiRead;
struct JPH_BodyLockMultiWrite;

struct JPH_ExtendedUpdateSettings
{
    JPH_Vec3 stickToFloorStepDown;
    JPH_Vec3 walkStairsStepUp;
    float walkStairsMinStepForward;
    float walkStairsStepForwardTest;
    float walkStairsCosAngleForwardContact;
    JPH_Vec3 walkStairsStepDownExtra;
}

struct JPH_CharacterBaseSettings
{
    JPH_Vec3 up;
    JPH_Plane supportingVolume;
    float maxSlopeAngle;
    bool enhancedInternalEdgeRemoval;
    const(JPH_Shape)* shape;
}

/* Character */
struct JPH_CharacterSettings
{
    JPH_CharacterBaseSettings base; /* Inherits JPH_CharacterBaseSettings */
    JPH_ObjectLayer layer;
    float mass;
    float friction;
    float gravityFactor;
    JPH_AllowedDOFs allowedDOFs;
}

/* CharacterVirtual */
struct JPH_CharacterVirtualSettings
{
    JPH_CharacterBaseSettings base; /* Inherits JPH_CharacterBaseSettings */
    JPH_CharacterID ID;
    float mass;
    float maxStrength;
    JPH_Vec3 shapeOffset;
    JPH_BackFaceMode backFaceMode;
    float predictiveContactDistance;
    uint maxCollisionIterations;
    uint maxConstraintIterations;
    float minTimeRemaining;
    float collisionTolerance;
    float characterPadding;
    uint maxNumHits;
    float hitReductionCosMaxAngle;
    float penetrationRecoverySpeed;
    const(JPH_Shape)* innerBodyShape;
    JPH_BodyID innerBodyIDOverride;
    JPH_ObjectLayer innerBodyLayer;
}

struct JPH_CharacterContactSettings
{
    bool canPushCharacter;
    bool canReceiveImpulses;
}

struct JPH_CharacterVirtualContact
{
    ulong hash;
    JPH_BodyID bodyB;
    JPH_CharacterID characterIDB;
    JPH_SubShapeID subShapeIDB;
    JPH_RVec3 position;
    JPH_Vec3 linearVelocity;
    JPH_Vec3 contactNormal;
    JPH_Vec3 surfaceNormal;
    float distance;
    float fraction;
    JPH_MotionType motionTypeB;
    bool isSensorB;
    const(JPH_CharacterVirtual)* characterB;
    ulong userData;
    const(JPH_PhysicsMaterial)* material;
    bool hadCollision;
    bool wasDiscarded;
    bool canPushCharacter;
}

alias JPH_TraceFunc = void function(const(char)* mssage);
alias JPH_AssertFailureFunc = bool function(const(char)* expression, const(char)* mssage, const(
        char)* file, uint line);

alias JPH_JobFunction = void function(void* arg);
alias JPH_QueueJobCallback = void function(void* context, void function() job, void* arg);
alias JPH_QueueJobsCallback = void function(void* context, void function() job, void** args, uint count);

struct JobSystemThreadPoolConfig
{
    uint maxJobs;
    uint maxBarriers;
    int numThreads;
}

struct JPH_JobSystemConfig
{
    void* context;
    void function() queueJob;
    void function() queueJobs;
    uint maxConcurrency;
    uint maxBarriers;
}

struct JPH_JobSystem;

JPH_JobSystem* JPH_JobSystemThreadPool_Create(const(JobSystemThreadPoolConfig)* config);
JPH_JobSystem* JPH_JobSystemCallback_Create(const(JPH_JobSystemConfig)* config);
void JPH_JobSystem_Destroy(JPH_JobSystem* jobSystem);

bool JPH_Init();
void JPH_Shutdown();
void JPH_SetTraceHandler(JPH_TraceFunc handler);
void JPH_SetAssertFailureHandler(JPH_AssertFailureFunc handler);

/* Structs free members */
void JPH_CollideShapeResult_FreeMembers(JPH_CollideShapeResult* result);
void JPH_CollisionEstimationResult_FreeMembers(JPH_CollisionEstimationResult* result);

/* JPH_BroadPhaseLayerInterface */
JPH_BroadPhaseLayerInterface* JPH_BroadPhaseLayerInterfaceMask_Create(uint numBroadPhaseLayers);
void JPH_BroadPhaseLayerInterfaceMask_ConfigureLayer(JPH_BroadPhaseLayerInterface* bpInterface, JPH_BroadPhaseLayer broadPhaseLayer, uint groupsToInclude, uint groupsToExclude);

JPH_BroadPhaseLayerInterface* JPH_BroadPhaseLayerInterfaceTable_Create(
    uint numObjectLayers, uint numBroadPhaseLayers);
void JPH_BroadPhaseLayerInterfaceTable_MapObjectToBroadPhaseLayer(JPH_BroadPhaseLayerInterface* bpInterface, JPH_ObjectLayer objectLayer, JPH_BroadPhaseLayer broadPhaseLayer);

/* JPH_ObjectLayerPairFilter */
JPH_ObjectLayerPairFilter* JPH_ObjectLayerPairFilterMask_Create();
JPH_ObjectLayer JPH_ObjectLayerPairFilterMask_GetObjectLayer(uint group, uint mask);
uint JPH_ObjectLayerPairFilterMask_GetGroup(JPH_ObjectLayer layer);
uint JPH_ObjectLayerPairFilterMask_GetMask(JPH_ObjectLayer layer);

JPH_ObjectLayerPairFilter* JPH_ObjectLayerPairFilterTable_Create(uint numObjectLayers);
void JPH_ObjectLayerPairFilterTable_DisableCollision(
    JPH_ObjectLayerPairFilter* objectFilter, JPH_ObjectLayer layer1, JPH_ObjectLayer layer2);
void JPH_ObjectLayerPairFilterTable_EnableCollision(JPH_ObjectLayerPairFilter* objectFilter, JPH_ObjectLayer layer1, JPH_ObjectLayer layer2);
bool JPH_ObjectLayerPairFilterTable_ShouldCollide(JPH_ObjectLayerPairFilter* objectFilter, JPH_ObjectLayer layer1, JPH_ObjectLayer layer2);

/* JPH_ObjectVsBroadPhaseLayerFilter */
JPH_ObjectVsBroadPhaseLayerFilter* JPH_ObjectVsBroadPhaseLayerFilterMask_Create(
    const(JPH_BroadPhaseLayerInterface)* broadPhaseLayerInterface);

JPH_ObjectVsBroadPhaseLayerFilter* JPH_ObjectVsBroadPhaseLayerFilterTable_Create(
    JPH_BroadPhaseLayerInterface* broadPhaseLayerInterface,
    uint numBroadPhaseLayers,
    JPH_ObjectLayerPairFilter* objectLayerPairFilter,
    uint numObjectLayers);

void JPH_DrawSettings_InitDefault(JPH_DrawSettings* settings);

/* JPH_PhysicsSystem */
struct JPH_PhysicsSystemSettings
{
    uint maxBodies; /* 10240 */
    uint numBodyMutexes; /* 0 */
    uint maxBodyPairs; /* 65536 */
    uint maxContactConstraints; /* 10240 */
    uint _padding;
    JPH_BroadPhaseLayerInterface* broadPhaseLayerInterface;
    JPH_ObjectLayerPairFilter* objectLayerPairFilter;
    JPH_ObjectVsBroadPhaseLayerFilter* objectVsBroadPhaseLayerFilter;
}

struct JPH_PhysicsSettings
{
    int maxInFlightBodyPairs;
    int stepListenersBatchSize;
    int stepListenerBatchesPerJob;
    float baumgarte;
    float speculativeContactDistance;
    float penetrationSlop;
    float linearCastThreshold;
    float linearCastMaxPenetration;
    float manifoldTolerance;
    float maxPenetrationDistance;
    float bodyPairCacheMaxDeltaPositionSq;
    float bodyPairCacheCosMaxDeltaRotationDiv2;
    float contactNormalCosMaxDeltaRotation;
    float contactPointPreserveLambdaMaxDistSq;
    uint numVelocitySteps;
    uint numPositionSteps;
    float minVelocityForRestitution;
    float timeBeforeSleep;
    float pointVelocitySleepThreshold;
    bool deterministicSimulation;
    bool constraintWarmStart;
    bool useBodyPairContactCache;
    bool useManifoldReduction;
    bool useLargeIslandSplitter;
    bool allowSleeping;
    bool checkActiveEdges;
}

JPH_PhysicsSystem* JPH_PhysicsSystem_Create(const(JPH_PhysicsSystemSettings)* settings);
void JPH_PhysicsSystem_Destroy(JPH_PhysicsSystem* system);

void JPH_PhysicsSystem_SetPhysicsSettings(JPH_PhysicsSystem* system, JPH_PhysicsSettings* settings);
void JPH_PhysicsSystem_GetPhysicsSettings(JPH_PhysicsSystem* system, JPH_PhysicsSettings* result);

void JPH_PhysicsSystem_OptimizeBroadPhase(JPH_PhysicsSystem* system);
JPH_PhysicsUpdateError JPH_PhysicsSystem_Update(JPH_PhysicsSystem* system, float deltaTime, int collisionSteps, JPH_JobSystem* jobSystem);

JPH_BodyInterface* JPH_PhysicsSystem_GetBodyInterface(JPH_PhysicsSystem* system);
JPH_BodyInterface* JPH_PhysicsSystem_GetBodyInterfaceNoLock(JPH_PhysicsSystem* system);

const(JPH_BodyLockInterface)* JPH_PhysicsSystem_GetBodyLockInterface(
    const(JPH_PhysicsSystem)* system);
const(JPH_BodyLockInterface)* JPH_PhysicsSystem_GetBodyLockInterfaceNoLock(
    const(JPH_PhysicsSystem)* system);

const(JPH_BroadPhaseQuery)* JPH_PhysicsSystem_GetBroadPhaseQuery(const(JPH_PhysicsSystem)* system);

const(JPH_NarrowPhaseQuery)* JPH_PhysicsSystem_GetNarrowPhaseQuery(const(JPH_PhysicsSystem)* system);
const(JPH_NarrowPhaseQuery)* JPH_PhysicsSystem_GetNarrowPhaseQueryNoLock(
    const(JPH_PhysicsSystem)* system);

void JPH_PhysicsSystem_SetContactListener(JPH_PhysicsSystem* system, JPH_ContactListener* listener);
void JPH_PhysicsSystem_SetBodyActivationListener(JPH_PhysicsSystem* system, JPH_BodyActivationListener* listener);
void JPH_PhysicsSystem_SetSimShapeFilter(JPH_PhysicsSystem* system, JPH_SimShapeFilter* filter);

bool JPH_PhysicsSystem_WereBodiesInContact(const(JPH_PhysicsSystem)* system, JPH_BodyID body1, JPH_BodyID body2);

uint JPH_PhysicsSystem_GetNumBodies(const(JPH_PhysicsSystem)* system);
uint JPH_PhysicsSystem_GetNumActiveBodies(const(JPH_PhysicsSystem)* system, JPH_BodyType type);
uint JPH_PhysicsSystem_GetMaxBodies(const(JPH_PhysicsSystem)* system);
uint JPH_PhysicsSystem_GetNumConstraints(const(JPH_PhysicsSystem)* system);

void JPH_PhysicsSystem_SetGravity(JPH_PhysicsSystem* system, const(JPH_Vec3)* value);
void JPH_PhysicsSystem_GetGravity(JPH_PhysicsSystem* system, JPH_Vec3* result);

void JPH_PhysicsSystem_AddConstraint(JPH_PhysicsSystem* system, JPH_Constraint* constraint);
void JPH_PhysicsSystem_RemoveConstraint(JPH_PhysicsSystem* system, JPH_Constraint* constraint);

void JPH_PhysicsSystem_AddConstraints(JPH_PhysicsSystem* system, JPH_Constraint** constraints, uint count);
void JPH_PhysicsSystem_RemoveConstraints(JPH_PhysicsSystem* system, JPH_Constraint** constraints, uint count);

void JPH_PhysicsSystem_AddStepListener(JPH_PhysicsSystem* system, JPH_PhysicsStepListener* listener);
void JPH_PhysicsSystem_RemoveStepListener(JPH_PhysicsSystem* system, JPH_PhysicsStepListener* listener);

void JPH_PhysicsSystem_GetBodies(const(JPH_PhysicsSystem)* system, JPH_BodyID* ids, uint count);
void JPH_PhysicsSystem_GetConstraints(const(JPH_PhysicsSystem)* system, const(JPH_Constraint*)* constraints, uint count); /* = nullptr */
void JPH_PhysicsSystem_DrawBodies(JPH_PhysicsSystem* system, const(JPH_DrawSettings)* settings, JPH_DebugRenderer* renderer, const(
        JPH_BodyDrawFilter)* bodyFilter);
void JPH_PhysicsSystem_DrawConstraints(JPH_PhysicsSystem* system, JPH_DebugRenderer* renderer);
void JPH_PhysicsSystem_DrawConstraintLimits(JPH_PhysicsSystem* system, JPH_DebugRenderer* renderer);
void JPH_PhysicsSystem_DrawConstraintReferenceFrame(JPH_PhysicsSystem* system, JPH_DebugRenderer* renderer);

/* PhysicsStepListener */
struct JPH_PhysicsStepListenerContext
{
    float deltaTime;
    JPH_Bool isFirstStep;
    JPH_Bool isLastStep;
    JPH_PhysicsSystem* physicsSystem;
}

struct JPH_PhysicsStepListener_Procs
{
    void function(void* userData, const(JPH_PhysicsStepListenerContext)* context) OnStep;
}

void JPH_PhysicsStepListener_SetProcs(const(JPH_PhysicsStepListener_Procs)* procs);
JPH_PhysicsStepListener* JPH_PhysicsStepListener_Create(void* userData);
void JPH_PhysicsStepListener_Destroy(JPH_PhysicsStepListener* listener);

/* Math */
void JPH_Quaternion_FromTo(const(JPH_Vec3)* from, const(JPH_Vec3)* to, JPH_Quat* quat);
void JPH_Quat_GetAxisAngle(const(JPH_Quat)* quat, JPH_Vec3* outAxis, float* outAngle);
void JPH_Quat_GetEulerAngles(const(JPH_Quat)* quat, JPH_Vec3* result);
void JPH_Quat_RotateAxisX(const(JPH_Quat)* quat, JPH_Vec3* result);
void JPH_Quat_RotateAxisY(const(JPH_Quat)* quat, JPH_Vec3* result);
void JPH_Quat_RotateAxisZ(const(JPH_Quat)* quat, JPH_Vec3* result);
void JPH_Quat_Inversed(const(JPH_Quat)* quat, JPH_Quat* result);
void JPH_Quat_GetPerpendicular(const(JPH_Quat)* quat, JPH_Quat* result);
float JPH_Quat_GetRotationAngle(const(JPH_Quat)* quat, const(JPH_Vec3)* axis);
void JPH_Quat_FromEulerAngles(const(JPH_Vec3)* angles, JPH_Quat* result);

void JPH_Quat_Add(const(JPH_Quat)* q1, const(JPH_Quat)* q2, JPH_Quat* result);
void JPH_Quat_Subtract(const(JPH_Quat)* q1, const(JPH_Quat)* q2, JPH_Quat* result);
void JPH_Quat_Multiply(const(JPH_Quat)* q1, const(JPH_Quat)* q2, JPH_Quat* result);
void JPH_Quat_MultiplyScalar(const(JPH_Quat)* q, float scalar, JPH_Quat* result);
void JPH_Quat_DivideScalar(const(JPH_Quat)* q, float scalar, JPH_Quat* result);
void JPH_Quat_Dot(const(JPH_Quat)* q1, const(JPH_Quat)* q2, float* result);

void JPH_Quat_Conjugated(const(JPH_Quat)* quat, JPH_Quat* result);
void JPH_Quat_GetTwist(const(JPH_Quat)* quat, const(JPH_Vec3)* axis, JPH_Quat* result);
void JPH_Quat_GetSwingTwist(const(JPH_Quat)* quat, JPH_Quat* outSwing, JPH_Quat* outTwist);
void JPH_Quat_Lerp(const(JPH_Quat)* from, const(JPH_Quat)* to, float fraction, JPH_Quat* result);
void JPH_Quat_Slerp(const(JPH_Quat)* from, const(JPH_Quat)* to, float fraction, JPH_Quat* result);
void JPH_Quat_Rotate(const(JPH_Quat)* quat, const(JPH_Vec3)* vec, JPH_Vec3* result);
void JPH_Quat_InverseRotate(const(JPH_Quat)* quat, const(JPH_Vec3)* vec, JPH_Vec3* result);

bool JPH_Vec3_IsClose(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, float maxDistSq);
bool JPH_Vec3_IsNearZero(const(JPH_Vec3)* v, float maxDistSq);
bool JPH_Vec3_IsNormalized(const(JPH_Vec3)* v, float tolerance);
bool JPH_Vec3_IsNaN(const(JPH_Vec3)* v);

void JPH_Vec3_Negate(const(JPH_Vec3)* v, JPH_Vec3* result);
void JPH_Vec3_Normalized(const(JPH_Vec3)* v, JPH_Vec3* result);
void JPH_Vec3_Cross(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, JPH_Vec3* result);
void JPH_Vec3_Abs(const(JPH_Vec3)* v, JPH_Vec3* result);

float JPH_Vec3_Length(const(JPH_Vec3)* v);
float JPH_Vec3_LengthSquared(const(JPH_Vec3)* v);

void JPH_Vec3_DotProduct(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, float* result);
void JPH_Vec3_Normalize(const(JPH_Vec3)* v, JPH_Vec3* result);

void JPH_Vec3_Add(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, JPH_Vec3* result);
void JPH_Vec3_Subtract(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, JPH_Vec3* result);
void JPH_Vec3_Multiply(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, JPH_Vec3* result);
void JPH_Vec3_MultiplyScalar(const(JPH_Vec3)* v, float scalar, JPH_Vec3* result);
void JPH_Vec3_Divide(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, JPH_Vec3* result);
void JPH_Vec3_DivideScalar(const(JPH_Vec3)* v, float scalar, JPH_Vec3* result);

void JPH_Matrix4x4_Add(const(JPH_Matrix4x4)* m1, const(JPH_Matrix4x4)* m2, JPH_Matrix4x4* result);
void JPH_Matrix4x4_Subtract(const(JPH_Matrix4x4)* m1, const(JPH_Matrix4x4)* m2, JPH_Matrix4x4* result);
void JPH_Matrix4x4_Multiply(const(JPH_Matrix4x4)* m1, const(JPH_Matrix4x4)* m2, JPH_Matrix4x4* result);
void JPH_Matrix4x4_MultiplyScalar(const(JPH_Matrix4x4)* m, float scalar, JPH_Matrix4x4* result);

void JPH_Matrix4x4_Zero(JPH_Matrix4x4* result);
void JPH_Matrix4x4_Identity(JPH_Matrix4x4* result);
void JPH_Matrix4x4_Rotation(JPH_Matrix4x4* result, const(JPH_Quat)* rotation);
void JPH_Matrix4x4_Translation(JPH_Matrix4x4* result, const(JPH_Vec3)* translation);
void JPH_Matrix4x4_RotationTranslation(JPH_Matrix4x4* result, const(JPH_Quat)* rotation, const(
        JPH_Vec3)* translation);
void JPH_Matrix4x4_InverseRotationTranslation(JPH_Matrix4x4* result, const(JPH_Quat)* rotation, const(
        JPH_Vec3)* translation);
void JPH_Matrix4x4_Scale(JPH_Matrix4x4* result, const(JPH_Vec3)* scale);
void JPH_Matrix4x4_Inversed(const(JPH_Matrix4x4)* m, JPH_Matrix4x4* result);
void JPH_Matrix4x4_Transposed(const(JPH_Matrix4x4)* m, JPH_Matrix4x4* result);

void JPH_RMatrix4x4_Zero(JPH_RMatrix4x4* result);
void JPH_RMatrix4x4_Identity(JPH_RMatrix4x4* result);
void JPH_RMatrix4x4_Rotation(JPH_RMatrix4x4* result, const(JPH_Quat)* rotation);
void JPH_RMatrix4x4_Translation(JPH_RMatrix4x4* result, const(JPH_RVec3)* translation);
void JPH_RMatrix4x4_RotationTranslation(JPH_RMatrix4x4* result, const(JPH_Quat)* rotation, const(
        JPH_RVec3)* translation);
void JPH_RMatrix4x4_InverseRotationTranslation(JPH_RMatrix4x4* result, const(JPH_Quat)* rotation, const(
        JPH_RVec3)* translation);
void JPH_RMatrix4x4_Scale(JPH_RMatrix4x4* result, const(JPH_Vec3)* scale);
void JPH_RMatrix4x4_Inversed(const(JPH_RMatrix4x4)* m, JPH_RMatrix4x4* result);

void JPH_Matrix4x4_GetAxisX(const(JPH_Matrix4x4)* matrix, JPH_Vec3* result);
void JPH_Matrix4x4_GetAxisY(const(JPH_Matrix4x4)* matrix, JPH_Vec3* result);
void JPH_Matrix4x4_GetAxisZ(const(JPH_Matrix4x4)* matrix, JPH_Vec3* result);
void JPH_Matrix4x4_GetTranslation(const(JPH_Matrix4x4)* matrix, JPH_Vec3* result);
void JPH_Matrix4x4_GetQuaternion(const(JPH_Matrix4x4)* matrix, JPH_Quat* result);

/* Material */
JPH_PhysicsMaterial* JPH_PhysicsMaterial_Create(const(char)* name, uint color);
void JPH_PhysicsMaterial_Destroy(JPH_PhysicsMaterial* material);
const(char)* JPH_PhysicsMaterial_GetDebugName(const(JPH_PhysicsMaterial)* material);
uint JPH_PhysicsMaterial_GetDebugColor(const(JPH_PhysicsMaterial)* material);

/* GroupFilter/GroupFilterTable */
void JPH_GroupFilter_Destroy(JPH_GroupFilter* groupFilter);
bool JPH_GroupFilter_CanCollide(JPH_GroupFilter* groupFilter, const(JPH_CollisionGroup)* group1, const(
        JPH_CollisionGroup)* group2); /* = 0*/
JPH_GroupFilterTable* JPH_GroupFilterTable_Create(uint numSubGroups);
void JPH_GroupFilterTable_DisableCollision(JPH_GroupFilterTable* table, JPH_CollisionSubGroupID subGroup1, JPH_CollisionSubGroupID subGroup2);
void JPH_GroupFilterTable_EnableCollision(JPH_GroupFilterTable* table, JPH_CollisionSubGroupID subGroup1, JPH_CollisionSubGroupID subGroup2);
bool JPH_GroupFilterTable_IsCollisionEnabled(JPH_GroupFilterTable* table, JPH_CollisionSubGroupID subGroup1, JPH_CollisionSubGroupID subGroup2);

/* ShapeSettings */
void JPH_ShapeSettings_Destroy(JPH_ShapeSettings* settings);
ulong JPH_ShapeSettings_GetUserData(const(JPH_ShapeSettings)* settings);
void JPH_ShapeSettings_SetUserData(JPH_ShapeSettings* settings, ulong userData);

/* Shape */
void JPH_Shape_Destroy(JPH_Shape* shape);
JPH_ShapeType JPH_Shape_GetType(const(JPH_Shape)* shape);
JPH_ShapeSubType JPH_Shape_GetSubType(const(JPH_Shape)* shape);
ulong JPH_Shape_GetUserData(const(JPH_Shape)* shape);
void JPH_Shape_SetUserData(JPH_Shape* shape, ulong userData);
bool JPH_Shape_MustBeStatic(const(JPH_Shape)* shape);
void JPH_Shape_GetCenterOfMass(const(JPH_Shape)* shape, JPH_Vec3* result);
void JPH_Shape_GetLocalBounds(const(JPH_Shape)* shape, JPH_AABox* result);
uint JPH_Shape_GetSubShapeIDBitsRecursive(const(JPH_Shape)* shape);
void JPH_Shape_GetWorldSpaceBounds(const(JPH_Shape)* shape, JPH_RMatrix4x4* centerOfMassTransform, JPH_Vec3* scale, JPH_AABox* result);
float JPH_Shape_GetInnerRadius(const(JPH_Shape)* shape);
void JPH_Shape_GetMassProperties(const(JPH_Shape)* shape, JPH_MassProperties* result);
const(JPH_Shape)* JPH_Shape_GetLeafShape(const(JPH_Shape)* shape, JPH_SubShapeID subShapeID, JPH_SubShapeID* remainder);
const(JPH_PhysicsMaterial)* JPH_Shape_GetMaterial(const(JPH_Shape)* shape, JPH_SubShapeID subShapeID);
void JPH_Shape_GetSurfaceNormal(const(JPH_Shape)* shape, JPH_SubShapeID subShapeID, JPH_Vec3* localPosition, JPH_Vec3* normal);
void JPH_Shape_GetSupportingFace(const(JPH_Shape)* shape, const JPH_SubShapeID subShapeID, const(
        JPH_Vec3)* direction, const(JPH_Vec3)* scale, const(JPH_Matrix4x4)* centerOfMassTransform, JPH_SupportingFace* outVertices);
float JPH_Shape_GetVolume(const(JPH_Shape)* shape);
bool JPH_Shape_IsValidScale(const(JPH_Shape)* shape, const(JPH_Vec3)* scale);
void JPH_Shape_MakeScaleValid(const(JPH_Shape)* shape, const(JPH_Vec3)* scale, JPH_Vec3* result);
JPH_Shape* JPH_Shape_ScaleShape(const(JPH_Shape)* shape, const(JPH_Vec3)* scale);
bool JPH_Shape_CastRay(const(JPH_Shape)* shape, const(JPH_Vec3)* origin, const(JPH_Vec3)* direction, JPH_RayCastResult* hit);
bool JPH_Shape_CastRay2(const(JPH_Shape)* shape, const(JPH_Vec3)* origin, const(JPH_Vec3)* direction, const(
        JPH_RayCastSettings)* rayCastSettings, JPH_CollisionCollectorType collectorType, void function() callback, void* userData, const(
        JPH_ShapeFilter)* shapeFilter);
bool JPH_Shape_CollidePoint(const(JPH_Shape)* shape, const(JPH_Vec3)* point, const(JPH_ShapeFilter)* shapeFilter);
bool JPH_Shape_CollidePoint2(const(JPH_Shape)* shape, const(JPH_Vec3)* point, JPH_CollisionCollectorType collectorType, void function() callback, void* userData, const(
        JPH_ShapeFilter)* shapeFilter);

/* JPH_ConvexShape */
float JPH_ConvexShapeSettings_GetDensity(const(JPH_ConvexShapeSettings)* shape);
void JPH_ConvexShapeSettings_SetDensity(JPH_ConvexShapeSettings* shape, float value);
float JPH_ConvexShape_GetDensity(const(JPH_ConvexShape)* shape);
void JPH_ConvexShape_SetDensity(JPH_ConvexShape* shape, float inDensity);

/* BoxShape */
JPH_BoxShapeSettings* JPH_BoxShapeSettings_Create(const(JPH_Vec3)* halfExtent, float convexRadius);
JPH_BoxShape* JPH_BoxShapeSettings_CreateShape(const(JPH_BoxShapeSettings)* settings);

JPH_BoxShape* JPH_BoxShape_Create(const(JPH_Vec3)* halfExtent, float convexRadius);
void JPH_BoxShape_GetHalfExtent(const(JPH_BoxShape)* shape, JPH_Vec3* halfExtent);
float JPH_BoxShape_GetConvexRadius(const(JPH_BoxShape)* shape);

/* SphereShape */
JPH_SphereShapeSettings* JPH_SphereShapeSettings_Create(float radius);
JPH_SphereShape* JPH_SphereShapeSettings_CreateShape(const(JPH_SphereShapeSettings)* settings);

float JPH_SphereShapeSettings_GetRadius(const(JPH_SphereShapeSettings)* settings);
void JPH_SphereShapeSettings_SetRadius(JPH_SphereShapeSettings* settings, float radius);
JPH_SphereShape* JPH_SphereShape_Create(float radius);
float JPH_SphereShape_GetRadius(const(JPH_SphereShape)* shape);

/* PlaneShape */
JPH_PlaneShapeSettings* JPH_PlaneShapeSettings_Create(const(JPH_Plane)* plane, const(
        JPH_PhysicsMaterial)* material, float halfExtent);
JPH_PlaneShape* JPH_PlaneShapeSettings_CreateShape(const(JPH_PlaneShapeSettings)* settings);
JPH_PlaneShape* JPH_PlaneShape_Create(const(JPH_Plane)* plane, const(JPH_PhysicsMaterial)* material, float halfExtent);
void JPH_PlaneShape_GetPlane(const(JPH_PlaneShape)* shape, JPH_Plane* result);
float JPH_PlaneShape_GetHalfExtent(const(JPH_PlaneShape)* shape);

/* TriangleShape */
JPH_TriangleShapeSettings* JPH_TriangleShapeSettings_Create(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, const(
        JPH_Vec3)* v3, float convexRadius);
JPH_TriangleShape* JPH_TriangleShapeSettings_CreateShape(const(JPH_TriangleShapeSettings)* settings);

JPH_TriangleShape* JPH_TriangleShape_Create(const(JPH_Vec3)* v1, const(JPH_Vec3)* v2, const(
        JPH_Vec3)* v3, float convexRadius);
float JPH_TriangleShape_GetConvexRadius(const(JPH_TriangleShape)* shape);
void JPH_TriangleShape_GetVertex1(const(JPH_TriangleShape)* shape, JPH_Vec3* result);
void JPH_TriangleShape_GetVertex2(const(JPH_TriangleShape)* shape, JPH_Vec3* result);
void JPH_TriangleShape_GetVertex3(const(JPH_TriangleShape)* shape, JPH_Vec3* result);

/* CapsuleShape */
JPH_CapsuleShapeSettings* JPH_CapsuleShapeSettings_Create(float halfHeightOfCylinder, float radius);
JPH_CapsuleShape* JPH_CapsuleShapeSettings_CreateShape(const(JPH_CapsuleShapeSettings)* settings);
JPH_CapsuleShape* JPH_CapsuleShape_Create(float halfHeightOfCylinder, float radius);
float JPH_CapsuleShape_GetRadius(const(JPH_CapsuleShape)* shape);
float JPH_CapsuleShape_GetHalfHeightOfCylinder(const(JPH_CapsuleShape)* shape);

/* CylinderShape */
JPH_CylinderShapeSettings* JPH_CylinderShapeSettings_Create(float halfHeight, float radius, float convexRadius);
JPH_CylinderShape* JPH_CylinderShapeSettings_CreateShape(const(JPH_CylinderShapeSettings)* settings);

JPH_CylinderShape* JPH_CylinderShape_Create(float halfHeight, float radius);
float JPH_CylinderShape_GetRadius(const(JPH_CylinderShape)* shape);
float JPH_CylinderShape_GetHalfHeight(const(JPH_CylinderShape)* shape);

/* TaperedCylinderShape */ /* = cDefaultConvexRadius*/  /* = NULL*/
JPH_TaperedCylinderShapeSettings* JPH_TaperedCylinderShapeSettings_Create(float halfHeightOfTaperedCylinder, float topRadius, float bottomRadius, float convexRadius, const(
        JPH_PhysicsMaterial)* material);
JPH_TaperedCylinderShape* JPH_TaperedCylinderShapeSettings_CreateShape(
    const(JPH_TaperedCylinderShapeSettings)* settings);
float JPH_TaperedCylinderShape_GetTopRadius(const(JPH_TaperedCylinderShape)* shape);
float JPH_TaperedCylinderShape_GetBottomRadius(const(JPH_TaperedCylinderShape)* shape);
float JPH_TaperedCylinderShape_GetConvexRadius(const(JPH_TaperedCylinderShape)* shape);
float JPH_TaperedCylinderShape_GetHalfHeight(const(JPH_TaperedCylinderShape)* shape);

/* ConvexHullShape */
JPH_ConvexHullShapeSettings* JPH_ConvexHullShapeSettings_Create(
    const(JPH_Vec3)* points, uint pointsCount, float maxConvexRadius);
JPH_ConvexHullShape* JPH_ConvexHullShapeSettings_CreateShape(
    const(JPH_ConvexHullShapeSettings)* settings);
uint JPH_ConvexHullShape_GetNumPoints(const(JPH_ConvexHullShape)* shape);
void JPH_ConvexHullShape_GetPoint(const(JPH_ConvexHullShape)* shape, uint index, JPH_Vec3* result);
uint JPH_ConvexHullShape_GetNumFaces(const(JPH_ConvexHullShape)* shape);
uint JPH_ConvexHullShape_GetNumVerticesInFace(const(JPH_ConvexHullShape)* shape, uint faceIndex);
uint JPH_ConvexHullShape_GetFaceVertices(const(JPH_ConvexHullShape)* shape, uint faceIndex, uint maxVertices, uint* vertices);

/* MeshShape */
JPH_MeshShapeSettings* JPH_MeshShapeSettings_Create(const(JPH_Triangle)* triangles, uint triangleCount);
JPH_MeshShapeSettings* JPH_MeshShapeSettings_Create2(const(JPH_Vec3)* vertices, uint verticesCount, const(
        JPH_IndexedTriangle)* triangles, uint triangleCount);
uint JPH_MeshShapeSettings_GetMaxTrianglesPerLeaf(const(JPH_MeshShapeSettings)* settings);
void JPH_MeshShapeSettings_SetMaxTrianglesPerLeaf(JPH_MeshShapeSettings* settings, uint value);
float JPH_MeshShapeSettings_GetActiveEdgeCosThresholdAngle(const(JPH_MeshShapeSettings)* settings);
void JPH_MeshShapeSettings_SetActiveEdgeCosThresholdAngle(
    JPH_MeshShapeSettings* settings, float value);
bool JPH_MeshShapeSettings_GetPerTriangleUserData(const(JPH_MeshShapeSettings)* settings);
void JPH_MeshShapeSettings_SetPerTriangleUserData(JPH_MeshShapeSettings* settings, bool value);
JPH_Mesh_Shape_BuildQuality JPH_MeshShapeSettings_GetBuildQuality(
    const(JPH_MeshShapeSettings)* settings);
void JPH_MeshShapeSettings_SetBuildQuality(JPH_MeshShapeSettings* settings, JPH_Mesh_Shape_BuildQuality value);

void JPH_MeshShapeSettings_Sanitize(JPH_MeshShapeSettings* settings);
JPH_MeshShape* JPH_MeshShapeSettings_CreateShape(const(JPH_MeshShapeSettings)* settings);
uint JPH_MeshShape_GetTriangleUserData(const(JPH_MeshShape)* shape, JPH_SubShapeID id);

/* HeightFieldShape */
JPH_HeightFieldShapeSettings* JPH_HeightFieldShapeSettings_Create(const(float)* samples, const(
        JPH_Vec3)* offset, const(JPH_Vec3)* scale, uint sampleCount);
JPH_HeightFieldShape* JPH_HeightFieldShapeSettings_CreateShape(
    JPH_HeightFieldShapeSettings* settings);
void JPH_HeightFieldShapeSettings_DetermineMinAndMaxSample(const(JPH_HeightFieldShapeSettings)* settings, float* pOutMinValue, float* pOutMaxValue, float* pOutQuantizationScale);
uint JPH_HeightFieldShapeSettings_CalculateBitsPerSampleForError(
    const(JPH_HeightFieldShapeSettings)* settings, float maxError);

uint JPH_HeightFieldShape_GetSampleCount(const(JPH_HeightFieldShape)* shape);
uint JPH_HeightFieldShape_GetBlockSize(const(JPH_HeightFieldShape)* shape);
const(JPH_PhysicsMaterial)* JPH_HeightFieldShape_GetMaterial(
    const(JPH_HeightFieldShape)* shape, uint x, uint y);
void JPH_HeightFieldShape_GetPosition(const(JPH_HeightFieldShape)* shape, uint x, uint y, JPH_Vec3* result);
bool JPH_HeightFieldShape_IsNoCollision(const(JPH_HeightFieldShape)* shape, uint x, uint y);
bool JPH_HeightFieldShape_ProjectOntoSurface(const(JPH_HeightFieldShape)* shape, const(JPH_Vec3)* localPosition, JPH_Vec3* outSurfacePosition, JPH_SubShapeID* outSubShapeID);
float JPH_HeightFieldShape_GetMinHeightValue(const(JPH_HeightFieldShape)* shape);
float JPH_HeightFieldShape_GetMaxHeightValue(const(JPH_HeightFieldShape)* shape);

/* TaperedCapsuleShape */
JPH_TaperedCapsuleShapeSettings* JPH_TaperedCapsuleShapeSettings_Create(
    float halfHeightOfTaperedCylinder, float topRadius, float bottomRadius);
JPH_TaperedCapsuleShape* JPH_TaperedCapsuleShapeSettings_CreateShape(
    JPH_TaperedCapsuleShapeSettings* settings);

float JPH_TaperedCapsuleShape_GetTopRadius(const(JPH_TaperedCapsuleShape)* shape);
float JPH_TaperedCapsuleShape_GetBottomRadius(const(JPH_TaperedCapsuleShape)* shape);
float JPH_TaperedCapsuleShape_GetHalfHeight(const(JPH_TaperedCapsuleShape)* shape);

/* CompoundShape */
void JPH_CompoundShapeSettings_AddShape(JPH_CompoundShapeSettings* settings, const(JPH_Vec3)* position, const(
        JPH_Quat)* rotation, const(JPH_ShapeSettings)* shapeSettings, uint userData);
void JPH_CompoundShapeSettings_AddShape2(JPH_CompoundShapeSettings* settings, const(JPH_Vec3)* position, const(
        JPH_Quat)* rotation, const(JPH_Shape)* shape, uint userData);
uint JPH_CompoundShape_GetNumSubShapes(const(JPH_CompoundShape)* shape);
void JPH_CompoundShape_GetSubShape(const(JPH_CompoundShape)* shape, uint index, const(JPH_Shape*)* subShape, JPH_Vec3* positionCOM, JPH_Quat* rotation, uint* userData);
uint JPH_CompoundShape_GetSubShapeIndexFromID(const(JPH_CompoundShape)* shape, JPH_SubShapeID id, JPH_SubShapeID* remainder);

/* StaticCompoundShape */
JPH_StaticCompoundShapeSettings* JPH_StaticCompoundShapeSettings_Create();
JPH_StaticCompoundShape* JPH_StaticCompoundShape_Create(
    const(JPH_StaticCompoundShapeSettings)* settings);

/* MutableCompoundShape */
JPH_MutableCompoundShapeSettings* JPH_MutableCompoundShapeSettings_Create();
JPH_MutableCompoundShape* JPH_MutableCompoundShape_Create(
    const(JPH_MutableCompoundShapeSettings)* settings); /* = 0 */  /* = UINT32_MAX */
uint JPH_MutableCompoundShape_AddShape(JPH_MutableCompoundShape* shape, const(JPH_Vec3)* position, const(
        JPH_Quat)* rotation, const(JPH_Shape)* child, uint userData, uint index);
void JPH_MutableCompoundShape_RemoveShape(JPH_MutableCompoundShape* shape, uint index);
void JPH_MutableCompoundShape_ModifyShape(JPH_MutableCompoundShape* shape, uint index, const(
        JPH_Vec3)* position, const(JPH_Quat)* rotation);
void JPH_MutableCompoundShape_ModifyShape2(JPH_MutableCompoundShape* shape, uint index, const(
        JPH_Vec3)* position, const(JPH_Quat)* rotation, const(JPH_Shape)* newShape);
void JPH_MutableCompoundShape_AdjustCenterOfMass(JPH_MutableCompoundShape* shape);

/* DecoratedShape */
const(JPH_Shape)* JPH_DecoratedShape_GetInnerShape(const(JPH_DecoratedShape)* shape);

/* RotatedTranslatedShape */
JPH_RotatedTranslatedShapeSettings* JPH_RotatedTranslatedShapeSettings_Create(
    const(JPH_Vec3)* position, const(JPH_Quat)* rotation, const(JPH_ShapeSettings)* shapeSettings);
JPH_RotatedTranslatedShapeSettings* JPH_RotatedTranslatedShapeSettings_Create2(
    const(JPH_Vec3)* position, const(JPH_Quat)* rotation, const(JPH_Shape)* shape);
JPH_RotatedTranslatedShape* JPH_RotatedTranslatedShapeSettings_CreateShape(
    const(JPH_RotatedTranslatedShapeSettings)* settings);
JPH_RotatedTranslatedShape* JPH_RotatedTranslatedShape_Create(
    const(JPH_Vec3)* position, const(JPH_Quat)* rotation, const(JPH_Shape)* shape);
void JPH_RotatedTranslatedShape_GetPosition(
    const(JPH_RotatedTranslatedShape)* shape, JPH_Vec3* position);
void JPH_RotatedTranslatedShape_GetRotation(
    const(JPH_RotatedTranslatedShape)* shape, JPH_Quat* rotation);

/* ScaledShape */
JPH_ScaledShapeSettings* JPH_ScaledShapeSettings_Create(
    const(JPH_ShapeSettings)* shapeSettings, const(JPH_Vec3)* scale);
JPH_ScaledShapeSettings* JPH_ScaledShapeSettings_Create2(const(JPH_Shape)* shape, const(JPH_Vec3)* scale);
JPH_ScaledShape* JPH_ScaledShapeSettings_CreateShape(const(JPH_ScaledShapeSettings)* settings);
JPH_ScaledShape* JPH_ScaledShape_Create(const(JPH_Shape)* shape, const(JPH_Vec3)* scale);
void JPH_ScaledShape_GetScale(const(JPH_ScaledShape)* shape, JPH_Vec3* result);

/* OffsetCenterOfMassShape */
JPH_OffsetCenterOfMassShapeSettings* JPH_OffsetCenterOfMassShapeSettings_Create(
    const(JPH_Vec3)* offset, const(JPH_ShapeSettings)* shapeSettings);
JPH_OffsetCenterOfMassShapeSettings* JPH_OffsetCenterOfMassShapeSettings_Create2(
    const(JPH_Vec3)* offset, const(JPH_Shape)* shape);
JPH_OffsetCenterOfMassShape* JPH_OffsetCenterOfMassShapeSettings_CreateShape(
    const(JPH_OffsetCenterOfMassShapeSettings)* settings);

JPH_OffsetCenterOfMassShape* JPH_OffsetCenterOfMassShape_Create(
    const(JPH_Vec3)* offset, const(JPH_Shape)* shape);
void JPH_OffsetCenterOfMassShape_GetOffset(
    const(JPH_OffsetCenterOfMassShape)* shape, JPH_Vec3* result);

/* EmptyShape */
JPH_EmptyShapeSettings* JPH_EmptyShapeSettings_Create(const(JPH_Vec3)* centerOfMass);
JPH_EmptyShape* JPH_EmptyShapeSettings_CreateShape(const(JPH_EmptyShapeSettings)* settings);

/* JPH_BodyCreationSettings */
JPH_BodyCreationSettings* JPH_BodyCreationSettings_Create();
JPH_BodyCreationSettings* JPH_BodyCreationSettings_Create2(
    const(JPH_ShapeSettings)* settings,
    const(JPH_RVec3)* position,
    const(JPH_Quat)* rotation,
    JPH_MotionType motionType,
    JPH_ObjectLayer objectLayer);
JPH_BodyCreationSettings* JPH_BodyCreationSettings_Create3(
    const(JPH_Shape)* shape,
    const(JPH_RVec3)* position,
    const(JPH_Quat)* rotation,
    JPH_MotionType motionType,
    JPH_ObjectLayer objectLayer);
void JPH_BodyCreationSettings_Destroy(JPH_BodyCreationSettings* settings);

void JPH_BodyCreationSettings_GetPosition(JPH_BodyCreationSettings* settings, JPH_RVec3* result);
void JPH_BodyCreationSettings_SetPosition(JPH_BodyCreationSettings* settings, const(JPH_RVec3)* value);

void JPH_BodyCreationSettings_GetRotation(JPH_BodyCreationSettings* settings, JPH_Quat* result);
void JPH_BodyCreationSettings_SetRotation(JPH_BodyCreationSettings* settings, const(JPH_Quat)* value);

void JPH_BodyCreationSettings_GetLinearVelocity(JPH_BodyCreationSettings* settings, JPH_Vec3* velocity);
void JPH_BodyCreationSettings_SetLinearVelocity(JPH_BodyCreationSettings* settings, const(JPH_Vec3)* velocity);

void JPH_BodyCreationSettings_GetAngularVelocity(
    JPH_BodyCreationSettings* settings, JPH_Vec3* velocity);
void JPH_BodyCreationSettings_SetAngularVelocity(
    JPH_BodyCreationSettings* settings, const(JPH_Vec3)* velocity);

ulong JPH_BodyCreationSettings_GetUserData(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetUserData(JPH_BodyCreationSettings* settings, ulong value);

JPH_ObjectLayer JPH_BodyCreationSettings_GetObjectLayer(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetObjectLayer(JPH_BodyCreationSettings* settings, JPH_ObjectLayer value);

void JPH_BodyCreationSettings_GetCollissionGroup(
    const(JPH_BodyCreationSettings)* settings, JPH_CollisionGroup* result);
void JPH_BodyCreationSettings_SetCollissionGroup(
    JPH_BodyCreationSettings* settings, const(JPH_CollisionGroup)* value);

JPH_MotionType JPH_BodyCreationSettings_GetMotionType(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetMotionType(JPH_BodyCreationSettings* settings, JPH_MotionType value);

JPH_AllowedDOFs JPH_BodyCreationSettings_GetAllowedDOFs(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetAllowedDOFs(JPH_BodyCreationSettings* settings, JPH_AllowedDOFs value);

bool JPH_BodyCreationSettings_GetAllowDynamicOrKinematic(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetAllowDynamicOrKinematic(
    JPH_BodyCreationSettings* settings, bool value);

bool JPH_BodyCreationSettings_GetIsSensor(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetIsSensor(JPH_BodyCreationSettings* settings, bool value);

bool JPH_BodyCreationSettings_GetCollideKinematicVsNonDynamic(
    const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetCollideKinematicVsNonDynamic(
    JPH_BodyCreationSettings* settings, bool value);

bool JPH_BodyCreationSettings_GetUseManifoldReduction(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetUseManifoldReduction(JPH_BodyCreationSettings* settings, bool value);

bool JPH_BodyCreationSettings_GetApplyGyroscopicForce(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetApplyGyroscopicForce(JPH_BodyCreationSettings* settings, bool value);

JPH_MotionQuality JPH_BodyCreationSettings_GetMotionQuality(
    const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetMotionQuality(JPH_BodyCreationSettings* settings, JPH_MotionQuality value);

bool JPH_BodyCreationSettings_GetEnhancedInternalEdgeRemoval(
    const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetEnhancedInternalEdgeRemoval(
    JPH_BodyCreationSettings* settings, bool value);

bool JPH_BodyCreationSettings_GetAllowSleeping(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetAllowSleeping(JPH_BodyCreationSettings* settings, bool value);

float JPH_BodyCreationSettings_GetFriction(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetFriction(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetRestitution(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetRestitution(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetLinearDamping(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetLinearDamping(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetAngularDamping(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetAngularDamping(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetMaxLinearVelocity(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetMaxLinearVelocity(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetMaxAngularVelocity(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetMaxAngularVelocity(JPH_BodyCreationSettings* settings, float value);

float JPH_BodyCreationSettings_GetGravityFactor(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetGravityFactor(JPH_BodyCreationSettings* settings, float value);

uint JPH_BodyCreationSettings_GetNumVelocityStepsOverride(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetNumVelocityStepsOverride(
    JPH_BodyCreationSettings* settings, uint value);

uint JPH_BodyCreationSettings_GetNumPositionStepsOverride(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetNumPositionStepsOverride(
    JPH_BodyCreationSettings* settings, uint value);

JPH_OverrideMassProperties JPH_BodyCreationSettings_GetOverrideMassProperties(
    const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetOverrideMassProperties(
    JPH_BodyCreationSettings* settings, JPH_OverrideMassProperties value);

float JPH_BodyCreationSettings_GetInertiaMultiplier(const(JPH_BodyCreationSettings)* settings);
void JPH_BodyCreationSettings_SetInertiaMultiplier(JPH_BodyCreationSettings* settings, float value);

void JPH_BodyCreationSettings_GetMassPropertiesOverride(
    const(JPH_BodyCreationSettings)* settings, JPH_MassProperties* result);
void JPH_BodyCreationSettings_SetMassPropertiesOverride(
    JPH_BodyCreationSettings* settings, const(JPH_MassProperties)* massProperties);

/* JPH_SoftBodyCreationSettings */
JPH_SoftBodyCreationSettings* JPH_SoftBodyCreationSettings_Create();
void JPH_SoftBodyCreationSettings_Destroy(JPH_SoftBodyCreationSettings* settings);

/* JPH_Constraint */
void JPH_Constraint_Destroy(JPH_Constraint* constraint);
JPH_ConstraintType JPH_Constraint_GetType(const(JPH_Constraint)* constraint);
JPH_ConstraintSubType JPH_Constraint_GetSubType(const(JPH_Constraint)* constraint);
uint JPH_Constraint_GetConstraintPriority(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetConstraintPriority(JPH_Constraint* constraint, uint priority);
uint JPH_Constraint_GetNumVelocityStepsOverride(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetNumVelocityStepsOverride(JPH_Constraint* constraint, uint value);
uint JPH_Constraint_GetNumPositionStepsOverride(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetNumPositionStepsOverride(JPH_Constraint* constraint, uint value);
bool JPH_Constraint_GetEnabled(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetEnabled(JPH_Constraint* constraint, bool enabled);
ulong JPH_Constraint_GetUserData(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetUserData(JPH_Constraint* constraint, ulong userData);
void JPH_Constraint_NotifyShapeChanged(JPH_Constraint* constraint, JPH_BodyID bodyID, JPH_Vec3* deltaCOM);
void JPH_Constraint_ResetWarmStart(JPH_Constraint* constraint);
bool JPH_Constraint_IsActive(const(JPH_Constraint)* constraint);
void JPH_Constraint_SetupVelocityConstraint(JPH_Constraint* constraint, float deltaTime);
void JPH_Constraint_WarmStartVelocityConstraint(JPH_Constraint* constraint, float warmStartImpulseRatio);
bool JPH_Constraint_SolveVelocityConstraint(JPH_Constraint* constraint, float deltaTime);
bool JPH_Constraint_SolvePositionConstraint(JPH_Constraint* constraint, float deltaTime, float baumgarte);

/* JPH_TwoBodyConstraint */
JPH_Body* JPH_TwoBodyConstraint_GetBody1(const(JPH_TwoBodyConstraint)* constraint);
JPH_Body* JPH_TwoBodyConstraint_GetBody2(const(JPH_TwoBodyConstraint)* constraint);
void JPH_TwoBodyConstraint_GetConstraintToBody1Matrix(
    const(JPH_TwoBodyConstraint)* constraint, JPH_Matrix4x4* result);
void JPH_TwoBodyConstraint_GetConstraintToBody2Matrix(
    const(JPH_TwoBodyConstraint)* constraint, JPH_Matrix4x4* result);

/* JPH_FixedConstraint */
void JPH_FixedConstraintSettings_Init(JPH_FixedConstraintSettings* settings);
JPH_FixedConstraint* JPH_FixedConstraint_Create(
    const(JPH_FixedConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_FixedConstraint_GetSettings(const(JPH_FixedConstraint)* constraint, JPH_FixedConstraintSettings* settings);
void JPH_FixedConstraint_GetTotalLambdaPosition(
    const(JPH_FixedConstraint)* constraint, JPH_Vec3* result);
void JPH_FixedConstraint_GetTotalLambdaRotation(
    const(JPH_FixedConstraint)* constraint, JPH_Vec3* result);

/* JPH_DistanceConstraint */
void JPH_DistanceConstraintSettings_Init(JPH_DistanceConstraintSettings* settings);
JPH_DistanceConstraint* JPH_DistanceConstraint_Create(const(
        JPH_DistanceConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_DistanceConstraint_GetSettings(const(JPH_DistanceConstraint)* constraint, JPH_DistanceConstraintSettings* settings);
void JPH_DistanceConstraint_SetDistance(JPH_DistanceConstraint* constraint, float minDistance, float maxDistance);
float JPH_DistanceConstraint_GetMinDistance(JPH_DistanceConstraint* constraint);
float JPH_DistanceConstraint_GetMaxDistance(JPH_DistanceConstraint* constraint);
void JPH_DistanceConstraint_GetLimitsSpringSettings(
    JPH_DistanceConstraint* constraint, JPH_SpringSettings* result);
void JPH_DistanceConstraint_SetLimitsSpringSettings(
    JPH_DistanceConstraint* constraint, JPH_SpringSettings* settings);
float JPH_DistanceConstraint_GetTotalLambdaPosition(const(JPH_DistanceConstraint)* constraint);

/* JPH_PointConstraint */
void JPH_PointConstraintSettings_Init(JPH_PointConstraintSettings* settings);
JPH_PointConstraint* JPH_PointConstraint_Create(
    const(JPH_PointConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_PointConstraint_GetSettings(const(JPH_PointConstraint)* constraint, JPH_PointConstraintSettings* settings);
void JPH_PointConstraint_SetPoint1(JPH_PointConstraint* constraint, JPH_ConstraintSpace space, JPH_RVec3* value);
void JPH_PointConstraint_SetPoint2(JPH_PointConstraint* constraint, JPH_ConstraintSpace space, JPH_RVec3* value);
void JPH_PointConstraint_GetLocalSpacePoint1(
    const(JPH_PointConstraint)* constraint, JPH_Vec3* result);
void JPH_PointConstraint_GetLocalSpacePoint2(
    const(JPH_PointConstraint)* constraint, JPH_Vec3* result);
void JPH_PointConstraint_GetTotalLambdaPosition(
    const(JPH_PointConstraint)* constraint, JPH_Vec3* result);

/* JPH_HingeConstraint */
void JPH_HingeConstraintSettings_Init(JPH_HingeConstraintSettings* settings);
JPH_HingeConstraint* JPH_HingeConstraint_Create(
    const(JPH_HingeConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_HingeConstraint_GetSettings(JPH_HingeConstraint* constraint, JPH_HingeConstraintSettings* settings);
void JPH_HingeConstraint_GetLocalSpacePoint1(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetLocalSpacePoint2(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetLocalSpaceHingeAxis1(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetLocalSpaceHingeAxis2(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetLocalSpaceNormalAxis1(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetLocalSpaceNormalAxis2(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
float JPH_HingeConstraint_GetCurrentAngle(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_SetMaxFrictionTorque(JPH_HingeConstraint* constraint, float frictionTorque);
float JPH_HingeConstraint_GetMaxFrictionTorque(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_SetMotorSettings(JPH_HingeConstraint* constraint, JPH_MotorSettings* settings);
void JPH_HingeConstraint_GetMotorSettings(JPH_HingeConstraint* constraint, JPH_MotorSettings* result);
void JPH_HingeConstraint_SetMotorState(JPH_HingeConstraint* constraint, JPH_MotorState state);
JPH_MotorState JPH_HingeConstraint_GetMotorState(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_SetTargetAngularVelocity(JPH_HingeConstraint* constraint, float angularVelocity);
float JPH_HingeConstraint_GetTargetAngularVelocity(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_SetTargetAngle(JPH_HingeConstraint* constraint, float angle);
float JPH_HingeConstraint_GetTargetAngle(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_SetLimits(JPH_HingeConstraint* constraint, float inLimitsMin, float inLimitsMax);
float JPH_HingeConstraint_GetLimitsMin(JPH_HingeConstraint* constraint);
float JPH_HingeConstraint_GetLimitsMax(JPH_HingeConstraint* constraint);
bool JPH_HingeConstraint_HasLimits(JPH_HingeConstraint* constraint);
void JPH_HingeConstraint_GetLimitsSpringSettings(JPH_HingeConstraint* constraint, JPH_SpringSettings* result);
void JPH_HingeConstraint_SetLimitsSpringSettings(JPH_HingeConstraint* constraint, JPH_SpringSettings* settings);
void JPH_HingeConstraint_GetTotalLambdaPosition(
    const(JPH_HingeConstraint)* constraint, JPH_Vec3* result);
void JPH_HingeConstraint_GetTotalLambdaRotation(
    const(JPH_HingeConstraint)* constraint, ref float[2] rotation);
float JPH_HingeConstraint_GetTotalLambdaRotationLimits(const(JPH_HingeConstraint)* constraint);
float JPH_HingeConstraint_GetTotalLambdaMotor(const(JPH_HingeConstraint)* constraint);

/* JPH_SliderConstraint */
void JPH_SliderConstraintSettings_Init(JPH_SliderConstraintSettings* settings);
void JPH_SliderConstraintSettings_SetSliderAxis(
    JPH_SliderConstraintSettings* settings, const(JPH_Vec3)* axis);

JPH_SliderConstraint* JPH_SliderConstraint_Create(
    const(JPH_SliderConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_SliderConstraint_GetSettings(JPH_SliderConstraint* constraint, JPH_SliderConstraintSettings* settings);
float JPH_SliderConstraint_GetCurrentPosition(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_SetMaxFrictionForce(JPH_SliderConstraint* constraint, float frictionForce);
float JPH_SliderConstraint_GetMaxFrictionForce(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_SetMotorSettings(JPH_SliderConstraint* constraint, JPH_MotorSettings* settings);
void JPH_SliderConstraint_GetMotorSettings(const(JPH_SliderConstraint)* constraint, JPH_MotorSettings* result);
void JPH_SliderConstraint_SetMotorState(JPH_SliderConstraint* constraint, JPH_MotorState state);
JPH_MotorState JPH_SliderConstraint_GetMotorState(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_SetTargetVelocity(JPH_SliderConstraint* constraint, float velocity);
float JPH_SliderConstraint_GetTargetVelocity(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_SetTargetPosition(JPH_SliderConstraint* constraint, float position);
float JPH_SliderConstraint_GetTargetPosition(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_SetLimits(JPH_SliderConstraint* constraint, float inLimitsMin, float inLimitsMax);
float JPH_SliderConstraint_GetLimitsMin(JPH_SliderConstraint* constraint);
float JPH_SliderConstraint_GetLimitsMax(JPH_SliderConstraint* constraint);
bool JPH_SliderConstraint_HasLimits(JPH_SliderConstraint* constraint);
void JPH_SliderConstraint_GetLimitsSpringSettings(JPH_SliderConstraint* constraint, JPH_SpringSettings* result);
void JPH_SliderConstraint_SetLimitsSpringSettings(JPH_SliderConstraint* constraint, JPH_SpringSettings* settings);
void JPH_SliderConstraint_GetTotalLambdaPosition(
    const(JPH_SliderConstraint)* constraint, ref float[2] position);
float JPH_SliderConstraint_GetTotalLambdaPositionLimits(const(JPH_SliderConstraint)* constraint);
void JPH_SliderConstraint_GetTotalLambdaRotation(
    const(JPH_SliderConstraint)* constraint, JPH_Vec3* result);
float JPH_SliderConstraint_GetTotalLambdaMotor(const(JPH_SliderConstraint)* constraint);

/* JPH_ConeConstraint */
void JPH_ConeConstraintSettings_Init(JPH_ConeConstraintSettings* settings);
JPH_ConeConstraint* JPH_ConeConstraint_Create(
    const(JPH_ConeConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_ConeConstraint_GetSettings(JPH_ConeConstraint* constraint, JPH_ConeConstraintSettings* settings);
void JPH_ConeConstraint_SetHalfConeAngle(JPH_ConeConstraint* constraint, float halfConeAngle);
float JPH_ConeConstraint_GetCosHalfConeAngle(const(JPH_ConeConstraint)* constraint);
void JPH_ConeConstraint_GetTotalLambdaPosition(
    const(JPH_ConeConstraint)* constraint, JPH_Vec3* result);
float JPH_ConeConstraint_GetTotalLambdaRotation(const(JPH_ConeConstraint)* constraint);

/* JPH_SwingTwistConstraint */
void JPH_SwingTwistConstraintSettings_Init(JPH_SwingTwistConstraintSettings* settings);
JPH_SwingTwistConstraint* JPH_SwingTwistConstraint_Create(const(
        JPH_SwingTwistConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_SwingTwistConstraint_GetSettings(JPH_SwingTwistConstraint* constraint, JPH_SwingTwistConstraintSettings* settings);
float JPH_SwingTwistConstraint_GetNormalHalfConeAngle(JPH_SwingTwistConstraint* constraint);
void JPH_SwingTwistConstraint_GetTotalLambdaPosition(
    const(JPH_SwingTwistConstraint)* constraint, JPH_Vec3* result);
float JPH_SwingTwistConstraint_GetTotalLambdaTwist(const(JPH_SwingTwistConstraint)* constraint);
float JPH_SwingTwistConstraint_GetTotalLambdaSwingY(const(JPH_SwingTwistConstraint)* constraint);
float JPH_SwingTwistConstraint_GetTotalLambdaSwingZ(const(JPH_SwingTwistConstraint)* constraint);
void JPH_SwingTwistConstraint_GetTotalLambdaMotor(
    const(JPH_SwingTwistConstraint)* constraint, JPH_Vec3* result);

/* JPH_SixDOFConstraint */
void JPH_SixDOFConstraintSettings_Init(JPH_SixDOFConstraintSettings* settings);
void JPH_SixDOFConstraintSettings_MakeFreeAxis(
    JPH_SixDOFConstraintSettings* settings, JPH_SixDOFConstraintAxis axis);
bool JPH_SixDOFConstraintSettings_IsFreeAxis(
    const(JPH_SixDOFConstraintSettings)* settings, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraintSettings_MakeFixedAxis(
    JPH_SixDOFConstraintSettings* settings, JPH_SixDOFConstraintAxis axis);
bool JPH_SixDOFConstraintSettings_IsFixedAxis(
    const(JPH_SixDOFConstraintSettings)* settings, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraintSettings_SetLimitedAxis(JPH_SixDOFConstraintSettings* settings, JPH_SixDOFConstraintAxis axis, float min, float max);

JPH_SixDOFConstraint* JPH_SixDOFConstraint_Create(
    const(JPH_SixDOFConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_SixDOFConstraint_GetSettings(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintSettings* settings);
float JPH_SixDOFConstraint_GetLimitsMin(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis);
float JPH_SixDOFConstraint_GetLimitsMax(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_GetTotalLambdaPosition(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetTotalLambdaRotation(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetTotalLambdaMotorTranslation(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetTotalLambdaMotorRotation(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetTranslationLimitsMin(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetTranslationLimitsMax(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetRotationLimitsMin(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_GetRotationLimitsMax(
    const(JPH_SixDOFConstraint)* constraint, JPH_Vec3* result);
bool JPH_SixDOFConstraint_IsFixedAxis(const(JPH_SixDOFConstraint)* constraint, JPH_SixDOFConstraintAxis axis);
bool JPH_SixDOFConstraint_IsFreeAxis(const(JPH_SixDOFConstraint)* constraint, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_GetLimitsSpringSettings(JPH_SixDOFConstraint* constraint, JPH_SpringSettings* result, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_SetLimitsSpringSettings(JPH_SixDOFConstraint* constraint, JPH_SpringSettings* settings, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_SetMaxFriction(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis, float inFriction);
float JPH_SixDOFConstraint_GetMaxFriction(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_GetRotationInConstraintSpace(
    JPH_SixDOFConstraint* constraint, JPH_Quat* result);
void JPH_SixDOFConstraint_GetMotorSettings(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis, JPH_MotorSettings* settings);
void JPH_SixDOFConstraint_SetMotorState(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis, JPH_MotorState state);
JPH_MotorState JPH_SixDOFConstraint_GetMotorState(JPH_SixDOFConstraint* constraint, JPH_SixDOFConstraintAxis axis);
void JPH_SixDOFConstraint_SetTargetVelocityCS(JPH_SixDOFConstraint* constraint, JPH_Vec3* inVelocity);
void JPH_SixDOFConstraint_GetTargetVelocityCS(JPH_SixDOFConstraint* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_SetTargetAngularVelocityCS(
    JPH_SixDOFConstraint* constraint, JPH_Vec3* inAngularVelocity);
void JPH_SixDOFConstraint_GetTargetAngularVelocityCS(
    JPH_SixDOFConstraint* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_SetTargetPositionCS(JPH_SixDOFConstraint* constraint, JPH_Vec3* inPosition);
void JPH_SixDOFConstraint_GetTargetPositionCS(JPH_SixDOFConstraint* constraint, JPH_Vec3* result);
void JPH_SixDOFConstraint_SetTargetOrientationCS(JPH_SixDOFConstraint* constraint, JPH_Quat* inOrientation);
void JPH_SixDOFConstraint_GetTargetOrientationCS(JPH_SixDOFConstraint* constraint, JPH_Quat* result);
void JPH_SixDOFConstraint_SetTargetOrientationBS(JPH_SixDOFConstraint* constraint, JPH_Quat* inOrientation);

/* JPH_GearConstraint */
void JPH_GearConstraintSettings_Init(JPH_GearConstraintSettings* settings);
JPH_GearConstraint* JPH_GearConstraint_Create(
    const(JPH_GearConstraintSettings)* settings, JPH_Body* body1, JPH_Body* body2);
void JPH_GearConstraint_GetSettings(JPH_GearConstraint* constraint, JPH_GearConstraintSettings* settings);
void JPH_GearConstraint_SetConstraints(JPH_GearConstraint* constraint, const(JPH_Constraint)* gear1, const(
        JPH_Constraint)* gear2);
float JPH_GearConstraint_GetTotalLambda(const(JPH_GearConstraint)* constraint);

/* BodyInterface */
void JPH_BodyInterface_DestroyBody(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
JPH_BodyID JPH_BodyInterface_CreateAndAddBody(JPH_BodyInterface* interface_, const(
        JPH_BodyCreationSettings)* settings, JPH_Activation activationMode);
JPH_Body* JPH_BodyInterface_CreateBody(JPH_BodyInterface* interface_, const(
        JPH_BodyCreationSettings)* settings);
JPH_Body* JPH_BodyInterface_CreateBodyWithID(JPH_BodyInterface* interface_, JPH_BodyID bodyID, const(
        JPH_BodyCreationSettings)* settings);
JPH_Body* JPH_BodyInterface_CreateBodyWithoutID(JPH_BodyInterface* interface_, const(
        JPH_BodyCreationSettings)* settings);
void JPH_BodyInterface_DestroyBodyWithoutID(JPH_BodyInterface* interface_, JPH_Body* body_);
bool JPH_BodyInterface_AssignBodyID(JPH_BodyInterface* interface_, JPH_Body* body_);
bool JPH_BodyInterface_AssignBodyID2(JPH_BodyInterface* interface_, JPH_Body* body_, JPH_BodyID bodyID);
JPH_Body* JPH_BodyInterface_UnassignBodyID(JPH_BodyInterface* interface_, JPH_BodyID bodyID);

JPH_Body* JPH_BodyInterface_CreateSoftBody(JPH_BodyInterface* interface_, const(
        JPH_SoftBodyCreationSettings)* settings);
JPH_Body* JPH_BodyInterface_CreateSoftBodyWithID(JPH_BodyInterface* interface_, JPH_BodyID bodyID, const(
        JPH_SoftBodyCreationSettings)* settings);
JPH_Body* JPH_BodyInterface_CreateSoftBodyWithoutID(JPH_BodyInterface* interface_, const(
        JPH_SoftBodyCreationSettings)* settings);
JPH_BodyID JPH_BodyInterface_CreateAndAddSoftBody(JPH_BodyInterface* interface_, const(
        JPH_SoftBodyCreationSettings)* settings, JPH_Activation activationMode);

void JPH_BodyInterface_AddBody(JPH_BodyInterface* interface_, JPH_BodyID bodyID, JPH_Activation activationMode);
void JPH_BodyInterface_RemoveBody(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
void JPH_BodyInterface_RemoveAndDestroyBody(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
bool JPH_BodyInterface_IsActive(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
bool JPH_BodyInterface_IsAdded(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
JPH_BodyType JPH_BodyInterface_GetBodyType(JPH_BodyInterface* interface_, JPH_BodyID bodyID);

void JPH_BodyInterface_SetLinearVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyID, const(
        JPH_Vec3)* velocity);
void JPH_BodyInterface_GetLinearVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyID, JPH_Vec3* velocity);
void JPH_BodyInterface_GetCenterOfMassPosition(JPH_BodyInterface* interface_, JPH_BodyID bodyID, JPH_RVec3* position);

JPH_MotionType JPH_BodyInterface_GetMotionType(JPH_BodyInterface* interface_, JPH_BodyID bodyID);
void JPH_BodyInterface_SetMotionType(JPH_BodyInterface* interface_, JPH_BodyID bodyID, JPH_MotionType motionType, JPH_Activation activationMode);

float JPH_BodyInterface_GetRestitution(const(JPH_BodyInterface)* interface_, JPH_BodyID bodyID);
void JPH_BodyInterface_SetRestitution(JPH_BodyInterface* interface_, JPH_BodyID bodyID, float restitution);

float JPH_BodyInterface_GetFriction(const(JPH_BodyInterface)* interface_, JPH_BodyID bodyID);
void JPH_BodyInterface_SetFriction(JPH_BodyInterface* interface_, JPH_BodyID bodyID, float friction);

void JPH_BodyInterface_SetPosition(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* position, JPH_Activation activationMode);
void JPH_BodyInterface_GetPosition(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* result);

void JPH_BodyInterface_SetRotation(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Quat* rotation, JPH_Activation activationMode);
void JPH_BodyInterface_GetRotation(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Quat* result);

void JPH_BodyInterface_SetPositionAndRotation(JPH_BodyInterface* interface_, JPH_BodyID bodyId, const(
        JPH_RVec3)* position, const(JPH_Quat)* rotation, JPH_Activation activationMode);
void JPH_BodyInterface_SetPositionAndRotationWhenChanged(JPH_BodyInterface* interface_, JPH_BodyID bodyId, const(
        JPH_RVec3)* position, const(JPH_Quat)* rotation, JPH_Activation activationMode);
void JPH_BodyInterface_GetPositionAndRotation(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* position, JPH_Quat* rotation);
void JPH_BodyInterface_SetPositionRotationAndVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* position, JPH_Quat* rotation, JPH_Vec3* linearVelocity, JPH_Vec3* angularVelocity);

void JPH_BodyInterface_GetCollissionGroup(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_CollisionGroup* result);
void JPH_BodyInterface_SetCollissionGroup(JPH_BodyInterface* interface_, JPH_BodyID bodyId, const(
        JPH_CollisionGroup)* group);

const(JPH_Shape)* JPH_BodyInterface_GetShape(JPH_BodyInterface* interface_, JPH_BodyID bodyId);
void JPH_BodyInterface_SetShape(JPH_BodyInterface* interface_, JPH_BodyID bodyId, const(JPH_Shape)* shape, bool updateMassProperties, JPH_Activation activationMode);
void JPH_BodyInterface_NotifyShapeChanged(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* previousCenterOfMass, bool updateMassProperties, JPH_Activation activationMode);

void JPH_BodyInterface_ActivateBody(JPH_BodyInterface* interface_, JPH_BodyID bodyId);
void JPH_BodyInterface_DeactivateBody(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

JPH_ObjectLayer JPH_BodyInterface_GetObjectLayer(JPH_BodyInterface* interface_, JPH_BodyID bodyId);
void JPH_BodyInterface_SetObjectLayer(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_ObjectLayer layer);

void JPH_BodyInterface_GetWorldTransform(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RMatrix4x4* result);
void JPH_BodyInterface_GetCenterOfMassTransform(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RMatrix4x4* result);

void JPH_BodyInterface_MoveKinematic(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* targetPosition, JPH_Quat* targetRotation, float deltaTime);
bool JPH_BodyInterface_ApplyBuoyancyImpulse(JPH_BodyInterface* interface_, JPH_BodyID bodyId, const(JPH_RVec3)* surfacePosition, const(
        JPH_Vec3)* surfaceNormal, float buoyancy, float linearDrag, float angularDrag, const(
        JPH_Vec3)* fluidVelocity, const(JPH_Vec3)* gravity, float deltaTime);

void JPH_BodyInterface_SetLinearAndAngularVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* linearVelocity, JPH_Vec3* angularVelocity);
void JPH_BodyInterface_GetLinearAndAngularVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* linearVelocity, JPH_Vec3* angularVelocity);

void JPH_BodyInterface_AddLinearVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* linearVelocity);
void JPH_BodyInterface_AddLinearAndAngularVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* linearVelocity, JPH_Vec3* angularVelocity);

void JPH_BodyInterface_SetAngularVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* angularVelocity);
void JPH_BodyInterface_GetAngularVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* angularVelocity);

void JPH_BodyInterface_GetPointVelocity(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_RVec3* point, JPH_Vec3* velocity);

void JPH_BodyInterface_AddForce(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* force);
void JPH_BodyInterface_AddForce2(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* force, JPH_RVec3* point);
void JPH_BodyInterface_AddTorque(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* torque);
void JPH_BodyInterface_AddForceAndTorque(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* force, JPH_Vec3* torque);

void JPH_BodyInterface_AddImpulse(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* impulse);
void JPH_BodyInterface_AddImpulse2(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* impulse, JPH_RVec3* point);
void JPH_BodyInterface_AddAngularImpulse(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Vec3* angularImpulse);

void JPH_BodyInterface_SetMotionQuality(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_MotionQuality quality);
JPH_MotionQuality JPH_BodyInterface_GetMotionQuality(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

void JPH_BodyInterface_GetInverseInertia(JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_Matrix4x4* result);

void JPH_BodyInterface_SetGravityFactor(JPH_BodyInterface* interface_, JPH_BodyID bodyId, float value);
float JPH_BodyInterface_GetGravityFactor(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

void JPH_BodyInterface_SetUseManifoldReduction(JPH_BodyInterface* interface_, JPH_BodyID bodyId, bool value);
bool JPH_BodyInterface_GetUseManifoldReduction(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

void JPH_BodyInterface_SetUserData(JPH_BodyInterface* interface_, JPH_BodyID bodyId, ulong inUserData);
ulong JPH_BodyInterface_GetUserData(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

const(JPH_PhysicsMaterial)* JPH_BodyInterface_GetMaterial(
    JPH_BodyInterface* interface_, JPH_BodyID bodyId, JPH_SubShapeID subShapeID);

void JPH_BodyInterface_InvalidateContactCache(JPH_BodyInterface* interface_, JPH_BodyID bodyId);

//--------------------------------------------------------------------------------------------------
// JPH_BodyLockInterface
//--------------------------------------------------------------------------------------------------
void JPH_BodyLockInterface_LockRead(const(JPH_BodyLockInterface)* lockInterface, JPH_BodyID bodyID, JPH_BodyLockRead* outLock);
void JPH_BodyLockInterface_UnlockRead(const(JPH_BodyLockInterface)* lockInterface, JPH_BodyLockRead* ioLock);

void JPH_BodyLockInterface_LockWrite(const(JPH_BodyLockInterface)* lockInterface, JPH_BodyID bodyID, JPH_BodyLockWrite* outLock);
void JPH_BodyLockInterface_UnlockWrite(const(JPH_BodyLockInterface)* lockInterface, JPH_BodyLockWrite* ioLock);

JPH_BodyLockMultiRead* JPH_BodyLockInterface_LockMultiRead(const(
        JPH_BodyLockInterface)* lockInterface, const(JPH_BodyID)* bodyIDs, uint count);
void JPH_BodyLockMultiRead_Destroy(JPH_BodyLockMultiRead* ioLock);
const(JPH_Body)* JPH_BodyLockMultiRead_GetBody(JPH_BodyLockMultiRead* ioLock, uint bodyIndex);

JPH_BodyLockMultiWrite* JPH_BodyLockInterface_LockMultiWrite(const(
        JPH_BodyLockInterface)* lockInterface, const(JPH_BodyID)* bodyIDs, uint count);
void JPH_BodyLockMultiWrite_Destroy(JPH_BodyLockMultiWrite* ioLock);
JPH_Body* JPH_BodyLockMultiWrite_GetBody(JPH_BodyLockMultiWrite* ioLock, uint bodyIndex);

//--------------------------------------------------------------------------------------------------
// JPH_MotionProperties
//--------------------------------------------------------------------------------------------------
JPH_AllowedDOFs JPH_MotionProperties_GetAllowedDOFs(const(JPH_MotionProperties)* properties);
void JPH_MotionProperties_SetLinearDamping(JPH_MotionProperties* properties, float damping);
float JPH_MotionProperties_GetLinearDamping(const(JPH_MotionProperties)* properties);
void JPH_MotionProperties_SetAngularDamping(JPH_MotionProperties* properties, float damping);
float JPH_MotionProperties_GetAngularDamping(const(JPH_MotionProperties)* properties);
void JPH_MotionProperties_SetMassProperties(JPH_MotionProperties* properties, JPH_AllowedDOFs allowedDOFs, const(
        JPH_MassProperties)* massProperties);
float JPH_MotionProperties_GetInverseMassUnchecked(JPH_MotionProperties* properties);
void JPH_MotionProperties_SetInverseMass(JPH_MotionProperties* properties, float inverseMass);
void JPH_MotionProperties_GetInverseInertiaDiagonal(
    JPH_MotionProperties* properties, JPH_Vec3* result);
void JPH_MotionProperties_GetInertiaRotation(JPH_MotionProperties* properties, JPH_Quat* result);
void JPH_MotionProperties_SetInverseInertia(JPH_MotionProperties* properties, JPH_Vec3* diagonal, JPH_Quat* rot);
void JPH_MotionProperties_ScaleToMass(JPH_MotionProperties* properties, float mass);

//--------------------------------------------------------------------------------------------------
// JPH_RayCast
//--------------------------------------------------------------------------------------------------
void JPH_RayCast_GetPointOnRay(const(JPH_Vec3)* origin, const(JPH_Vec3)* direction, float fraction, JPH_Vec3* result);
void JPH_RRayCast_GetPointOnRay(const(JPH_RVec3)* origin, const(JPH_Vec3)* direction, float fraction, JPH_RVec3* result);

//--------------------------------------------------------------------------------------------------
// JPH_MassProperties
//--------------------------------------------------------------------------------------------------
void JPH_MassProperties_DecomposePrincipalMomentsOfInertia(
    JPH_MassProperties* properties, JPH_Matrix4x4* rotation, JPH_Vec3* diagonal);
void JPH_MassProperties_ScaleToMass(JPH_MassProperties* properties, float mass);
void JPH_MassProperties_GetEquivalentSolidBoxSize(float mass, const(JPH_Vec3)* inertiaDiagonal, JPH_Vec3* result);

//--------------------------------------------------------------------------------------------------
// JPH_CollideShapeSettings
//--------------------------------------------------------------------------------------------------
void JPH_CollideShapeSettings_Init(JPH_CollideShapeSettings* settings);

//--------------------------------------------------------------------------------------------------
// JPH_ShapeCastSettings
//--------------------------------------------------------------------------------------------------
void JPH_ShapeCastSettings_Init(JPH_ShapeCastSettings* settings);

//--------------------------------------------------------------------------------------------------
// JPH_BroadPhaseQuery
//--------------------------------------------------------------------------------------------------
bool JPH_BroadPhaseQuery_CastRay(
    const(JPH_BroadPhaseQuery)* query,
    const(JPH_Vec3)* origin,
    const(JPH_Vec3)* direction,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter);

bool JPH_BroadPhaseQuery_CastRay2(
    const(JPH_BroadPhaseQuery)* query,
    const(JPH_Vec3)* origin,
    const(JPH_Vec3)* direction,
    JPH_CollisionCollectorType collectorType,
    void function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter);

bool JPH_BroadPhaseQuery_CollideAABox(
    const(JPH_BroadPhaseQuery)* query,
    const(JPH_AABox)* box,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter);

bool JPH_BroadPhaseQuery_CollideSphere(
    const(JPH_BroadPhaseQuery)* query,
    const(JPH_Vec3)* center,
    float radius,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter);

bool JPH_BroadPhaseQuery_CollidePoint(
    const(JPH_BroadPhaseQuery)* query,
    const(JPH_Vec3)* point,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter);

//--------------------------------------------------------------------------------------------------
// JPH_NarrowPhaseQuery
//--------------------------------------------------------------------------------------------------
bool JPH_NarrowPhaseQuery_CastRay(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_RVec3)* origin,
    const(JPH_Vec3)* direction,
    JPH_RayCastResult* hit,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter);

bool JPH_NarrowPhaseQuery_CastRay2(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_RVec3)* origin,
    const(JPH_Vec3)* direction,
    const(JPH_RayCastSettings)* rayCastSettings,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CastRay3(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_RVec3)* origin,
    const(JPH_Vec3)* direction,
    const(JPH_RayCastSettings)* rayCastSettings,
    JPH_CollisionCollectorType collectorType,
    void function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CollidePoint(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_RVec3)* point,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CollidePoint2(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_RVec3)* point,
    JPH_CollisionCollectorType collectorType,
    void function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CollideShape(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_Shape)* shape,
    const(JPH_Vec3)* scale,
    const(JPH_RMatrix4x4)* centerOfMassTransform,
    const(JPH_CollideShapeSettings)* settings,
    JPH_RVec3* baseOffset,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CollideShape2(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_Shape)* shape,
    const(JPH_Vec3)* scale,
    const(JPH_RMatrix4x4)* centerOfMassTransform,
    const(JPH_CollideShapeSettings)* settings,
    JPH_RVec3* baseOffset,
    JPH_CollisionCollectorType collectorType,
    void function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CastShape(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_Shape)* shape,
    const(JPH_RMatrix4x4)* worldTransform,
    const(JPH_Vec3)* direction,
    const(JPH_ShapeCastSettings)* settings,
    JPH_RVec3* baseOffset,
    float function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_NarrowPhaseQuery_CastShape2(
    const(JPH_NarrowPhaseQuery)* query,
    const(JPH_Shape)* shape,
    const(JPH_RMatrix4x4)* worldTransform,
    const(JPH_Vec3)* direction,
    const(JPH_ShapeCastSettings)* settings,
    JPH_RVec3* baseOffset,
    JPH_CollisionCollectorType collectorType,
    void function() callback,
    void* userData,
    JPH_BroadPhaseLayerFilter* broadPhaseLayerFilter,
    JPH_ObjectLayerFilter* objectLayerFilter,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

//--------------------------------------------------------------------------------------------------
// JPH_Body
//--------------------------------------------------------------------------------------------------
JPH_BodyID JPH_Body_GetID(const(JPH_Body)* body_);
JPH_BodyType JPH_Body_GetBodyType(const(JPH_Body)* body_);
bool JPH_Body_IsRigidBody(const(JPH_Body)* body_);
bool JPH_Body_IsSoftBody(const(JPH_Body)* body_);
bool JPH_Body_IsActive(const(JPH_Body)* body_);
bool JPH_Body_IsStatic(const(JPH_Body)* body_);
bool JPH_Body_IsKinematic(const(JPH_Body)* body_);
bool JPH_Body_IsDynamic(const(JPH_Body)* body_);
bool JPH_Body_CanBeKinematicOrDynamic(const(JPH_Body)* body_);

void JPH_Body_SetIsSensor(JPH_Body* body_, bool value);
bool JPH_Body_IsSensor(const(JPH_Body)* body_);

void JPH_Body_SetCollideKinematicVsNonDynamic(JPH_Body* body_, bool value);
bool JPH_Body_GetCollideKinematicVsNonDynamic(const(JPH_Body)* body_);

void JPH_Body_SetUseManifoldReduction(JPH_Body* body_, bool value);
bool JPH_Body_GetUseManifoldReduction(const(JPH_Body)* body_);
bool JPH_Body_GetUseManifoldReductionWithBody(const(JPH_Body)* body_, const(JPH_Body)* other);

void JPH_Body_SetApplyGyroscopicForce(JPH_Body* body_, bool value);
bool JPH_Body_GetApplyGyroscopicForce(const(JPH_Body)* body_);

void JPH_Body_SetEnhancedInternalEdgeRemoval(JPH_Body* body_, bool value);
bool JPH_Body_GetEnhancedInternalEdgeRemoval(const(JPH_Body)* body_);
bool JPH_Body_GetEnhancedInternalEdgeRemovalWithBody(const(JPH_Body)* body_, const(JPH_Body)* other);

JPH_MotionType JPH_Body_GetMotionType(const(JPH_Body)* body_);
void JPH_Body_SetMotionType(JPH_Body* body_, JPH_MotionType motionType);

JPH_BroadPhaseLayer JPH_Body_GetBroadPhaseLayer(const(JPH_Body)* body_);
JPH_ObjectLayer JPH_Body_GetObjectLayer(const(JPH_Body)* body_);

void JPH_Body_GetCollissionGroup(const(JPH_Body)* body_, JPH_CollisionGroup* result);
void JPH_Body_SetCollissionGroup(JPH_Body* body_, const(JPH_CollisionGroup)* value);

bool JPH_Body_GetAllowSleeping(JPH_Body* body_);
void JPH_Body_SetAllowSleeping(JPH_Body* body_, bool allowSleeping);
void JPH_Body_ResetSleepTimer(JPH_Body* body_);

float JPH_Body_GetFriction(const(JPH_Body)* body_);
void JPH_Body_SetFriction(JPH_Body* body_, float friction);
float JPH_Body_GetRestitution(const(JPH_Body)* body_);
void JPH_Body_SetRestitution(JPH_Body* body_, float restitution);
void JPH_Body_GetLinearVelocity(JPH_Body* body_, JPH_Vec3* velocity);
void JPH_Body_SetLinearVelocity(JPH_Body* body_, const(JPH_Vec3)* velocity);
void JPH_Body_SetLinearVelocityClamped(JPH_Body* body_, const(JPH_Vec3)* velocity);
void JPH_Body_GetAngularVelocity(JPH_Body* body_, JPH_Vec3* velocity);
void JPH_Body_SetAngularVelocity(JPH_Body* body_, const(JPH_Vec3)* velocity);
void JPH_Body_SetAngularVelocityClamped(JPH_Body* body_, const(JPH_Vec3)* velocity);

void JPH_Body_GetPointVelocityCOM(JPH_Body* body_, const(JPH_Vec3)* pointRelativeToCOM, JPH_Vec3* velocity);
void JPH_Body_GetPointVelocity(JPH_Body* body_, const(JPH_RVec3)* point, JPH_Vec3* velocity);

void JPH_Body_AddForce(JPH_Body* body_, const(JPH_Vec3)* force);
void JPH_Body_AddForceAtPosition(JPH_Body* body_, const(JPH_Vec3)* force, const(JPH_RVec3)* position);
void JPH_Body_AddTorque(JPH_Body* body_, const(JPH_Vec3)* force);
void JPH_Body_GetAccumulatedForce(JPH_Body* body_, JPH_Vec3* force);
void JPH_Body_GetAccumulatedTorque(JPH_Body* body_, JPH_Vec3* force);
void JPH_Body_ResetForce(JPH_Body* body_);
void JPH_Body_ResetTorque(JPH_Body* body_);
void JPH_Body_ResetMotion(JPH_Body* body_);

void JPH_Body_GetInverseInertia(JPH_Body* body_, JPH_Matrix4x4* result);

void JPH_Body_AddImpulse(JPH_Body* body_, const(JPH_Vec3)* impulse);
void JPH_Body_AddImpulseAtPosition(JPH_Body* body_, const(JPH_Vec3)* impulse, const(JPH_RVec3)* position);
void JPH_Body_AddAngularImpulse(JPH_Body* body_, const(JPH_Vec3)* angularImpulse);
void JPH_Body_MoveKinematic(JPH_Body* body_, JPH_RVec3* targetPosition, JPH_Quat* targetRotation, float deltaTime);
bool JPH_Body_ApplyBuoyancyImpulse(JPH_Body* body_, const(JPH_RVec3)* surfacePosition, const(
        JPH_Vec3)* surfaceNormal, float buoyancy, float linearDrag, float angularDrag, const(
        JPH_Vec3)* fluidVelocity, const(JPH_Vec3)* gravity, float deltaTime);

bool JPH_Body_IsInBroadPhase(JPH_Body* body_);
bool JPH_Body_IsCollisionCacheInvalid(JPH_Body* body_);

const(JPH_Shape)* JPH_Body_GetShape(JPH_Body* body_);

void JPH_Body_GetPosition(const(JPH_Body)* body_, JPH_RVec3* result);
void JPH_Body_GetRotation(const(JPH_Body)* body_, JPH_Quat* result);
void JPH_Body_GetWorldTransform(const(JPH_Body)* body_, JPH_RMatrix4x4* result);
void JPH_Body_GetCenterOfMassPosition(const(JPH_Body)* body_, JPH_RVec3* result);
void JPH_Body_GetCenterOfMassTransform(const(JPH_Body)* body_, JPH_RMatrix4x4* result);
void JPH_Body_GetInverseCenterOfMassTransform(const(JPH_Body)* body_, JPH_RMatrix4x4* result);

void JPH_Body_GetWorldSpaceBounds(const(JPH_Body)* body_, JPH_AABox* result);
void JPH_Body_GetWorldSpaceSurfaceNormal(const(JPH_Body)* body_, JPH_SubShapeID subShapeID, const(
        JPH_RVec3)* position, JPH_Vec3* normal);

JPH_MotionProperties* JPH_Body_GetMotionProperties(JPH_Body* body_);
JPH_MotionProperties* JPH_Body_GetMotionPropertiesUnchecked(JPH_Body* body_);

void JPH_Body_SetUserData(JPH_Body* body_, ulong userData);
ulong JPH_Body_GetUserData(JPH_Body* body_);

JPH_Body* JPH_Body_GetFixedToWorldBody();

/* JPH_BroadPhaseLayerFilter_Procs */
struct JPH_BroadPhaseLayerFilter_Procs
{
    bool function(void* userData, JPH_BroadPhaseLayer layer) ShouldCollide;
}

void JPH_BroadPhaseLayerFilter_SetProcs(const(JPH_BroadPhaseLayerFilter_Procs)* procs);
JPH_BroadPhaseLayerFilter* JPH_BroadPhaseLayerFilter_Create(void* userData);
void JPH_BroadPhaseLayerFilter_Destroy(JPH_BroadPhaseLayerFilter* filter);

/* JPH_ObjectLayerFilter */
struct JPH_ObjectLayerFilter_Procs
{
    bool function(void* userData, JPH_ObjectLayer layer) ShouldCollide;
}

void JPH_ObjectLayerFilter_SetProcs(const(JPH_ObjectLayerFilter_Procs)* procs);
JPH_ObjectLayerFilter* JPH_ObjectLayerFilter_Create(void* userData);
void JPH_ObjectLayerFilter_Destroy(JPH_ObjectLayerFilter* filter);

/* JPH_BodyFilter */
struct JPH_BodyFilter_Procs
{
    bool function(void* userData, JPH_BodyID bodyID) ShouldCollide;
    bool function(void* userData, const(JPH_Body)* bodyID) ShouldCollideLocked;
}

void JPH_BodyFilter_SetProcs(const(JPH_BodyFilter_Procs)* procs);
JPH_BodyFilter* JPH_BodyFilter_Create(void* userData);
void JPH_BodyFilter_Destroy(JPH_BodyFilter* filter);

/* JPH_ShapeFilter */
struct JPH_ShapeFilter_Procs
{
    bool function(void* userData, const(JPH_Shape)* shape2, const(JPH_SubShapeID)* subShapeIDOfShape2) ShouldCollide;
    bool function(void* userData, const(JPH_Shape)* shape1, const(JPH_SubShapeID)* subShapeIDOfShape1, const(
            JPH_Shape)* shape2, const(JPH_SubShapeID)* subShapeIDOfShape2) ShouldCollide2;
}

void JPH_ShapeFilter_SetProcs(const(JPH_ShapeFilter_Procs)* procs);
JPH_ShapeFilter* JPH_ShapeFilter_Create(void* userData);
void JPH_ShapeFilter_Destroy(JPH_ShapeFilter* filter);
JPH_BodyID JPH_ShapeFilter_GetBodyID2(JPH_ShapeFilter* filter);
void JPH_ShapeFilter_SetBodyID2(JPH_ShapeFilter* filter, JPH_BodyID id);

/* JPH_SimShapeFilter */
struct JPH_SimShapeFilter_Procs
{
    bool function(
        void* userData,
        const(JPH_Body)* body1,
        const(JPH_Shape)* shape1,
        const(JPH_SubShapeID)* subShapeIDOfShape1,
        const(JPH_Body)* body2,
        const(JPH_Shape)* shape2,
        const(JPH_SubShapeID)* subShapeIDOfShape2) ShouldCollide;
}

void JPH_SimShapeFilter_SetProcs(const(JPH_SimShapeFilter_Procs)* procs);
JPH_SimShapeFilter* JPH_SimShapeFilter_Create(void* userData);
void JPH_SimShapeFilter_Destroy(JPH_SimShapeFilter* filter);

/* Contact listener */
struct JPH_ContactListener_Procs
{
    JPH_ValidateResult function(
        void* userData,
        const(JPH_Body)* body1,
        const(JPH_Body)* body2,
        const(JPH_RVec3)* baseOffset,
        const(JPH_CollideShapeResult)* collisionResult) OnContactValidate;

    void function(
        void* userData,
        const(JPH_Body)* body1,
        const(JPH_Body)* body2,
        const(JPH_ContactManifold)* manifold,
        JPH_ContactSettings* settings) OnContactAdded;

    void function(
        void* userData,
        const(JPH_Body)* body1,
        const(JPH_Body)* body2,
        const(JPH_ContactManifold)* manifold,
        JPH_ContactSettings* settings) OnContactPersisted;

    void function(
        void* userData,
        const(JPH_SubShapeIDPair)* subShapePair) OnContactRemoved;
}

void JPH_ContactListener_SetProcs(const(JPH_ContactListener_Procs)* procs);
JPH_ContactListener* JPH_ContactListener_Create(void* userData);
void JPH_ContactListener_Destroy(JPH_ContactListener* listener);

/* BodyActivationListener */
struct JPH_BodyActivationListener_Procs
{
    void function(void* userData, JPH_BodyID bodyID, ulong bodyUserData) OnBodyActivated;
    void function(void* userData, JPH_BodyID bodyID, ulong bodyUserData) OnBodyDeactivated;
}

void JPH_BodyActivationListener_SetProcs(const(JPH_BodyActivationListener_Procs)* procs);
JPH_BodyActivationListener* JPH_BodyActivationListener_Create(void* userData);
void JPH_BodyActivationListener_Destroy(JPH_BodyActivationListener* listener);

/* JPH_BodyDrawFilter */
struct JPH_BodyDrawFilter_Procs
{
    bool function(void* userData, const(JPH_Body)* body_) ShouldDraw;
}

void JPH_BodyDrawFilter_SetProcs(const(JPH_BodyDrawFilter_Procs)* procs);
JPH_BodyDrawFilter* JPH_BodyDrawFilter_Create(void* userData);
void JPH_BodyDrawFilter_Destroy(JPH_BodyDrawFilter* filter);

/* ContactManifold */
void JPH_ContactManifold_GetWorldSpaceNormal(const(JPH_ContactManifold)* manifold, JPH_Vec3* result);
float JPH_ContactManifold_GetPenetrationDepth(const(JPH_ContactManifold)* manifold);
JPH_SubShapeID JPH_ContactManifold_GetSubShapeID1(const(JPH_ContactManifold)* manifold);
JPH_SubShapeID JPH_ContactManifold_GetSubShapeID2(const(JPH_ContactManifold)* manifold);
uint JPH_ContactManifold_GetPointCount(const(JPH_ContactManifold)* manifold);
void JPH_ContactManifold_GetWorldSpaceContactPointOn1(
    const(JPH_ContactManifold)* manifold, uint index, JPH_RVec3* result);
void JPH_ContactManifold_GetWorldSpaceContactPointOn2(
    const(JPH_ContactManifold)* manifold, uint index, JPH_RVec3* result);

/* ContactSettings */
float JPH_ContactSettings_GetFriction(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetFriction(JPH_ContactSettings* settings, float friction);
float JPH_ContactSettings_GetRestitution(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetRestitution(JPH_ContactSettings* settings, float restitution);
float JPH_ContactSettings_GetInvMassScale1(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetInvMassScale1(JPH_ContactSettings* settings, float scale);
float JPH_ContactSettings_GetInvInertiaScale1(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetInvInertiaScale1(JPH_ContactSettings* settings, float scale);
float JPH_ContactSettings_GetInvMassScale2(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetInvMassScale2(JPH_ContactSettings* settings, float scale);
float JPH_ContactSettings_GetInvInertiaScale2(JPH_ContactSettings* settings);
void JPH_ContactSettings_SetInvInertiaScale2(JPH_ContactSettings* settings, float scale);
bool JPH_ContactSettings_GetIsSensor(const(JPH_ContactSettings)* settings);
void JPH_ContactSettings_SetIsSensor(JPH_ContactSettings* settings, bool sensor);
void JPH_ContactSettings_GetRelativeLinearSurfaceVelocity(
    JPH_ContactSettings* settings, JPH_Vec3* result);
void JPH_ContactSettings_SetRelativeLinearSurfaceVelocity(
    JPH_ContactSettings* settings, JPH_Vec3* velocity);
void JPH_ContactSettings_GetRelativeAngularSurfaceVelocity(
    JPH_ContactSettings* settings, JPH_Vec3* result);
void JPH_ContactSettings_SetRelativeAngularSurfaceVelocity(
    JPH_ContactSettings* settings, JPH_Vec3* velocity);

/* CharacterBase */
void JPH_CharacterBase_Destroy(JPH_CharacterBase* character);
float JPH_CharacterBase_GetCosMaxSlopeAngle(JPH_CharacterBase* character);
void JPH_CharacterBase_SetMaxSlopeAngle(JPH_CharacterBase* character, float maxSlopeAngle);
void JPH_CharacterBase_GetUp(JPH_CharacterBase* character, JPH_Vec3* result);
void JPH_CharacterBase_SetUp(JPH_CharacterBase* character, const(JPH_Vec3)* value);
bool JPH_CharacterBase_IsSlopeTooSteep(JPH_CharacterBase* character, const(JPH_Vec3)* value);
const(JPH_Shape)* JPH_CharacterBase_GetShape(JPH_CharacterBase* character);

JPH_GroundState JPH_CharacterBase_GetGroundState(JPH_CharacterBase* character);
bool JPH_CharacterBase_IsSupported(JPH_CharacterBase* character);
void JPH_CharacterBase_GetGroundPosition(JPH_CharacterBase* character, JPH_RVec3* position);
void JPH_CharacterBase_GetGroundNormal(JPH_CharacterBase* character, JPH_Vec3* normal);
void JPH_CharacterBase_GetGroundVelocity(JPH_CharacterBase* character, JPH_Vec3* velocity);
const(JPH_PhysicsMaterial)* JPH_CharacterBase_GetGroundMaterial(JPH_CharacterBase* character);
JPH_BodyID JPH_CharacterBase_GetGroundBodyId(JPH_CharacterBase* character);
JPH_SubShapeID JPH_CharacterBase_GetGroundSubShapeId(JPH_CharacterBase* character);
ulong JPH_CharacterBase_GetGroundUserData(JPH_CharacterBase* character);

/* CharacterSettings */
void JPH_CharacterSettings_Init(JPH_CharacterSettings* settings);

/* Character */
JPH_Character* JPH_Character_Create(
    const(JPH_CharacterSettings)* settings,
    const(JPH_RVec3)* position,
    const(JPH_Quat)* rotation,
    ulong userData,
    JPH_PhysicsSystem* system); /*= JPH_ActivationActivate */  /* = true */
void JPH_Character_AddToPhysicsSystem(JPH_Character* character, JPH_Activation activationMode, bool lockBodies); /* = true */
void JPH_Character_RemoveFromPhysicsSystem(JPH_Character* character, bool lockBodies); /* = true */
void JPH_Character_Activate(JPH_Character* character, bool lockBodies); /* = true */
void JPH_Character_PostSimulation(JPH_Character* character, float maxSeparationDistance, bool lockBodies); /* = true */
void JPH_Character_SetLinearAndAngularVelocity(JPH_Character* character, JPH_Vec3* linearVelocity, JPH_Vec3* angularVelocity, bool lockBodies);
void JPH_Character_GetLinearVelocity(JPH_Character* character, JPH_Vec3* result); /* = true */
void JPH_Character_SetLinearVelocity(JPH_Character* character, const(JPH_Vec3)* value, bool lockBodies); /* = true */
void JPH_Character_AddLinearVelocity(JPH_Character* character, const(JPH_Vec3)* value, bool lockBodies); /* = true */
void JPH_Character_AddImpulse(JPH_Character* character, const(JPH_Vec3)* value, bool lockBodies);
JPH_BodyID JPH_Character_GetBodyID(const(JPH_Character)* character); /* = true */
void JPH_Character_GetPositionAndRotation(JPH_Character* character, JPH_RVec3* position, JPH_Quat* rotation, bool lockBodies); /* = true */
void JPH_Character_SetPositionAndRotation(JPH_Character* character, const(JPH_RVec3)* position, const(
        JPH_Quat)* rotation, JPH_Activation activationMode, bool lockBodies); /* = true */
void JPH_Character_GetPosition(JPH_Character* character, JPH_RVec3* position, bool lockBodies); /* = true */
void JPH_Character_SetPosition(JPH_Character* character, const(JPH_RVec3)* position, JPH_Activation activationMode, bool lockBodies); /* = true */
void JPH_Character_GetRotation(JPH_Character* character, JPH_Quat* rotation, bool lockBodies); /* = true */
void JPH_Character_SetRotation(JPH_Character* character, const(JPH_Quat)* rotation, JPH_Activation activationMode, bool lockBodies); /* = true */
void JPH_Character_GetCenterOfMassPosition(JPH_Character* character, JPH_RVec3* result, bool lockBodies); /* = true */
void JPH_Character_GetWorldTransform(JPH_Character* character, JPH_RMatrix4x4* result, bool lockBodies);
JPH_ObjectLayer JPH_Character_GetLayer(const(JPH_Character)* character); /*= true*/
void JPH_Character_SetLayer(JPH_Character* character, JPH_ObjectLayer value, bool lockBodies); /*= true*/
void JPH_Character_SetShape(JPH_Character* character, const(JPH_Shape)* shape, float maxPenetrationDepth, bool lockBodies);

/* CharacterVirtualSettings */
void JPH_CharacterVirtualSettings_Init(JPH_CharacterVirtualSettings* settings);

/* CharacterVirtual */
JPH_CharacterVirtual* JPH_CharacterVirtual_Create(
    const(JPH_CharacterVirtualSettings)* settings,
    const(JPH_RVec3)* position,
    const(JPH_Quat)* rotation,
    ulong userData,
    JPH_PhysicsSystem* system);

JPH_CharacterID JPH_CharacterVirtual_GetID(const(JPH_CharacterVirtual)* character);
void JPH_CharacterVirtual_SetListener(JPH_CharacterVirtual* character, JPH_CharacterContactListener* listener);
void JPH_CharacterVirtual_SetCharacterVsCharacterCollision(JPH_CharacterVirtual* character, JPH_CharacterVsCharacterCollision* characterVsCharacterCollision);

void JPH_CharacterVirtual_GetLinearVelocity(JPH_CharacterVirtual* character, JPH_Vec3* velocity);
void JPH_CharacterVirtual_SetLinearVelocity(JPH_CharacterVirtual* character, const(JPH_Vec3)* velocity);
void JPH_CharacterVirtual_GetPosition(JPH_CharacterVirtual* character, JPH_RVec3* position);
void JPH_CharacterVirtual_SetPosition(JPH_CharacterVirtual* character, const(JPH_RVec3)* position);
void JPH_CharacterVirtual_GetRotation(JPH_CharacterVirtual* character, JPH_Quat* rotation);
void JPH_CharacterVirtual_SetRotation(JPH_CharacterVirtual* character, const(JPH_Quat)* rotation);
void JPH_CharacterVirtual_GetWorldTransform(JPH_CharacterVirtual* character, JPH_RMatrix4x4* result);
void JPH_CharacterVirtual_GetCenterOfMassTransform(JPH_CharacterVirtual* character, JPH_RMatrix4x4* result);
float JPH_CharacterVirtual_GetMass(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetMass(JPH_CharacterVirtual* character, float value);
float JPH_CharacterVirtual_GetMaxStrength(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetMaxStrength(JPH_CharacterVirtual* character, float value);

float JPH_CharacterVirtual_GetPenetrationRecoverySpeed(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetPenetrationRecoverySpeed(JPH_CharacterVirtual* character, float value);
bool JPH_CharacterVirtual_GetEnhancedInternalEdgeRemoval(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetEnhancedInternalEdgeRemoval(JPH_CharacterVirtual* character, bool value);
float JPH_CharacterVirtual_GetCharacterPadding(JPH_CharacterVirtual* character);
uint JPH_CharacterVirtual_GetMaxNumHits(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetMaxNumHits(JPH_CharacterVirtual* character, uint value);
float JPH_CharacterVirtual_GetHitReductionCosMaxAngle(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_SetHitReductionCosMaxAngle(JPH_CharacterVirtual* character, float value);
bool JPH_CharacterVirtual_GetMaxHitsExceeded(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_GetShapeOffset(JPH_CharacterVirtual* character, JPH_Vec3* result);
void JPH_CharacterVirtual_SetShapeOffset(JPH_CharacterVirtual* character, const(JPH_Vec3)* value);
ulong JPH_CharacterVirtual_GetUserData(const(JPH_CharacterVirtual)* character);
void JPH_CharacterVirtual_SetUserData(JPH_CharacterVirtual* character, ulong value);
JPH_BodyID JPH_CharacterVirtual_GetInnerBodyID(const(JPH_CharacterVirtual)* character);

void JPH_CharacterVirtual_CancelVelocityTowardsSteepSlopes(JPH_CharacterVirtual* character, const(
        JPH_Vec3)* desiredVelocity, JPH_Vec3* velocity);
void JPH_CharacterVirtual_StartTrackingContactChanges(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_FinishTrackingContactChanges(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_Update(JPH_CharacterVirtual* character, float deltaTime, JPH_ObjectLayer layer, JPH_PhysicsSystem* system, const(
        JPH_BodyFilter)* bodyFilter, const(JPH_ShapeFilter)* shapeFilter);

void JPH_CharacterVirtual_ExtendedUpdate(
    JPH_CharacterVirtual* character,
    float deltaTime,
    const(JPH_ExtendedUpdateSettings)* settings,
    JPH_ObjectLayer layer,
    JPH_PhysicsSystem* system,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);
void JPH_CharacterVirtual_RefreshContacts(JPH_CharacterVirtual* character, JPH_ObjectLayer layer, JPH_PhysicsSystem* system, const(
        JPH_BodyFilter)* bodyFilter, const(JPH_ShapeFilter)* shapeFilter);

bool JPH_CharacterVirtual_CanWalkStairs(JPH_CharacterVirtual* character, const(JPH_Vec3)* linearVelocity);
bool JPH_CharacterVirtual_WalkStairs(
    JPH_CharacterVirtual* character,
    float deltaTime,
    const(JPH_Vec3)* stepUp,
    const(JPH_Vec3)* stepForward,
    const(JPH_Vec3)* stepForwardTest,
    const(JPH_Vec3)* stepDownExtra,
    JPH_ObjectLayer layer,
    JPH_PhysicsSystem* system,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_CharacterVirtual_StickToFloor(
    JPH_CharacterVirtual* character,
    const(JPH_Vec3)* stepDown,
    JPH_ObjectLayer layer,
    JPH_PhysicsSystem* system,
    const(JPH_BodyFilter)* bodyFilter,
    const(JPH_ShapeFilter)* shapeFilter);

void JPH_CharacterVirtual_UpdateGroundVelocity(JPH_CharacterVirtual* character);
bool JPH_CharacterVirtual_SetShape(JPH_CharacterVirtual* character, const(JPH_Shape)* shape, float maxPenetrationDepth, JPH_ObjectLayer layer, JPH_PhysicsSystem* system, const(
        JPH_BodyFilter)* bodyFilter, const(JPH_ShapeFilter)* shapeFilter);
void JPH_CharacterVirtual_SetInnerBodyShape(JPH_CharacterVirtual* character, const(JPH_Shape)* shape);

uint JPH_CharacterVirtual_GetNumActiveContacts(JPH_CharacterVirtual* character);
void JPH_CharacterVirtual_GetActiveContact(JPH_CharacterVirtual* character, uint index, JPH_CharacterVirtualContact* result);

bool JPH_CharacterVirtual_HasCollidedWithBody(JPH_CharacterVirtual* character, const JPH_BodyID body_);
bool JPH_CharacterVirtual_HasCollidedWith(JPH_CharacterVirtual* character, const JPH_CharacterID other);
bool JPH_CharacterVirtual_HasCollidedWithCharacter(JPH_CharacterVirtual* character, const(
        JPH_CharacterVirtual)* other);

/* CharacterContactListener */
struct JPH_CharacterContactListener_Procs
{
    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_Body)* body2,
        JPH_Vec3* ioLinearVelocity,
        JPH_Vec3* ioAngularVelocity) OnAdjustBodyVelocity;

    bool function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_BodyID bodyID2,
        const JPH_SubShapeID subShapeID2) OnContactValidate;

    bool function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_CharacterVirtual)* otherCharacter,
        const JPH_SubShapeID subShapeID2) OnCharacterContactValidate;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_BodyID bodyID2,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        JPH_CharacterContactSettings* ioSettings) OnContactAdded;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_BodyID bodyID2,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        JPH_CharacterContactSettings* ioSettings) OnContactPersisted;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_BodyID bodyID2,
        const JPH_SubShapeID subShapeID2) OnContactRemoved;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_CharacterVirtual)* otherCharacter,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        JPH_CharacterContactSettings* ioSettings) OnCharacterContactAdded;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_CharacterVirtual)* otherCharacter,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        JPH_CharacterContactSettings* ioSettings) OnCharacterContactPersisted;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_CharacterID otherCharacterID,
        const JPH_SubShapeID subShapeID2) OnCharacterContactRemoved;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const JPH_BodyID bodyID2,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        const(JPH_Vec3)* contactVelocity,
        const(JPH_PhysicsMaterial)* contactMaterial,
        const(JPH_Vec3)* characterVelocity,
        JPH_Vec3* newCharacterVelocity) OnContactSolve;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_CharacterVirtual)* otherCharacter,
        const JPH_SubShapeID subShapeID2,
        const(JPH_RVec3)* contactPosition,
        const(JPH_Vec3)* contactNormal,
        const(JPH_Vec3)* contactVelocity,
        const(JPH_PhysicsMaterial)* contactMaterial,
        const(JPH_Vec3)* characterVelocity,
        JPH_Vec3* newCharacterVelocity) OnCharacterContactSolve;
}

void JPH_CharacterContactListener_SetProcs(const(JPH_CharacterContactListener_Procs)* procs);
JPH_CharacterContactListener* JPH_CharacterContactListener_Create(void* userData);
void JPH_CharacterContactListener_Destroy(JPH_CharacterContactListener* listener);

/* JPH_CharacterVsCharacterCollision */
struct JPH_CharacterVsCharacterCollision_Procs
{
    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_RMatrix4x4)* centerOfMassTransform,
        const(JPH_CollideShapeSettings)* collideShapeSettings,
        const(JPH_RVec3)* baseOffset) CollideCharacter;

    void function(
        void* userData,
        const(JPH_CharacterVirtual)* character,
        const(JPH_RMatrix4x4)* centerOfMassTransform,
        const(JPH_Vec3)* direction,
        const(JPH_ShapeCastSettings)* shapeCastSettings,
        const(JPH_RVec3)* baseOffset) CastCharacter;
}

void JPH_CharacterVsCharacterCollision_SetProcs(
    const(JPH_CharacterVsCharacterCollision_Procs)* procs);
JPH_CharacterVsCharacterCollision* JPH_CharacterVsCharacterCollision_Create(void* userData);
JPH_CharacterVsCharacterCollision* JPH_CharacterVsCharacterCollision_CreateSimple();
void JPH_CharacterVsCharacterCollisionSimple_AddCharacter(
    JPH_CharacterVsCharacterCollision* characterVsCharacter, JPH_CharacterVirtual* character);
void JPH_CharacterVsCharacterCollisionSimple_RemoveCharacter(
    JPH_CharacterVsCharacterCollision* characterVsCharacter, JPH_CharacterVirtual* character);
void JPH_CharacterVsCharacterCollision_Destroy(JPH_CharacterVsCharacterCollision* listener);

/* CollisionDispatch */
bool JPH_CollisionDispatch_CollideShapeVsShape(
    const(JPH_Shape)* shape1,
    const(JPH_Shape)* shape2,
    const(JPH_Vec3)* scale1,
    const(JPH_Vec3)* scale2,
    const(JPH_Matrix4x4)* centerOfMassTransform1,
    const(JPH_Matrix4x4)* centerOfMassTransform2,
    const(JPH_CollideShapeSettings)* collideShapeSettings,
    float function() callback,
    void* userData,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_CollisionDispatch_CastShapeVsShapeLocalSpace(
    const(JPH_Vec3)* direction,
    const(JPH_Shape)* shape1,
    const(JPH_Shape)* shape2,
    const(JPH_Vec3)* scale1InShape2LocalSpace,
    const(JPH_Vec3)* scale2,
    JPH_Matrix4x4* centerOfMassTransform1InShape2LocalSpace,
    JPH_Matrix4x4* centerOfMassWorldTransform2,
    const(JPH_ShapeCastSettings)* shapeCastSettings,
    float function() callback,
    void* userData,
    const(JPH_ShapeFilter)* shapeFilter);

bool JPH_CollisionDispatch_CastShapeVsShapeWorldSpace(
    const(JPH_Vec3)* direction,
    const(JPH_Shape)* shape1,
    const(JPH_Shape)* shape2,
    const(JPH_Vec3)* scale1,
    const(JPH_Vec3)* inScale2,
    const(JPH_Matrix4x4)* centerOfMassWorldTransform1,
    const(JPH_Matrix4x4)* centerOfMassWorldTransform2,
    const(JPH_ShapeCastSettings)* shapeCastSettings,
    float function() callback,
    void* userData,
    const(JPH_ShapeFilter)* shapeFilter);

/* DebugRenderer */
struct JPH_DebugRenderer_Procs
{
    void function(void* userData, const(JPH_RVec3)* from, const(JPH_RVec3)* to, JPH_Color color) DrawLine;
    void function(void* userData, const(JPH_RVec3)* v1, const(JPH_RVec3)* v2, const(JPH_RVec3)* v3, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow) DrawTriangle;
    void function(void* userData, const(JPH_RVec3)* position, const(char)* str, JPH_Color color, float height) DrawText3D;
}

void JPH_DebugRenderer_SetProcs(const(JPH_DebugRenderer_Procs)* procs);
JPH_DebugRenderer* JPH_DebugRenderer_Create(void* userData);
void JPH_DebugRenderer_Destroy(JPH_DebugRenderer* renderer);
void JPH_DebugRenderer_NextFrame(JPH_DebugRenderer* renderer);
void JPH_DebugRenderer_SetCameraPos(JPH_DebugRenderer* renderer, const(JPH_RVec3)* position);

void JPH_DebugRenderer_DrawLine(JPH_DebugRenderer* renderer, const(JPH_RVec3)* from, const(
        JPH_RVec3)* to, JPH_Color color);
void JPH_DebugRenderer_DrawWireBox(JPH_DebugRenderer* renderer, const(JPH_AABox)* box, JPH_Color color);
void JPH_DebugRenderer_DrawWireBox2(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, const(
        JPH_AABox)* box, JPH_Color color);
void JPH_DebugRenderer_DrawMarker(JPH_DebugRenderer* renderer, const(JPH_RVec3)* position, JPH_Color color, float size);
void JPH_DebugRenderer_DrawArrow(JPH_DebugRenderer* renderer, const(JPH_RVec3)* from, const(
        JPH_RVec3)* to, JPH_Color color, float size);
void JPH_DebugRenderer_DrawCoordinateSystem(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, float size);
void JPH_DebugRenderer_DrawPlane(JPH_DebugRenderer* renderer, const(JPH_RVec3)* point, const(
        JPH_Vec3)* normal, JPH_Color color, float size);
void JPH_DebugRenderer_DrawWireTriangle(JPH_DebugRenderer* renderer, const(JPH_RVec3)* v1, const(
        JPH_RVec3)* v2, const(JPH_RVec3)* v3, JPH_Color color);
void JPH_DebugRenderer_DrawWireSphere(JPH_DebugRenderer* renderer, const(JPH_RVec3)* center, float radius, JPH_Color color, int level);
void JPH_DebugRenderer_DrawWireUnitSphere(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, JPH_Color color, int level);
void JPH_DebugRenderer_DrawTriangle(JPH_DebugRenderer* renderer, const(JPH_RVec3)* v1, const(JPH_RVec3)* v2, const(
        JPH_RVec3)* v3, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow);
void JPH_DebugRenderer_DrawBox(JPH_DebugRenderer* renderer, const(JPH_AABox)* box, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawBox2(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, const(JPH_AABox)* box, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawSphere(JPH_DebugRenderer* renderer, const(JPH_RVec3)* center, float radius, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawUnitSphere(JPH_DebugRenderer* renderer, JPH_RMatrix4x4 matrix, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawCapsule(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, float halfHeightOfCylinder, float radius, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawCylinder(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, float halfHeight, float radius, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawOpenCone(JPH_DebugRenderer* renderer, const(JPH_RVec3)* top, const(
        JPH_Vec3)* axis, const(JPH_Vec3)* perpendicular, float halfAngle, float length, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawSwingConeLimits(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, float swingYHalfAngle, float swingZHalfAngle, float edgeLength, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawSwingPyramidLimits(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* matrix, float minSwingYAngle, float maxSwingYAngle, float minSwingZAngle, float maxSwingZAngle, float edgeLength, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawPie(JPH_DebugRenderer* renderer, const(JPH_RVec3)* center, float radius, const(JPH_Vec3)* normal, const(JPH_Vec3)* axis, float minAngle, float maxAngle, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);
void JPH_DebugRenderer_DrawTaperedCylinder(JPH_DebugRenderer* renderer, const(JPH_RMatrix4x4)* inMatrix, float top, float bottom, float topRadius, float bottomRadius, JPH_Color color, JPH_DebugRenderer_CastShadow castShadow, JPH_DebugRenderer_DrawMode drawMode);

/* Skeleton */
struct JPH_SkeletonJoint
{
    const(char)* name;
    const(char)* parentName;
    int parentJointIndex;
}

JPH_Skeleton* JPH_Skeleton_Create();
void JPH_Skeleton_Destroy(JPH_Skeleton* skeleton);

uint JPH_Skeleton_AddJoint(JPH_Skeleton* skeleton, const(char)* name);
uint JPH_Skeleton_AddJoint2(JPH_Skeleton* skeleton, const(char)* name, int parentIndex);
uint JPH_Skeleton_AddJoint3(JPH_Skeleton* skeleton, const(char)* name, const(char)* parentName);
int JPH_Skeleton_GetJointCount(const(JPH_Skeleton)* skeleton);
void JPH_Skeleton_GetJoint(const(JPH_Skeleton)* skeleton, int index, JPH_SkeletonJoint* joint);
int JPH_Skeleton_GetJointIndex(const(JPH_Skeleton)* skeleton, const(char)* name);
void JPH_Skeleton_CalculateParentJointIndices(JPH_Skeleton* skeleton);
bool JPH_Skeleton_AreJointsCorrectlyOrdered(const(JPH_Skeleton)* skeleton);

/* Ragdoll */
JPH_RagdollSettings* JPH_RagdollSettings_Create();
void JPH_RagdollSettings_Destroy(JPH_RagdollSettings* settings);

const(JPH_Skeleton)* JPH_RagdollSettings_GetSkeleton(const(JPH_RagdollSettings)* character);
void JPH_RagdollSettings_SetSkeleton(JPH_RagdollSettings* character, JPH_Skeleton* skeleton);
bool JPH_RagdollSettings_Stabilize(JPH_RagdollSettings* settings); /*=nullptr*/  /* = 0.0f*/
void JPH_RagdollSettings_DisableParentChildCollisions(JPH_RagdollSettings* settings, const(
        JPH_Matrix4x4)* jointMatrices, float minSeparationDistance);
void JPH_RagdollSettings_CalculateBodyIndexToConstraintIndex(JPH_RagdollSettings* settings);
int JPH_RagdollSettings_GetConstraintIndexForBodyIndex(JPH_RagdollSettings* settings, int bodyIndex);
void JPH_RagdollSettings_CalculateConstraintIndexToBodyIdxPair(JPH_RagdollSettings* settings); /*=0*/  /* = 0*/
JPH_Ragdoll* JPH_RagdollSettings_CreateRagdoll(JPH_RagdollSettings* settings, JPH_PhysicsSystem* system, JPH_CollisionGroupID collisionGroup, ulong userData);
void JPH_Ragdoll_Destroy(JPH_Ragdoll* ragdoll); /*= JPH_ActivationActivate */  /* = true */
void JPH_Ragdoll_AddToPhysicsSystem(JPH_Ragdoll* ragdoll, JPH_Activation activationMode, bool lockBodies); /* = true */
void JPH_Ragdoll_RemoveFromPhysicsSystem(JPH_Ragdoll* ragdoll, bool lockBodies); /* = true */
void JPH_Ragdoll_Activate(JPH_Ragdoll* ragdoll, bool lockBodies); /* = true */
bool JPH_Ragdoll_IsActive(const(JPH_Ragdoll)* ragdoll, bool lockBodies);
void JPH_Ragdoll_ResetWarmStart(JPH_Ragdoll* ragdoll);

/* JPH_EstimateCollisionResponse */
void JPH_EstimateCollisionResponse(const(JPH_Body)* body1, const(JPH_Body)* body2, const(JPH_ContactManifold)* manifold, float combinedFriction, float combinedRestitution, float minVelocityForRestitution, uint numIterations, JPH_CollisionEstimationResult* result);

/* Vehicle */
struct JPH_WheelSettings;
struct JPH_WheelSettingsWV; /* Inherits JPH_WheelSettings */
struct JPH_WheelSettingsTV; /* Inherits JPH_WheelSettings */

struct JPH_Wheel;
struct JPH_WheelWV; /* Inherits JPH_Wheel */
struct JPH_WheelTV; /* Inherits JPH_Wheel */

struct JPH_VehicleTransmissionSettings;
struct JPH_VehicleCollisionTester;
struct JPH_VehicleCollisionTesterRay; /* Inherits JPH_VehicleCollisionTester */
struct JPH_VehicleCollisionTesterCastSphere; /* Inherits JPH_VehicleCollisionTester */
struct JPH_VehicleCollisionTesterCastCylinder; /* Inherits JPH_VehicleCollisionTester */
struct JPH_VehicleConstraint; /* Inherits JPH_Constraint */

struct JPH_VehicleControllerSettings;
struct JPH_WheeledVehicleControllerSettings; /* Inherits JPH_VehicleControllerSettings */
struct JPH_MotorcycleControllerSettings; /* Inherits JPH_WheeledVehicleControllerSettings */
struct JPH_TrackedVehicleControllerSettings; /* Inherits JPH_VehicleControllerSettings */

struct JPH_WheeledVehicleController; /* Inherits JPH_VehicleController */
struct JPH_MotorcycleController; /* Inherits JPH_WheeledVehicleController */
struct JPH_TrackedVehicleController; /* Inherits JPH_VehicleController */

struct JPH_VehicleController;

struct JPH_VehicleAntiRollBar
{
    int leftWheel;
    int rightWheel;
    float stiffness;
}

struct JPH_VehicleConstraintSettings
{
    JPH_ConstraintSettings base; /* Inherits JPH_ConstraintSettings */

    JPH_Vec3 up;
    JPH_Vec3 forward;
    float maxPitchRollAngle;
    uint wheelsCount;
    JPH_WheelSettings** wheels;
    uint antiRollBarsCount;
    const(JPH_VehicleAntiRollBar)* antiRollBars;
    JPH_VehicleControllerSettings* controller;
}

struct JPH_VehicleEngineSettings
{
    float maxTorque;
    float minRPM;
    float maxRPM;
    //LinearCurve			normalizedTorque;
    float inertia;
    float angularDamping;
}

struct JPH_VehicleDifferentialSettings
{
    int leftWheel;
    int rightWheel;
    float differentialRatio;
    float leftRightSplit;
    float limitedSlipRatio;
    float engineTorqueRatio;
}

void JPH_VehicleConstraintSettings_Init(JPH_VehicleConstraintSettings* settings);

JPH_VehicleConstraint* JPH_VehicleConstraint_Create(JPH_Body* body_, const(
        JPH_VehicleConstraintSettings)* settings);
JPH_PhysicsStepListener* JPH_VehicleConstraint_AsPhysicsStepListener(
    JPH_VehicleConstraint* constraint);

void JPH_VehicleConstraint_SetMaxPitchRollAngle(JPH_VehicleConstraint* constraint, float maxPitchRollAngle);
void JPH_VehicleConstraint_SetVehicleCollisionTester(
    JPH_VehicleConstraint* constraint, const(JPH_VehicleCollisionTester)* tester);

void JPH_VehicleConstraint_OverrideGravity(JPH_VehicleConstraint* constraint, const(JPH_Vec3)* value);
bool JPH_VehicleConstraint_IsGravityOverridden(const(JPH_VehicleConstraint)* constraint);
void JPH_VehicleConstraint_GetGravityOverride(
    const(JPH_VehicleConstraint)* constraint, JPH_Vec3* result);
void JPH_VehicleConstraint_ResetGravityOverride(JPH_VehicleConstraint* constraint);

void JPH_VehicleConstraint_GetLocalForward(
    const(JPH_VehicleConstraint)* constraint, JPH_Vec3* result);
void JPH_VehicleConstraint_GetLocalUp(const(JPH_VehicleConstraint)* constraint, JPH_Vec3* result);
void JPH_VehicleConstraint_GetWorldUp(const(JPH_VehicleConstraint)* constraint, JPH_Vec3* result);

const(JPH_Body)* JPH_VehicleConstraint_GetVehicleBody(const(JPH_VehicleConstraint)* constraint);
JPH_VehicleController* JPH_VehicleConstraint_GetController(JPH_VehicleConstraint* constraint);
uint JPH_VehicleConstraint_GetWheelsCount(JPH_VehicleConstraint* constraint);
JPH_Wheel* JPH_VehicleConstraint_GetWheel(JPH_VehicleConstraint* constraint, uint index);

/* Wheel */
JPH_WheelSettings* JPH_WheelSettings_Create();
void JPH_WheelSettings_Destroy(JPH_WheelSettings* settings);
void JPH_WheelSettings_GetPosition(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetPosition(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
void JPH_WheelSettings_GetSuspensionForcePoint(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetSuspensionForcePoint(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
void JPH_WheelSettings_GetSuspensionDirection(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetSuspensionDirection(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
void JPH_WheelSettings_GetSteeringAxis(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetSteeringAxis(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
void JPH_WheelSettings_GetWheelUp(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetWheelUp(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
void JPH_WheelSettings_GetWheelForward(const(JPH_WheelSettings)* settings, JPH_Vec3* result);
void JPH_WheelSettings_SetWheelForward(JPH_WheelSettings* settings, const(JPH_Vec3)* value);
float JPH_WheelSettings_GetSuspensionMinLength(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetSuspensionMinLength(JPH_WheelSettings* settings, float value);
float JPH_WheelSettings_GetSuspensionMaxLength(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetSuspensionMaxLength(JPH_WheelSettings* settings, float value);
float JPH_WheelSettings_GetSuspensionPreloadLength(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetSuspensionPreloadLength(JPH_WheelSettings* settings, float value);
void JPH_WheelSettings_GetSuspensionSpring(const(JPH_WheelSettings)* settings, JPH_SpringSettings* result);
void JPH_WheelSettings_SetSuspensionSpring(JPH_WheelSettings* settings, JPH_SpringSettings* springSettings);
float JPH_WheelSettings_GetRadius(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetRadius(JPH_WheelSettings* settings, float value);
float JPH_WheelSettings_GetWidth(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetWidth(JPH_WheelSettings* settings, float value);
bool JPH_WheelSettings_GetEnableSuspensionForcePoint(const(JPH_WheelSettings)* settings);
void JPH_WheelSettings_SetEnableSuspensionForcePoint(JPH_WheelSettings* settings, bool value);

JPH_Wheel* JPH_Wheel_Create(const(JPH_WheelSettings)* settings);
void JPH_Wheel_Destroy(JPH_Wheel* wheel);
float JPH_Wheel_GetAngularVelocity(const(JPH_Wheel)* wheel);
void JPH_Wheel_SetAngularVelocity(JPH_Wheel* wheel, float value);
float JPH_Wheel_GetRotationAngle(const(JPH_Wheel)* wheel);
void JPH_Wheel_SetRotationAngle(JPH_Wheel* wheel, float value);
float JPH_Wheel_GetSteerAngle(const(JPH_Wheel)* wheel);
void JPH_Wheel_SetSteerAngle(JPH_Wheel* wheel, float value);
bool JPH_Wheel_HasContact(const(JPH_Wheel)* wheel);
JPH_BodyID JPH_Wheel_GetContactBodyID(const(JPH_Wheel)* wheel);
JPH_SubShapeID JPH_Wheel_GetContactSubShapeID(const(JPH_Wheel)* wheel);
void JPH_Wheel_GetContactPosition(const(JPH_Wheel)* wheel, JPH_RVec3* result);
void JPH_Wheel_GetContactPointVelocity(const(JPH_Wheel)* wheel, JPH_Vec3* result);
void JPH_Wheel_GetContactNormal(const(JPH_Wheel)* wheel, JPH_Vec3* result);
void JPH_Wheel_GetContactLongitudinal(const(JPH_Wheel)* wheel, JPH_Vec3* result);
void JPH_Wheel_GetContactLateral(const(JPH_Wheel)* wheel, JPH_Vec3* result);
float JPH_Wheel_GetSuspensionLength(const(JPH_Wheel)* wheel);
float JPH_Wheel_GetSuspensionLambda(const(JPH_Wheel)* wheel);
float JPH_Wheel_GetLongitudinalLambda(const(JPH_Wheel)* wheel);
float JPH_Wheel_GetLateralLambda(const(JPH_Wheel)* wheel);
bool JPH_Wheel_HasHitHardPoint(const(JPH_Wheel)* wheel);

/* VehicleAntiRollBar */
void JPH_VehicleAntiRollBar_Init(JPH_VehicleAntiRollBar* antiRollBar);

/* VehicleEngine */
void JPH_VehicleEngineSettings_Init(JPH_VehicleEngineSettings* settings);

/* VehicleDifferentialSettings */
void JPH_VehicleDifferentialSettings_Init(JPH_VehicleDifferentialSettings* settings);

/* VehicleTransmission */
JPH_VehicleTransmissionSettings* JPH_VehicleTransmissionSettings_Create();
void JPH_VehicleTransmissionSettings_Destroy(JPH_VehicleTransmissionSettings* settings);

JPH_TransmissionMode JPH_VehicleTransmissionSettings_GetMode(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetMode(
    JPH_VehicleTransmissionSettings* settings, JPH_TransmissionMode value);

uint JPH_VehicleTransmissionSettings_GetGearRatioCount(
    const(JPH_VehicleTransmissionSettings)* settings);
float JPH_VehicleTransmissionSettings_GetGearRatio(
    const(JPH_VehicleTransmissionSettings)* settings, uint index);
void JPH_VehicleTransmissionSettings_SetGearRatio(
    JPH_VehicleTransmissionSettings* settings, uint index, float value);
const(float)* JPH_VehicleTransmissionSettings_GetGearRatios(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetGearRatios(
    JPH_VehicleTransmissionSettings* settings, const(float)* values, uint count);

uint JPH_VehicleTransmissionSettings_GetReverseGearRatioCount(
    const(JPH_VehicleTransmissionSettings)* settings);
float JPH_VehicleTransmissionSettings_GetReverseGearRatio(
    const(JPH_VehicleTransmissionSettings)* settings, uint index);
void JPH_VehicleTransmissionSettings_SetReverseGearRatio(
    JPH_VehicleTransmissionSettings* settings, uint index, float value);
const(float)* JPH_VehicleTransmissionSettings_GetReverseGearRatios(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetReverseGearRatios(
    JPH_VehicleTransmissionSettings* settings, const(float)* values, uint count);

float JPH_VehicleTransmissionSettings_GetSwitchTime(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetSwitchTime(
    JPH_VehicleTransmissionSettings* settings, float value);
float JPH_VehicleTransmissionSettings_GetClutchReleaseTime(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetClutchReleaseTime(
    JPH_VehicleTransmissionSettings* settings, float value);
float JPH_VehicleTransmissionSettings_GetSwitchLatency(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetSwitchLatency(
    JPH_VehicleTransmissionSettings* settings, float value);
float JPH_VehicleTransmissionSettings_GetShiftUpRPM(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetShiftUpRPM(
    JPH_VehicleTransmissionSettings* settings, float value);
float JPH_VehicleTransmissionSettings_GetShiftDownRPM(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetShiftDownRPM(
    JPH_VehicleTransmissionSettings* settings, float value);
float JPH_VehicleTransmissionSettings_GetClutchStrength(
    const(JPH_VehicleTransmissionSettings)* settings);
void JPH_VehicleTransmissionSettings_SetClutchStrength(
    JPH_VehicleTransmissionSettings* settings, float value);

/* VehicleColliionTester */
void JPH_VehicleCollisionTester_Destroy(JPH_VehicleCollisionTester* tester);
JPH_ObjectLayer JPH_VehicleCollisionTester_GetObjectLayer(const(JPH_VehicleCollisionTester)* tester);
void JPH_VehicleCollisionTester_SetObjectLayer(JPH_VehicleCollisionTester* tester, JPH_ObjectLayer value);

JPH_VehicleCollisionTesterRay* JPH_VehicleCollisionTesterRay_Create(
    JPH_ObjectLayer layer, const(JPH_Vec3)* up, float maxSlopeAngle);
JPH_VehicleCollisionTesterCastSphere* JPH_VehicleCollisionTesterCastSphere_Create(
    JPH_ObjectLayer layer, float radius, const(JPH_Vec3)* up, float maxSlopeAngle);
JPH_VehicleCollisionTesterCastCylinder* JPH_VehicleCollisionTesterCastCylinder_Create(
    JPH_ObjectLayer layer, float convexRadiusFraction);

/* VehicleControllerSettings/VehicleController */
void JPH_VehicleControllerSettings_Destroy(JPH_VehicleControllerSettings* settings);
const(JPH_VehicleConstraint)* JPH_VehicleController_GetConstraint(JPH_VehicleController* controller);

/* ---- WheelSettingsWV - WheelWV - WheeledVehicleController ---- */

JPH_WheelSettingsWV* JPH_WheelSettingsWV_Create();
float JPH_WheelSettingsWV_GetInertia(const(JPH_WheelSettingsWV)* settings);
void JPH_WheelSettingsWV_SetInertia(JPH_WheelSettingsWV* settings, float value);
float JPH_WheelSettingsWV_GetAngularDamping(const(JPH_WheelSettingsWV)* settings);
void JPH_WheelSettingsWV_SetAngularDamping(JPH_WheelSettingsWV* settings, float value);
float JPH_WheelSettingsWV_GetMaxSteerAngle(const(JPH_WheelSettingsWV)* settings);
void JPH_WheelSettingsWV_SetMaxSteerAngle(JPH_WheelSettingsWV* settings, float value);
//JPH_CAPI JPH_LinearCurve* JPH_WheelSettingsWV_GetLongitudinalFriction(const JPH_WheelSettingsWV* settings);
//JPH_CAPI void JPH_WheelSettingsWV_SetLongitudinalFriction(JPH_WheelSettingsWV* settings, const JPH_LinearCurve* value);
//JPH_CAPI JPH_LinearCurve* JPH_WheelSettingsWV_GetLateralFriction(const JPH_WheelSettingsWV* settings);
//JPH_CAPI void JPH_WheelSettingsWV_SetLateralFriction(JPH_WheelSettingsWV* settings, const JPH_LinearCurve* value);
float JPH_WheelSettingsWV_GetMaxBrakeTorque(const(JPH_WheelSettingsWV)* settings);
void JPH_WheelSettingsWV_SetMaxBrakeTorque(JPH_WheelSettingsWV* settings, float value);
float JPH_WheelSettingsWV_GetMaxHandBrakeTorque(const(JPH_WheelSettingsWV)* settings);
void JPH_WheelSettingsWV_SetMaxHandBrakeTorque(JPH_WheelSettingsWV* settings, float value);

JPH_WheelWV* JPH_WheelWV_Create(const(JPH_WheelSettingsWV)* settings);
const(JPH_WheelSettingsWV)* JPH_WheelWV_GetSettings(const(JPH_WheelWV)* wheel);
void JPH_WheelWV_ApplyTorque(JPH_WheelWV* wheel, float torque, float deltaTime);

JPH_WheeledVehicleControllerSettings* JPH_WheeledVehicleControllerSettings_Create();

void JPH_WheeledVehicleControllerSettings_GetEngine(const(
        JPH_WheeledVehicleControllerSettings)* settings, JPH_VehicleEngineSettings* result);
void JPH_WheeledVehicleControllerSettings_SetEngine(
    JPH_WheeledVehicleControllerSettings* settings, const(JPH_VehicleEngineSettings)* value);
const(JPH_VehicleTransmissionSettings)* JPH_WheeledVehicleControllerSettings_GetTransmission(
    const(JPH_WheeledVehicleControllerSettings)* settings);
void JPH_WheeledVehicleControllerSettings_SetTransmission(
    JPH_WheeledVehicleControllerSettings* settings, const(JPH_VehicleTransmissionSettings)* value);

uint JPH_WheeledVehicleControllerSettings_GetDifferentialsCount(
    const(JPH_WheeledVehicleControllerSettings)* settings);
void JPH_WheeledVehicleControllerSettings_SetDifferentialsCount(
    JPH_WheeledVehicleControllerSettings* settings, uint count);
void JPH_WheeledVehicleControllerSettings_GetDifferential(const(
        JPH_WheeledVehicleControllerSettings)* settings, uint index, JPH_VehicleDifferentialSettings* result);
void JPH_WheeledVehicleControllerSettings_SetDifferential(JPH_WheeledVehicleControllerSettings* settings, uint index, const(
        JPH_VehicleDifferentialSettings)* value);
void JPH_WheeledVehicleControllerSettings_SetDifferentials(JPH_WheeledVehicleControllerSettings* settings, const(
        JPH_VehicleDifferentialSettings)* values, uint count);

float JPH_WheeledVehicleControllerSettings_GetDifferentialLimitedSlipRatio(
    const(JPH_WheeledVehicleControllerSettings)* settings);
void JPH_WheeledVehicleControllerSettings_SetDifferentialLimitedSlipRatio(
    JPH_WheeledVehicleControllerSettings* settings, float value);

void JPH_WheeledVehicleController_SetDriverInput(JPH_WheeledVehicleController* controller, float forward, float right, float brake, float handBrake);
void JPH_WheeledVehicleController_SetForwardInput(
    JPH_WheeledVehicleController* controller, float forward);
float JPH_WheeledVehicleController_GetForwardInput(const(JPH_WheeledVehicleController)* controller);
void JPH_WheeledVehicleController_SetRightInput(
    JPH_WheeledVehicleController* controller, float rightRatio);
float JPH_WheeledVehicleController_GetRightInput(const(JPH_WheeledVehicleController)* controller);
void JPH_WheeledVehicleController_SetBrakeInput(
    JPH_WheeledVehicleController* controller, float brakeInput);
float JPH_WheeledVehicleController_GetBrakeInput(const(JPH_WheeledVehicleController)* controller);
void JPH_WheeledVehicleController_SetHandBrakeInput(
    JPH_WheeledVehicleController* controller, float handBrakeInput);
float JPH_WheeledVehicleController_GetHandBrakeInput(
    const(JPH_WheeledVehicleController)* controller);
float JPH_WheeledVehicleController_GetWheelSpeedAtClutch(
    const(JPH_WheeledVehicleController)* controller);

/* WheelSettingsTV - WheelTV - TrackedVehicleController */
/* TODO: Add VehicleTrack and VehicleTrackSettings */
JPH_WheelSettingsTV* JPH_WheelSettingsTV_Create();
float JPH_WheelSettingsTV_GetLongitudinalFriction(const(JPH_WheelSettingsTV)* settings);
void JPH_WheelSettingsTV_SetLongitudinalFriction(JPH_WheelSettingsTV* settings, float value);
float JPH_WheelSettingsTV_GetLateralFriction(const(JPH_WheelSettingsTV)* settings);
void JPH_WheelSettingsTV_SetLateralFriction(JPH_WheelSettingsTV* settings, float value);

JPH_WheelTV* JPH_WheelTV_Create(const(JPH_WheelSettingsTV)* settings);
const(JPH_WheelSettingsTV)* JPH_WheelTV_GetSettings(const(JPH_WheelTV)* wheel);

JPH_TrackedVehicleControllerSettings* JPH_TrackedVehicleControllerSettings_Create();

void JPH_TrackedVehicleControllerSettings_GetEngine(const(
        JPH_TrackedVehicleControllerSettings)* settings, JPH_VehicleEngineSettings* result);
void JPH_TrackedVehicleControllerSettings_SetEngine(
    JPH_TrackedVehicleControllerSettings* settings, const(JPH_VehicleEngineSettings)* value);
const(JPH_VehicleTransmissionSettings)* JPH_TrackedVehicleControllerSettings_GetTransmission(
    const(JPH_TrackedVehicleControllerSettings)* settings);
void JPH_TrackedVehicleControllerSettings_SetTransmission(
    JPH_TrackedVehicleControllerSettings* settings, const(JPH_VehicleTransmissionSettings)* value);

void JPH_TrackedVehicleController_SetDriverInput(JPH_TrackedVehicleController* controller, float forward, float leftRatio, float rightRatio, float brake);
float JPH_TrackedVehicleController_GetForwardInput(const(JPH_TrackedVehicleController)* controller);
void JPH_TrackedVehicleController_SetForwardInput(
    JPH_TrackedVehicleController* controller, float value);
float JPH_TrackedVehicleController_GetLeftRatio(const(JPH_TrackedVehicleController)* controller);
void JPH_TrackedVehicleController_SetLeftRatio(JPH_TrackedVehicleController* controller, float value);
float JPH_TrackedVehicleController_GetRightRatio(const(JPH_TrackedVehicleController)* controller);
void JPH_TrackedVehicleController_SetRightRatio(
    JPH_TrackedVehicleController* controller, float value);
float JPH_TrackedVehicleController_GetBrakeInput(const(JPH_TrackedVehicleController)* controller);
void JPH_TrackedVehicleController_SetBrakeInput(
    JPH_TrackedVehicleController* controller, float value);

/* MotorcycleController */
JPH_MotorcycleControllerSettings* JPH_MotorcycleControllerSettings_Create();
float JPH_MotorcycleControllerSettings_GetMaxLeanAngle(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetMaxLeanAngle(
    JPH_MotorcycleControllerSettings* settings, float value);
float JPH_MotorcycleControllerSettings_GetLeanSpringConstant(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetLeanSpringConstant(
    JPH_MotorcycleControllerSettings* settings, float value);
float JPH_MotorcycleControllerSettings_GetLeanSpringDamping(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetLeanSpringDamping(
    JPH_MotorcycleControllerSettings* settings, float value);
float JPH_MotorcycleControllerSettings_GetLeanSpringIntegrationCoefficient(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetLeanSpringIntegrationCoefficient(
    JPH_MotorcycleControllerSettings* settings, float value);
float JPH_MotorcycleControllerSettings_GetLeanSpringIntegrationCoefficientDecay(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetLeanSpringIntegrationCoefficientDecay(
    JPH_MotorcycleControllerSettings* settings, float value);
float JPH_MotorcycleControllerSettings_GetLeanSmoothingFactor(
    const(JPH_MotorcycleControllerSettings)* settings);
void JPH_MotorcycleControllerSettings_SetLeanSmoothingFactor(
    JPH_MotorcycleControllerSettings* settings, float value);

float JPH_MotorcycleController_GetWheelBase(const(JPH_MotorcycleController)* controller);
bool JPH_MotorcycleController_IsLeanControllerEnabled(const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_EnableLeanController(JPH_MotorcycleController* controller, bool value);
bool JPH_MotorcycleController_IsLeanSteeringLimitEnabled(
    const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_EnableLeanSteeringLimit(
    JPH_MotorcycleController* controller, bool value);
float JPH_MotorcycleController_GetLeanSpringConstant(const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_SetLeanSpringConstant(
    JPH_MotorcycleController* controller, float value);
float JPH_MotorcycleController_GetLeanSpringDamping(const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_SetLeanSpringDamping(JPH_MotorcycleController* controller, float value);
float JPH_MotorcycleController_GetLeanSpringIntegrationCoefficient(
    const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_SetLeanSpringIntegrationCoefficient(
    JPH_MotorcycleController* controller, float value);
float JPH_MotorcycleController_GetLeanSpringIntegrationCoefficientDecay(
    const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_SetLeanSpringIntegrationCoefficientDecay(
    JPH_MotorcycleController* controller, float value);
float JPH_MotorcycleController_GetLeanSmoothingFactor(const(JPH_MotorcycleController)* controller);
void JPH_MotorcycleController_SetLeanSmoothingFactor(
    JPH_MotorcycleController* controller, float value);

/* JOLT_C_H_ */
