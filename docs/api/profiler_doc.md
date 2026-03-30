# Syngine API Documentation


## Profiler.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Utils/Profiler.h)

---

## Goto:


- [Member Variables](#member-variables)
- [Constructor](#class-constructor)
- [SpanEvent](#synginespanevent)
- [TimerEvent](#synginetimerevent)
- [EventType](#profilereventtype)
- [PushEvent](#profileruipushevent)
- [Reset](#profileruireset)
- [GetThreadData](#profileruigetthreaddata)
- [StartTimer](#profileruistarttimer)
- [EndTimer](#profileruiendtimer)

---

## Class Constructor


#### **`Profiler::Thread`**


 Profiler utility class for performance profiling. Helpful for debugging potential bottlenecks.

Signature:

```cpp
 // Thread-local storage for profiler events to avoid contention static thread_local std::vector<SpanEvent> m_threadData; //* List of events
```

**This function has been available since:** v0.0.1

---

## Class & Related Members


#### **`Syngine::SpanEvent`**


 Span POD struct for profiling. Contains name, start time, and end time.

Signature:

```cpp
struct SpanEvent
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `const` | `char*` | name 8 byte pointer to name string |
| `uint32_t` | `threadID` | Thread ID |
| `uint64_t` | `timestamp` | Timestamp in microseconds |
| `uint8_t` | `type` | 0 = start, 1 = end |
| `uint8_t` | `depth` | Depth in call stack |
| `uint8_t[10]` | `padding` | 32 byte alignment |

---

#### **`Syngine::TimerEvent`**


 Timer event struct for paired start/end events

Signature:

```cpp
struct TimerEvent
```

**Members:**

| Type | Name | Description |
| --- | --- | --- |
| `SpanEvent` | `startEvent` | Start event |
| `SpanEvent` | `endEvent` | End event |

---

#### **`Profiler::EventType`**


 Event types for profiling

Signature:

```cpp
 enum class EventType : uint8_t
```

**Members:**

| Name | Description |
| --- | --- |
| `EVENT_START` | Start. |
| `EVENT_END` | End. |

**This function has been available since:** v0.0.1

---

#### **`ProfilerUI::PushEvent`**


 Push a profiling event onto the thread-local stack

**Note:** This is called automatically by ProfilerScope. You usually don't need to call this directly.

Signature:

```cpp
 static void PushEvent(const char* name, EventType type);
```

**Parameters:**

- `name`: Name of the event
- `type`: Type of the event (start or end)

**This function has been available since:** v0.0.1

---

#### **`ProfilerUI::Reset`**


 Reset the profiler for the next frame

Signature:

```cpp
 static void Reset();
```

**This function has been available since:** v0.0.1

---

#### **`ProfilerUI::GetThreadData`**


 Get the thread-local profiling data

Signature:

```cpp
 static const std::vector<SpanEvent>& GetThreadData();
```

**Returns:** Vector of SpanEvent for the current thread

**This function has been available since:** v0.0.1

---

#### **`ProfilerUI::StartTimer`**


 Start a named timer

Signature:

```cpp
 static int StartTimer(const char* name);
```

**Parameters:**

- `name`: Name of the timer

**Returns:** Timer ID to be used with EndTimer

**This function has been available since:** v0.0.1

---

#### **`ProfilerUI::EndTimer`**


 End a timer by ID

Signature:

```cpp
 static void EndTimer(int); // End a timer by ID
```

**Parameters:**

- `timerID`: ID of the timer to end

**This function has been available since:** v0.0.1

---

## Member Variables


| Type | Name | Description |
| --- | --- | --- |
| `thread_local` | `std` | Last frame's events |
| `thread_local` | `std` | Call stack names |
| `thread_local` | `uint8_t` | Call stack depth |
| `std::vector<TimerEvent>` | `m_timers` | Paired start/end events for StartTimer/EndTimer |
| `int` | `m_nextTimerID` | Next timer ID |

---
