#pragma once

#include <cstdint>
#include <sys/mman.h>

namespace tobi_engine
{

    class SharedMemory
    {
        public:
            SharedMemory(uint32_t size);
            ~SharedMemory();

            int32_t get_fd() const;
            void resize(uint32_t size);
            void fill(uint8_t data);

            //const uint8_t* get_memory() { return memory; };

        private:
            
            void allocate_shm();
            void create_shared_memory();

            int32_t file_descriptor;
            uint32_t size;
            uint8_t* memory;
    };

}
