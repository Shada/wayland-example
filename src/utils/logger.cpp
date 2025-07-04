#include "logger.hpp"

#include <format>
#include <iostream>
#include <sstream>
#include <string_view>
#include <filesystem>
#include <print>

namespace tobi_engine
{
    
    std::mutex Logger::log_mutex;
    // TODO: Make configurable
    LogLevel Logger::loglevel = LogLevel::Debug;

    void Logger::log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        if(level < loglevel)
            return;

        std::ostream& out = (level == LogLevel::Warning || level == LogLevel::Error) ? std::cerr : std::cout;

        std::println(out, "{}", message);

    }

}