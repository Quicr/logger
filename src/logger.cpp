/*
 *  logger.cpp
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      Module to provide system logging capabilities.
 *
 *  Portability Issues:
 *      None.
 */

#ifdef _WIN32
#include <io.h>
#define _CRT_SECURE_NO_WARNINGS 1
#elif __ANDROID_API__
#include <android/log.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#endif
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include <ctime>
#include "cantina/logger.h"
#include "cantina/ansi.h"

namespace cantina
{

/*
 *  Logger::Logger
 *
 *  Description:
 *      Constructor for the logging object.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(bool output_to_console) :
    Logger("", "", nullptr, output_to_console)
{
}

/*
 *  Logger::Logger
 *
 *  Description:
 *      Constructor for the logging object.
 *
 *  Parameters:
 *      process_name [in]
 *          The name of the running process (for syslog).
 *
 *      output_to_console [in]
 *          Should log messages also be output to the console?
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(const std::string &process_name, bool output_to_console) :
    Logger(process_name, "", nullptr, output_to_console)
{
}

/*
 *  Logger::Logger
 *
 *  Description:
 *      Constructor for the logging object.
 *
 *  Parameters:
 *      process_name [in]
 *          The name of the running process (for syslog).
 *
 *      component_name [in]
 *          The name of the component to print in log messages.
 *
 *      output_to_console [in]
 *          Should log messages also be output to the console?
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(const std::string &process_name,
               const std::string &component_name,
               bool output_to_console) :
    Logger(process_name, component_name, nullptr, output_to_console)
{
}

/*
 *  Logger::Logger
 *
 *  Description:
 *      Constructor to create child Logger objects.
 *
 *  Parameters:
 *      component_name [in]
 *          The name of the component to print in log messages.
 *
 *      parent_logger [in]
 *          A pointer to the parent logging object that will ultimately
 *          be called to actually log messages.
 *
 *      output_to_console [in]
 *          Should log messages also be output to the console?
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(const std::string &component_name,
               const LoggerPointer &parent_logger,
               bool output_to_console) :
    Logger("", component_name, parent_logger, output_to_console)
{
}

/*
 *  Logger::Logger
 *
 *  Description:
 *      Constructor called by other constructors to perform the actual
 *      Logger initialization.
 *
 *  Parameters:
 *      process_name [in]
 *          The name of the running process (for syslog).
 *
 *      component_name [in]
 *          The name of the component to print in log messages.
 *
 *      parent_logger [in]
 *          A pointer to the parent logging object that will ultimately
 *          be called to actually log messages.
 *
 *      output_to_console [in]
 *          Should log messages also be output to the console?
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::Logger(const std::string &process_name,
               const std::string &component_name,
               const LoggerPointer &parent_logger,
               bool output_to_console) :
    process_name(process_name),
    component_name(component_name),
    parent_logger(parent_logger),
    log_facility(LogFacility::Console),
    log_level(parent_logger ? parent_logger->GetLogLevel() : LogLevel::Info),
    info_buf(this, LogLevel::Info),
    warning_buf(this, LogLevel::Warning),
    error_buf(this, LogLevel::Error),
    critical_buf(this, LogLevel::Critical),
    debug_buf(this, LogLevel::Debug),
    console_buf(this, LogLevel::Info, true),
    output_to_console(output_to_console),
    colorize(parent_logger ? parent_logger->IsColorized() : IsColorPossible()),
    time_digits(6),
    time_modulo(1'000'000),
    info(&info_buf),
    warning(&warning_buf),
    error(&error_buf),
    critical(&critical_buf),
    debug(&debug_buf),
    console(&console_buf)
{
#ifdef ANDROID
    SetLogFacility(LogFacility::ANDROIDLOG);
#endif
}

/*
 *  Logger::~Logger
 *
 *  Description:
 *      Destructor for the logging object.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
Logger::~Logger()
{
    // Only the root logger deals with actual facilities
    if (!parent_logger)
    {
        // Close the syslog if it is open
        if (log_facility == LogFacility::Syslog) closelog();

        // Close the open log file
        if (log_facility == LogFacility::File) log_file.close();
    }
}

/*
 *  Logger::Log
 *
 *  Description:
 *      This function will log messages.
 *
 *  Parameters:
 *      level [in]
 *          The logging level for the message.
 *
 *      message [in]
 *          The message to be logged.
 *
 *      console [in]
 *          Log to the console in addition to the default LogFacility.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Logger::Log(LogLevel level, const std::string &message, bool console)
{
    std::string formatted_message;              // Formatted message to log
    std::string formatted_console_message;      // Formatted console message

    console = console || output_to_console;

    // If not logging, return. Ask the parent about the facility facility
    // since only the root knows the actual facility.
    if (GetLogFacility() == LogFacility::None) return;

    // Do not log higher level (i.e., lesser importance) messages
    if (level > log_level) return;

    // Prepare the formatted message to produce
    if (component_name.length() > 0)
    {
        formatted_message = "[" + component_name + "] " + message;
    }
    else
    {
        formatted_message = message;
    }

    // If there is a parent logger, send the logging message to it
    if (parent_logger)
    {
        parent_logger->Log(level, formatted_message, console);
        return;
    }

    // Emit the log message
    EmitLog(level, formatted_message, console);
}

/*
 *  Logger::EmitLog
 *
 *  Description:
 *      This function will emit a log message to the appropiate logging
 *      facility.  Custom loggers may override this function to redirect
 *      logging output elsewhere.
 *
 *  Parameters:
 *      level [in]
 *          The logging level for the message.
 *
 *      message [in]
 *          The message to be logged.
 *
 *      console [in]
 *          Request to log the specific message to the console in addition to
 *          the default logging facility.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Logger::EmitLog(LogLevel level, const std::string &message, bool console)
{
    std::string stamped_message;                // Timestamped log message

    // Log to the terminal or syslog as appropriate
    if (log_facility == LogFacility::Syslog)
    {
        syslog(MapLogLevelToSysLog(level), "%s", message.c_str());
    }

    if ((log_facility != LogFacility::Syslog) || (console))
    {
        // Get the current time in human-readable form
        std::string timestamp = GetTimestamp();

        // Lock the mutex to ensure only one thread is writing
        std::lock_guard<std::mutex> lock(logger_mutex);

        // Update the formatted message to include the log level
        stamped_message = timestamp + " [" + LogLevelString(level) + "] " +
                          message;

        // Output the log message to the appropriate facility
        if (log_facility == LogFacility::File)
        {
            log_file << stamped_message << std::endl;
        }

        if ((log_facility == LogFacility::Console) || console)
        {
            bool colorize_log = colorize;

            if (colorize_log)
            {
                switch (level)
                {
                    case LogLevel::Critical:
                        std::clog << ansi::bold_on << ansi::fg_red;
                        break;

                    case LogLevel::Error:
                        std::clog << ansi::bold_on << ansi::fg_magenta;
                        break;

                    case LogLevel::Warning:
                        std::clog << ansi::bold_on << ansi::fg_yellow;
                        break;

                    case LogLevel::Debug:
                        std::clog << ansi::fg_green;
                        break;

                    default:
                        std::clog << ansi::fg_reset;
                        break;
                }
            }

            std::clog << stamped_message;

            if (colorize_log) std::clog << ansi::reset;

            std::clog << std::endl;
        }
    }

    if (log_facility == LogFacility::AndroidLog)
    {
#ifdef __ANDROID_API__
        auto android_level = ANDROID_LOG_INFO;
        switch (level)
        {
            case LogLevel::Debug:
                android_level = ANDROID_LOG_DEBUG;
                break;
            case LogLevel::Info:
                android_level = ANDROID_LOG_INFO;
                break;
            case LogLevel::Warning:
                android_level = ANDROID_LOG_WARN;
                break;
            case LogLevel::Error:
                android_level = ANDROID_LOG_ERROR;
                break;
            case LogLevel::Critical:
                android_level = ANDROID_LOG_FATAL;
                break;
        }
        __android_log_print(android_level, process_name.c_str(), "%s",
                            stamped_message.c_str());
#else
        throw std::runtime_error("Android LogLevel only supported on "
                                 "Android platform");
#endif
    }
}

/*
 *  Logger::Log
 *
 *  Description:
 *      This function will log messages using the LogLevel::INFO.
 *
 *  Parameters:
 *      message [in]
 *          The message to be logged.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
void Logger::Log(const std::string &message)
{
    Log(LogLevel::Info, message);
}

/*
 *  Logger::SetLogFacility
 *
 *  Description:
 *      Set the logging facility.  The default is to log to the console,
 *      but calling this with LogFacility::SYSLOG allow logs to be sent
 *      only to syslog.
 *
 *  Parameters:
 *      facility [in]
 *          The logging facility to utilize.
 *
 *      filename [in]
 *          The filename to open for logging.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      This function has no effect on child logger objects.
 */
void Logger::SetLogFacility(LogFacility facility, std::string filename)
{
    // Just return if this is a child Logger object
    if (parent_logger) return;

    // Make a change only if the facility changed
    if (log_facility != facility)
    {
        // Stop logging to syslog if we were
        if (log_facility == LogFacility::Syslog) closelog();

        // Stop logging to a file if we were
        if (log_facility == LogFacility::File) log_file.close();

        // Open syslog if appropriate
        if (facility == LogFacility::Syslog)
        {
#ifdef _WIN32
            error << "Syslog not supported on Windows" << std::flush;
#else
            openlog(process_name.c_str(),
                    LOG_PID,
                    LOG_DAEMON);
#endif
        }

        // Open logging file if appropriate
        if (facility == LogFacility::File)
        {
            log_file.open(filename, std::ios::out | std::ios::app);
            if (!log_file.is_open())
            {
                std::cerr << "ERROR: Logger unable to open log file for "
                             "writing: "
                          << filename
                          << std::endl;
                facility = LogFacility::None;
            }
        }

        // Set the logging facility
        log_facility = facility;
    }
}

/*
 *  Logger::GetLogFacility
 *
 *  Description:
 *      Return the current logging facility employed.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The current logging facility.
 *
 *  Comments:
 *      None.
 */
LogFacility Logger::GetLogFacility() const
{
    if (parent_logger) return parent_logger->GetLogFacility();

    return log_facility;
}

/*
 *  Logger::SetLogLevel
 *
 *  Description:
 *      This function will set the log level for messages to be logged.
 *
 *  Parameters:
 *      level [in]
 *          Log level of log messages to be logged.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Note that if there is a parent logger, the ultimate log level used
 *      will be that of the root logger.  The child logger merely passes the
 *      messages to be logged to its parent and the root logger's log level
 *      ultimately determines what does and does not get logged.
 */
void Logger::SetLogLevel(LogLevel level)
{
    log_level = level;
}

/*
 *  Logger::SetLogLevel
 *
 *  Description:
 *      This function will set the log level for messages to be logged
 *      given a string, which must be one of: Critical, Error, Warning,
 *      Info, or Debug.
 *
 *  Parameters:
 *      level [in]
 *          Log level of log messages to be logged.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Note that if there is a parent logger, the ultimate log level used
 *      will be that of the root logger.  The child logger merely passes the
 *      messages to be logged to its parent and the root logger's log level
 *      ultimately determines what does and does not get logged.
 *
 *      Note, also, that if the LogLevel string is invalid, an error
 *      message will be logged.
 */
void Logger::SetLogLevel(const std::string level)
{
    std::string level_comparator = level;

    // Convert the level string to uppercase
    std::transform(level.begin(),
                   level.end(),
                   level_comparator.begin(),
                   ::toupper);

    if (level_comparator == "INFO")
    {
        log_level = LogLevel::Info;
    }
    else if (level_comparator == "ERROR")
    {
        log_level = LogLevel::Error;
    }
    else if (level_comparator == "WARNING")
    {
        log_level = LogLevel::Warning;
    }
    else if (level_comparator == "CRITICAL")
    {
        log_level = LogLevel::Critical;
    }
    else if (level_comparator == "DEBUG")
    {
        log_level = LogLevel::Debug;
    }
    else
    {
        std::string level_error = "Unknown log level \"" + level +
                                  "\"; setting log level to \"INFO\"";
        Log(LogLevel::Error, level_error, true);
        log_level = LogLevel::Info;
    }
}

/*
 *  Logger::GetLogLevel
 *
 *  Description:
 *      Returns the current log level used by the logger.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The logger's current log level.
 *
 *  Comments:
 *      Note that if there is a parent logger, the ultimate log level used
 *      will be that of the root logger.  The child logger merely passes the
 *      messages to be logged to its parent and the root logger's log level
 *      ultimately determines what does and does not get logged.
 */
LogLevel Logger::GetLogLevel() const
{
    return log_level;
}

/*
 *  Logger::GetLogLevelString
 *
 *  Description:
 *      Returns the current log level used by the logger.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      The logger's current log level as a string
 *
 *  Comments:
 *      Note that if there is a parent logger, the ultimate log level used
 *      will be that of the root logger.  The child logger merely passes the
 *      messages to be logged to its parent and the root logger's log level
 *      ultimately determines what does and does not get logged.
 */
std::string Logger::GetLogLevelString() const
{
    return LogLevelString(log_level);
}

/*
 *  Logger::IsDebugging
 *
 *  Description:
 *      Check to see if debug messages are to be logged.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if the log level currently set would result in debug messages
 *      being logged.
 *
 *  Comments:
 *      Note that if there is a parent logger, the ultimate log level used
 *      will be that of the root logger.  The child logger merely passes the
 *      messages to be logged to its parent and the root logger's log level
 *      ultimately determines what does and does not get logged.  Thus, this
 *      function might return true if the child logger passes debug messages
 *      to the parent logger, but the messages would be discarded if the
 *      parent logger is not logging debug messages.
 */
bool Logger::IsDebugging() const
{
    return log_level >= LogLevel::Debug;
}

/*
 *  Logger::Colorize
 *
 *  Description:
 *      Enable or disable colorized console output.
 *
 *  Parameters:
 *      colorize_output [in]
 *          Indicates whether console output should be colorized or not.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Note.
 */
void Logger::Colorize(bool colorize_output)
{
    // Color will be enabled only if possible
    if (colorize_output && IsColorPossible())
    {
        colorize = true;
    }
    else
    {
        colorize = false;
    }
}

/*
 *  Logger::IsColorized
 *
 *  Description:
 *      Indicates whether logging is currently colorized or not.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      True if logging output is in color, false otherwise.
 *
 *  Comments:
 *      Note.
 */
bool Logger::IsColorized() const
{
    // Parent logger controls color, if there is one
    if (parent_logger) return parent_logger->IsColorized();

    return colorize;
}

/*
 *  Logger::SetTimePrecision
 *
 *  Description:
 *      By default, the Logger produces timestamps using microsecond-level
 *      precision.  One may use this function to change that precision to
 *      milliseconds.
 *
 *  Parameters:
 *      precision [in]
 *          The precision with which timestamps should be produced.
 *
 *  Returns:
 *      None.
 *
 *  Comments:
 *      Note.
 */
void Logger::SetTimePrecision(LogTimePrecision precision)
{
    switch (precision)
    {
        case LogTimePrecision::Milliseconds:
            time_digits = 3;
            time_modulo = 1'000;
            break;

        case LogTimePrecision::Microseconds:
            [[fallthrough]];

        default:
            time_digits = 6;
            time_modulo = 1'000'000;
            break;
    }
}

/*
 *  Logger::GetLoggingStream
 *
 *  Description:
 *      Return a reference to the logging output stream for the given
 *      log level.
 *
 *  Parameters:
 *      level [in]
 *          Log level of the output stream
 *
 *  Returns:
 *      A reference to the output stream for the given log level.
 *
 *  Comments:
 *      None.
 */
std::ostream &Logger::GetLoggingStream(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Critical:
            return critical;
            break;

        case LogLevel::Error:
            return error;
            break;

        case LogLevel::Warning:
            return warning;
            break;

        case LogLevel::Info:
            return info;
            break;

        case LogLevel::Debug:
            return debug;
            break;

        default:
            return info;
            break;
    }
}

