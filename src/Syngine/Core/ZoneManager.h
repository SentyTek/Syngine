// ╒══════════════════════ ZoneManager.h ═╕
// │ Syngine                              │
// │ Created 2025-10-02                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once
#include "Syngine/ECS/Components/ZoneComponent.h"

#include <vector>

namespace Syngine {
/// @brief ZoneManager is responsible for managing all ZoneComponents in the
/// game.
/// @section ZoneManager
/// @internal
class ZoneManager {
    std::vector<ZoneComponent*> m_zones; //* List of all zones in the game
  public:
    
    /// @brief Registers a zone with the manager.
    /// @param zone The zone to register.
    /// @since v0.0.1
    /// @threadsafety not-safe
    /// @internal
    /// @note This is called automatically when a ZoneComponent is created.
    void _RegisterZone(ZoneComponent* zone);

    /// @brief Unregisters a zone from the manager.
    /// @param zone The zone to unregister.
    /// @since v0.0.1
    /// @threadsafety not-safe
    /// @internal
    /// @note This is called automatically when a ZoneComponent is destroyed.
    void _UnregisterZone(ZoneComponent* zone);

    /// @brief Updates all zones, checking for GameObjects entering or exiting
    /// zones and triggering events as necessary.
    /// @since v0.0.1
    /// @threadsafety not-safe
    /// @internal
    void _UpdateZones();
};

} // namespace Syngine
