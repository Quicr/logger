# Logger

The `Logger` object facilitates emitting logging output from the application
to any number of logging sinks, including the console, file, or syslog.

## Logger Usage

To help with thread synchronization, the application should
create a "root" `Logger` object and each component in the system
will construct its own `Logger` object, passing a pointer to the
root `Logger` object.  In this way, each single-threaded component
does not have to worry about thread synchronization, as all logging
requests from "child" logging objects are forwarded to the parent
and ultimately to the root `Logger` where the mutex controls output.

Note that it is possible (and even desirable) to have a hierarchy of
`Logger` objects where the child calls the parent who calls its parent
and so on until the root object is reached.  Each will contribute its
component name to the output so that one can better see exactly
what component and object hierarchy produced the message.

The "root" `Logger` has an optional process name associated with it
which is used only when calling `syslog()`.

There is an optional component name, which is intended to provide hints
about which functional component in the software created the logging
messages.

Given this parent/child relationship, suppose a `Logger` object is created
in `main()` and that pointer is passed to an object called `Foo` that creates
a child logger.  Suppose Foo then passes its logger to a child object
called `Bar` that also creates a child logger.  Each of these objects will
provide a "component name" as a parameter when creating its logger.  The
resulting output will be something like this:

```text
2023-08-24T14:38:05.824097 [INFO] [Foo] [Bar] Message from object Bar
```

Use of the parent/child construct is optional, though it does help address
issues with multiple threads emitting logging output at the same time and
provides the extra benefit of seeing component names in the logging output.

The LogLevel parameter indicates the severity of the log.  Those may be
Critical, Error, Warning, Info, or Debug.

The logging facility indicates where logs are to be created.  At
present, the useful options are `Console`, `Syslog`, and `File`.  The default
facility is CONSOLE and only the root logging object should set
this value, since all log messages ultimately flow through the root.
That is, setting this value has no effect on child `Logger` objects.

To help address issues that might arise with multiple threads
creating logs simultaneously, the `Log()` function will lock a shared
a mutex.

```cpp
logger->info << "Log message" << std::flush;
logger->debug << "This is a debug message" << std::flush;
```

Note the used of `std::flush`.  This is required in order to signal
to the `Logger` that a complete message has been constructed.  Users
should not use `std::endl`, as that will result in unwanted linefeed
characters in logging output.

**IMPORTANT!** Failing to call `std::flush` on streaming interfaces will
result in logs being output in a timely manner, but will also cause
other threads waiting to output log entries to be blocked.

If you are this `Logger` as components in an existing project that
already has logging facilities and want to continue using those
existing facilities, or if you wish to capture the logging output and
direct it to a logging facility not supported by the `Logger` object,
create a class derived from `Logger` and override the `EmitLog()`
function.  The `EmitLog()` function will receive the message to log
without the prefixed timestamp.  You should use your derived class
as the "parent" `Logger`, setting the logging facility to `Console`.
To simplify this process, you may use the `CustomLogger` class defined in
`logger.h`, passing a lambda function to invoke as each message to be logged
is received.  See the test `test_custom_logger` for example usage.

By default, the `Logger` will use color if it detects that the output
device may support color.  This may be disabled by calling the
the `Colorize()` function.

The timestamps produced by the `Logger` use microseconds by default.
One may change this to milliseconds by calling the `SetTimePrecision()`
function.

## Logger Macros

`Logger` macros may be used to invoke the `Logger`, with a benefit of using
allowing one to "compile out" logging messages below a certain log level
(e.g., to "compile out" debug messages), one merely needs to define
`LOGGER_LEVEL` to be equal to the desired log level
(e.g., `LOGGER_LEVEL=LOGGER_LEVEL_DEBUG`).

Concretely, if one wishes to "compile out" only debug messages, use the
below macros for all `Logger` calls and define `LOGGER_LEVEL=LOGGER_LEVEL_INFO`.

This is an example of how one would use these macros:

```cpp
Logger logger();

LOGGER_DEBUG(&logger, "This is a debug message");
```

Note that the address of the `Logger` object is provided as the first
parameter.  If the `Logger` object is a shared pointer, then passing
the address would not be necessary.  For shared pointers, usage would
look like this:

```cpp
LoggerPointer logger = std::make_shared<Logger>();

LOGGER_DEBUG(logger, "This is a debug message");
```

If `LOGGER_LEVEL` is not defined, LOGGER_LEVEL_DEBUG is assumed.

The `LOGGER_X()` macros utilize the `Logger`'s streaming interfaces, so
syntax like this is allowed:

```cpp
LOGGER_DEBUG(logger, "ID: " << id << ", Length: " << length)
```

Note that `std::flush` is appended, so there is no need to explicitly
attempt to flush the output stream when using these macros.

## Enabling or Disabling Logger Options

When using Logger in your software, you may disable options exposed in the
top-level CMakeLists.txt file.  For example, to disable the syslog interface
for platforms that do not support syslog, set this option in your CMakeLists.txt
file before adding the Logger directory, FetchContent, etc.

```CMake
set(logger_ENABLE_SYSLOG OFF CACHE BOOL "Enable Logger's Syslog Support")
```
