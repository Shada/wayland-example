#pragma once

#include <random>

namespace tobi_engine
{
        
    inline
    std::string generate_random_string(uint32_t length = 10)
    {
        static const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device random_device;
        thread_local static std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<> distribution(0, characters.size() - 1);

        std::string random_string;

        for(size_t i = 0; i < length; i++)
        {
            random_string += characters[distribution(generator)];
        }
        return random_string;
    }

}