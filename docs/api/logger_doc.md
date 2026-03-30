# Syngine API Documentation


## Logger.h header


[<- Back](../index.md)

[See source](./../../include/Syngine/Core/Logger.h)

---

## Goto:


- [LogLevel](#syngineloglevel)
- [Logger](#loggerlogger)
- [_GetTimestamp](#logger_gettimestamp)
- [_LogLevelToString](#logger_logleveltostring)
- [Init](#loggerinit)
- [Shutdown](#loggershutdown)
- [Log](#loggerlog)
- [LogF](#loggerlogf)
- [Error](#loggererror)
- [Info](#loggerinfo)
- [InfoPopup](#loggerinfopopup)
- [Warn](#loggerwarn)
- [Fatal](#loggerfatal)
- [LogHardwareInfo](#loggerloghardwareinfo)
- [SetAutoFlush](#loggersetautoflush)
- [Flush](#loggerflush)
- [IsOpen](#loggerisopen)
- [SetMinLogLevel](#loggersetminloglevel)
- [GetMinLogLevel](#loggergetminloglevel)
- [SetupCrashHandler](#loggersetupcrashhandler)
- [PrintStackTrace](#loggerprintstacktrace)

---

#### **`Syngine::LogLevel`**


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

#### **`Logger::Logger`**


 Logger class for logging messages

Signature:

```cpp
 private: Logger() = default; // Private constructor for singleton pattern
```

**This function has been available since:** v0.0.1

---

#### **`Logger::_GetTimestamp`**


 Get the current timestamp as a string

#### This function is internal use only and not intended for public use!


Signature:

```cpp
 static std::string _GetTimestamp();
```

**Returns:** Current timestamp as a string

**This function has been available since:** v0.0.1

---

#### **`Logger::_LogLevelToString`**


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

#### **`Logger::Init`**


 Initialize the logger with the name of the app and an optional log path, relative to the app's data directory.

**Note:** Should be called once at the start of the application.

Signature:

```cpp
 static void Init(const std::string& appname, const std::filesystem::path& logPath = std::filesystem::path("log.txt"));
```

**Parameters:**

- `appname`: Name of the application
- `logPath`: Path to the log file, defaults to "log.txt" in the app's data directory

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** Could throw std::runtime_error if the log file cannot be opened

---

#### **`Logger::Shutdown`**


 Shutdown the logger

**Note:** Should be called once at the end of the application.

Signature:

```cpp
 static void Shutdown();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::Log`**


 Log a message to disk with an optional log level

Signature:

```cpp
 static void Log(const std::string_view message, LogLevel level = LogLevel::INFO, bool toConsole = true);
```

**Parameters:**

- `message`: Message to log
- `level`: Log level, defaults to LogLevel::INFO
- `toConsole`: Whether to log to the console, defaults to true

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if the log file is locked or unavailable

---

#### **`Logger::LogF`**


 Log a formatted message to disk with an optional log level

Signature:

```cpp
 static void LogF(LogLevel level, const char* fmt, ...);
```

**Parameters:**

- `level`: Log level, defaults to LogLevel::INFO
- `fmt`: Format string
- `...`: Format string arguments

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

**Throws:** std::runtime_error if the log file is locked or unavailable

---

#### **`Logger::Error`**


 Log an error

Signature:

```cpp
 static void Error(const std::string_view message);
```

**Parameters:**

- `message`: Error message to log

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::Info`**


 Log an informational message

Signature:

```cpp
 static void Info(const std::string_view message);
```

**Parameters:**

- `message`: Informational message to log

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::InfoPopup`**


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

#### **`Logger::Warn`**


 Log a warning

Signature:

```cpp
 static void Warn(const std::string_view message);
```

**Parameters:**

- `message`: Warning message to log

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::Fatal`**


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

#### **`Logger::LogHardwareInfo`**


 Log hardware information

Signature:

```cpp
 static void LogHardwareInfo();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::SetAutoFlush`**


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

#### **`Logger::Flush`**


 Force flush the log file

Signature:

```cpp
 static void Flush();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::IsOpen`**


 Returns true if the log file is open

Signature:

```cpp
 [[nodiscard]] static inline bool IsOpen() noexcept;
```

**Returns:** True if the log file is open, false otherwise

**Thread Safety:** safe

**This function has been available since:** v0.0.1

**The return value of this function must not be discarded.**

---

#### **`Logger::SetMinLogLevel`**


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

#### **`Logger::GetMinLogLevel`**


 Get the current minimum log level

Signature:

```cpp
 static inline LogLevel GetMinLogLevel() noexcept;
```

**Returns:** Current minimum log level

**Thread Safety:** read-only

**This function has been available since:** v0.0.1

---

#### **`Logger::SetupCrashHandler`**


 Setup crash handler to log crashes

Signature:

```cpp
 static void SetupCrashHandler();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---

#### **`Logger::PrintStackTrace`**


 Print the current stack trace to the log

Signature:

```cpp
 static void PrintStackTrace();
```

**Thread Safety:** not-safe

**This function has been available since:** v0.0.1

---
