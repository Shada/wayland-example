<!-- design-plan.md: High-level refactoring plan for Wayland subsystem -->
# Wayland Subsystem Refactoring Plan

## 1. Goals
- Eliminate the monolithic `WaylandClient` god-object
- Enforce Single-Responsibility Principle per module
- Achieve compile-time safety via template traits and RAII
- Enable explicit dependency injection for testability

## 2. Proposed Modules
```text
┌────────────────┐
│ WaylandDisplay │       ┌────────────────────┐
└────────────────┘       │      MainLoop      │
         │               └────────────────────┘
         ▼                         ▲
┌────────────────┐       ┌────────────────────┐
│ WaylandRegistry├──────▶│ WindowManager      │
└────────────────┘       └────────────────────┘
         │                         ▲
         ▼                         │
┌────────────────┐       ┌────────────────────┐
│ WaylandContext │──────▶│ WaylandInputManager│
└────────────────┘       └────────────────────┘
         │
         ▼
┌────────────────┐
│ WaylandWindow  │
└────────────────┘
```  
**Legend**: arrows denote “depends on / uses”

### 2.1 WaylandDisplay
- Sole responsibility: wrap `wl_display_connect()` / `wl_display_disconnect()` and own the `wl_display*` handle
- Exposes raw `wl_display*` for event dispatch and flush

### 2.2 WaylandRegistry
- Binds all core Wayland globals (e.g. `wl_compositor`, `wl_shm`, `xdg_wm_base`, `wl_seat`, data-device manager, etc.)
- Constructor takes `(wl_display*, wl_event_queue*)`, sets up listeners for global add/remove
- Provides typed getters, e.g. `compositor()`, `shm()`, `xdgShell()`, `seat()`, etc.
- Exposes a `sync()` or uses `wl_display_roundtrip_queue` to complete binding

### 2.3 WaylandContext
- Composition root: owns `WaylandDisplay`, a `wl_event_queue*`, and `WaylandRegistry`
- In ctor:
  1. connect display (`WaylandDisplay`)
  2. create event queue (`wl_display_create_queue`)
  3. construct `WaylandRegistry(display.handle(), queue)`
  4. call `registry.sync()` to finish global binds
- Exposes:
  - `dispatch(int timeout_ms)` / `dispatch_pending()`
  - `flush()`
  - template `get<Protocol>()` to access bound interfaces

### 2.4 WaylandInputManager
- Constructs with `WaylandRegistry&`
- Binds `wl_seat`, pointer, keyboard, and XKB state listeners
- Exposes input events and state to higher-level code

### 2.5 WaylandWindow & WindowManager
- `WaylandWindow` manages `wl_surface`, `xdg_surface`, toplevel setup, frame callbacks
- `WindowManager` owns and tracks multiple windows, provides factory methods
- Both take dependencies on `WaylandContext` (for display/registry/queue) and `WaylandInputManager` as needed

### 2.6 MainLoop
- Orchestrates:
  1. `context.dispatch(timeout_ms)`
  2. application update/draw logic
  3. `context.flush()`
  4. exit condition checks

## 3. Dependency Matrix
| Module                 | Depends On                                |
|------------------------|-------------------------------------------|
| WaylandDisplay         | —                                         |
| WaylandRegistry        | WaylandDisplay                            |
| WaylandContext         | WaylandDisplay, WaylandRegistry           |
| WaylandInputManager    | WaylandRegistry                           |
| WaylandWindow          | WaylandContext, WaylandInputManager (opt) |
| WindowManager          | WaylandContext, WaylandInputManager       |
| MainLoop               | WaylandContext, WindowManager             |

## 4. Step-by-Step Refactoring Tasks
1. **Refactor `WaylandRegistry`** 
   - Implement `bind<Protocol>()` using `InterfaceTraits<Protocol>`
   - Retire `bind_interface(string, void*)` API
2. **Define `InterfaceTraits<Protocol>`**
   - Specialize for each Wayland protocol object (compositor, shm, xdg_wm_base…)
3. **Implement RAII pointer**
   - Replace lambdas with `WlDeleter<T>` in `wayland_types.hpp`
   - Alias `WlUniquePtr<T>` for safe ownership
4. **Create `WaylandContext`**
   - Move display connection and binding code from `WaylandClient` into ctor
   - Store registry as `std::unique_ptr<WaylandRegistry>`
   - Bind all core globals: compositor, subcompositor, shell, shm, etc.
   - Implement and expose:
     - `dispatch(int timeout_ms)` and/or `dispatch_pending()`
     - `flush()`
     - template `get<Protocol>()` to retrieve raw pointers or `WlUniquePtr` references
   - Add error handling for missing interfaces (not just `wl_shm`)
5. **Adapt `WaylandInputManager`**
   - Inject `WaylandRegistry&`, bind seat and devices in ctor
   - Remove any direct registry calls elsewhere
6. **Build `WindowManager` / Factory**
   - Takes `(WaylandContext&, WaylandInputManager&)`
   - Produces `WaylandWindow` instances
7. **Refactor `WaylandWindow`**
   - Pull compositor/shell/shm pointers from `WaylandContext`
   - Register surface listeners per instance
8. **Remove `WaylandClient` singleton**
   - Replace all references with explicit `WaylandContext` injection
9. **Wire up application entrypoint**
   - Instantiate Context, InputManager, WindowManager in `main()`
   - Drive the loop via a dedicated `MainLoop`

## 5. Timeline & Milestones
- **Week 1**: `WaylandRegistry` refactor + traits + RAII pointers  
- **Week 2**: `WaylandContext` extraction + `WaylandInputManager` adaptation  
- **Week 3**: `WindowManager` & `WaylandWindow` refactoring  
- **Week 4**: Remove singletons, integrate `MainLoop`, heavy testing

---
*This document outlines a clear path toward a modular, type-safe, and testable Wayland layer.*
