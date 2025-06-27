#pragma once

#include <cstdint>
#include <string>

namespace tobi_engine
{
        
    std::string generate_random_string(uint32_t length = 10);
    uint64_t generate_uid();
}