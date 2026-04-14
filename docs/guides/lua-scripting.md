# Syngine Documentation
# Guide: Lua Scripting in Syngine
[<-- Back](../index.md)

## Lua scripting in Syngine is a very large work in progress. This guide is currently valid for Syngine v0.0.1 as released for the One Year Technology Demo. All features are subject to change.

---

## Available Base Libraries in OYTD
- `math.*`
- `string.*`
- `setmetatable`/`getmetatable`
- `table.*`

## Added Functions in OYTD
- `require(moduleName)` - Searches in `%AppData%/SentyTek/SyngineDemo/scripts` (or your OS equivalent) for a `.lua` file with the specified name.
- `syngine`:
    - `.log(message)` - Logs a message to the log file.
