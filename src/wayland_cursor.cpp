#include "wayland_cursor.hpp"
#include "utils/logger.hpp"

#include "wayland_client.hpp"
#include "wayland_deleters.hpp"

#include <wayland-client-protocol.h>
#include <wayland-cursor.h>


namespace tobi_engine
{

    WaylandCursor::WaylandCursor(const WaylandClient *client) 
        :   current_cursor_name("none"),
            cursor_size(24)
    {
        
        const auto cursor_size_string = getenv("XCURSOR_SIZE");
        
        if(cursor_size_string)
        {
            LOG_DEBUG("Using XCURSOR_SIZE: {}", cursor_size_string);
            cursor_size = std::atoi(cursor_size_string);
        }
        else
        {
            LOG_DEBUG("XCURSOR_SIZE not set, using default value of {}", cursor_size);
        }
        
        const auto cursor_theme_name = getenv("XCURSOR_THEME");
        if(!cursor_theme_name)
        {
            LOG_DEBUG("XCURSOR_THEME not set, using default theme");
        }
        else
        {
            LOG_DEBUG("Using XCURSOR_THEME: {}", cursor_theme_name);
        }
        
        theme = CursorThemePtr(wl_cursor_theme_load(cursor_theme_name, cursor_size, client->get_shm()));
        if (!theme) 
            throw std::runtime_error("Failed to load Wayland cursor theme");

        surface = SurfacePtr(wl_compositor_create_surface(client->get_compositor()));
        if(!surface)
            throw std::runtime_error("Failed to create Wayland cursor surface");

        
    }

    void WaylandCursor::draw()
    {
        auto cursor = wl_cursor_theme_get_cursor(theme.get(), current_cursor_name.c_str());

        if (!cursor)
        {
            LOG_DEBUG("Failed to load cursor: {}", current_cursor_name);
            return;
        }

        struct wl_cursor_image* image = cursor->images[0];
        if (!image)
        {
            LOG_DEBUG("Failed to load cursor image for cursor: {}", current_cursor_name);
            return;
        }

        wl_pointer_set_cursor(
            WaylandClient::get_instance()->get_pointer(),
            0, // Serial number, can be 0 for static cursors
            surface.get(),
            image->hotspot_x,
            image->hotspot_y
        );

        wl_surface_attach(surface.get(), wl_cursor_image_get_buffer(image), 0, 0);
        wl_surface_damage(surface.get(), 0, 0, image->width, image->height);
        wl_surface_commit(surface.get());
    }

    void WaylandCursor::set_cursor(const std::string& cursor_name)
    {
        LOG_DEBUG("Setting cursor to: {}", cursor_name);

        if(cursor_name == current_cursor_name)
        {
            LOG_DEBUG("Cursor is already set to: {}", cursor_name);
            return;
        }
        current_cursor_name = cursor_name;
        draw();
    }

}