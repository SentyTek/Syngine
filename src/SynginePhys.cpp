#include "SynginePhys.h"
#include <thread> //for hardware_concurrency

#include "Components/CameraComponent.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Geometry/IndexedTriangle.h"
#include "Jolt/Math/Quat.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/CylinderShape.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/RegisterTypes.h"
#include "SDL3/SDL_log.h"
#include <Jolt/Core/JobSystemSingleThreaded.h> //for single threaded fallback
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Core/IssueReporting.h>

using namespace JPH;
using namespace Syngine;

void Phys::TraceImpl(const char* inFMT, ...) {
    //format
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    //print to the tty
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Jolt Trace: %s", buffer);
}

bool Phys::AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine) {
    //format
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Jolt Assert Failed: %s\n%s\nFile: %s\nLine: %u", inExpression, inMessage, inFile, inLine);
    return true; //true to break, false to continue
}

Phys::Phys() {}
Phys::~Phys() { Shutdown(); }

void Phys::Init(bool debug) {

    RegisterDefaultAllocator();
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)
    Factory::sInstance = new Factory();

    // Set Jolt's debug renderer to our custom debug renderer if debugging is enabled
    if (debug) {
        mDebugRenderer = new Syngine::DebugRender();
        JPH::DebugRenderer::sInstance = mDebugRenderer;
    }

    RegisterTypes();

    //We need a temp allocator for temp allocations during physics update.
    //Pre-allocating 10mb allows to avoid having to allocate memory during the update.
    //10MB is a good size for most games, but you can increase it if you need more.
    mTempAllocator = new TempAllocatorImpl(10 * 1024 * 1024);

    //Need a job system for the physics update.
    //In the future this will be replaced with a job system from the Engine itself.
    //As well, using the engine's existing thread pool is preferred
    int numThreads = max(1, (int)std::thread::hardware_concurrency() - 1);
    mJobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, numThreads);

    const uint32_t cMaxBodies = 65536;
    const uint32_t cMaxBodyPairs = 65536;
    const uint32_t cMaxContactConstraints = 10240;

    mPhysicsSystem.Init(cMaxBodies, 0 /*numBodyMutexes, 0 means default*/, cMaxBodyPairs, cMaxContactConstraints,
        mBroadPhaseLayerInterface, mObjectVsBroadPhaseLayerFilter, mObjectLayerPairFilter);
    
    //activate listeners
    mPhysicsSystem.SetBodyActivationListener(&mBodyActivationListener);
    mPhysicsSystem.SetContactListener(&mContactListener);

    SDL_Log("Jolt initialized");
}

void Phys::Shutdown() {
    if (!Factory::sInstance) //Already shutdown
        return;
    
    SDL_Log("Shutting down Jolt");

    UnregisterTypes();
    delete Factory::sInstance;
    Factory::sInstance = nullptr;

    if (mTempAllocator) {
        delete mTempAllocator;
        mTempAllocator = nullptr;
    }

    if (mJobSystem) {
        delete mJobSystem;
        mJobSystem = nullptr;
    }

    if (mDebugRenderer) {
        delete mDebugRenderer;
        mDebugRenderer = nullptr;
    }
}

void Phys::Update(float deltaTime, int collisionSteps) {
    if (!Factory::sInstance) return;

    mPhysicsSystem.Update(deltaTime, collisionSteps, mTempAllocator, mJobSystem);
}

void Phys::DrawDebug(int                 width,
                     int                 height,
                     bgfx::ProgramHandle program,
                     Syngine::Camera     camera,
                     Syngine::Camera     finalCam) {
    if (mDebugRenderer) {
        JPH::BodyManager::DrawSettings drawSettings;
        drawSettings.mDrawShapeWireframe = true;
        //drawSettings.mDrawShape = false;
        
        mPhysicsSystem.DrawBodies(drawSettings, mDebugRenderer);
        mDebugRenderer->DrawFrustum(camera);
        mDebugRenderer->RenderLines(finalCam.view, finalCam.proj, width, height, program);

        mDebugRenderer->ClearLines();
    }
}

