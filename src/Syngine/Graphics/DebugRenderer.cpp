// ╒══════════════════ DebugRenderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-06-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Graphics/DebugRenderer.h"
#include "Syngine/ECS/Components/CameraComponent.h"

#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "SDL3/SDL.h"
#include "bgfx/defines.h"

namespace Syngine {

DebugRender::DebugRender() {
    Initialize();
    DebugVertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();
}
DebugRender::~DebugRender() {
    debugLines.clear();
}

void DebugRender::DrawLine(JPH::RVec3Arg from, JPH::RVec3Arg to, JPH::ColorArg color) {
    debugLines.push_back({ .from  = { from.GetX(), from.GetY(), from.GetZ() },
                           .to    = { to.GetX(), to.GetY(), to.GetZ() },
                           .color = color.GetUInt32() });
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
    if (!geometry) return;
    if (geometry->mLODs.empty()) return;

    const Batch& batchRef = geometry->mLODs[0].mTriangleBatch;
    if (batchRef == nullptr) return;

    // Cast back to our implementation (do not ever do dynamic_cast in Jolt or you will explode)
    const BatchImpl* batch = static_cast<const BatchImpl*>(batchRef.GetPtr());

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
    return batch;
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
    return batch;
}

void DebugRender::DrawFrustum(Syngine::Camera camera) {
    float viewMtx[16], projMtx[16];

    memcpy(&viewMtx, camera.view, sizeof(float) * 16);
    memcpy(&projMtx, camera.proj, sizeof(float) * 16);

    float viewProj[16];
    bx::mtxMul(viewProj, viewMtx, projMtx);

    float invViewProj[16];
    bx::mtxInverse(invViewProj, viewProj);

    // Define 8 corners of the normalized device coordinates (NDC) frustum
    // We use Z from 0-1, standard for all but OGL for some fucking reason
    const bx::Vec3 corners[8] = {
        bx::Vec3(-1.0f, -1.0f, 0.0f), bx::Vec3(1.0f, -1.0f, 0.0f),
        bx::Vec3(1.0f, 1.0f, 0.0f),   bx::Vec3(-1.0f, 1.0f, 0.0f),
        bx::Vec3(-1.0f, -1.0f, 1.0f), bx::Vec3(1.0f, -1.0f, 1.0f),
        bx::Vec3(1.0f, 1.0f, 1.0f),   bx::Vec3(-1.0f, 1.0f, 1.0f)
    };

    // Transform NDC to world space using the inverse view-projection matrix
    float worldCorners[8][4];
    for (int i = 0; i < 8; ++i) {
        float corner[4] = { corners[i].x, corners[i].y, corners[i].z, 1.0f };
        bx::vec4MulMtx(worldCorners[i], corner, invViewProj);
        float w = worldCorners[i][3];
        if (w != 0.0f) {
            worldCorners[i][0] /= w; // Normalize by w
            worldCorners[i][1] /= w;
            worldCorners[i][2] /= w;
        }
    }

    auto toJolt = [](const float* v) { return JPH::RVec3(v[0], v[1], v[2]); };

    // Draw the frustum lines
    DrawLine(toJolt(worldCorners[0]), toJolt(worldCorners[1]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[1]), toJolt(worldCorners[2]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[2]), toJolt(worldCorners[3]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[3]), toJolt(worldCorners[0]), JPH::Color::sWhite);
    // Far plane
    DrawLine(toJolt(worldCorners[4]), toJolt(worldCorners[5]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[5]), toJolt(worldCorners[6]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[6]), toJolt(worldCorners[7]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[7]), toJolt(worldCorners[4]), JPH::Color::sWhite);
    // Connecting edges
    DrawLine(toJolt(worldCorners[0]), toJolt(worldCorners[4]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[1]), toJolt(worldCorners[5]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[2]), toJolt(worldCorners[6]), JPH::Color::sWhite);
    DrawLine(toJolt(worldCorners[3]), toJolt(worldCorners[7]), JPH::Color::sWhite);
}

void DebugRender::DrawOtherFrustum(const float* view, const float* proj) {
    float viewMtx[16], projMtx[16];

    memcpy(&viewMtx, view, sizeof(float) * 16);
    memcpy(&projMtx, proj, sizeof(float) * 16);

    float viewProj[16];
    bx::mtxMul(viewProj, viewMtx, projMtx);

    float invViewProj[16];
    bx::mtxInverse(invViewProj, viewProj);

    // Define 8 corners of the normalized device coordinates (NDC) frustum
    // We use Z from 0-1, standard for all but OGL for some fucking reason
    const bx::Vec3 corners[8] = {
        bx::Vec3(-1.0f, -1.0f, 0.0f), bx::Vec3(1.0f, -1.0f, 0.0f),
        bx::Vec3(1.0f, 1.0f, 0.0f),   bx::Vec3(-1.0f, 1.0f, 0.0f),
        bx::Vec3(-1.0f, -1.0f, 1.0f), bx::Vec3(1.0f, -1.0f, 1.0f),
        bx::Vec3(1.0f, 1.0f, 1.0f),   bx::Vec3(-1.0f, 1.0f, 1.0f)
    };

    // Transform NDC to world space using the inverse view-projection matrix
    float worldCorners[8][4];
    for (int i = 0; i < 8; ++i) {
        float corner[4] = { corners[i].x, corners[i].y, corners[i].z, 1.0f };
        bx::vec4MulMtx(worldCorners[i], corner, invViewProj);
        float w = worldCorners[i][3];
        if (w != 0.0f) {
            worldCorners[i][0] /= w; // Normalize by w
            worldCorners[i][1] /= w;
            worldCorners[i][2] /= w;
        }
    }

    auto toJolt = [](const float* v) { return JPH::RVec3(v[0], v[1], v[2]); };

    // Draw the frustum lines
    DrawLine(toJolt(worldCorners[0]), toJolt(worldCorners[1]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[1]), toJolt(worldCorners[2]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[2]), toJolt(worldCorners[3]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[3]), toJolt(worldCorners[0]), JPH::Color::sPurple);
    // Far plane
    DrawLine(toJolt(worldCorners[4]), toJolt(worldCorners[5]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[5]), toJolt(worldCorners[6]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[6]), toJolt(worldCorners[7]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[7]), toJolt(worldCorners[4]), JPH::Color::sPurple);
    // Connecting edges
    DrawLine(toJolt(worldCorners[0]), toJolt(worldCorners[4]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[1]), toJolt(worldCorners[5]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[2]), toJolt(worldCorners[6]), JPH::Color::sPurple);
    DrawLine(toJolt(worldCorners[3]), toJolt(worldCorners[7]), JPH::Color::sPurple);
}


void DebugRender::RenderLines(const float*        view,
                              const float*        proj,
                              int                 width,
                              int                 height,
                              bgfx::ProgramHandle program) {
    unsigned short viewId = ViewID::VIEW_DEBUG; // Why the hell does anyone use shorts
    bgfx::setViewTransform(viewId, view, proj);
    bgfx::setViewRect(viewId, 0, 0, width, height);
    bgfx::touch(viewId);

    if (debugLines.empty()) return;

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(
        &tvb, debugLines.size() * 2, DebugVertexLayout);

    DebugVertex* vertex = (DebugVertex*)tvb.data;
    for (size_t i = 0; i < debugLines.size(); ++i) {
        const DebugLine& line = debugLines[i];
        vertex[i * 2 + 0]     = { line.from.x, line.from.y, line.from.z, line.color };
        vertex[i * 2 + 1] = { line.to.x, line.to.y, line.to.z, line.color };
    }

    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_PT_LINES | BGFX_STATE_DEPTH_TEST_ALWAYS); // may change this later to LEQUAL
    bgfx::submit(viewId, program);
}

void DebugRender::ClearLines() {
    debugLines.clear();
}

} // namespace Syngine