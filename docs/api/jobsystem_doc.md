# Syngine API Documentation

## JobSystem.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/JobSystem.h)

JobSystem class to work with jobs and multithreading. Dipsatch() is best for async fire-and-forget CPU heavy jobs that do not require immediate results. DispatchWithResult() is best for jobs that require a result to be returned, and can be waited on. ParallelFor() is best for large data-parellel workloads. It divides the workload across the engine's worker threads and blocks until all work is complete. @section JobSystem

---
## Goto: 


## Additional Functions: 

### Functions: 

- [DispatchWithResult()](#jobsystem-dispatchwithresult)
- [Dispatch()](#jobsystem-dispatch)
- [DispatchAdvanced()](#jobsystem-dispatchadvanced)
- [ParallelFor()](#jobsystem-parallelfor)
- [Wait()](#jobsystem-wait)
- [WaitAll()](#jobsystem-waitall)
- [GetWorkerCount()](#jobsystem-getworkercount)

---
<a id="jobsystem-dispatchwithresult"></a>

#### **`JobSystem::DispatchWithResult()`**

 Dispatch a job and return a handle to its result.

**Example:** ```cpp auto result = Syngine::Jobs().DispatchWithResult([]() { // Your job code here return 42; }); auto value = result.Get(); // value will be 42 ```

Signature:
```cpp
 template <typename F> auto DispatchWithResult(F&& func) noexcept -> JobResult<std::invoke_result_t<F>>;
```
**Template Parameters:**
- `F`: The type of the job function.

**Parameters:**
- `func`: The job function to dispatch.

**Returns:** A JobResult representing the dispatched job.

---
<a id="jobsystem-dispatch"></a>

#### **`JobSystem::Dispatch()`**

 Dispatch a job without expecting a result.

**Example:** ```cpp Syngine::Jobs().Dispatch([]() { // Your job code here }); ```

Signature:
```cpp
 template <typename F> void Dispatch(F&& func);
```
**Template Parameters:**
- `F`: The type of the job function.

**Parameters:**
- `func`: The job function to dispatch.

---
<a id="jobsystem-dispatchadvanced"></a>

#### **`JobSystem::DispatchAdvanced()`**

 Dispatch a job with advanced options.

**Example:** ```cpp Syngine::Jobs().DispatchAdvanced([](const scl::jobs::JobWorker& worker) { // Your job code here }); ```

Signature:
```cpp
 void DispatchAdvanced(std::function<void(const scl::jobs::JobWorker&)> func);
```
**Parameters:**
- `func`: The job function to dispatch.

---
<a id="jobsystem-parallelfor"></a>

#### **`JobSystem::ParallelFor()`**

 Dispatch a parallel for loop.

**Example:** ```cpp Syngine::Jobs().ParallelFor(100, [](size_t begin, size_t end) { for (size_t i = begin; i < end; ++i) { // Your loop code here } }); ```

Signature:
```cpp
 void ParallelFor(size_t count, std::function<void(size_t begin, size_t end)> func);
```
**Parameters:**
- `count`: The number of iterations.
- `func`: The function to execute for each iteration range.

---
<a id="jobsystem-wait"></a>

#### **`JobSystem::Wait()`**

 Wait for a specific job to complete.

Signature:
```cpp
 template <typename T> void Wait(JobResult<T>& handle);
```
**Template Parameters:**
- `T`: The type of the job result.

**Parameters:**
- `handle`: The handle to the job result.

---
<a id="jobsystem-waitall"></a>

#### **`JobSystem::WaitAll()`**

 Wait for all dispatched jobs to complete.

Signature:
```cpp
 void WaitAll();
```
---
<a id="jobsystem-getworkercount"></a>

#### **`JobSystem::GetWorkerCount()`**

 Get the number of worker threads in the job system.

Signature:
```cpp
 uint32_t GetWorkerCount();
```
---
