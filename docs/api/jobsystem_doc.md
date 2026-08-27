# Syngine API Documentation

## JobSystem.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/JobSystem.h)

JobSystem class to work with jobs and multithreading. Dipsatch() is best for async fire-and-forget CPU heavy jobs that do not require immediate results. DispatchWithResult() is best for jobs that require a result to be returned, and can be waited on. ParallelFor() is best for large data-parellel workloads. It divides the workload across the engine's worker threads and blocks until all work is complete. @section JobSystem

---
## Goto: 


## Additional Functions: 

