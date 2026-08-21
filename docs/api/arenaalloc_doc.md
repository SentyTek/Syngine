# Syngine API Documentation

## ArenaAlloc.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Memory/ArenaAlloc.h)

---
## Goto: 


## Additional Functions: 

### Constructors: 

- [ArenaAlloc(size_t capacity)](#arenaalloc-arenaallocsize_t-capacity)

### Functions: 

- [Allocate()](#arenaalloc-allocate)
- [Reset()](#arenaalloc-reset)
- [Create()](#arenaalloc-create)

---
<a id="arenaalloc-arenaallocsize_t-capacity"></a>

## Class Constructor

#### **`ArenaAlloc(size_t capacity)`**

 Creates an arena allocator with the specified capacity.

Signature:
```cpp
 explicit ArenaAlloc(size_t capacity);
```
**Parameters:**
- `capacity`: The total capacity of the arena in bytes.

**This function has been available since:** v0.0.2

---
<a id="arenaalloc-allocate"></a>

#### **`ArenaAlloc::Allocate()`**

 Allocates a block of memory from the arena.

Signature:
```cpp
 template <typename T> std::span<T> Allocate(size_t count, size_t alignment = alignof(T));
```
**Parameters:**
- `size`: The size of the memory block in bytes.
- `alignment`: The alignment requirement for the memory block.

**Returns:** A pointer to the allocated memory, or nullptr if allocation fails.

**This function has been available since:** v0.0.2

---
<a id="arenaalloc-reset"></a>

#### **`ArenaAlloc::Reset()`**

 Resets the arena allocator, making all previously allocated memory available again.

Signature:
```cpp
 void Reset();
```
**This function has been available since:** v0.0.2

---
<a id="arenaalloc-create"></a>

#### **`ArenaAlloc::Create()`**

 Creates an object of type T in the arena allocator.

Signature:
```cpp
 template <typename T, typename... Args> T* Create(Args&&... args);
```
**Template Parameters:**
- `T`: The type of the object to create.
- `Args`: The types of the constructor arguments.

**Parameters:**
- `args`: The constructor arguments for the object.

**Returns:** A pointer to the created object, or nullptr if allocation fails.

**This function has been available since:** v0.0.2

---
