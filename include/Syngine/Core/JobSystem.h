// ╒════════════════════════ JobSystem.h ═╕
// │ Syngine                              │
// │ Created 2026-08-12                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2026     │
// │ Licensed under the MIT License       │
// ╰──────────────────────────────────────╯

#pragma once

#include "Syngine/Core/Logger.h"
#include <Syngine/Core/JobSystem.h>

#include <miniscl.hpp>

#include <functional>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <memory>

namespace Syngine {

enum class JobCategory {
    General,
    AssetLoading,
    Physics,
    Rendering,
    Animation,
    Audio,
    Game,
    AI,
    Networking,
    UI,
    Count
};

template <typename T> struct JobState {
    T result;
};

template <typename T> class JobResult {
    scl::jobs::waitable*         m_waitable = nullptr;
    std::shared_ptr<JobState<T>> m_result;

  public:
    JobResult() : m_waitable(nullptr), m_result(nullptr) {}

    JobResult(scl::jobs::waitable*         waitable,
              std::shared_ptr<JobState<T>> result)
        : m_waitable(waitable), m_result(std::move(result)) {}

    ~JobResult() { delete m_waitable; }

    JobResult(const JobResult&)            = delete;
    JobResult& operator=(const JobResult&) = delete;

    JobResult(JobResult&& other) noexcept
        : m_waitable(std::exchange(other.m_waitable, nullptr)),
          m_result(std::move(other.m_result)) {}

    JobResult& operator=(JobResult&& other) noexcept {
        if (this != &other) {
            delete m_waitable;
            m_waitable = std::exchange(other.m_waitable, nullptr);
            m_result   = std::move(other.m_result);
        }
        return *this;
    }

    void Wait() { m_waitable->wait(); }

    T& Get() {
        Wait();
        return m_result->result;
    }

    const T& Get() const {
        Wait();
        return m_result->result;
    }

    bool IsComplete() const { return m_waitable && m_waitable->status(); }

    bool IsValid() const { return m_waitable != nullptr; }
};

/// @brief JobSystem class to work with jobs and multithreading. Dipsatch() is
/// best for async fire-and-forget CPU heavy jobs that do not require immediate
/// results. DispatchWithResult() is best for jobs that require a result to be
/// returned, and can be waited on. ParallelFor() is best for large
/// data-parellel workloads. It divides the workload across the engine's worker
/// threads and blocks until all work is complete.
/// @section JobSystem
class JobSystem {
    uint32_t             m_workerCount     = 0;
    uint32_t             m_joltWorkerCount = 0;
    scl::jobs::JobServer m_server;

    friend class Phys;

    // Since there are a minimum of 2 threads (main, bgfx), and Jolt is going to
    // want some, we need to split up threads accordingly.
    uint32_t CalculateAvailCount() const {
        const uint32_t hardware = std::thread::hardware_concurrency();

        constexpr uint32_t RESERVED =
            2; // Reserve 2 threads for main + rendering/os/driver headroom

        const uint32_t available =
            (hardware > RESERVED) ? (hardware - RESERVED) : 1;
        Syngine::Logger::LogF(Syngine::LogLevel::INFO,
                              true,
                              "Detected %u hardware threads, reserving %u for "
                              "main + rendering, leaving %u for engine",
                              hardware,
                              RESERVED,
                              available);

        return available;
    }

    uint32_t CalculateWorkerCount() {
        constexpr uint32_t MIN_WORKERS = 1;
        constexpr uint32_t MAX_WORKERS = 6;
        const uint32_t     available   = CalculateAvailCount();
        m_workerCount = std::clamp(available / 3, MIN_WORKERS, MAX_WORKERS);
        return m_workerCount;
    }

    uint32_t CalculateJoltWorkerCount() {
        const uint32_t available     = CalculateAvailCount();
        const uint32_t engineWorkers = CalculateWorkerCount();
        m_joltWorkerCount            = std::max(1u, available - engineWorkers);
        return m_joltWorkerCount;
    }

    int _GetJoltWorkers() const { return static_cast<int>(m_joltWorkerCount); }

  public:
    JobSystem();
    ~JobSystem();

    /// @brief Dispatch a job and return a handle to its result.
    /// @tparam F The type of the job function.
    /// @param func The job function to dispatch.
    /// @return A JobResult representing the dispatched job.
    /// @example
    /// ```cpp
    /// auto result = Syngine::Jobs().DispatchWithResult([]() {
    ///     // Your job code here
    ///     return 42;
    /// });
    /// auto value = result.Get(); // value will be 42
    /// ```
    template <typename F>
    auto DispatchWithResult(F&& func) noexcept
        -> JobResult<std::invoke_result_t<F>> {
        using T     = std::invoke_result_t<F>;
        auto result = std::make_shared<JobState<T>>();

        scl::jobs::waitable* waitable = m_server.submitJob(
            [func = std::forward<F>(func),
             result](const scl::jobs::JobWorker&) mutable {
                result->result = func();
            },
            false);

        return JobResult<T>(waitable, std::move(result));
    }

    /// @brief Dispatch a job without expecting a result.
    /// @tparam F The type of the job function.
    /// @param func The job function to dispatch.
    /// @example
    /// ```cpp
    /// Syngine::Jobs().Dispatch([]() {
    ///     // Your job code here
    /// });
    /// ```
    template <typename F> void Dispatch(F&& func) {
        m_server.submitJob(
            [func = std::forward<F>(func)](
                const scl::jobs::JobWorker&) mutable { func(); });
    };

    /// @brief Dispatch a job with advanced options.
    /// @param func The job function to dispatch.
    /// @example
    /// ```cpp
    /// Syngine::Jobs().DispatchAdvanced([](const scl::jobs::JobWorker& worker)
    /// {
    ///     // Your job code here
    /// });
    /// ```
    void
    DispatchAdvanced(std::function<void(const scl::jobs::JobWorker&)> func);

    /// @brief Dispatch a parallel for loop.
    /// @param count The number of iterations.
    /// @param func The function to execute for each iteration range.
    /// @example
    /// ```cpp
    /// Syngine::Jobs().ParallelFor(100, [](size_t begin, size_t end) {
    ///     for (size_t i = begin; i < end; ++i) {
    ///         // Your loop code here
    ///     }
    /// });
    /// ```
    void ParallelFor(size_t                                        count,
                     std::function<void(size_t begin, size_t end)> func);

    /// @brief Wait for a specific job to complete.
    /// @tparam T The type of the job result.
    /// @param handle The handle to the job result.
    template <typename T> void Wait(JobResult<T>& handle);

    /// @brief Wait for all dispatched jobs to complete.
    void WaitAll();

    /// @brief Get the number of worker threads in the job system.
    uint32_t GetWorkerCount();
};

} // namespace Syngine
