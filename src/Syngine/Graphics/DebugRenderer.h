// ╒════════════════════ DebugRenderer.h ═╕
// │ Syngine                              │
// │ Created 2025-06-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#pragma once
#ifndef JPH_DEBUG_RENDERER
#define JPH_DEBUG_RENDERER
#endif

#include <atomic>

#include "Jolt/Jolt.h"
#include "Jolt/Core/Color.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Renderer/DebugRenderer.h"
#include "Jolt/Core/Reference.h"

#include "bgfx/bgfx.h"
#include "bx/math.h"

// Forward declarations
namespace Syngine {
class Phys;
struct Camera;
}

#include "Syngine/Physics/Physics.h"

namespace Syngine {

/// @brief Debug renderer class for Syngine
/// @section DebugRenderer
class DebugRender : public JPH::DebugRenderer {
  private:
    /// @brief Internal class to manage batches of debug triangles. Not all
    /// functions are implemented or documented.
    /// @section DebugRenderer
    class BatchImpl : public JPH::RefTargetVirtual {
      public:
        JPH_OVERRIDE_NEW_DELETE

        std::vector<JPH::DebugRenderer::Triangle> mTriangles;

        virtual void AddRef() override {
            mRefCount.fetch_add(1, std::memory_order_relaxed);
        }
        virtual void Release() override {
            if (mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete this;
            }
        }

      private:
        std::atomic<uint32_t> mRefCount = { 0 }; // Reference count for memory management
    };
  public:
    DebugRender();
    ~DebugRender();

    /// @brief Draw a line with a color
    /// @param from Starting point of the line
    /// @param to Ending point of the line
    /// @param color Color of the line
    /// @since v0.0.1
    /// @internal
    virtual void DrawLine(JPH::RVec3Arg from,
                          JPH::RVec3Arg to,
                          JPH::ColorArg color) override;

    /// @brief Draw geometry
    /// @param modelMatrix Transformation matrix for the geometry
    /// @param worldSpaceBounds Bounding box in world space
    /// @param LODScaleSq Level of detail scale squared
    /// @param modelColor Color of the model
    /// @param geometry Geometry reference to draw
    /// @param cullMode Culling mode for the geometry
    /// @param castShadow Whether to cast shadows
    /// @param drawMode Draw mode for the geometry
    /// @since v0.0.1
    /// @internal
    virtual void DrawGeometry(JPH::RMat44Arg     modelMatrix,
                              const JPH::AABox&  worldSpaceBounds,
                              float              LODScaleSq,
                              JPH::ColorArg      modelColor,
                              const GeometryRef& geometry,
                              ECullMode   cullMode = ECullMode::CullBackFace,
                              ECastShadow castShadow = ECastShadow::Off,
                              EDrawMode drawMode = EDrawMode::Wireframe) override;

    /// @brief Draw a text string in 3D space (Not implemented)
    /// @param position Position in 3D space
    /// @param inString Text string to draw
    /// @param color Color of the text
    /// @param height Height of the text
    virtual void DrawText3D(JPH::RVec3Arg           position,
                            const std::string_view& inString,
                            JPH::ColorArg           color  = JPH::Color::sWhite,
                            float                   height = 0.5f) override;

    /// @brief Draw a triangle
    /// @param v1 First vertex of the triangle
    /// @param v2 Second vertex of the triangle
    /// @param v3 Third vertex of the triangle
    /// @param color Color of the triangle
    /// @param castShadow Whether to cast shadows
    /// @since v0.0.1
    /// @internal
    virtual void
    DrawTriangle(JPH::RVec3Arg v1,
                 JPH::RVec3Arg v2,
                 JPH::RVec3Arg v3,
                 JPH::ColorArg color,
                 ECastShadow   castShadow = ECastShadow::Off) override;

    /// @brief Create a batch of triangles
    /// @param inTriangles Array of triangles to create the batch from
    /// @param inTriangleCount Number of triangles in the array
    /// @return Batch of triangles
    /// @since v0.0.1
    /// @internal
    virtual JPH::DebugRenderer::Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;

    /// @brief Create a batch of triangles from vertices and indices
    /// @param inVertices Array of vertices
    /// @param inVertexCount Number of vertices in the array
    /// @param inIndices Array of indices
    /// @param inIndexCount Number of indices in the array
    virtual JPH::DebugRenderer::Batch
    CreateTriangleBatch(const Vertex*      inVertices,
                        int                inVertexCount,
                        const JPH::uint32* inIndices,
                        int                inIndexCount) override;

    /// @brief Draw a frustum from a camera object
    /// @param camera Camera to draw the frustum for
    /// @pre Camera must be initialized
    /// @since v0.0.1
    /// @internal
    void DrawFrustum(Syngine::Camera camera);

    /// @brief Draw a frustum from given view and projection matrices
    /// @param view View matrix
    /// @param proj Projection matrix
    /// @pre View and projection matrices must be valid
    /// @since v0.0.1
    /// @internal
    void DrawFrustum(const float* view, const float* proj);

    /// @brief Draw a box given min and max points
    /// @param min Minimum point of the box
    /// @param max Maximum point of the box
    /// @param color Color of the box
    /// @since v0.0.1
    /// @internal
    void DrawBox(const float* min, const float* max, JPH::ColorArg color);

    /// @brief Draw a sphere given center and radius
    /// @param center Center of the sphere
    /// @param radius Radius of the sphere
    /// @param color Color of the sphere
    /// @since v0.0.1
    /// @internal
    void DrawSphere(JPH::RVec3Arg center, float radius, JPH::ColorArg color);

    /// @brief Render debug lines
    /// @param view View matrix
    /// @param proj Projection matrix
    /// @param width Width of the viewport
    /// @param height Height of the viewport
    /// @param program Shader program to use for rendering
    /// @pre Rendering system must be initialized
    /// @since v0.0.1
    /// @internal
    void RenderLines(const float*        view,
                     const float*        proj,
                     int                 width,
                     int                 height,
                     bgfx::ProgramHandle program);

    /// @brief Clear all debug lines
    /// @since v0.0.1
    /// @post All debug lines are cleared
    /// @internal
    void ClearLines();
    
  private:
    struct DebugLine {
        bx::Vec3 from;
        bx::Vec3 to;
        uint32_t color; //TODO: Change this to custom color class once that's implemented
    };
    struct DebugVertex {
        float x, y, z;
        uint32_t abgr;
    };

    std::vector<DebugLine> debugLines;
    bgfx::VertexLayout     DebugVertexLayout;
};

} // namespace Syngine
