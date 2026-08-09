// ╒════════════════════ ZoneSystem.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include "Syngine/Scene/ZoneSystem.h"
#include "Syngine/Scene/GameObjectRegistry.h"

namespace Syngine {

std::vector<ZoneComponent*> ZoneSystem::m_zones;

void ZoneSystem::_RegisterZone(ZoneComponent* zone) noexcept {
    m_zones.push_back(zone);
}

void ZoneSystem::_UnregisterZone(ZoneComponent* zone) noexcept {
    m_zones.erase(std::remove(m_zones.begin(), m_zones.end(), zone),
                  m_zones.end());
}

void ZoneSystem::_UpdateZones() {
    for (auto* zone : m_zones) {
        if (!zone->IsActive() || !zone->_GetOwner()->IsActive()) continue;

        auto gameObjects = GameObjectRegistry::GetAllGameObjects();
        for (auto obj : gameObjects) {
            GameObject* objPtr = obj.second;
            if (objPtr == zone->_GetOwner() || !objPtr->IsActive()) continue;

            bool inZone = zone->IsInZone(objPtr);

            // Handle zones
            // If an object is in, and not already tracked, it just entered.
            // Or, if an object is in, and is in the onetime list, don't do
            // anything If an object is not in, and is tracked, it just exited.
            if (inZone && !zone->_IsTrackingObject(objPtr) &&
                !zone->_HasOneTimeObject(objPtr)) {
                if (zone->OnEnter) zone->OnEnter(objPtr);
                zone->_AddObject(objPtr);
            } else if (!inZone && zone->_IsTrackingObject(objPtr)) {
                if (zone->OnExit) zone->OnExit(objPtr);
                zone->_RemoveObject(objPtr);
            }
        }
    }
}

const std::vector<ZoneComponent*>& ZoneSystem::GetZones() { return m_zones; }

} // namespace Syngine
