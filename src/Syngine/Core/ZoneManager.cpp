// ╒════════════════════ ZoneManager.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Core/ZoneManager.h"
#include "Syngine/Core/Registry.h"

namespace Syngine {

std::vector<ZoneComponent*> ZoneManager::m_zones;

void ZoneManager::_RegisterZone(ZoneComponent* zone) noexcept {
    m_zones.push_back(zone);
}

void ZoneManager::_UnregisterZone(ZoneComponent* zone) noexcept {
    m_zones.erase(std::remove(m_zones.begin(), m_zones.end(), zone),
                  m_zones.end());
}

void ZoneManager::_UpdateZones() {
    for (auto* zone : m_zones) {
        if (!zone->IsActive()) continue;

        auto gameObjects = Registry::GetAllGameObjects();
        for (auto obj : gameObjects) {
            GameObject* objPtr = obj.second;
            if (objPtr == zone->_GetOwner()) continue;

            bool inZone = zone->IsInZone(objPtr);

            // Handle zones
            // If an object is in, and not already tracked, it just entered.
            // Or, if an object is in, and is in the onetime list, don't do
            // anything If an object is not in, and is tracked, it just exited.
            if (inZone && !zone->_IsTrackingObject(objPtr) && !zone->_HasOneTimeObject(objPtr)) {
                zone->OnEnter(objPtr);
                zone->_AddObject(objPtr);
            } else if (!inZone && zone->_IsTrackingObject(objPtr)) {
                zone->OnExit(objPtr);
                zone->_RemoveObject(objPtr);
            }
        }
    }
}

const std::vector<ZoneComponent*>& ZoneManager::GetZones() {
    return m_zones;
}

} // namespace Syngine
