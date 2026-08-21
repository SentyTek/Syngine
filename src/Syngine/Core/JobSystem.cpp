// ╒══════════════════════ JobSystem.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Core/JobSystem.h>
#include <Syngine/Core/Logger.h>

#include <cstdint>

namespace Syngine {

JobSystem::JobSystem()
    : m_workerCount(CalculateWorkerCount()),
      m_joltWorkerCount(CalculateJoltWorkerCount()), m_server(m_workerCount) {
    Logger::LogF(LogLevel::INFO,
                 true,
                 "Created job system with %u workers (%u for Jolt)",
                 m_workerCount,
                 m_joltWorkerCount);
    m_server.start();
}

JobSystem::~JobSystem() { m_server.stop(); }

void JobSystem::DispatchAdvanced(
    std::function<void(const scl::jobs::JobWorker&)> func) {
    m_server.submitJob(func);
}

void JobSystem::ParallelFor(
    size_t count, std::function<void(size_t begin, size_t end)> func) {
    if (count == 0) {
        return;
    }

    const int workers =
        std::min(static_cast<int>(count), static_cast<int>(m_workerCount));

    scl::jobs::JobServer::Multithread(
        [count, func](int id, int workers) {
            const size_t begin =
                count * static_cast<size_t>(id) / static_cast<size_t>(workers);
            const size_t end = count * static_cast<size_t>(id + 1) /
                               static_cast<size_t>(workers);
            func(begin, end);
        },
        workers);
}

uint32_t JobSystem::GetWorkerCount() { return m_workerCount; }

} // namespace Syngine
