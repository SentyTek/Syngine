# Code policies
[<- Back](index.md)

## Code update policy
The update policy is fairly easy. Document your code as best you can, undocumented code might not get accepted!
The API documentation and glossary are generated automatically by a custom Lua generator using custom SynDoc code formatting found below. Adhere to this format and check if your changes or additions get reflected accurately in the Documentation by running the Lua generator yourself before creating a Pull Request.

In addition, the repository comes with `.clang-format` and `.editor-format` files preconfigured with the recommended code formatting guidelines for Syngine, which your code editor can use and autoformat. These should make it easy to write clean and complete documentation as you go along writing code.

All functions designed for internal engine use only should be marked with `_functionname`. We use PascalCase for function names and camelCase for variable names, generally using CAPITAL_SNAKE_CASE for macros and enum cases.

We use top-of-file comments on all header and source files which look like this:
```cpp
// ╒═════════════════════════════ File.h ═╕
// │ Syngine                              │
// │ Created YYYY-MM-DD                   │
// ├──────────────────────────────────────┤
// │ Copyright (c) SentyTek 2025-2025     │
// │ Placeholder License                  │
// ╰──────────────────────────────────────╯
```
You can copy-and-paste that comment into any new header and source files you create. Our SyngineTests package will also ensure this top-of-file comment is added.

## Code documentation format
SynDoc Format v1

Core tags:
- `@brief` Short description, used in summaries (`@brief Creates an explosion particle effect`)
- `@param` Documents a function parameter (`@param name Description`)
- `@return` Describes the return value (`@return 0 on success, 1 on failure`)
- `@note` Highlight behavior, limitations, or gotchas in plain text
- `@example` Code snippet showing how to use the function
- `@since` Version where the function or system was added
- `@deprecated` Marks function or system as deprecated with an optional reason

Optional Behavior tags:
- `@threadsafety` Whether it's thread safe. Either `safe`, `not-safe`, or `read-only`
- `@pre` Pre-condition, must be true before call (`@pre Entity must be valid`)
- `@post` Post-condition, what happens after call (`@post Buffer will be cleared`)
- `@throws` Indicates what errors a function might throw (`@throw May throw std::runtime_error if file is not found`)

Optional tags:
- `@section` Local grouping (`Rendering`, `Physics`) for search. Every function, class, struct, and enum with the same section will end up on the same page. All public member functions within a class will automatically be on the class/section page.
- `@internal` Indicates an internal use function
- `@noexcept` Indicates this function will not throw a runtime error or call `Logger::Fatal`
- `@block` Indicates this function will block the thread
- `@nodiscard` Indicated the return value should not be ignored
- `@see` Links an existing page in the documentation (`@see GameObject`)
- To reference a function, struct, enum, or public class member in the same section or class, do \``Components`` to create a hyperlink to it

### Full block example
```cpp
/// @brief Gets all ``GameObjects`` with a specific component.
/// @param type The component type to search for.
/// @return A vector of GameObject pointers with the component.
/// @threadsafety read-only
/// @since v0.3.0
/// @section ECS
/// @noexcept
/// @example
/// auto renderables = GetGameObjectsWithComponent(RendererComponent);
static std::vector<GameObject*> GetGameObjectsWithComponent(Syngine::Components type) noexcept;
```

### Variable, struct, enum, etc tags
These work similarly. Public variables in public headers should be documented with a DOUBLE slash + asterisk comment and a brief description:
```cpp
float sprintSpeed = 0.0f //* The speed the player sprints at
```
Structs and Enums should be documented similarly to functions, with any tags that make sense:
```cpp
/// @brief An enum to hold the state
/// @section PlayerComponent
enum States = {
    RUNNING   = 0, /// When running!
    EXPLODING = 1, /// When exploding!
}
```
Multiline or block comments are also supported like so:
```cpp
/**
 * @brief An enum to hold the state
 * @section PlayerComponent
 */
enum States = {
    RUNNING   = 0, /// When running!
    EXPLODING = 1, /// When exploding!
}
```
Note the use of `/**` and ` */`, along with ` * @brief` bullet points. (space-asterisk-space-at symbol)

Classes can be documented like this, similarly to everything else:
```cpp
/// @brief Wowie is it a super car alright!
/// @section Supercar
/// @internal
class Syngine::Supercar {
    
};
```

The generator ignores double slash comments all together, and tags in variable comments are also ignored, simply linked as a member in the class.
The generator only goes through header files `.h` and `.hpp` for comments, not `.c`, `.inl`, or `.cpp`

Classes that inherit from `Syngine::Component` do not need documentation on their constructor or destructor, as this is typically handled by `GameObject::AddComponent<>()`