/*
 *  Logger::MapLogLevelToSysLog
 *
 *  Description:
 *      Map the Logger log level value to syslog priority level values.
 *
 *  Parameters:
 *      level [in]
 *          Log level value to map to syslog priority values.
 *
 *  Returns:
 *      The syslog priority level corresponding to the internal log level value
 *
 *  Comments:
 *      None.
 */
int Logger::MapLogLevelToSysLog(LogLevel level) const
{
#ifdef _WIN32
    return 0;
#else
    int priority;

    switch (level)
    {
        case LogLevel::Critical:
            priority = LOG_CRIT;
            break;

        case LogLevel::Error:
            priority = LOG_ERR;
            break;

        case LogLevel::Warning:
            priority = LOG_WARNING;
            break;

        case LogLevel::Info:
            priority = LOG_INFO;
            break;

        case LogLevel::Debug:
            priority = LOG_DEBUG;
            break;

        default:
            priority = LOG_INFO;
            break;
    }

    return priority;
#endif
}

/*
 *  Logger::LogLevelString
 *
 *  Description:
 *      Return the string representation for the given log level.
 *
 *  Parameters:
 *      level [in]
 *          Log level value to convert to string form.
 *
 *  Returns:
 *      A string representing the given log level.
 *
 *  Comments:
 *      None.
 */
