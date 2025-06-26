#pragma once

#include <mutex>
#include <source_location>
#include <string>


namespace tobi_engine
{
    
enum LogLevel 
{
    Debug = 0,
    Info,
    Warning,
    Error
};

class Logger final
{
public:
    static void log(LogLevel level, const std::string& message);

    static void debug(const std::string& message,
         const std::source_location location =
               std::source_location::current());

    static void info(const std::string& message);

    static void warning(const std::string& message,
         const std::source_location location =
               std::source_location::current());

    static void error(const std::string& message,
         const std::source_location location =
               std::source_location::current());

    static std::mutex log_mutex;
    static LogLevel loglevel;
};


}
