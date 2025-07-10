#include "wayland_cursor.hpp"
#include "utils/logger.hpp"

#include "wayland_client.hpp"
#include "wayland_types.hpp"

#include <cstdlib>
#include <memory>
#include <optional>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>


namespace tobi_engine
{

    constexpr const char* get_env_or_empty(const char* var_name) 
    {
        const char* value = std::getenv(var_name);
        return value ? value : "";
    }

    static constexpr uint32_t DEFAULT_CURSOR_SIZE = 24;
    static constexpr std::string DEFAULT_CURSOR_THEME = "default";
    static constexpr std::string DEFAULT_CURSOR = "left_ptr";

    static std::string_view DEFAULT_CURSOR_SIZE_ENV = get_env_or_empty("XCURSOR_SIZE");
    static std::string_view DEFAULT_CURSOR_THEME_ENV = get_env_or_empty("XCURSOR_THEME");

    std::optional<uint32_t> get_cursor_size_from_env()
    {
        auto cursor_size_string = DEFAULT_CURSOR_SIZE_ENV;
        
        if (cursor_size_string.empty())
        {
            LOG_DEBUG("XCURSOR_SIZE environment variable not set");
            return std::nullopt;
        }

        try
        {
            uint32_t cursor_size = std::strtol(cursor_size_string.data(), nullptr, 10);
            if (cursor_size < 16 || cursor_size > 64)
            {
                LOG_WARNING("XCURSOR_SIZE value out of range (16-64): {}, using default size", cursor_size);
                return std::nullopt;
            }
            LOG_DEBUG("Using XCURSOR_SIZE: {}", cursor_size);
            return cursor_size;
        }
        catch (const std::exception&)
        {
            LOG_WARNING("Invalid XCURSOR_SIZE value: {}, using default size", cursor_size_string);
            return std::nullopt;
        }
    }
    std::optional<std::string> get_cursor_theme_from_env()
    {
        auto cursor_theme = DEFAULT_CURSOR_THEME_ENV;
        if (cursor_theme.empty())
        {
            LOG_DEBUG("XCURSOR_THEME environment variable not set");
            return std::nullopt;
        }
        
        LOG_DEBUG("Using XCURSOR_THEME: {}", cursor_theme);
        return std::string(cursor_theme);
    }

    WaylandCursor::WaylandCursor(std::shared_ptr<WaylandClient> client) 
        :   current_cursor_name(DEFAULT_CURSOR),
            cursor_size(0),
            client(client)
    {
        
        cursor_size = get_cursor_size_from_env().value_or(DEFAULT_CURSOR_SIZE);
        LOG_DEBUG("Cursor size set to: {}", cursor_size);
                
        current_theme_name = get_cursor_theme_from_env().value_or(DEFAULT_CURSOR_THEME);
        LOG_DEBUG("Cursor theme set to: {}", current_theme_name);
                
        theme = WlCursorThemePtr(wl_cursor_theme_load(current_theme_name.c_str(), cursor_size, client->get_shm()));
        if (!theme) 
            throw std::runtime_error("Failed to load Wayland cursor theme " + current_theme_name);

        surface = WlSurfacePtr(wl_compositor_create_surface(client->get_compositor()));
        if(!surface)
            throw std::runtime_error("Failed to create Wayland cursor surface");
       
    }

    void WaylandCursor::draw()
    {
        if(!client->get_pointer())
            return;

        wl_cursor *cursor;
        if(cursors.contains(current_cursor_name))
        {
            cursor = cursors[current_cursor_name];
        }
        else
        {
            cursor = wl_cursor_theme_get_cursor(theme.get(), current_cursor_name.c_str());
            if (!cursor)
            {
                LOG_DEBUG("Cursor '{}' not found in theme, using default cursor", current_cursor_name);
                cursor = wl_cursor_theme_get_cursor(theme.get(), DEFAULT_CURSOR.c_str());
            }
            if (!cursor)
            {
                LOG_DEBUG("Failed to load default cursor, cannot set cursor");
                return;
            }
            cursors[current_cursor_name] = cursor;
        }

        struct wl_cursor_image* image = cursor->images[0];
        if (!image)
        {
            LOG_DEBUG("Failed to load cursor image for cursor: {}", current_cursor_name);
            return;
        }

        wl_pointer_set_cursor(
            client->get_pointer(),
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