std::string Logger::LogLevelString(LogLevel level) const
{
    std::string log_level_string;

    switch (level)
    {
        case LogLevel::Critical:
            log_level_string = "CRITICAL";
            break;

        case LogLevel::Error:
            log_level_string = "ERROR";
            break;

        case LogLevel::Warning:
            log_level_string = "WARNING";
            break;

        case LogLevel::Info:
            log_level_string = "INFO";
            break;

        case LogLevel::Debug:
            log_level_string = "DEBUG";
            break;

        default:
            log_level_string = "UNKNOWN";
            break;
    }

    return log_level_string;
}

/*
 *  Logger::GetTimestamp
 *
 *  Description:
 *      Return a string representing the current time down to milliseconds.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string representing the current time.
 *
 *  Comments:
 *      None.
 */
std::string Logger::GetTimestamp() const
{
    std::ostringstream oss;

    auto now = std::chrono::system_clock::now();
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    struct tm tm_result{};
#ifdef _WIN32
    localtime_s(&tm_result, &t);
#else
    localtime_r(&t, &tm_result);
#endif
    oss << std::put_time(&tm_result, "%FT%T")
        << "."
        << std::setfill('0')
        << std::setw(time_digits)
        << (now_us.time_since_epoch().count()) % time_modulo;

    return oss.str();
}

/*
 *  Logger::IsColorPossible
 *
 *  Description:
 *      Determine if rendering color output is possible.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      A string representing the current time.
 *
 *  Comments:
 *      None.
 */
bool Logger::IsColorPossible() const
{
    bool color_possible = false;

#ifdef _WIN32
    if (_isatty(_fileno(stdout))) color_possible = true;
#else
    const char *term = getenv("TERM");
    if (isatty(STDOUT_FILENO) && (term != NULL) &&
        (strcasecmp(term, "dumb") != 0))
    {
        color_possible = true;
    }
#endif

    return color_possible;
}

} // namespace cantina
