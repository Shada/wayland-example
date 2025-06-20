#pragma once
#include <iostream>
#include <string>

namespace tobi_engine
{
    
enum class LogLevel 
{
    Debug,
    Info,
    Warning
};

class Logger 
{
public:
    static void log(LogLevel level, const std::string& message) 
    {
        switch (level) 
        {
            case LogLevel::Debug:
                std::cout << "[DEBUG] " << message << std::endl;
                break;
            case LogLevel::Info:
                std::cout << "[INFO] " << message << std::endl;
                break;
            case LogLevel::Warning:
                std::cout << "[WARNING] " << message << std::endl;
                break;
        }
    }

    static void debug(const std::string& message) 
    {
        log(LogLevel::Debug, message);
    }

    static void info(const std::string& message) 
    {
        log(LogLevel::Info, message);
    }

    static void warning(const std::string& message) 
    {
        log(LogLevel::Warning, message);
    }
};

}
