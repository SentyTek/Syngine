#include "SyngineDebug.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"

namespace Syngine {

DebugRender::DebugRender() {
    DebugVertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
}
DebugRender::~DebugRender() {
    for (auto& line : debugLines) {
        delete[] line.from;
        delete[] line.to;
    }
    debugLines.clear();
}

void DebugRender::DrawLine(JPH::RVec3Arg from, JPH::RVec3Arg to, JPH::ColorArg color) {
    debugLines.push_back({
        .from = new float[3]{ from.GetX(), from.GetY(), from.GetZ() },
        .to   = new float[3]{ to.GetX(), to.GetY(), to.GetZ() },
        .color = color.GetUInt32()
    });
}

void DebugRender::DrawTriangle(JPH::RVec3Arg v1,
                               JPH::RVec3Arg v2,
                               JPH::RVec3Arg v3,
                               JPH::ColorArg color,
                               ECastShadow   castShadow) {
    DrawLine(v1, v2, color);
    DrawLine(v2, v3, color);
    DrawLine(v3, v1, color);
}

void DebugRender::DrawGeometry(JPH::RMat44Arg     modelMatrix,
                               const JPH::AABox&  worldSpaceBounds,
                               float              LODScaleSq,
                               JPH::ColorArg      modelColor,
                               const GeometryRef& geometry,
                               ECullMode          cullMode,
                               ECastShadow        castShadow,
                               EDrawMode          drawMode) {
    if (drawMode != EDrawMode::Wireframe) return; // Only wireframe is supported
    if (geometry->mLODs.empty()) return;

    const Batch& batchRef = geometry->mLODs[0].mTriangleBatch;
    if (batchRef == nullptr) return;

    // Cast back to our implementation
    const BatchImpl* batch = dynamic_cast<const BatchImpl*>(batchRef.GetPtr());

    // Draw the batch
    if (batch) {
        for (const auto& tri : batch->mTriangles) {
            JPH::RVec3 v0 = modelMatrix * JPH::Vec3(tri.mV[0].mPosition);
            JPH::RVec3 v1 = modelMatrix * JPH::Vec3(tri.mV[1].mPosition);
            JPH::RVec3 v2 = modelMatrix * JPH::Vec3(tri.mV[2].mPosition);

            DrawLine(v0, v1, modelColor);
            DrawLine(v1, v2, modelColor);
            DrawLine(v2, v0, modelColor);
        }
    }
}

void DebugRender::DrawText3D(JPH::RVec3Arg           position,
                             const std::string_view& inString,
                             JPH::ColorArg           color,
                             float                   height) {
    // Not implemented
}

JPH::DebugRenderer::Batch DebugRender::CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) {
    BatchImpl* batch = new BatchImpl();
    if (inTriangles != nullptr && inTriangleCount > 0) {
        batch->mTriangles.assign(inTriangles, inTriangles + inTriangleCount);
    }
    return *batch;
}

JPH::DebugRenderer::Batch DebugRender::CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) {
    BatchImpl* batch = new BatchImpl();
    if (inVertices != nullptr && inIndices != nullptr && inIndexCount > 0) {
        batch->mTriangles.reserve(inIndexCount / 3);
        for (int i = 0; i < inIndexCount; i += 3) {
            JPH::DebugRenderer::Triangle t;
            t.mV[0] = inVertices[inIndices[i]];
            t.mV[1] = inVertices[inIndices[i + 1]];
            t.mV[2] = inVertices[inIndices[i + 2]];
            batch->mTriangles.push_back(t);
        }
    }
    return *batch;
}

void DebugRender::RenderLines(const float* view, const float* proj, int width, int height, bgfx::ProgramHandle program) {
    bgfx::setViewTransform(9999, &view, &proj);
    bgfx::setViewRect(9999, 0, 0, width, height);
    bgfx::touch(9999);

    if (debugLines.empty()) return;

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(
        &tvb, debugLines.size() * 2, DebugVertexLayout);

    DebugVertex* vertex = (DebugVertex*)tvb.data;
    for (size_t i = 0; i < debugLines.size(); ++i) {
        const DebugLine& line = debugLines[i];
        vertex[i * 2 + 0]     = { line.from[0], line.from[1], line.from[2], line.color };
        vertex[i * 2 + 1] = { line.to[0], line.to[1], line.to[2], line.color };
    }

    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_PT_LINES);
    bgfx::submit(9999, program);
}

} // namespace Syngine