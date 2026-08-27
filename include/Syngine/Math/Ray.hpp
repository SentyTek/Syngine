// ╒════════════════════════════ Ray.hpp ═╕
// │ Syngine                              │
// │ Created 2026-07-06                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <DirectXMath.h>

#include "Syngine/Math/Quaternion.hpp"
#include "Syngine/Math/Vector3.hpp"

namespace Syngine::Math {

/// @brief Ray class representing a ray in 3D space
class Ray {
    DirectX::XMFLOAT3 m_origin;
    DirectX::XMFLOAT3 m_direction;

  public:
    // MARK: Constructors

    inline Ray() = default;
    inline Ray(const Vector3& origin, const Quaternion& direction)
        : m_origin(origin.m_getStorage()),
          m_direction(direction.toEulerAngles().x(),
                      direction.toEulerAngles().y(),
                      direction.toEulerAngles().z()) {}

    inline Ray(const Vector3& origin, const Vector3& direction)
        : m_origin(origin.m_getStorage()),
          m_direction(direction.m_getStorage()) {}

    // MARK: Accessors

    inline Vector3 origin() const { return Vector3(m_origin); }
    inline Vector3 direction() const { return Vector3(m_direction); }

    inline void setOrigin(const Vector3& origin) {
        m_origin = origin.m_getStorage();
    }
    inline void setDirection(const Quaternion& direction) {
        Vector3 euler = direction.toEulerAngles();
        m_direction.x = euler.x();
        m_direction.y = euler.y();
        m_direction.z = euler.z();
    }
    inline void setDirection(const Vector3& direction) {
        m_direction = direction.m_getStorage();
    }

    // MARK: Ray operations

    inline Vector3 pointAt(float t) const {
        DirectX::XMVECTOR o = DirectX::XMLoadFloat3(&m_origin);
        DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&m_direction);
        DirectX::XMVECTOR p = DirectX::XMVectorAdd(o, DirectX::XMVectorScale(d, t));
        Vector3 res;
        DirectX::XMStoreFloat3(&res.m_storage, p);
        return res;
    }

    inline Vector3 closestPoint(const Vector3& point) const {
        DirectX::XMVECTOR o = DirectX::XMLoadFloat3(&m_origin);
        DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&m_direction);
        DirectX::XMFLOAT3 pointStorage = point.m_getStorage();
        DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pointStorage);
        DirectX::XMVECTOR op = DirectX::XMVectorSubtract(p, o);
        float t = DirectX::XMVectorGetX(DirectX::XMVector3Dot(op, d));
        return pointAt(t);
    }

    inline float distanceTo(const Vector3& point) const {
        Vector3 closest = closestPoint(point);
        DirectX::XMFLOAT3 closestStorage = closest.m_getStorage();
        DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&closestStorage);
        DirectX::XMFLOAT3 pointStorage = point.m_getStorage();
        DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&pointStorage);
        DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(p, c);
        return DirectX::XMVectorGetX(DirectX::XMVector3Length(diff));
    }

    inline bool intersects(const Vector3& point, float radius) const {
        return distanceTo(point) <= radius;
    }

    inline bool intersectsSphere(const Vector3& center, float radius) const {
        return distanceTo(center) <= radius;
    }

    inline bool intersectsAABB(const Vector3& min, const Vector3& max) const {
        // Using the slab method for ray-AABB intersection
        DirectX::XMFLOAT3 minStorage = min.m_getStorage();
        DirectX::XMFLOAT3 maxStorage = max.m_getStorage();
        DirectX::XMVECTOR o = DirectX::XMLoadFloat3(&m_origin);
        DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&m_direction);
        DirectX::XMVECTOR minVec = DirectX::XMLoadFloat3(&minStorage);
        DirectX::XMVECTOR maxVec = DirectX::XMLoadFloat3(&maxStorage);

        DirectX::XMVECTOR invD = DirectX::XMVectorReciprocal(d);
        DirectX::XMVECTOR t1 = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(minVec, o), invD);
        DirectX::XMVECTOR t2 = DirectX::XMVectorMultiply(DirectX::XMVectorSubtract(maxVec, o), invD);

        DirectX::XMVECTOR tMin = DirectX::XMVectorMin(t1, t2);
        DirectX::XMVECTOR tMax = DirectX::XMVectorMax(t1, t2);

        float tMinVal = DirectX::XMVectorGetX(DirectX::XMVectorMax(tMin, tMax));
        float tMaxVal = DirectX::XMVectorGetY(DirectX::XMVectorMin(tMin, tMax));

        return tMaxVal >= 0.0f && tMinVal <= tMaxVal;
    }

    inline bool intersectsPlane(const Vector3& planeNormal, float planeDistance) const {
        DirectX::XMFLOAT3 normalStorage = planeNormal.m_getStorage();
        DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normalStorage);
        DirectX::XMVECTOR o = DirectX::XMLoadFloat3(&m_origin);
        DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&m_direction);

        float denom = DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, d));
        if (fabs(denom) < 1e-6f) {
            // Ray is parallel to the plane
            return false;
        }

        float t = (planeDistance - DirectX::XMVectorGetX(DirectX::XMVector3Dot(n, o))) / denom;
        return t >= 0.0f;
    }

    inline bool intersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) const {
        // Möller–Trumbore intersection algorithm
        DirectX::XMFLOAT3 v0Storage = v0.m_getStorage();
        DirectX::XMFLOAT3 v1Storage = v1.m_getStorage();
        DirectX::XMFLOAT3 v2Storage = v2.m_getStorage();

        DirectX::XMVECTOR o = DirectX::XMLoadFloat3(&m_origin);
        DirectX::XMVECTOR d = DirectX::XMLoadFloat3(&m_direction);
        DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&v0Storage);
        DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&v1Storage);
        DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&v2Storage);

        DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(p1, p0);
        DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(p2, p0);
        DirectX::XMVECTOR h = DirectX::XMVector3Cross(d, edge2);
        float a = DirectX::XMVectorGetX(DirectX::XMVector3Dot(edge1, h));

        if (fabs(a) < 1e-6f) {
            return false; // Ray is parallel to the triangle
        }

        float f = 1.0f / a;
        DirectX::XMVECTOR s = DirectX::XMVectorSubtract(o, p0);
        float u = f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(s, h));

        if (u < 0.0f || u > 1.0f) {
            return false;
        }

        DirectX::XMVECTOR q = DirectX::XMVector3Cross(s, edge1);
        float v = f * DirectX::XMVectorGetX(DirectX::XMVector3Dot(d, q));

        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }

        float t = f * DirectX::XMVectorGetY(DirectX::XMVector3Dot(edge2, q));
        return t >= 0.0f;
    }
};

} // namespace Syngine::Math
