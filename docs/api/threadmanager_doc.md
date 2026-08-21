# Syngine API Documentation

## ThreadManager.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/ThreadManager.h)

---
## Goto: 


## Additional Functions: 

### Functions: 

- [Start()](#thread-start)
- [Stop()](#thread-stop)
- [Join()](#thread-join)
- [Wake()](#thread-wake)
- [Wait()](#thread-wait)
- [IsRunning()](#thread-isrunning)
- [GetName()](#thread-getname)
- [Create()](#threadmanager-create)
- [StopAll()](#threadmanager-stopall)
- [JoinAll()](#threadmanager-joinall)

---
<a id="thread-start"></a>

#### **`Thread::Start()`**

 Starts the thread and begins executing the thread function.

**Note:** This function should be called before any other operations on the thread. @thread-safety not-safe

Signature:
```cpp
 void Start();
```
**This function has been available since:** v0.0.2

---
<a id="thread-stop"></a>

#### **`Thread::Stop()`**

 Stops the thread and requests it to exit. @thread-safety not-safe

Signature:
```cpp
 void Stop();
```
**This function has been available since:** v0.0.2

---
<a id="thread-join"></a>

#### **`Thread::Join()`**

 Waits for the thread to finish execution and joins it. @thread-safety not-safe

Signature:
```cpp
 void Join();
```
**This function has been available since:** v0.0.2

---
<a id="thread-wake"></a>

#### **`Thread::Wake()`**

 Wakes the thread if it is waiting. @thread-safety not-safe

Signature:
```cpp
 void Wake();
```
**This function has been available since:** v0.0.2

---
<a id="thread-wait"></a>

#### **`Thread::Wait()`**

 Waits for the thread to be woken up or stopped.

Signature:
```cpp
 void Wait(std::stop_token stop);
```
**Parameters:**
- `stop`: The stop token to allow early exit. @thread-safety not-safe

**This function has been available since:** v0.0.2

---
<a id="thread-isrunning"></a>

#### **`Thread::IsRunning()`**

 Checks if the thread is currently running.

Signature:
```cpp
 bool IsRunning() const;
```
**Returns:** True if the thread is running, false otherwise. @thread-safety not-safe

**This function has been available since:** v0.0.2

---
<a id="thread-getname"></a>

#### **`Thread::GetName()`**

 Gets the name of the thread.

Signature:
```cpp
 const std::string& GetName() const;
```
**Returns:** The name of the thread. @thread-safety not-safe

**This function has been available since:** v0.0.2

---
<a id="threadmanager-create"></a>

#### **`ThreadManager::Create()`**

 Creates a new thread with the given name and function.

Signature:
```cpp
 Thread& Create(std::string name, Thread::ThreadFunction function) noexcept;
```
**Parameters:**
- `name`: The name of the thread.
- `function`: The function to be executed by the thread.

**Returns:** A reference to the created thread. @thread-safety not-safe

**This function has been available since:** v0.0.2

---
<a id="threadmanager-stopall"></a>

#### **`ThreadManager::StopAll()`**

 Stops all managed threads. @thread-safety not-safe

Signature:
```cpp
 void StopAll();
```
**This function has been available since:** v0.0.2

---
<a id="threadmanager-joinall"></a>

#### **`ThreadManager::JoinAll()`**

 Joins all managed threads. @thread-safety not-safe

Signature:
```cpp
 void JoinAll();
```
**This function has been available since:** v0.0.2

---
