// ╒══════════════════ ThreadManager.cpp ═╕
// │ Syngine                              │
// │ Created 2026-08-19                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#include <Syngine/Core/ThreadManager.h>
#include <mutex>
#include <stop_token>

namespace Syngine {

Thread& ThreadManager::Create(std::string            name,
                              Thread::ThreadFunction function) noexcept {
    m_threads.push_back(
        std::make_unique<Thread>(std::move(name), std::move(function)));
    return *m_threads.back();
}

void ThreadManager::StopAll() {
    for (auto& thread : m_threads) {
        thread->Stop();
    }
}

void ThreadManager::JoinAll() {
    for (auto& thread : m_threads) {
        thread->Join();
    }
}

void Thread::Start() {
    if (!m_thread.joinable()) {
        m_thread =
            std::jthread([this](std::stop_token st) { m_function(st, *this); });
    }
}

void Thread::Stop() {
    if (m_thread.joinable()) {
        m_thread.request_stop();
    }
}

void Thread::Join() {
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void Thread::Wake() {
    {
        std::lock_guard lock(m_mutex);
        m_wakeRequested = true;
    }
    m_cv.notify_one();
}

void Thread::Wait(std::stop_token stop) {
    std::unique_lock lock(m_mutex);
    m_cv.wait(lock, stop, [this] { return m_wakeRequested; });
    m_wakeRequested = false;
}

bool Thread::IsRunning() const { return m_thread.joinable(); }

} // namespace Syngine
