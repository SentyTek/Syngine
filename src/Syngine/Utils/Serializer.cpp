// ╒═════════════════════ Serializer.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-29                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#include "Syngine/Utils/Serializer.h"
#include "Syngine/Utils/FsUtils.h"

namespace Syngine {
namespace Internal {
std::string ResolvePath(const char* path) { return _ResolveOSPath(path); }
} // namespace Internal

bool Serializer::DataNode::Has(const std::string& key) const {
    if (!std::holds_alternative<NodeMap>(m_data)) return false;
    const auto& map = std::get<NodeMap>(m_data);
    return map.find(key) != map.end();
}

} // namespace Syngine
