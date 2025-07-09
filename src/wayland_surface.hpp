#pragma once

#include "wayland_types.hpp"
#include "wayland_surface_buffer.hpp"
#include <cstdint>
namespace tobi_engine
{
    using SurfaceBufferPtr = std::unique_ptr<SurfaceBuffer>;
    class WaylandSurface
    {
    public:
        enum class Type { Decoration, Content, Popup, Overlay, Cursor };

        WaylandSurface(uint32_t width, uint32_t height, const WaylandSurface *parent = nullptr);
        WaylandSurface(WaylandSurface &&) = default;
        WaylandSurface(const WaylandSurface &) = delete;
        WaylandSurface &operator=(WaylandSurface &&) = default;
        WaylandSurface &operator=(const WaylandSurface &) = delete;
        virtual ~WaylandSurface() = default;

        virtual Type get_type() const = 0;

        wl_surface* get_surface() const { return surface.get(); }
        wl_buffer*  get_buffer() const { return buffer.get()->get_buffer(); }

        void draw();

        virtual void resize(uint32_t width, uint32_t height);
    
    protected:
        SurfacePtr surface;
        SubSurfacePtr subsurface;
        SurfaceBufferPtr buffer;

        uint32_t width;
        uint32_t height;

        uint32_t clear_colour = 0;

        static const uint32_t DECORATIONS_BORDER_SIZE = 4;
        static const uint32_t DECORATIONS_TOPBAR_SIZE = 32;
        static const uint32_t DECORATIONS_BUTTON_SIZE = 28;
        static const uint32_t WINDOW_MINIMUM_SIZE = 10;

    private:
        void create_subsurface(const WaylandSurface *parent);        
    };

    class DecorationSurface : public WaylandSurface 
    {
    public:
        DecorationSurface(uint32_t width, uint32_t height, const WaylandSurface *parent = nullptr);
        Type get_type() const override { return Type::Decoration; }
        // Decoration-specific members...

        virtual void resize(uint32_t width, uint32_t height) override;

    private:
        
    };

    class ContentSurface : public WaylandSurface 
    {
    public:
        ContentSurface(uint32_t width, uint32_t height, const WaylandSurface *parent = nullptr);
        Type get_type() const override { return Type::Content; }
        // Decoration-specific members...

    private:
        
    };
    class CursorSurface : public WaylandSurface 
    {
    public:
        CursorSurface(uint32_t width, uint32_t height, const WaylandSurface *parent = nullptr);
        Type get_type() const override { return Type::Cursor; }
        // Decoration-specific members...

    private:
        
    };
}
