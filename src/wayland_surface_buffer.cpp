#include "wayland_surface_buffer.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <stdexcept>
#include <string>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

#include "utils/logger.hpp"
#include "utils/utils.hpp"
#include "wayland_client.hpp"


namespace tobi_engine
{

    SurfaceBuffer::SurfaceBuffer(uint32_t width, uint32_t height) 
        :   file_descriptor(-1),
            width(width),
            height(height),
            size(height * width * PIXEL_SIZE),
            memory(nullptr)
    {
        LOG_DEBUG("width = {}, height = {}", width, height);
        initialize();
    }

    SurfaceBuffer::~SurfaceBuffer() 
    { 
        munmap(memory, size);
        close(file_descriptor);
    }

    void SurfaceBuffer::initialize()
    {
        allocate_shm();
        create_shared_memory();
        create_buffer();
    }

    int32_t SurfaceBuffer::get_fd() const
    { 
        return file_descriptor; 
    }

    void SurfaceBuffer::resize(uint32_t width, uint32_t height) 
    { 
        LOG_DEBUG("width = {}, height = {}", width, height);
        if(width == this->width && height == this->height) 
            return;

        munmap(memory, this->size);
        close(file_descriptor);
        
        this->width = width;
        this->height = height;
        this->size = width * height * PIXEL_SIZE;
        
        allocate_shm();
        create_shared_memory();
        create_buffer();
    }

    void SurfaceBuffer::fill(uint8_t data)
    {
        std::fill((uint8_t*)memory, (uint8_t*)memory + this->size, data);
    }
    void SurfaceBuffer::fill(uint32_t data)
    {
        std::fill(memory, memory + height * width, data);
    }

    void SurfaceBuffer::allocate_shm() 
    {
        // create random filename
        auto name = std::string("/window-handle-")
            .append(generate_random_string(10));

        LOG_DEBUG("Created SHM: {}", name);

        file_descriptor = memfd_create(name.c_str(), 0);
        if(file_descriptor == -1)
            throw std::runtime_error("Failed to open file " + name);
        
        shm_unlink(name.c_str());

        if(ftruncate(file_descriptor, size) == -1)
            throw std::runtime_error("Failed to truncate file " + name);
    }

    void SurfaceBuffer::create_shared_memory()
    {
        memory = static_cast<uint32_t*>(mmap(nullptr, size,
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_SHARED, file_descriptor, 0));
        if (memory == MAP_FAILED)
        {
            throw std::runtime_error("Failed to map shared memory.");
        }
    }

    void SurfaceBuffer::create_buffer()
    {
        auto &client = WaylandClient::get_instance();
        auto shm = client.get_shm().value_or(nullptr);
        if (!shm)
        {
            throw std::runtime_error("Failed to get Wayland SHM");
        }
        wl_shm_pool* pool = wl_shm_create_pool(shm, file_descriptor, size);
        if (!pool)
            throw std::runtime_error("Failed to create Wayland SHM pool.");

        buffer.reset(wl_shm_pool_create_buffer(pool, 0, width, height, width * PIXEL_SIZE, WL_SHM_FORMAT_ARGB8888));
        wl_shm_pool_destroy(pool);
        if (!buffer)
            throw std::runtime_error("Failed to create Wayland buffer.");
    }
}
