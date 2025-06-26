#include "logger.hpp"

#include <iostream>
#include <sstream>
#include <string_view>
#include <filesystem>

namespace tobi_engine
{
    
    std::mutex Logger::log_mutex;
    // TODO: Make configurable
    LogLevel Logger::loglevel = LogLevel::Debug;

    const std::stringstream location_message(const std::source_location &location)
    {
        auto filename = std::filesystem::path(location.file_name()).filename().string();

        std::stringstream location_message;
        location_message << "(" << filename << ":" << location.line() << "): ";

        return location_message;
    }

    void Logger::log(LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        if(level < loglevel)
            return;

        std::ostream& out = (level == LogLevel::Warning || level == LogLevel::Error) ? std::cerr : std::cout;

        out << message << '\n';
    }

    void Logger::debug(const std::string& message,
         const std::source_location location) 
    {
        std::stringstream debug_message;
        debug_message << "[DEBUG] " << location_message(location).str() << message;
        log(LogLevel::Debug, debug_message.str());
    }

    void Logger::info(const std::string& message) 
    {
        std::stringstream info_message;
        info_message << "[INFO] " << message;
        log(LogLevel::Info, info_message.str());
    }

    void Logger::warning(const std::string& message,
         const std::source_location location) 
    {
        std::stringstream warning_message;
        warning_message << "[WARNING] " << location_message(location).str() << message;
        log(LogLevel::Warning, warning_message.str());
    }

    void Logger::error(const std::string& message,
         const std::source_location location) 
    {
        std::stringstream error_message;
        error_message << "[ERROR] " << location_message(location).str() << message;
        log(LogLevel::Error, error_message.str());
    }

}