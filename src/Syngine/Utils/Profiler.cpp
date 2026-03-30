// ╒═══════════════════════ Profiler.cpp ═╕
// │ Syngine                              │
// │ Created 2026-01-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// | Licensed under the MIT License       |
// ╰──────────────────────────────────────╯

#ifndef SYN_DEBUG_GRAPHICS
#include "Syngine/Utils/Profiler.h"
#include "Syngine/Utils/FsUtils.h"
#include "Syngine/Core/Core.h"

#include <fstream>
#include <thread>
#include <chrono>
#include <functional>

namespace Syngine {

thread_local std::vector<const char*> Profiler::m_nameStack;
thread_local std::vector<SpanEvent>   Profiler::m_threadData;
thread_local std::vector<SpanEvent>   Profiler::m_lastFrameData;
thread_local uint8_t                  Profiler::m_currentDepth = 0;

std::vector<TimerEvent> Profiler::m_timers;
int Profiler::m_nextTimerID = 0;

void Profiler::ProfilerUI::SaveCapture(const std::string& filepath) {
    std::filesystem::path realPath = Syngine::_GetAppdataPath(Core::_GetApp()->config.windowTitle) / filepath;
    std::ofstream out(realPath);
    out << "["; // Start JSON array

    // Flush all thread buffers
    // (In a real engine, you'd iterate over all registered thread buffers)
    const auto& events = m_lastFrameData;

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& e = events[i];

        // Convert your internal Type to Chrome's "phase"
        char phase = (e.type == 0) ? 'B' : 'E';

        out << "{"
            << "\"name\":\"" << (e.name ? e.name : "") << "\","
            << "\"cat\":\"PERF\","
            << "\"ph\":\"" << phase << "\","
            << "\"pid\":0,"
            << "\"tid\":" << e.threadID << ","
            << "\"ts\":" << e.timestamp // Microseconds works best
            << "}";

        if (i < events.size() - 1) out << ",";
        out << "\n";
    }

    out << "]"; // End JSON array
    out.close();
}

void Profiler::PushEvent(const char* name, EventType type) {
    SpanEvent event;
    event.name = name;
    event.threadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    event.timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    );
    event.type = static_cast<uint8_t>(type);
    event.depth = m_currentDepth++;

    m_threadData.push_back(event);

    if (type == Profiler::EventType::EVENT_END && m_currentDepth > 0) {
        m_currentDepth--;
    }
}

void Profiler::Reset() {
    m_lastFrameData = m_threadData;
    m_threadData.clear();
    m_nameStack.clear();
    m_currentDepth = 0;

    m_threadData.reserve(4096); // Pre-allocate for performance
}

const std::vector<SpanEvent>& Profiler::GetThreadData() { return m_threadData; }

int Profiler::StartTimer(const char* name) {
    SpanEvent startEvent;
    startEvent.name = name;
    startEvent.threadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    startEvent.timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    );
    startEvent.type = static_cast<uint8_t>(EventType::EVENT_START);
    startEvent.depth = 0; // Timers are independent of call stack depth

    TimerEvent timerEvent;
    timerEvent.startEvent = startEvent;

    m_timers.push_back(timerEvent);
    return m_nextTimerID++;
}

void Profiler::EndTimer(int timerID) {
    if (timerID < 0 || timerID >= m_nextTimerID) {
        // Invalid timer ID
        return;
    }

    SpanEvent endEvent;
    endEvent.name = m_timers[timerID].startEvent.name;
    endEvent.threadID = static_cast<uint32_t>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    endEvent.timestamp = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    );
    endEvent.type = static_cast<uint8_t>(EventType::EVENT_END);
    endEvent.depth = 0; // Timers are independent of call stack depth

    m_timers[timerID].endEvent = endEvent;
}

} // namespace Syngine
#endif
