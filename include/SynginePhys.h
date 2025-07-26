#pragma once

// Forward declarations
namespace Syngine {
class DebugRender;
}

#include "SynModelLoader.h"
#include "SyngineDebug.h"
#include "bgfx/bgfx.h"

//Jolt includes
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/ContactListener.h>

#include <SDL3/SDL_log.h>
#include <cstdint>

//Disables common Jolt warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

using namespace JPH;

namespace Syngine {

    //Layer definitions for Joly
    namespace Layers {
        static constexpr ObjectLayer NON_MOVING = 0;
        static constexpr ObjectLayer MOVING = 1;
        static constexpr ObjectLayer NUM_LAYERS = 2; 
    };

    //BroadPhaseLayer definitions for Jolt
    namespace BroadPhaseLayers {
        static constexpr BroadPhaseLayer NON_MOVING(0);
        static constexpr BroadPhaseLayer MOVING(1);
        static constexpr uint32_t NUM_BP_LAYERS = 2; 
    };

    //BroadPhaseLayerInterface implementation
    //Defined a mapping between ObjectLayer and BroadPhaseLayer
    class BPLayerInterfaceImpl : public BroadPhaseLayerInterface {
        public:
        BPLayerInterfaceImpl() {
            //Create a mapping table from object to broadphase layer
            mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
            mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
        }

        virtual uint32_t GetNumBroadPhaseLayers() const override {
            return BroadPhaseLayers::NUM_BP_LAYERS;
        }

        virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
            JPH_ASSERT(inLayer < Layers::NUM_LAYERS, "Invalid layer");
            return mObjectToBroadPhase[inLayer];
        }

        #if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
        virtual const char * GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
            switch((BroadPhaseLayer::Type)inLayer) {
                case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
                    return "NON_MOVING";
                case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
                    return "MOVING";
                default:
                    JPH_ASSERT(false, "Invalid layer");
                    return "INVALID";
            }
        }
        #endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

        private:
        BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
    };

    //ObjectVsBroadPhaseLayerFilter implementation
    //Defines how object layers interact with broadphase layers
    class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter {
        public:
        virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override {
            switch(inLayer1) {
                case Layers::NON_MOVING:
                    return inLayer2 == BroadPhaseLayers::NON_MOVING;
                case Layers::MOVING:
                    return true; //Moving objects can collide with both moving and non-moving objects
                default:
                    JPH_ASSERT(false, "Invalid layer");
                    return false;
            }
        }
    };

    //ObjectLayerPairFilter implementation
    //Defines how object layers interact with each other
    class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
        public:
        virtual bool ShouldCollide(ObjectLayer inLayer1, ObjectLayer inLayer2) const override {
            switch(inLayer1) {
                case Layers::NON_MOVING:
                    return inLayer2 == Layers::MOVING;
                case Layers::MOVING:
                    return true; //Moving objects can collide with both moving and non-moving objects
                default:
                    JPH_ASSERT(false, "Invalid layer");
                    return false;
            }
        }
    };

    //Contact listener placeholder
    class SynContactListener : public JPH::ContactListener {
        public:
        virtual ValidateResult OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inResult) override {
            // This is called when a contact is created. Can be used to filter out contacts.
            // For example, check if the bodies are in the same layer and return false if they are not.
            return ValidateResult::AcceptAllContactsForThisBodyPair;
        }

        virtual void OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &inSettings) override {
            // This is called when a contact is added. Can be used to modify the contact settings.
            // For example, change the friction or restitution of the contact.
            // SDL_Log("Contact added between %u and %u", inBody1.GetID(), inBody2.GetID());
        }

        virtual void OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override {
            // This is called when a contact is persisted. For example, to update the contact settings.
            // SDL_Log("Contact persisted between %u and %u", inBody1.GetID(), inBody2.GetID());
        }

        virtual void OnContactRemoved(const SubShapeIDPair &inSubShapePair) override {
            // This is called when a contact is removed. For example, to clean up any resources allocated for the contact.
            // SDL_Log("Contact removed");
        }
    };

    //Body adtivation lister placeholder
    class SynBodyActivationListener : public BodyActivationListener {
        public:
        virtual void OnBodyActivated(const JPH::BodyID &inBodyID, uint64_t /*inBodyUserData*/) override
        {
            // SDL_Log("Body %u activated", inBodyID.GetIndex());
        }

        virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, uint64_t /*inBodyUserData*/) override
        {
            // SDL_Log("Body %u deactivated", inBodyID.GetIndex());
        }
    };

    class Phys {
        public:
        Phys();
        ~Phys();

        void Init(bool debug);
        void Shutdown();
        void Update(float deltaTime, int collisionSteps);
        void _DrawDebug(int width, int height, bgfx::ProgramHandle program, Syngine::Camera camera, Syngine::Camera finalCam);

        BodyInterface& GetBodyInterface() { return mPhysicsSystem.GetBodyInterface(); }
        PhysicsSystem& GetPhysicsSystem() { return mPhysicsSystem; }
        const BPLayerInterfaceImpl& GetBroadPhaseLayerInterface() const {
            return mBroadPhaseLayerInterface;
        }
        const ObjectLayerPairFilterImpl& GetObjectLayerPairFilter() const {
            return mObjectLayerPairFilter;
        }
        const ObjectVsBroadPhaseLayerFilterImpl& GetObjectVsBroadPhaseLayerFilter() const {
            return mObjectVsBroadPhaseLayerFilter;
        }

        BodyID CreateSphere(RVec3Arg    position,
                            float       radius,
                            EMotionType motionType,
                            ObjectLayer layer,
                            float       mass = 0.0f);
        BodyID CreateBox(RVec3Arg    position,
                         QuatArg     rotation,
                         Vec3Arg     halfExtent,
                         EMotionType motionType,
                         ObjectLayer layer,
                         float       mass = 0.0f);
        BodyID CreateMeshBody(RVec3Arg         position,
                              QuatArg          rotation,
                              const MeshData&  meshData,
                              EMotionType      motionType,
                              ObjectLayer      layer,
                              const JPH::Vec3& scale = JPH::Vec3(1.0f, 1.0f, 1.0f));
        
        BodyID CreateCapsule(RVec3Arg    position,
                             float       radius,
                             float       halfHeight,
                             EMotionType motionType,
                             ObjectLayer layer,
                             float       mass = 0.0f);
        BodyID CreateCylinder(RVec3Arg    position,
                              QuatArg     rotation,
                              float       radius,
                              float       halfHeight,
                              EMotionType motionType,
                              ObjectLayer layer,
                              float       mass = 0.0f);

        private:
        //Jolt specific variables
        PhysicsSystem mPhysicsSystem;
        TempAllocatorImpl* mTempAllocator = nullptr;
        JobSystemThreadPool* mJobSystem = nullptr;

        //Layer and filter implementations
        BPLayerInterfaceImpl mBroadPhaseLayerInterface;
        ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadPhaseLayerFilter;
        ObjectLayerPairFilterImpl mObjectLayerPairFilter;

        //Listeners
        SynContactListener mContactListener;
        SynBodyActivationListener mBodyActivationListener;

        DebugRender* mDebugRenderer = nullptr;

        //Jolt foundation for tracing and asserts
        static void TraceImpl(const char* inFMT, ...);
        static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint32_t inLine);
    };
} // namespace Syngine

#ifdef _MSC_VER
#pragma warning(pop)
#endif