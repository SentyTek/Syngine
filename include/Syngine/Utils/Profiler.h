// ╒═════════════════════════ Profiler.h ═╕
// │ Syngine                              │
// │ Created 2026-01-04                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯

#pragma once

#include <cstdint>
#include <vector>
#include <string>

#define SYN_PROFILE_SCOPE(name)                                                \
    Syngine::Profiler::ProfilerScope timer##__LINE__(name);

#define SYN_PROFILE_FUNCTION() SYN_PROFILE_SCOPE(__FUNCTION__);

namespace Syngine {

/// @brief Span POD struct for profiling. Contains name, start time, and end
/// time.
/// @section Profiler
struct SpanEvent {
    const char* name; //* 8 byte pointer to name string
    uint32_t    threadID; //* Thread ID
    uint64_t    timestamp; //* Timestamp in microseconds
    uint8_t     type; //* 0 = start, 1 = end
    uint8_t     depth; //* Depth in call stack
    uint8_t     padding[10]; //* 32 byte alignment
};

/// @brief Timer event struct for paired start/end events
/// @section Profiler
struct TimerEvent {
    SpanEvent startEvent; //* Start event
    SpanEvent endEvent;   //* End event
    uint64_t GetDuration() const {
        return endEvent.timestamp - startEvent.timestamp;
    }
};

/// @brief Profiler utility class for performance profiling. Helpful for
/// debugging potential bottlenecks.
/// @section Profiler
/// @since v0.0.1
class Profiler {
    // Thread-local storage for profiler events to avoid contention
    static thread_local std::vector<SpanEvent> m_threadData; //* List of events
    static thread_local std::vector<SpanEvent> m_lastFrameData; //* Last frame's events
    static thread_local std::vector<const char*> m_nameStack; //* Call stack names
    static thread_local uint8_t m_currentDepth; //* Call stack depth

    static std::vector<TimerEvent> m_timers; //* Paired start/end events for StartTimer/EndTimer
    static int m_nextTimerID; //* Next timer ID

    /// @brief Event types for profiling
    /// @section Profiler
    /// @since v0.0.1
    enum class EventType : uint8_t {
        EVENT_START = 0, //* Start.
        EVENT_END = 1 //* End.
    };

    friend class RenderCore;
  public:
    class ProfilerScope {
      public:
        inline ProfilerScope(const char* name) {
            m_nameStack.push_back(name);
            Profiler::PushEvent(name, EventType::EVENT_START);
        }

        inline ~ProfilerScope() {
            Profiler::PushEvent(m_nameStack.back(), EventType::EVENT_END);
            m_nameStack.pop_back();
        }
    };
    class ProfilerUI {
      public:
        static void Render(); // TBD
        static void SaveCapture(const std::string& path);
    };

    /// @brief Push a profiling event onto the thread-local stack
    /// @param name Name of the event
    /// @param type Type of the event (start or end)
    /// @section Profiler
    /// @since v0.0.1
    /// @note This is called automatically by ProfilerScope. You usually don't
    /// need to call this directly.
    static void PushEvent(const char* name, EventType type);

    /// @brief Reset the profiler for the next frame
    /// @section Profiler
    /// @since v0.0.1
    static void Reset();

    /// @brief Get the thread-local profiling data
    /// @return Vector of SpanEvent for the current thread
    /// @section Profiler
    /// @since v0.0.1
    static const std::vector<SpanEvent>& GetThreadData();

    /// @brief Start a named timer
    /// @param name Name of the timer
    /// @return Timer ID to be used with EndTimer
    /// @section Profiler
    /// @since v0.0.1
    static int StartTimer(const char* name);

    /// @brief End a timer by ID
    /// @param timerID ID of the timer to end
    /// @section Profiler
    /// @since v0.0.1
    static void EndTimer(int); // End a timer by ID
};

} // namespace Syngine
