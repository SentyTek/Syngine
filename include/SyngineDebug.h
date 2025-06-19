
#ifndef JPH_DEBUG_RENDERER
#define JPH_DEBUG_RENDERER
#endif

#include "Jolt/Jolt.h"
#include "Jolt/Core/Color.h"
#include "Jolt/Math/Real.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Renderer/DebugRenderer.h"

#include "bgfx/bgfx.h"
namespace Syngine {

class DebugRender : public JPH::DebugRenderer {
  private:
    class BatchImpl : public JPH::DebugRenderer::Batch {
      public:
        JPH_OVERRIDE_NEW_DELETE

        std::vector<JPH::DebugRenderer::Triangle> mTriangles;
    };
  public:
    DebugRender();
    ~DebugRender();

    virtual void DrawLine(JPH::RVec3Arg from,
                          JPH::RVec3Arg to,
                          JPH::ColorArg color) override;

    virtual void DrawGeometry(JPH::RMat44Arg     modelMatrix,
                              const JPH::AABox&  worldSpaceBounds,
                              float              LODScaleSq,
                              JPH::ColorArg      modelColor,
                              const GeometryRef& geometry,
                              ECullMode   cullMode = ECullMode::CullBackFace,
                              ECastShadow castShadow = ECastShadow::Off,
                              EDrawMode drawMode = EDrawMode::Wireframe) override;

    virtual void DrawText3D(JPH::RVec3Arg           position,
                            const std::string_view& inString,
                            JPH::ColorArg           color  = JPH::Color::sWhite,
                            float                   height = 0.5f) override;

    virtual void
    DrawTriangle(JPH::RVec3Arg v1,
                 JPH::RVec3Arg v2,
                 JPH::RVec3Arg v3,
                 JPH::ColorArg color,
                 ECastShadow   castShadow = ECastShadow::Off) override;

    virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;

    virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;

    void RenderLines(const float*        view,
                     const float*        proj,
                     int                 width,
                     int                 height,
                     bgfx::ProgramHandle program);

    void ClearLines() { debugLines.clear(); }

  private:
    struct DebugLine {
        float* from;
        float* to;
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