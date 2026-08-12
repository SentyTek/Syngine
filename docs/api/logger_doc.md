# Syngine API Documentation

## Logger.h header

[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Logger.h)

Logger class for logging messages @section Logger

**This class has been available since:** v0.0.1. Some of its functions may have been added later, check the function documentation for details.

---
## Goto: 


## Additional Functions: 

### Enums and Structs: 

- [LogLevel](#syngine-loglevel)

### Functions: 

- [_GetTimestamp()](#logger-_gettimestamp)
- [_LogLevelToString()](#logger-_logleveltostring)
- [_SetMainWindow()](#logger-_setmainwindow)
- [Log()](#logger-log)
- [LogF()](#logger-logf)
- [Error()](#logger-error)
- [Info()](#logger-info)
- [InfoPopup()](#logger-infopopup)
- [Warn()](#logger-warn)
- [Fatal()](#logger-fatal)
- [ToConsole()](#logger-toconsole)
- [LogHardwareInfo()](#logger-loghardwareinfo)
- [SetAutoFlush()](#logger-setautoflush)
- [Flush()](#logger-flush)
- [IsOpen()](#logger-isopen)
- [SetMinLogLevel()](#logger-setminloglevel)
- [GetMinLogLevel()](#logger-getminloglevel)
- [SetupCrashHandler()](#logger-setupcrashhandler)
- [PrintStackTrace()](#logger-printstacktrace)
- [SetVerbose()](#logger-setverbose)
- [IsVerbose()](#logger-isverbose)

---
<a id="syngine-loglevel"></a>

#### **`Syngine::LogLevel()`**

 Log levels for the logger

Signature:
```cpp
enum class LogLevel
```
**Members:**
| Name | Description |
| --- | --- | 
| `INFO` | Informational messages |
| `WARN` | Warning messages |
| `ERR` | Error messages |
| `FATAL` | Fatal error messages. Will terminate the application |
**This function has been available since:** v0.0.1

---
<a id="logger-_gettimestamp"></a>

#### **`Logger::_GetTimestamp()`**

 Get the current timestamp as a string

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 static std::string _GetTimestamp();
```
**Returns:** Current timestamp as a string

**This function has been available since:** v0.0.1

---
<a id="logger-_logleveltostring"></a>

#### **`Logger::_LogLevelToString()`**

 Convert a LogLevel to a string

#### This function is internal use only and not intended for public use!

Signature:
```cpp
 [[nodiscard]] static std::string _LogLevelToString(LogLevel level) noexcept;
```
**Parameters:**
- `level`: LogLevel to convert

**Returns:** String representation of the LogLevel

**This function has been available since:** v0.0.1

**The return value of this function must not be discarded.**

---
<a id="logger-_setmainwindow"></a>

#### **`Logger::_SetMainWindow()`**

 Set the main window for error dialogs

Signature:
```cpp
 static inline void _SetMainWindow(SDL_Window* window) noexcept;
```
**Parameters:**
- `window`: SDL window pointer

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="logger-log"></a>

#### **`Logger::Log()`**

 Log a message to disk with an optional log level

Signature:
```cpp
 static void Log(const std::string_view message, LogLevel level = LogLevel::INFO, bool writeOnlyInDebug = true, bool toConsole = true);
```
**Parameters:**
- `message`: Message to log
- `level`: Log level, defaults to LogLevel::INFO
- `toConsole`: Whether to log to the console, defaults to true
- `writeOnlyInDebug`: When true, only in debug mode will this message be written to disk. In release mode, it will be ignored. Defaults to false.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if the log file is locked or unavailable

---
<a id="logger-logf"></a>

#### **`Logger::LogF()`**

 Log a formatted message to disk with an optional log level

Signature:
```cpp
 static void LogF(LogLevel level, bool writeOnlyInDebug, const char* fmt, ...);
```
**Parameters:**
- `level`: Log level, defaults to LogLevel::INFO
- `writeOnlyInDebug`: When true, only in debug mode will this message be written to disk. In release mode, it will be ignored. Defaults to false.
- `fmt`: Format string
- `...`: Format string arguments

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if the log file is locked or unavailable

---
<a id="logger-error"></a>

#### **`Logger::Error()`**

 Log an error

Signature:
```cpp
 static void Error(const std::string_view message, bool writeOnlyInDebug = false);
```
**Parameters:**
- `message`: Error message to log
- `writeOnlyInDebug`: When true, only in debug mode will this message be written to disk. In release mode, it will be ignored. Defaults to false.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-info"></a>

#### **`Logger::Info()`**

 Log an informational message

Signature:
```cpp
 static void Info(const std::string_view message, bool writeOnlyInDebug = false);
```
**Parameters:**
- `message`: Informational message to log
- `writeOnlyInDebug`: When true, only in debug mode will this message be written to disk. In release mode, it will be ignored. Defaults to false.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-infopopup"></a>

#### **`Logger::InfoPopup()`**

 Show a popup and log an informational message

Signature:
```cpp
 static void InfoPopup(const std::string_view message);
```
**Parameters:**
- `message`: Informational message to log

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-warn"></a>

#### **`Logger::Warn()`**

 Log a warning

Signature:
```cpp
 static void Warn(const std::string_view message, bool writeOnlyInDebug = false);
```
**Parameters:**
- `message`: Warning message to log
- `writeOnlyInDebug`: When true, only in debug mode will this message be written to disk. In release mode, it will be ignored. Defaults to false.

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-fatal"></a>

#### **`Logger::Fatal()`**

 Log a fatal error and shutdown the logger

**Note:** This will terminate the application. If core debug is on, will also pause the app.

Signature:
```cpp
 static void Fatal(const std::string_view message);
```
**Parameters:**
- `message`: Fatal error message to log

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-toconsole"></a>

#### **`Logger::ToConsole()`**

 Log a message directly to the console, bypassing the log file

Signature:
```cpp
 static void ToConsole(const char* fmt, ...);
```
**Parameters:**
- `fmt`: Format string
- `...`: Format string arguments

**Thread Safety:** not-safe

**This function has been available since:** v0.0.2

---
<a id="logger-loghardwareinfo"></a>

#### **`Logger::LogHardwareInfo()`**

 Log hardware information

Signature:
```cpp
 static void LogHardwareInfo();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-setautoflush"></a>

#### **`Logger::SetAutoFlush()`**

 Set the autoflush behavior. Flushing is good for debug, but can be bad on performance.

Signature:
```cpp
 static inline void SetAutoFlush(bool enable) noexcept;
```
**Parameters:**
- `enable`: Whether to enable autoflush

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="logger-flush"></a>

#### **`Logger::Flush()`**

 Force flush the log file

Signature:
```cpp
 static void Flush(); /// @brief Returns true if the log file is open
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-isopen"></a>

#### **`Logger::IsOpen()`**



Signature:
```cpp
 [[nodiscard]] static inline bool IsOpen() noexcept;
```
**Returns:** True if the log file is open, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

**The return value of this function must not be discarded.**

---
<a id="logger-setminloglevel"></a>

#### **`Logger::SetMinLogLevel()`**

 Set the minimum log level. Messages below this level will not be logged.

Signature:
```cpp
 static inline void SetMinLogLevel(LogLevel level) noexcept;
```
**Parameters:**
- `level`: Minimum log level to set

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="logger-getminloglevel"></a>

#### **`Logger::GetMinLogLevel()`**

 Get the current minimum log level

Signature:
```cpp
 static inline LogLevel GetMinLogLevel() noexcept;
```
**Returns:** Current minimum log level

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---
<a id="logger-setupcrashhandler"></a>

#### **`Logger::SetupCrashHandler()`**

 Setup crash handler to log crashes

Signature:
```cpp
 static void SetupCrashHandler();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-printstacktrace"></a>

#### **`Logger::PrintStackTrace()`**

 Print the current stack trace to the log

Signature:
```cpp
 static void PrintStackTrace();
```
**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
<a id="logger-setverbose"></a>

#### **`Logger::SetVerbose()`**

 Set the verbose mode. When verbose, certain messages that are normally only logged in debug mode will also be logged in release mode.

Signature:
```cpp
 static void SetVerbose(bool verbose);
```
**Parameters:**
- `verbose`: Whether to enable verbose mode

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
<a id="logger-isverbose"></a>

#### **`Logger::IsVerbose()`**

 Get whether verbose mode is enabled

Signature:
```cpp
 static bool IsVerbose();
```
**Returns:** True if verbose mode is enabled, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

---
