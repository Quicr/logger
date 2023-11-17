/*
 *  test_custom_logger.cpp
 *
 *  Copyright (C) 2023
 *  Cisco Systems, Inc.
 *  All Rights Reserved
 *
 *  Description:
 *      This file implements tests of the CustomLogger object, primarily to
 *      illustrate how that may be used to capture log messages.
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

#include <atomic>
#include <iostream>
#include <cstdio>
#include "cantina/logger.h"
#include "gtest/gtest.h"

namespace
{
    // The fixture for testing class Logger
    class CustomLoggerTest : public ::testing::Test
    {
        public:
            CustomLoggerTest() : events{0}, console_events{0}
            {
                // Create a custom logger passing the callback function,
                // which is how one may hook into Cantina's logger to have
                // messaged directed to another logger or logging facility
                custom_logger = std::make_shared<cantina::CustomLogger>(
                            [&](cantina::LogLevel level,
                                const std::string &message,
                                bool console)
                            {
                                LogEvent(level, message, console);
                            });
            }

            ~CustomLoggerTest() = default;

        protected:
            void LogEvent(cantina::LogLevel level,
                          const std::string &message,
                          bool console)
            {
                events++;
                if (console) console_events++;
                level_map[level]++;
                last_message = message;
            }

            unsigned events;
            unsigned console_events;
            std::map<cantina::LogLevel, unsigned> level_map;
            std::string last_message;
            std::shared_ptr<cantina::CustomLogger> custom_logger;
    };

    // Test that the constructor assigned variables as expected
    TEST_F(CustomLoggerTest, Constructor)
    {
        ASSERT_EQ(custom_logger->GetLogFacility(),
                  cantina::LogFacility::Console);
        ASSERT_EQ(custom_logger->GetLogLevel(), cantina::LogLevel::Info);
        ASSERT_EQ(custom_logger->IsDebugging(), false);
    }

    // Basic test showing that messages can be captured
    TEST_F(CustomLoggerTest, BasicTest)
    {
        // Create a child logging object, such as one that might be created
        // by one of the Cantina modules and passing the custom logger as
        // the parent logger
        cantina::LoggerPointer logger =
                    std::make_shared<cantina::Logger>(std::string("LTST"),
                                                      custom_logger);
        // This will set the log level in the child logger, but not the parent
        logger->SetLogLevel(cantina::LogLevel::Debug);

        // Emit an info message
        logger->info << "Sample log" << std::flush;

        ASSERT_EQ(1, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(std::string("[LTST] Sample log"), last_message);

        // Emit a "critical" message
        logger->critical << "Critical message" << std::flush;

        ASSERT_EQ(2, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(std::string("[LTST] Critical message"), last_message);

        // Emit a "debug" message (which should not get emitted since logging
        // level defaults to "Info")
        logger->debug << "Debug message" << std::flush;

        ASSERT_EQ(2, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(0, level_map[cantina::LogLevel::Debug]);
        ASSERT_EQ(std::string("[LTST] Critical message"), last_message);

        // Set the custom logger to debug level so the above debug log will work
        custom_logger->SetLogLevel(cantina::LogLevel::Debug);

        // Emit a "debug" message (which should not get emitted since logging
        // level defaults to "Info")
        logger->debug << "Debug message" << std::flush;

        ASSERT_EQ(3, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Debug]);
        ASSERT_EQ(std::string("[LTST] Debug message"), last_message);
    }

    // Test that custom logger's log level gets inherited
    TEST_F(CustomLoggerTest, InheritedLogLevel)
    {
        // Set the custom logger to use debug level so that gets inherited by
        // th following child logger
        custom_logger->SetLogLevel(cantina::LogLevel::Debug);

        // Create a child logging object, such as one that might be created
        // by one of the Cantina modules and passing the custom logger as
        // the parent logger
        cantina::LoggerPointer logger =
                    std::make_shared<cantina::Logger>(std::string("LTST"),
                                                      custom_logger);
        // This will set the log level in the child logger, but not the parent
        logger->SetLogLevel(cantina::LogLevel::Debug);

        // Emit an info message
        logger->info << "Sample log" << std::flush;

        ASSERT_EQ(1, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(std::string("[LTST] Sample log"), last_message);

        // Emit a "critical" message
        logger->critical << "Critical message" << std::flush;

        ASSERT_EQ(2, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(std::string("[LTST] Critical message"), last_message);

        // Emit a "debug" message (which should not get emitted since logging
        // level defaults to "Info")
        logger->debug << "Debug message" << std::flush;

        ASSERT_EQ(3, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Debug]);
        ASSERT_EQ(std::string("[LTST] Debug message"), last_message);
    }

    // Test that the console flag works
    TEST_F(CustomLoggerTest, ConsoleTest)
    {
        // Create a child logging object, such as one that might be created
        // by one of the Cantina modules and passing the custom logger as
        // the parent logger
        cantina::LoggerPointer logger =
                    std::make_shared<cantina::Logger>(std::string("LTST"),
                                                      custom_logger);

        // Emit an info message
        logger->info << "Sample log" << std::flush;

        ASSERT_EQ(1, events);
        ASSERT_EQ(0, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(std::string("[LTST] Sample log"), last_message);

        // Emit a "critical" message, indicating to also log to console
        logger->Log(cantina::LogLevel::Critical, "Critical message", true);

        ASSERT_EQ(2, events);
        ASSERT_EQ(1, console_events);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Info]);
        ASSERT_EQ(1, level_map[cantina::LogLevel::Critical]);
        ASSERT_EQ(std::string("[LTST] Critical message"), last_message);
    }
}
