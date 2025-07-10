#include <catch2/catch_test_macros.hpp>

#include "wayland_types.hpp"

// Directly qualify all types with the tobi_engine:: namespace
template <typename T>
void test_noexcept_and_nullptr() {
    T ptr{nullptr};
    REQUIRE_NOTHROW(ptr.reset());
}

template <typename D>
void test_deleter_noexcept() {
    D del;
    REQUIRE_NOTHROW(del(nullptr));
}

TEST_CASE("All smart pointers and deleters in tobi_engine handle nullptr and are noexcept", "[wayland_types]") {
    SECTION("WlBufferPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlBufferPtr>();
        test_deleter_noexcept<tobi_engine::WlBufferDeleter>();
    }
    SECTION("WlCallbackPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlCallbackPtr>();
        test_deleter_noexcept<tobi_engine::WlCallbackDeleter>();
    }
    SECTION("WlCompositorPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlCompositorPtr>();
        test_deleter_noexcept<tobi_engine::WlCompositorDeleter>();
    }
    SECTION("WlCursorThemePtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlCursorThemePtr>();
        test_deleter_noexcept<tobi_engine::WlCursorThemeDeleter>();
    }
    SECTION("WlDisplayPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlDisplayPtr>();
        test_deleter_noexcept<tobi_engine::WlDisplayDeleter>();
    }
    SECTION("WlKeyboardPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlKeyboardPtr>();
        test_deleter_noexcept<tobi_engine::WlKeyboardDeleter>();
    }
    SECTION("WlPointerPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlPointerPtr>();
        test_deleter_noexcept<tobi_engine::WlPointerDeleter>();
    }
    SECTION("WlRegistryPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlRegistryPtr>();
        test_deleter_noexcept<tobi_engine::WlRegistryDeleter>();
    }
    SECTION("WlSeatPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlSeatPtr>();
        test_deleter_noexcept<tobi_engine::WlSeatDeleter>();
    }
    SECTION("WlShmPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlShmPtr>();
        test_deleter_noexcept<tobi_engine::WlShmDeleter>();
    }
    SECTION("WlSubCompositorPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlSubCompositorPtr>();
        test_deleter_noexcept<tobi_engine::WlSubCompositorDeleter>();
    }
    SECTION("WlSubSurfacePtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlSubSurfacePtr>();
        test_deleter_noexcept<tobi_engine::WlSubSurfaceDeleter>();
    }
    SECTION("WlSurfacePtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::WlSurfacePtr>();
        test_deleter_noexcept<tobi_engine::WlSurfaceDeleter>();
    }
    SECTION("XdgShellPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XdgShellPtr>();
        test_deleter_noexcept<tobi_engine::XdgShellDeleter>();
    }
    SECTION("XdgSurfacePtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XdgSurfacePtr>();
        test_deleter_noexcept<tobi_engine::XdgSurfaceDeleter>();
    }
    SECTION("XdgToplevelPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XdgToplevelPtr>();
        test_deleter_noexcept<tobi_engine::XdgToplevelDeleter>();
    }
    SECTION("XkbContextPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XkbContextPtr>();
        test_deleter_noexcept<tobi_engine::XkbContextDeleter>();
    }
    SECTION("XkbKeymapPtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XkbKeymapPtr>();
        test_deleter_noexcept<tobi_engine::XkbKeymapDeleter>();
    }
    SECTION("XkbStatePtr/Deleter") {
        test_noexcept_and_nullptr<tobi_engine::XkbStatePtr>();
        test_deleter_noexcept<tobi_engine::XkbStateDeteter>();
    }
}

