#include "SynginePhys.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Math/Quat.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/RegisterTypes.h"
#include "SDL3/SDL_log.h"
#include <Jolt/Core/JobSystemSingleThreaded.h> //for single threaded fallback
#include <thread> //for hardware_concurrency

//Jolt includes
#include <Jolt/Core/Memory.h>
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Core/IssueReporting.h>

using namespace JPH;

namespace Syngine {
    void SynginePhys::TraceImpl(const char* inFMT, ...) {
        //format
        va_list list;
        va_start(list, inFMT);
        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), inFMT, list);
        va_end(list);

        //print to the tty
        SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Jolt Trace: %s", buffer);
    }

    bool SynginePhys::AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine) {
        //format
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Jolt Assert Failed: %s\n%s\nFile: %s\nLine: %u", inExpression, inMessage, inFile, inLine);
        return true; //true to break, false to continue
    }

    SynginePhys::SynginePhys() {}
    SynginePhys::~SynginePhys() { Shutdown(); }

    void SynginePhys::Init() {
        RegisterDefaultAllocator();
        Trace = TraceImpl;
        JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)
        Factory::sInstance = new Factory();
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

    void SynginePhys::Shutdown() {
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
    }

    void SynginePhys::Update(int collisionSteps) {
        if (!Factory::sInstance) return;

        //so it's a bad idea to do dynamic steps so we do a fixed step which i say is Better:tm:
        const float cFixedDeltaTime = 1.0f / 60.0f;

        mPhysicsSystem.Update(cFixedDeltaTime, collisionSteps, mTempAllocator, mJobSystem);
    }

    BodyID SynginePhys::CreateSphere(RVec3Arg position, float radius, EMotionType motionType, ObjectLayer layer) {
        BodyInterface &bodyInterface = mPhysicsSystem.GetBodyInterface();
        SphereShapeSettings sphereShapeSettings(radius);
        ShapeSettings::ShapeResult sphereShapeResult = sphereShapeSettings.Create();
        ShapeRefC sphereShape = sphereShapeResult.Get();

        BodyCreationSettings sphereSettings(sphereShape, position, Quat::sIdentity(), motionType, layer);
        Body *sphere = bodyInterface.CreateBody(sphereSettings);
        bodyInterface.AddBody(sphere->GetID(), EActivation::Activate);
        return sphere->GetID();
    }

    BodyID SynginePhys::CreateBox(RVec3Arg position, QuatArg rotation, Vec3Arg halfExtent, EMotionType motionType, ObjectLayer layer)
    {
        BodyInterface &body_interface = mPhysicsSystem.GetBodyInterface();
        BoxShapeSettings box_shape_settings(halfExtent);
        ShapeSettings::ShapeResult box_shape_result = box_shape_settings.Create();
        ShapeRefC box_shape = box_shape_result.Get();

        BodyCreationSettings box_settings(box_shape, position, rotation, motionType, layer);
        Body *box = body_interface.CreateBody(box_settings);
        body_interface.AddBody(box->GetID(), EActivation::Activate);
        return box->GetID();
    }
} // namespace Syngine