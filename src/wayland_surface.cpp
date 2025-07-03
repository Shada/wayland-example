#include "wayland_surface.hpp"
#include "utils/logger.hpp"
#include "wayland_client.hpp"
#include "wayland_deleters.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <wayland-client-protocol.h>

namespace tobi_engine
{

    WaylandSurface::WaylandSurface(uint32_t width, uint32_t height, const WaylandSurface *parent)
    {
        auto client = WaylandClient::get_instance();

        LOG_DEBUG("Width: {}, heigth: {}", width, height);

        buffer = std::make_unique<SurfaceBuffer>(width, height);
        surface = SurfacePtr(wl_compositor_create_surface(client->get_compositor()));
        create_subsurface(parent);

        wl_surface_attach(surface.get(), buffer->get_buffer(), 0, 0);

    }

    void WaylandSurface::create_subsurface(const WaylandSurface *parent)
    {
        if(!parent)
            return;
        auto client = WaylandClient::get_instance();

        subsurface = SubSurfacePtr(wl_subcompositor_get_subsurface(client->get_subcompositor(), surface.get(), parent->get_surface()));
        wl_subsurface_set_desync(subsurface.get());
        wl_subsurface_set_position(subsurface.get(), DECORATIONS_BORDER_SIZE, DECORATIONS_TOPBAR_SIZE);
    }

    void WaylandSurface::draw()
    {
        buffer->fill(clear_colour);

        wl_surface_damage(surface.get(), 0, 0, buffer->get_width(), buffer->get_height());
        wl_surface_commit(surface.get());
    }

    void WaylandSurface::resize(uint32_t width, uint32_t height)
    {
        if (this->width == width && this->height == height)
            return;
        this->width = width;
        this->height = height;
        buffer->resize(this->width, this->height);
        wl_surface_attach(surface.get(), buffer->get_buffer(), 0, 0);

        draw();
    }



    DecorationSurface::DecorationSurface(uint32_t width, uint32_t height, const WaylandSurface *parent)
        :   WaylandSurface( width + this->DECORATIONS_BORDER_SIZE * 2, 
                            height + this->DECORATIONS_BORDER_SIZE + this->DECORATIONS_TOPBAR_SIZE, 
                            parent)
    {
        this->clear_colour = 0xFF00DDDD;
    }
    void DecorationSurface::resize(uint32_t width, uint32_t height)
    {
        WaylandSurface::resize(width + DECORATIONS_BORDER_SIZE * 2, height + DECORATIONS_TOPBAR_SIZE + DECORATIONS_BORDER_SIZE);
   }
    ContentSurface::ContentSurface(uint32_t width, uint32_t height, const WaylandSurface *parent)
        :   WaylandSurface(width, height, parent)
    {
        this->clear_colour = 0xFFFFFFFF;
    }

}
