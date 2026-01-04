// ╒══════════════════════════ Physics.h ═╕
// │ Syngine                              │
// │ Created 2025-05-21                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

// Forward declarations
namespace Syngine {
class DebugRender;
}

#include "Syngine/Utils/ModelLoader.h"
#include "Syngine/ECS/Components/CameraComponent.h"
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

// Disables common Jolt warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

using namespace JPH;
namespace Syngine {
    struct DebugModes; // Forward declaration

    // Layer definitions for Jolt
    namespace Layers {
        static constexpr ObjectLayer NON_MOVING = 0;
        static constexpr ObjectLayer MOVING = 1;
        static constexpr ObjectLayer NUM_LAYERS = 2; 
    };

    // BroadPhaseLayer definitions for Jolt
    namespace BroadPhaseLayers {
        static constexpr BroadPhaseLayer NON_MOVING(0);
        static constexpr BroadPhaseLayer MOVING(1);
        static constexpr uint32_t NUM_BP_LAYERS = 2; 
    };

    // BroadPhaseLayerInterface implementation
    // Defined a mapping between ObjectLayer and BroadPhaseLayer
    class BPLayerInterfaceImpl : public BroadPhaseLayerInterface {
        public:
        BPLayerInterfaceImpl() {
            // Create a mapping table from object to broad phase layer
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
    //Defines how object layers interact with broad phase layers
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

    //Body activation lister placeholder
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

    /// @brief Physics manager class for Syngine
    /// @section Physics
    /// @since v0.0.1
    class Phys {
        public:
        Phys();
        ~Phys();

        /// @brief Initialize the physics system
        /// @param debug Whether to enable debug rendering
        /// @return 0 on success, non-zero on failure
        /// @since v0.0.1
        /// @internal
        void _Init();

        /// @brief Shutdown the physics system
        /// @since v0.0.1
        /// @internal
        void _Shutdown();

        /// @brief Update the physics system
        /// @param deltaTime The time elapsed since the last update
        /// @param collisionSteps The number of collision detection steps to perform
        /// @since v0.0.1
        /// @internal
        void _Update(float deltaTime, int collisionSteps);

        /// @brief Get the debug renderer
        /// @return Pointer to the debug renderer
        DebugRender* _GetDebugRenderer() { return mDebugRenderer; }

        /// @brief Draw debug information
        /// @param width Width of the viewport
        /// @param height Height of the viewport
        /// @param program Shader program to use for rendering
        void _DrawDebug(int                 width,
                        int                 height,
                        bgfx::ProgramHandle program,
                        Syngine::Camera     camera,
                        Syngine::Camera     finalCam,
                        DebugModes          debug);

        /// @brief Get the body interface for manipulating bodies
        /// @return Reference to the body interface
        /// @since v0.0.1
        /// @internal
        BodyInterface& _GetBodyInterface() {
            return mPhysicsSystem.GetBodyInterface();
        }

        /// @brief Get the physics system
        /// @return Reference to the physics system
        /// @since v0.0.1
        /// @internal
        PhysicsSystem& _GetPhysicsSystem() { return mPhysicsSystem; }

        /// @brief Get the broad phase layer interface
        /// @return Reference to the broad phase layer interface
        /// @since v0.0.1
        /// @internal
        const BPLayerInterfaceImpl& _GetBroadPhaseLayerInterface() const {
            return mBroadPhaseLayerInterface;
        }

        /// @brief Get the object vs broad phase layer filter
        /// @return Reference to the object vs broad phase layer filter
        /// @since v0.0.1
        /// @internal
        const ObjectLayerPairFilterImpl& _GetObjectLayerPairFilter() const {
            return mObjectLayerPairFilter;
        }

        /// @brief Get the object vs broad phase layer filter
        /// @return Reference to the object vs broad phase layer filter
        /// @since v0.0.1
        /// @internal
        const ObjectVsBroadPhaseLayerFilterImpl& _GetObjectVsBroadPhaseLayerFilter() const {
            return mObjectVsBroadPhaseLayerFilter;
        }

        /// @brief Create a sphere body
        /// @param position Position of the sphere
        /// @param radius Radius of the sphere
        /// @param motionType Motion type of the sphere
        /// @param layer Layer of the sphere
        /// @param mass Mass of the
        /// @return Body ID of the created sphere
        /// @since v0.0.1
        /// @internal
        BodyID _CreateSphere(RVec3Arg    position,
                             float       radius,
                             EMotionType motionType,
                             ObjectLayer layer,
                             float       mass = 0.0f);

        /// @brief Create a box body
        /// @param position Position of the box
        /// @param rotation Rotation of the box
        /// @param halfExtent Half extent of the box
        /// @param motionType Motion type of the box
        /// @param layer Layer of the box
        /// @param mass Mass of the box
        /// @return Body ID of the created box
        /// @since v0.0.1
        /// @internal
        BodyID _CreateBox(RVec3Arg    position,
                          QuatArg     rotation,
                          Vec3Arg     halfExtent,
                          EMotionType motionType,
                          ObjectLayer layer,
                          float       mass = 0.0f);

        /// @brief Create a mesh body
        /// @param position Position of the mesh
        /// @param rotation Rotation of the mesh
        /// @param meshData Mesh data to create the body from
        /// @param motionType Motion type of the mesh
        /// @param layer Layer of the mesh
        /// @param scale Scale of the mesh
        /// @return Body ID of the created mesh
        /// @note The mesh data should be in the Syngine::MeshData format
        /// @since v0.0.1
        /// @internal
        BodyID
        _CreateMeshBody(RVec3Arg         position,
                        QuatArg          rotation,
                        const MeshData&  meshData,
                        EMotionType      motionType,
                        ObjectLayer      layer,
                        const JPH::Vec3& scale = JPH::Vec3(1.0f, 1.0f, 1.0f));

        /// @brief Create a capsule body
        /// @param position Position of the capsule
        /// @param radius Radius of the capsule
        /// @param halfHeight Half height of the capsule
        /// @param motionType Motion type of the capsule
        /// @param layer Layer of the capsule
        /// @param mass Mass of the capsule
        /// @return Body ID of the created capsule
        /// @since v0.0.1
        /// @internal
        BodyID _CreateCapsule(RVec3Arg    position,
                              float       radius,
                              float       halfHeight,
                              EMotionType motionType,
                              ObjectLayer layer,
                              float       mass = 0.0f);

        /// @brief Create a cylinder body
        /// @param position Position of the cylinder
        /// @param rotation Rotation of the cylinder
        /// @param radius Radius of the cylinder
        /// @param halfHeight Half height of the cylinder
        /// @param motionType Motion type of the cylinder
        /// @param layer Layer of the cylinder
        /// @param mass Mass of the cylinder
        /// @return Body ID of the created cylinder
        /// @since v0.0.1
        /// @internal
        BodyID _CreateCylinder(RVec3Arg    position,
                               QuatArg     rotation,
                               float       radius,
                               float       halfHeight,
                               EMotionType motionType,
                               ObjectLayer layer,
                               float       mass = 0.0f);

        void _DrawFrustum(const float* view, const float* proj);

        void _DrawLine(const float* from, const float* to, JPH::ColorArg color);

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
