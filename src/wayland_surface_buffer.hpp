#pragma once

#include "wayland_client.hpp"
#include "wayland_types.hpp"

#include <cstdint>

namespace tobi_engine
{

    class SurfaceBuffer
    {
        public:
        
            SurfaceBuffer(uint32_t width, uint32_t height, WaylandClient *client);
            ~SurfaceBuffer();

            int32_t get_fd() const;
            wl_buffer* get_buffer() const { return buffer.get(); }
            uint32_t get_width() const { return width; }
            uint32_t get_height() const { return height; }

            void resize(uint32_t width, uint32_t height);
            void fill(uint8_t data);
            void fill(uint32_t data);

        private:
            
            void initialize();
            void allocate_shm();
            void create_shared_memory();
            void create_buffer();

            WaylandClient *client;
            
            int32_t file_descriptor;
            uint32_t width;
            uint32_t height;
            uint32_t size;
            uint32_t* memory;
            WlBufferPtr buffer;
            static constexpr uint32_t PIXEL_SIZE = sizeof(uint32_t);
    };

}
