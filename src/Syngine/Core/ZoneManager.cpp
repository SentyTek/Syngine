// ╒════════════════════ ZoneManager.cpp ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
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

            // Handle one-shot zones
            // Not sure if this'll work
            if (inZone && !zone->_HasTriggeredObject(objPtr)) {
                zone->OnEnter(objPtr);
                if (zone->IsOneShot()) zone->_AddTriggeredObject(objPtr);
            } else if (!inZone && zone->_HasTriggeredObject(objPtr)) {
                zone->OnExit(objPtr);
                if (zone->IsOneShot()) zone->_RemoveTriggeredObject(objPtr);
            }
        }
    }
}

} // namespace Syngine
