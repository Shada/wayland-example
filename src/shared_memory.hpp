#pragma once

#include <cstdint>
#include <sys/mman.h>

#include "wayland_deleters.hpp"

namespace tobi_engine
{

    class SharedMemory
    {
        public:
            SharedMemory(uint16_t width, uint16_t height);
            ~SharedMemory();

            int32_t get_fd() const;
            wl_buffer* get_buffer() const { return buffer.get(); }
            uint16_t get_width() const { return width; }
            uint16_t get_height() const { return height; }

            void resize(uint16_t width, uint16_t height);
            void fill(uint8_t data);
            void fill(uint32_t data);

        private:
            
            void initialize();
            void allocate_shm();
            void create_shared_memory();
            void create_buffer();

            int32_t file_descriptor;
            uint16_t width;
            uint16_t height;
            uint32_t size;
            uint32_t* memory;
            BufferPtr buffer;
            static constexpr uint32_t PIXEL_SIZE = sizeof(uint32_t);
    };

}