BodyID Phys::CreateSphere(RVec3Arg position, float radius, EMotionType motionType, ObjectLayer layer, float mass) {
    BodyInterface &bodyInterface = mPhysicsSystem.GetBodyInterface();
    SphereShapeSettings sphereShapeSettings(radius);
    ShapeSettings::ShapeResult sphereShapeResult = sphereShapeSettings.Create();
    if (sphereShapeResult.HasError()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateSphere: Failed to create sphere shape: %s", sphereShapeResult.GetError().c_str());
        return BodyID();
    }
    ShapeRefC sphereShape = sphereShapeResult.Get();

    BodyCreationSettings sphereSettings(sphereShape, position, Quat::sIdentity(), motionType, layer);

    if (motionType == EMotionType::Dynamic && mass > 0.0f) {
        sphereSettings.mMassPropertiesOverride = sphereShape->GetMassProperties();
        sphereSettings.mMassPropertiesOverride.ScaleToMass(mass);
        sphereSettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
    }

    Body *sphere = bodyInterface.CreateBody(sphereSettings);
    if (!sphere) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateSphere: Failed to create sphere body.");
        return BodyID();
    }
    bodyInterface.AddBody(sphere->GetID(), EActivation::Activate);
    return sphere->GetID();
}

BodyID Phys::CreateBox(RVec3Arg position, QuatArg rotation, Vec3Arg halfExtent, EMotionType motionType, ObjectLayer layer, float mass)
{
    BodyInterface &body_interface = mPhysicsSystem.GetBodyInterface();
    BoxShapeSettings box_shape_settings(halfExtent);
    ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
    if (box_shape_result.HasError()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateBox: Failed to create box shape: %s", box_shape_result.GetError().c_str());
        return BodyID();
    }
    ShapeRefC box_shape = box_shape_result.Get();

    BodyCreationSettings box_settings(box_shape, position, rotation, motionType, layer);

    // Set mass if dynamic
    if (motionType == EMotionType::Dynamic && mass > 0.0f) {
        box_settings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        box_settings.mMassPropertiesOverride = box_shape->GetMassProperties();
        box_settings.mMassPropertiesOverride.ScaleToMass(mass);
    }

    Body *box = body_interface.CreateBody(box_settings);
    if (!box) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateBox: Failed to create box body.");
        return BodyID();
    }
    body_interface.AddBody(box->GetID(), EActivation::Activate);
    return box->GetID();
}

BodyID Phys::CreateMeshBody(RVec3Arg position, QuatArg rotation, const MeshData& meshData, EMotionType motionType, ObjectLayer layer) {
    BodyInterface &bodyInterface = mPhysicsSystem.GetBodyInterface();
    if (meshData.numVertices == 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateMeshBody: Mesh data is empty.");
        return BodyID(); // Return an invalid BodyID
    }

    // convert SynMeshData to Jolt's mesh format
    JPH::VertexList vertices;
    vertices.reserve(meshData.vertices.size());
    for (const auto& synVertex : meshData.vertices) {
        vertices.push_back(JPH::Float3(
            synVertex.pos[0],
            synVertex.pos[1],
            synVertex.pos[2]
        ));
    }

    JPH::IndexedTriangleList triangles;
    triangles.reserve(meshData.indices.size() / 3);
    for (size_t i = 0; i < meshData.indices.size(); i += 3) {
        triangles.push_back(JPH::IndexedTriangle(
            meshData.indices[i],
            meshData.indices[i + 1],
            meshData.indices[i + 2]
        ));
    }

    if (vertices.empty() || triangles.empty()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateMeshBody: No valid vertices or triangles found in mesh data.");
        return BodyID();
    }

    MeshShapeSettings meshShapeSettings(vertices, triangles);
    
    ShapeSettings::ShapeResult meshShapeResult = meshShapeSettings.Create();
    if (meshShapeResult.HasError()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateMeshBody: Failed to create mesh shape: %s", meshShapeResult.GetError().c_str());
        return BodyID();
    }
    ShapeRefC meshShape = meshShapeResult.Get();
    BodyCreationSettings meshSettings(meshShape, position, rotation, motionType, layer);

    // For static meshes, mass properties are not strictly required, as Jolt treats them as infinite mass.
    // If it were dynamic, mass properties would be needed.
    // if (motionType == EMotionType::Dynamic) {
    //    bodySettings.mMassProertiesOverride = meshShape->GetMassProperties();
    //    bodySettings.mMassPropertiesOverride.ScaleToMass(desired_mass); //For specific mass
    // }

    Body* body = bodyInterface.CreateBody(meshSettings);
    if (!body) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateMeshBody: Failed to create body for mesh.");
        return BodyID(); // Return an invalid BodyID
    }
    bodyInterface.AddBody(body->GetID(), EActivation::Activate);
    return body->GetID();
}

