/*
 *  logger.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      The Logger object is used by applications to log messages.
 *
 *      To help with thread synchronization, the application should
 *      create a "root" Logger object and each component in the system
 *      will construct its own Logger object, passing a pointer to the
 *      root Logger object.  In this way, each single-threaded component
 *      does not have to worry about thread synchronization, as all logging
 *      requests from "child" logging objects are forwarded to the parent
 *      and ultimately to the root Logger where the mutex controls output.
 *
 *      Note that it is possible to have a hierarchy of Logger objects
 *      where the child calls the parent who calls its parent and so on
 *      until the root object is reached.  Each will contribute its
 *      component name to the output so that one can better see exactly
 *      what component and object hierarchy produced the message.
 *
 *      The "root" Logger has an optional process name associated with it
 *      which is used only when calling syslog().
 *
 *      There is an optional component name, which is intended to provide hints
 *      about which functional component in the software created the logging
 *      messages.
 *
 *      The LogLevel parameter indicates the severity of the log.
 *
 *      The logging facility indicates where logs are to be created.  At
 *      present, the useful options are Console, Syslog, and File.  The default
 *      facility is Console and only the root logging object should set
 *      this value, since all log messages ultimately flow through the root.
 *      That is, setting this value has no effect on child Logger objects.
 *
 *      To help address issues that might arise with multiple threads
 *      creating logs simultaneously, the Log() function will lock a shared
 *      a mutex.
 *
 *              logger.info << "Log message" << std::flush;
 *              logger.debug << "This is a debug message" << std::flush;
 *
 *      Note the used of "std::flush".  This is required in order to signal
 *      to the logger that a complete message has been constructed.  Users
 *      should not use std::endl, as that will result in unwanted linefeed
 *      characters in logging output.
 *
 *      IMPORTANT! Failing to call "std::flush" on streaming interfaces will
 *      result in logs being output in a timely manner, but will also cause
 *      other threads waiting to output log entries to be blocked.
 *
 *      If you are using Cantina modules as components in an existing project
 *      that already has logging facilities and want to continue using those
 *      existing facilities, or if you wish to capture the logging output and
 *      direct it to a logging facility not supported by the Logger class,
 *      simply create a class derived from Logger and override the EmitLog()
 *      function.  The EmitLog() function will receive the message to log
 *      without the prefixed timestamp.  You should use your derived class
 *      as the "parent" logger, setting the logging facility to "Console".
 *      To simplify this process, you may use the CustomLogger class defined
 *      below, passing a lambda function to invoke as each message to be logged
 *      is received.  See the test "test_custom_logger" for example usage.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <string>
#include <sstream>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <fstream>
#include "ansi.h"
#include "syslog_interface.h"
#include "logger_macros.h"

namespace cantina
{

// Define log level enumeration
enum class LogLevel
{
    Critical,
    Error,
    Warning,
    Info,
    Debug
};

// Define the logging facility enumeration
enum class LogFacility
{
    None,
    Console,
    Syslog,
    File,
    AndroidLog
};

// Define the logging precision
enum class LogTimePrecision
{
    Milliseconds,
    Microseconds
};

// Forward declaration to support parent/child logging relationship
class Logger;
typedef std::shared_ptr<Logger> LoggerPointer;

// Logger object declaration
class Logger : protected SyslogInterface
{
    protected:
        // Stream buffer used to capture log messages
        class LoggingBuf : public std::stringbuf
        {
            public:
                LoggingBuf(Logger *logger,
                           LogLevel log_level,
                           bool console = false) :
                    std::stringbuf(),
                    logger(logger),
                    log_level(log_level),
                    console(console),
                    busy(false)
                {
                }

            protected:
                Logger *logger;
                LogLevel log_level;
                bool console;
                std::atomic<bool> busy;
                std::thread::id owning_thread;
                std::mutex buffer_mutex;
                std::condition_variable signal;

                void thread_sync()
                {
                    bool wait_result;

                    std::unique_lock<std::mutex> lock(buffer_mutex);
                    wait_result = signal.wait_for(
                        lock,
                        std::chrono::seconds(1),
                        [&]() -> bool
                        {
                            return (!busy ||
                                    (busy && owning_thread ==
                                                std::this_thread::get_id()));
                        });

                    if (!wait_result)
                    {
                        logger->Log(LogLevel::Error,
                                    "Somebody forgot to call std::flush!?");
                        busy = false;
                    }

                    if (!busy)
                    {
                        busy = true;
                        owning_thread = std::this_thread::get_id();
                    }
                }

                virtual std::streamsize xsputn(const char *c, std::streamsize n)
                {
                    // Control thread access to the string buffer
                    thread_sync();
                    return std::stringbuf::xsputn(c, n);
                }

                virtual int sync()
                {
                    // Control thread access to the string buffer
                    thread_sync();

                    logger->Log(log_level, str(), console);
                    str("");

                    // We're done with the buffer now
                    busy = false;

                    // Let another thread take control
                    signal.notify_one();

                    return 0;
                }
        };

    public:
        // Constructor
        Logger(bool output_to_console = false);

        // Constructor with process name
        Logger(const std::string &process_name, bool output_to_console = false);

        // Constructor with process name and component name
        Logger(const std::string &process_name,
               const std::string &component_name,
               bool output_to_console = false);

        // Constructor for child Logger objects
        Logger(const std::string &component_name,
               const LoggerPointer &parent_logger,
               bool output_to_console = false);

        // Disallow the copy constructor, as this could be a problem for
        // creation of certain log types (e.g., logging to files)
        Logger(const Logger &) = delete;

        // Destructor
        virtual ~Logger();

        // Function to log messages
        void Log(LogLevel level,
                 const std::string &message,
                 bool console = false);

        // Function to log messages using LogLevel::INFO
        void Log(const std::string &message);

        // Set the logging facility
        void SetLogFacility(LogFacility facility, std::string filename = {});

        // What is the current logging facility?
        LogFacility GetLogFacility() const;

        // Set the log level to be output (default is LogLevel::INFO)
        void SetLogLevel(LogLevel level);

        // Set the log level to be output (default is "INFO")
        void SetLogLevel(const std::string level = "INFO");

        // Get the current log level
        LogLevel GetLogLevel() const;
        std::string GetLogLevelString() const;

        // Check to see if debug messages are to be logged
        bool IsDebugging() const;

        // Enable/disable color console output
        void Colorize(bool colorize_output);

        // Is color logging enabled?
        bool IsColorized() const;

        // Specify logging precision beyond seconds
        void SetTimePrecision(LogTimePrecision precision);

        // Get the streaming logger interface
        std::ostream &GetLoggingStream(LogLevel log_level);

    protected:
        // Constructor called by other constructors
        Logger(const std::string &process_name,
               const std::string &component_name,
               const LoggerPointer &parent_logger,
               bool output_to_console = false);

        // Function to emit the final log message
        virtual void EmitLog(LogLevel level,
                             const std::string &message,
                             bool console);

        int MapLogLevelToSysLog(LogLevel level) const;
                                                // Map log level to syslog level
        std::string LogLevelString(LogLevel level) const;
                                                // Log level string
        std::string GetTimestamp() const;       // Return current timestamp
        bool IsColorPossible() const;           // Is color output possible?

        std::string process_name;               // Program name for logging
        std::string component_name;             // Component name for logging
        LoggerPointer parent_logger;            // Parent logging object
        std::atomic<LogFacility> log_facility;  // Facility to which to log
        std::atomic<LogLevel> log_level;        // Log level to be logged

        // Buffers used with streaming interface
        LoggingBuf info_buf;
        LoggingBuf warning_buf;
        LoggingBuf error_buf;
        LoggingBuf critical_buf;
        LoggingBuf debug_buf;
        LoggingBuf console_buf;

        std::mutex logger_mutex;        // Mutex to synchronize logging
        std::ofstream log_file;         // Stream used for file logging
        bool output_to_console;         // Flag to force output to console
        std::atomic<bool> colorize;     // Colorize console output
        unsigned time_digits;           // Digits of precision beyond seconds
        std::uint64_t time_modulo;      // Modulo to produce time digits

    public:
        // Streaming interfaces
        std::ostream info;
        std::ostream warning;
        std::ostream error;
        std::ostream critical;
        std::ostream debug;
        std::ostream console;
};

// CustomLogger used to help integrate with an existing logging system
class CustomLogger : public Logger
{
    public:
        CustomLogger(std::function<void(LogLevel,
                                        const std::string &,
                                        bool)> callback) :
            Logger(),
            callback(callback)
        {
        }
        CustomLogger(const std::string &component_name,
                     std::function<void(LogLevel,
                                        const std::string &,
                                        bool)> callback) :
            Logger(std::string(), component_name),
            callback(callback)
        {
        }
        virtual ~CustomLogger() = default;

    protected:
        virtual void EmitLog(LogLevel level,
                             const std::string &message,
                             bool console) override
        {
            try
            {
                callback(level, message, console);
            }
            catch (...)
            {
                // Catch exceptions, but ignore since it's not safe to log
            }
        }

        // Function to serve as a callback to receive logging output
        std::function<void(LogLevel, const std::string &, bool)> callback;
};

} // namespace cantina
