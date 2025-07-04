#pragma once

#include <mutex>
#include <string>
#include <source_location>
#include <format>
#include <utility>


namespace tobi_engine
{

    enum LogLevel 
    {
        Debug = 0,
        Info,
        Warning,
        Error
    };


    constexpr std::string location_message(const std::source_location &location)
    {
        std::string file_path = location.file_name();
        auto last_slash = file_path.find_last_of("/\\");
        std::string file_name = (last_slash == std::string::npos) ? file_path : file_path.substr(last_slash + 1);
        return std::format("({}:{}): ", file_name, location.line());
    }

    class Logger final
    {
    public:
        static void log(LogLevel level, const std::string& message);

        template< typename... Args >
        static void debug(const std::source_location location,
            std::format_string<Args...> fmt, Args&&... args)
        {
            auto debug_message = std::format("[DEBUG] {}{}",
                location_message(location),
                std::format(fmt, std::forward<Args>(args)...)); 
            log(Debug, debug_message);
        }

        template< typename... Args >
        static void info(const std::source_location location,
            std::format_string<Args...> fmt, Args&&... args)
        {
            auto info_message = std::format("[INFO] {}{}",
                location_message(location),
                std::format(fmt, std::forward<Args>(args)...)); 
            log(Info, info_message);
        }
        template< typename... Args >
        static void warning(const std::source_location location,
            std::format_string<Args...> fmt, Args&&... args)
        {
            auto warning_message = std::format("[WARNING] {}{}",
                location_message(location),
                std::format(fmt, std::forward<Args>(args)...)); 
            log(Warning, warning_message);
        }
        template< typename... Args >
        static void error(const std::source_location location,
            std::format_string<Args...> fmt, Args&&... args)
        {
            auto error_message = std::format("[ERROR] {}{}",
                location_message(location),
                std::format(fmt, std::forward<Args>(args)...)); 
            log(Error, error_message);
        }
        static std::mutex log_mutex;
        static LogLevel loglevel;
    };

    #ifndef LOG_DEBUG
        // Usage: LOG_DEBUG("format string", arg1, arg2, ...);
        // Ensure the format string and arguments are correct to avoid runtime errors.
        #define LOG_DEBUG(...) Logger::debug(std::source_location::current(), __VA_ARGS__)
    #endif
    #ifndef LOG_INFO
        #define LOG_INFO(...) Logger::info(std::source_location::current(), __VA_ARGS__)
    #endif
    #ifndef LOG_WARNING
        #define LOG_WARNING(...) Logger::warning(std::source_location::current(), __VA_ARGS__)
    #endif
    #ifndef LOG_ERROR
        #define LOG_ERROR(...) Logger::error(std::source_location::current(), __VA_ARGS__)
    #endif

}
