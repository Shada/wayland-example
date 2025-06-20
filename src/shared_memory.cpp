#include "shared_memory.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

#include "utils/logger.hpp"
#include "utils/utils.hpp"

namespace tobi_engine
{

    SharedMemory::SharedMemory(uint32_t size) 
        :   file_descriptor(-1),
            size(0),
            memory(nullptr)
    {
        resize(size); 
    };
    SharedMemory::~SharedMemory() 
    { 
        munmap(memory, size);
        close(file_descriptor);
    }

    int32_t SharedMemory::get_fd() const
    { 
        return file_descriptor; 
    }

    void SharedMemory::resize(uint32_t size) 
    { 
        if(size == this->size) 
            return;
        this->size = size;
        close(file_descriptor);
        
        allocate_shm();
        create_shared_memory();
    }

    void SharedMemory::allocate_shm() 
    {
        // create random filename
        auto name = std::string("/window-handle-");
        name.append(generate_random_string(10));

        Logger::debug("Created SHM: " + name);

        file_descriptor = memfd_create(name.c_str(), 0);
        if(file_descriptor == -1)
            throw std::runtime_error("Failed to open file " + name);
        
        shm_unlink(name.c_str());

        if(ftruncate(file_descriptor, size) == -1)
        {   
            close(file_descriptor);
            throw std::runtime_error("Failed to truncate file " + name);
        }
    }

    void SharedMemory::create_shared_memory()
    {
        memory = static_cast<uint8_t*>(mmap(nullptr, size,
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_SHARED, file_descriptor, 0));
        if (memory == MAP_FAILED)
        {
            throw std::runtime_error("Failed to map shared memory.");
        }
    }
}
