// ╒════════════════════ ThreadManager.h ═╕
// │ Syngine                              │
// │ Created 2026-08-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include <condition_variable>
#include <memory>
#include <stop_token>
#include <thread>
#include <string>
#include <functional>
#include <mutex>

namespace Syngine {

class Thread {
  public:
    using ThreadFunction = std::function<void(std::stop_token, Thread&)>;

    Thread(std::string name, ThreadFunction function)
        : m_name(std::move(name)), m_function(std::move(function)) {}
    ~Thread() {
        Stop();
        Join();
    }

    Thread(const Thread&)            = delete;
    Thread& operator=(const Thread&) = delete;

    /// @brief Starts the thread and begins executing the thread function.
    /// @note This function should be called before any other operations on the
    /// thread.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void Start();

    /// @brief Stops the thread and requests it to exit.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void Stop();

    /// @brief Waits for the thread to finish execution and joins it.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void Join();

    /// @brief Wakes the thread if it is waiting.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void Wake();

    /// @brief Waits for the thread to be woken up or stopped.
    /// @param stop The stop token to allow early exit.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void Wait(std::stop_token stop);

    /// @brief Checks if the thread is currently running.
    /// @return True if the thread is running, false otherwise.
    /// @thread-safety not-safe
    /// @since v0.0.2
    bool IsRunning() const;

    /// @brief Gets the name of the thread.
    /// @return The name of the thread.
    /// @thread-safety not-safe
    /// @since v0.0.2
    const std::string& GetName() const { return m_name; }

  private:
    std::string    m_name;
    std::jthread   m_thread;
    ThreadFunction m_function;

    std::mutex                  m_mutex;
    std::condition_variable_any m_cv;
    bool                        m_wakeRequested = false;
};

class ThreadManager {
    std::vector<std::unique_ptr<Thread>> m_threads;

  public:
    /// @brief Creates a new thread with the given name and function.
    /// @param name The name of the thread.
    /// @param function The function to be executed by the thread.
    /// @return A reference to the created thread.
    /// @thread-safety not-safe
    /// @since v0.0.2
    Thread& Create(std::string name, Thread::ThreadFunction function) noexcept;

    /// @brief Stops all managed threads.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void StopAll();

    /// @brief Joins all managed threads.
    /// @thread-safety not-safe
    /// @since v0.0.2
    void JoinAll();
};

} // namespace Syngine