BodyID Phys::CreateCapsule(RVec3Arg position, float radius, float halfHeight, EMotionType motionType, ObjectLayer layer, float mass) {
    BodyInterface& bodyInterface = mPhysicsSystem.GetBodyInterface();
    CapsuleShapeSettings capsuleShapeSettings(halfHeight, radius);
    ShapeSettings::ShapeResult capsuleShapeResult = capsuleShapeSettings.Create();
    if (capsuleShapeResult.HasError()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateCapsule: Failed to create capsule shape: %s", capsuleShapeResult.GetError().c_str());
        return BodyID(); 
    }
    ShapeRefC capsuleShape = capsuleShapeResult.Get();

    BodyCreationSettings capsuleSettings(
        capsuleShape, position, Quat::sIdentity(), motionType, layer);
    
    capsuleSettings.mAllowSleeping = false; 

    if (motionType == EMotionType::Dynamic && mass > 0.0f) {
        capsuleSettings.mMassPropertiesOverride = capsuleShape->GetMassProperties();
        capsuleSettings.mMassPropertiesOverride.ScaleToMass(mass);
        capsuleSettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
    }

    Body* capsule = bodyInterface.CreateBody(capsuleSettings);
    if (!capsule) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateCapsule: Failed to create capsule body.");
        return BodyID(); 
    }
    bodyInterface.AddBody(capsule->GetID(), EActivation::Activate);
    return capsule->GetID();
}

BodyID Phys::CreateCylinder(RVec3Arg position, QuatArg rotation, float halfHeight, float radius, EMotionType motionType, ObjectLayer layer, float mass) {
    BodyInterface& bodyInterface = mPhysicsSystem.GetBodyInterface();
    CylinderShapeSettings cylinderShapeSettings(halfHeight, radius);
    ShapeSettings::ShapeResult cylinderShapeResult = cylinderShapeSettings.Create();
    if (cylinderShapeResult.HasError()) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateCylinder: Failed to create cylinder shape: %s", cylinderShapeResult.GetError().c_str());
        return BodyID();
    }
    ShapeRefC cylinderShape = cylinderShapeResult.Get();

    BodyCreationSettings cylinderSettings(cylinderShape, position, rotation, motionType, layer);

    if (motionType == EMotionType::Dynamic && mass > 0.0f) {
        cylinderSettings.mMassPropertiesOverride = cylinderShape->GetMassProperties();
        cylinderSettings.mMassPropertiesOverride.ScaleToMass(mass);
        cylinderSettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
    }
    
    // cylinderSettings.mAllowSleeping = false; // Optional: if cylinders are often characters or active objects

    Body* cylinder = bodyInterface.CreateBody(cylinderSettings);
    if (!cylinder) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SynginePhys::CreateCylinder: Failed to create cylinder body.");
        return BodyID();
    }
    bodyInterface.AddBody(cylinder->GetID(), EActivation::Activate);
    return cylinder->GetID();
}