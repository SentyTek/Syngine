// ╒══════════════════ DebugRenderer.cpp ═╕
// │ Syngine                              │
// │ Created 2025-06-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#include "Syngine/Graphics/Renderer.h"
#include "Syngine/Graphics/DebugRenderer.h"
#include "Syngine/ECS/Components/CameraComponent.h"

#include "Jolt/Physics/Collision/Shape/Shape.h"
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

void DebugRender::DrawSphere(JPH::RVec3Arg center, float radius, JPH::ColorArg color) {
    const int segments = 32; // Number of segments for the circle approximation
    const float step   = (2.0f * 3.14159265f) / segments;

    // Draw circles in XY, YZ, and XZ planes
    for (int i = 0; i < segments; ++i) {
        float theta1 = i * step;
        float theta2 = (i + 1) * step;

        // XY plane
        DrawLine(JPH::RVec3(center.GetX() + radius * cosf(theta1),
                            center.GetY() + radius * sinf(theta1),
                            center.GetZ()),
                 JPH::RVec3(center.GetX() + radius * cosf(theta2),
                            center.GetY() + radius * sinf(theta2),
                            center.GetZ()),
                 color);

        // YZ plane
        DrawLine(JPH::RVec3(center.GetX(),
                            center.GetY() + radius * cosf(theta1),
                            center.GetZ() + radius * sinf(theta1)),
                 JPH::RVec3(center.GetX(),
                            center.GetY() + radius * cosf(theta2),
                            center.GetZ() + radius * sinf(theta2)),
                 color);

        // XZ plane
        DrawLine(JPH::RVec3(center.GetX() + radius * cosf(theta1),
                            center.GetY(),
                            center.GetZ() + radius * sinf(theta1)),
                 JPH::RVec3(center.GetX() + radius * cosf(theta2),
                            center.GetY(),
                            center.GetZ() + radius * sinf(theta2)),
                 color);
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
    DrawFrustum(camera.view, camera.proj);
}

void DebugRender::DrawFrustum(const Math::Mat4& view, const Math::Mat4& proj) {
   Math::Mat4 invViewProj = (proj * view).inverse();

    // Define 8 corners of the normalized device coordinates (NDC) frustum
    // We use Z from 0-1, standard for all but OGL for some fucking reason
    const bx::Vec3 corners[8] = {
        bx::Vec3(-1.0f, -1.0f, 0.0f), bx::Vec3(1.0f, -1.0f, 0.0f),
        bx::Vec3(1.0f, 1.0f, 0.0f),   bx::Vec3(-1.0f, 1.0f, 0.0f),
        bx::Vec3(-1.0f, -1.0f, 1.0f), bx::Vec3(1.0f, -1.0f, 1.0f),
        bx::Vec3(1.0f, 1.0f, 1.0f),   bx::Vec3(-1.0f, 1.0f, 1.0f)
    };

    // Transform NDC to world space using the inverse view-projection matrix
    Math::Vector4 worldCorners[8];
    for (int i = 0; i < 8; ++i) {
        Math::Vector4 corner(corners[i], 1.0f);
        worldCorners[i] = invViewProj * corner;
        float w = worldCorners[i].w();
        if (w != 0.0f) {
            worldCorners[i].setX(worldCorners[i].x() / w); // Normalize by w
            worldCorners[i].setY(worldCorners[i].y() / w);
            worldCorners[i].setZ(worldCorners[i].z() / w);
        }
    }

    // Draw the frustum lines
    DrawLine(worldCorners[0].toJoltVec3(), worldCorners[1].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[1].toJoltVec3(), worldCorners[2].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[2].toJoltVec3(), worldCorners[3].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[3].toJoltVec3(), worldCorners[0].toJoltVec3(), JPH::Color::sWhite);
    // Far plane
    DrawLine(worldCorners[4].toJoltVec3(), worldCorners[5].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[5].toJoltVec3(), worldCorners[6].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[6].toJoltVec3(), worldCorners[7].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[7].toJoltVec3(), worldCorners[4].toJoltVec3(), JPH::Color::sWhite);
    // Connecting edges
    DrawLine(worldCorners[0].toJoltVec3(), worldCorners[4].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[1].toJoltVec3(), worldCorners[5].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[2].toJoltVec3(), worldCorners[6].toJoltVec3(), JPH::Color::sWhite);
    DrawLine(worldCorners[3].toJoltVec3(), worldCorners[7].toJoltVec3(), JPH::Color::sWhite);
}

void DebugRender::DrawBox(const Math::Vector3& min,
                          const Math::Vector3& max,
                          JPH::ColorArg        color) {
    Math::Vector3 v0 = min;
    Math::Vector3 v1 = Math::Vector3(max.x(), min.y(), min.z());
    Math::Vector3 v2 = Math::Vector3(max.x(), max.y(), min.z());
    Math::Vector3 v3 = Math::Vector3(min.x(), max.y(), min.z());
    Math::Vector3 v4 = Math::Vector3(min.x(), min.y(), max.z());
    Math::Vector3 v5 = Math::Vector3(max.x(), min.y(), max.z());
    Math::Vector3 v6 = max;
    Math::Vector3 v7 = Math::Vector3(min.x(), max.y(), max.z());

    // Bottom face
    DrawLine(v0.toJoltVec3(), v1.toJoltVec3(), color);
    DrawLine(v1.toJoltVec3(), v2.toJoltVec3(), color);
    DrawLine(v2.toJoltVec3(), v3.toJoltVec3(), color);
    DrawLine(v3.toJoltVec3(), v0.toJoltVec3(), color);
    // Top face
    DrawLine(v4.toJoltVec3(), v5.toJoltVec3(), color);
    DrawLine(v5.toJoltVec3(), v6.toJoltVec3(), color);
    DrawLine(v6.toJoltVec3(), v7.toJoltVec3(), color);
    DrawLine(v7.toJoltVec3(), v4.toJoltVec3(), color);
    // Vertical edges
    DrawLine(v0.toJoltVec3(), v4.toJoltVec3(), color);
    DrawLine(v1.toJoltVec3(), v5.toJoltVec3(), color);
    DrawLine(v2.toJoltVec3(), v6.toJoltVec3(), color);
    DrawLine(v3.toJoltVec3(), v7.toJoltVec3(), color);
}

void DebugRender::RenderLines(const Math::Mat4&   view,
                              const Math::Mat4&   proj,
                              int                 width,
                              int                 height,
                              bgfx::ProgramHandle program) {
    if (debugLines.empty()) return;

    unsigned short viewId = ViewID::VIEW_DEBUG; // Why the hell does anyone use shorts
    bgfx::setViewTransform(viewId, view.data(), proj.data());
    bgfx::setViewRect(viewId, 0, 0, width, height);
    bgfx::touch(viewId);

    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(
        &tvb, static_cast<uint32_t>(debugLines.size() * 2), DebugVertexLayout);

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
