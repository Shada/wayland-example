<!-- design-plan.md: High-level refactoring plan for Wayland subsystem -->
# Wayland Subsystem Refactoring Plan

## 1. Goals
- Eliminate the monolithic `WaylandClient` god-object
- Enforce Single-Responsibility Principle per module
- Achieve compile-time safety via template traits and RAII
- Enable explicit dependency injection for testability

## 2. Proposed Modules
```text
┌────────────────┐      ┌────────────────────┐
│ WaylandDisplay │      │    MainLoop        │
└────────────────┘      └────────────────────┘
         │                       ▲
         ▼                       │
┌────────────────┐      ┌────────────────────┐
│ WaylandRegistry├─────▶│  WindowManager     │
└────────────────┘      └────────────────────┘
         │                       ▲
         ▼                       │
┌────────────────┐      ┌────────────────────┐
│WaylandContext  │─────▶│WaylandInputManager │
└────────────────┘      └────────────────────┘
         │
         ▼
┌────────────────┐
│ WaylandWindow  │
└────────────────┘
```  
**Legend**: arrows denote “depends on / uses”

### 2.1 WaylandDisplay
- Low-level connection to `wl_display`
- Exposes raw `wl_display*` for dispatch/flush

### 2.2 WaylandRegistry
- Binds globals via `bind<Protocol>()`
- Populates `available_global_interfaces` on roundtrip
- No casting or stringly-typed APIs

### 2.3 WaylandContext
- Owns `WaylandDisplay` + `WaylandRegistry` (via `std::unique_ptr`)
- Holds typed `WlUniquePtr<...>` members for compositor, subcompositor, shell, shm, etc.
- Implements event-loop primitives:
  - `dispatch(int timeout_ms)` / `dispatch_pending()`
  - `flush()`
  - template `get<Protocol>()` accessors for bound interfaces

### 2.4 WaylandInputManager
- Constructs with `WaylandRegistry&`
- Binds seat, pointer, keyboard, XKB state
- Registers input listeners only

### 2.5 WaylandWindow & WindowManager
- `WaylandWindow` deals with surface creation, frame callbacks
- `WindowManager` (or Factory) owns multiple windows
- Constructed with `(WaylandContext&, WaylandInputManager&)`

### 2.6 MainLoop
- Orchestrates:  
  1. `ctx.dispatch(timeout_ms)`  
  2. App-level update/draw  
  3. `ctx.flush()`  
  4. Exit checks

## 3. Dependency Matrix
| Module                 | Depends On                   |
|------------------------|------------------------------|
| WaylandDisplay         | —                            |
| WaylandRegistry        | WaylandDisplay              |
| WaylandContext         | WaylandDisplay, WaylandRegistry |
| WaylandInputManager    | WaylandRegistry             |
| WaylandWindow          | WaylandContext              |
| WindowManager/Factory  | WaylandContext, InputManager|
| MainLoop               | WaylandContext, WindowManager|

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
