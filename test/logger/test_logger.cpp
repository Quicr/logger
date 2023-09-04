#include <atomic>
#include <iostream>
#include <cstdio>

// Ensure that all logging levels are being logged here
#undef LOGGER_LEVEL
#define LOGGER_LEVEL LOGGER_LEVEL_DEBUG

#include "cantina/logger.h"
#include "gtest/gtest.h"

namespace
{
    using namespace cantina;

    const std::string Log_File_Prefix = "logger_test";

    std::atomic<unsigned> test_counter = 0;     // Unique logs files per test

    // The fixture for testing class Logger
    class LoggerTest : public ::testing::Test
    {
        protected:
            LoggerTest() : logger(std::make_shared<Logger>("LTST"))
            {
                // Define log filename
                std::ostringstream oss;
                oss << Log_File_Prefix
                    << "_"
                    << this
                    << "_"
                    << ++test_counter;
                log_filename = oss.str();
            }

            ~LoggerTest()
            {
                // Disable logging
                logger->SetLogFacility(LogFacility::None);

                // Delete the log file (if it exists)
                std::remove(log_filename.c_str());
            }

            LoggerPointer logger;               // Logger
            std::string log_filename;           // Name of log file
    };

    // Test that the constructor assigned variables as expected
    TEST_F(LoggerTest, Constructor)
    {
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::Console);
        ASSERT_EQ(logger->GetLogLevel(), LogLevel::Info);
        ASSERT_EQ(logger->IsDebugging(), false);
    }

    // Test that the LogFacility() function
    TEST_F(LoggerTest, LogFacility)
    {
        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        log_file.close();

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);
    }

    // Test that the SetLogLevel() function
    TEST_F(LoggerTest, SetLogLevel)
    {
        // Set the log level to warning
        logger->SetLogLevel(LogLevel::Warning);

        // Verify the log level is set
        ASSERT_EQ(logger->GetLogLevel(), LogLevel::Warning);
    }

    // Test that the SetLogLevel() function (using strings)
    TEST_F(LoggerTest, SetLogLevelString)
    {
        // Set the log level to warning
        logger->SetLogLevel("Warning");

        // Verify the log level is set
        ASSERT_EQ(logger->GetLogLevel(), LogLevel::Warning);

        // If we attempt to set an invalid string, it should set to INFO

        // Set the log level to invalid string
        logger->SetLogLevel("foobar");

        // Verify the log level is set
        ASSERT_EQ(logger->GetLogLevel(), LogLevel::Info);
    }

    // Test the IsDebugging() function
    TEST_F(LoggerTest, IsDebugging)
    {
        // Set the log level to warning
        logger->SetLogLevel(LogLevel::Debug);

        // Verify the log level is set
        ASSERT_TRUE(logger->IsDebugging());
    }

    // Test logging to a file and check that it's really there
    TEST_F(LoggerTest, Log)
    {
        std::string log_line;

        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Log using regular Log() calls
        logger->Log("Test Log 1");
        logger->Log(LogLevel::Debug, "Test Log 2"); // Should not output
        logger->Log(LogLevel::Warning, "Test Log 3");
        logger->Log(LogLevel::Error, "Test Log 4");
        logger->Log(LogLevel::Critical, "Test Log 5");

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[WARNING]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[ERROR]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[CRITICAL]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_TRUE(log_file.eof());
        log_file.close();
    }

    // Test logging to a file and check that it's really there (w/ debug)
    TEST_F(LoggerTest, LogDebug)
    {
        std::string log_line;

        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Set the log level to debugging
        logger->SetLogLevel(LogLevel::Debug);

        // Verify the log level is set
        ASSERT_TRUE(logger->IsDebugging());

        // Log using regular Log() calls
        logger->Log("Test Log 1");
        logger->Log(LogLevel::Debug, "Test Log 2");
        logger->Log(LogLevel::Warning, "Test Log 3");
        logger->Log(LogLevel::Error, "Test Log 4");
        logger->Log(LogLevel::Critical, "Test Log 5");

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[DEBUG]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[WARNING]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[ERROR]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[CRITICAL]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_TRUE(log_file.eof());
        log_file.close();
    }

    // Test logging to a file and check that it's really there (w/ debug)
    // using macros to invoke the logger
    TEST_F(LoggerTest, LogDebugMacros)
    {
        std::string log_line;

        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Set the log level to debugging
        logger->SetLogLevel(LogLevel::Debug);

        // Verify the log level is set
        ASSERT_TRUE(logger->IsDebugging());

        // Log using regular Log() calls
        LOGGER_INFO(logger, "Test Log " << 1);
        LOGGER_DEBUG(logger, "Test Log " << 2);
        LOGGER_WARNING(logger, "Test Log " << 3);
        LOGGER_ERROR(logger, "Test Log " << 4);
        LOGGER_CRITICAL(logger, "Test Log " << 5);

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[DEBUG]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[WARNING]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[ERROR]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[CRITICAL]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_TRUE(log_file.eof());
        log_file.close();
    }

    // Test logging using streaming operators
    TEST_F(LoggerTest, LogStreams)
    {
        std::string log_line;

        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Set the log level to debugging
        logger->SetLogLevel(LogLevel::Debug);

        // Have logging use milliseconds
        logger->SetTimePrecision(LogTimePrecision::Milliseconds);

        // Verify the log level is set
        ASSERT_TRUE(logger->IsDebugging());

        // Log using streaming operator
        logger->info << "Test Log 1" << std::flush;
        logger->debug << "Test Log 2" << std::flush;
        logger->warning << "Test Log 3" << std::flush;
        logger->error << "Test Log 4" << std::flush;
        logger->critical << "Test Log 5" << std::flush;
        logger->console << "Test log to console" << std::flush;

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[DEBUG]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[WARNING]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[ERROR]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[CRITICAL]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        ASSERT_NE(log_line.find("log to console"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_TRUE(log_file.eof());
        log_file.close();
    }

    // Test child logger output
    TEST_F(LoggerTest, ChildLogger)
    {
        std::string log_line;

        // Create the child logger
        auto child_logger = std::make_shared<Logger>("CHLD", logger);

        // Set the log facility to log to a file
        logger->SetLogFacility(LogFacility::File, log_filename);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::File);

        // Set the log level to debugging
        logger->SetLogLevel(LogLevel::Debug);

        // Verify the log level is set
        ASSERT_TRUE(logger->IsDebugging());

        // Log using streaming operator
        child_logger->info << "Test Log 1" << std::flush;

        // Now turn off logging and verify it is off
        logger->SetLogFacility(LogFacility::None);
        ASSERT_EQ(logger->GetLogFacility(), LogFacility::None);

        // Check that the log file exists by opening it
        std::ifstream log_file(log_filename);
        ASSERT_TRUE(log_file.good());
        std::getline(log_file, log_line);
        ASSERT_NE(log_line.find("[INFO]"), std::string::npos);
        ASSERT_NE(log_line.find("[CHLD]"), std::string::npos);
        std::getline(log_file, log_line);
        ASSERT_TRUE(log_file.eof());
        log_file.close();
    }

    // Test forward and reverse log level mappings
    TEST_F(LoggerTest, ForwardAndReverseMappings)
    {
        std::vector<std::string> levels =
        {
            "CRITICAL",
            "ERROR",
            "WARNING",
            "INFO",
            "DEBUG"
        };

        for (auto &level : levels)
        {
            // Forward map from string to LogLevel
            logger->SetLogLevel(level);

            // Reverse map from LogLevel to string
            std::string log_level_string = logger->GetLogLevelString();

            // Values should agree
            ASSERT_EQ(level, log_level_string);
        }
    }

} // namespace
