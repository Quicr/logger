/*
 *  logger_macros.h
 *
 *  Copyright (C) 2022
 *  Cisco Systems, Inc.
 *  All Rights Reserved.
 *
 *  Description:
 *      The purpose for this file is to define macros that may be used to
 *      invoke the Logger.  A benefit of using these macros is that if one
 *      wishes to "compile out" logging messages below a certain log level
 *      (e.g., to "compile out" debug messages), one merely needs to define
 *      LOGGER_LEVEL to be equal to the desired log level
 *      (e.g., LOGGER_LEVEL=LOGGER_LEVEL_DEBUG).
 *
 *      Concretely, if one wishes to "compile out" only debug messages, use the
 *      below macros for all Logger calls and define
 *      LOGGER_LEVEL=LOGGER_LEVEL_INFO.
 *
 *      This is an example of how one would use these macros:
 *
 *          Logger logger();
 *
 *          LOGGER_DEBUG(&logger, "This is a debug message");
 *
 *      Note that the address of the Logger object is provided as the first
 *      parameter.  If the Logger object is a shared pointer, then passing
 *      the address would not be necessary.  For shared pointers, usage would
 *      look like this:
 *
 *          LoggerPointer logger = std::make_shared<Logger>();
 *
 *          LOGGER_DEBUG(logger, "This is a debug message");
 *
 *      If LOGGER_LEVEL is not defined, LOGGER_LEVEL_DEBUG is assumed.
 *
 *      The LOGGER_X() macros utilize the Logger's streaming interfaces, so
 *      syntax like this is allowed:
 *
 *          LOGGER_DEBUG(logger, "ID: " << id << ", Length: " << length)
 *
 *      Note that "std::flush" is appended, so there is no need to explicitly
 *      attempt to flush the output stream when using these macros.
 *
 *  Portability Issues:
 *      None.
 *
 *  License:
 *      BSD 2-Clause License
 *
 *      Copyright (c) 2022, Cisco Systems
 *      All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      1. Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *
 *      2. Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *      IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *      TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *      PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//  SPDX-FileCopyrightText: 2022 Cisco Systems, Inc.
//  SPDX-License-Identifier: BSD-2-Clause

#pragma once

#define LOGGER_LEVEL_CRITICAL 0
#define LOGGER_LEVEL_ERROR    1
#define LOGGER_LEVEL_WARNING  2
#define LOGGER_LEVEL_INFO     3
#define LOGGER_LEVEL_DEBUG    4

// Assume everything is logged by default
#ifndef LOGGER_LEVEL
#define LOGGER_LEVEL LOGGER_LEVEL_DEBUG
#endif

#if LOGGER_LEVEL <= LOGGER_LEVEL_CRITICAL

#define LOGGER_CRITICAL(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Critical) << message \
    << std::flush
#define LOGGER_ERROR(logger, message)
#define LOGGER_WARNING(logger, message)
#define LOGGER_INFO(logger, message)
#define LOGGER_DEBUG(logger, message)

#elif LOGGER_LEVEL <= LOGGER_LEVEL_ERROR

#define LOGGER_CRITICAL(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Critical) << message \
    << std::flush
#define LOGGER_ERROR(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Error) << message \
    << std::flush
#define LOGGER_WARNING(logger, message)
#define LOGGER_INFO(logger, message)
#define LOGGER_DEBUG(logger, message)

#elif LOGGER_LEVEL <= LOGGER_LEVEL_WARNING

#define LOGGER_CRITICAL(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Critical) << message \
    << std::flush
#define LOGGER_ERROR(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Error) << message \
    << std::flush
#define LOGGER_WARNING(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Warning) << message \
    << std::flush
#define LOGGER_INFO(logger, message)
#define LOGGER_DEBUG(logger, message)

#elif LOGGER_LEVEL <= LOGGER_LEVEL_INFO

#define LOGGER_CRITICAL(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Critical) << message \
    << std::flush
#define LOGGER_ERROR(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Error) << message \
    << std::flush
#define LOGGER_WARNING(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Warning) << message \
    << std::flush
#define LOGGER_INFO(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Info) << message << std::flush
#define LOGGER_DEBUG(logger, message)

#else

#define LOGGER_CRITICAL(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Critical) << message \
    << std::flush
#define LOGGER_ERROR(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Error) << message \
    << std::flush
#define LOGGER_WARNING(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Warning) << message \
    << std::flush
#define LOGGER_INFO(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Info) << message \
    << std::flush
#define LOGGER_DEBUG(logger, message) \
    (logger)->GetLoggingStream(cantina::LogLevel::Debug) << message \
    << std::flush

#endif
