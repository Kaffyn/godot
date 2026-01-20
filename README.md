# Zyris Engine

[![Godot Version](https://img.shields.io/badge/Godot-4.5.2--stable-blue.svg)](https://github.com/godotengine/godot)
[![Zyris Version](https://img.shields.io/badge/4.5.2-zyris.1-orange.svg)](https://github.com/Kaffyn/ZyrisEngine/tree/Zyris)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

**Zyris** is a powerful game engine distribution based on [Godot Engine](https://godotengine.org), developed by **Kaffyn** and contributors. Our mission is to push the boundaries of open-source game development by implementing advanced features and optimizations that rival industry-leading engines like Unreal Engine and Unity.

## Vision

Zyris is not a fork intended for upstream contributions to Godot Engine. Instead, we are a **parallel distribution** that:

- **Extends Godot's capabilities** with cutting-edge features
- **Optimizes performance** for high-end game development
- **Implements proprietary innovations** while maintaining open-source principles
- **Focuses on AAA production workflows and tools**

All credit for the base engine goes to the [official Godot Engine repository](https://github.com/godotengine/godot) and its amazing community of contributors.

## Versioning and Sync Policy

Zyris maintains continuous synchronization with the official Godot Engine repository to ensure compatibility and access to the latest features and bug fixes.

### Version Format

**Zyris Version Format:** `Godot-X.Y.Z-zyris.N`

- **X.Y.Z** - Official Godot Engine version (e.g., `4.5.0`)
- **N** - Zyris sub-version increment (e.g., `zyris.1`, `zyris.2`)

**Current Version:** `4.5.2-zyris.1`

### Sync Strategy

- **Upstream Tracking:** Zyris automatically syncs with the latest Godot stable release
- **Automated Rebasing:** GitHub Actions workflows perform continuous rebase of Zyris changes over official Godot updates
- **Compatibility First:** All Zyris features are designed to be compatible with upstream changes
- **Release Cadence:** New Zyris sub-versions are released after each major Godot update and when significant features are added

This approach ensures that Zyris users benefit from both the stability of official Godot releases and the advanced features unique to Zyris.

## Roadmap

Zyris is implementing a comprehensive set of systems. Below is our development roadmap:

### Implemented

- [x] **Virtual Input Devices** - Cross-platform input abstraction layer

  A complete virtual input system integrated directly into the engine core, providing fluid touch controls for mobile and hybrid devices.

  **Interface Nodes:**
  - `VirtualButton` - Configurable touch button with visual feedback and action mapping
  - `VirtualJoystick` - Analog control with customizable dead zones and sensitivity
  - `VirtualDPad` - Directional pad with 4 and 8-way support
  - `VirtualTouchPad` - Multi-touch gesture area for camera and viewport control

  **Key Features:**
  - **Haptic Feedback Integration** - Vibration support for mobile devices and gamepads
  - **Input Device Tracking** - `LastInputType` API automatically detects and tracks the active input method (Touch, Keyboard/Mouse, Gamepad)
  - **Dynamic UI Adaptation** - Virtual controls automatically show/hide based on the detected device
  - **Visibility Modes** - `ALWAYS`, `TOUCHSCREEN_ONLY`, `WHEN_PAUSED` for flexible UI behavior
  - **Action Mapping** - Direct integration with Godot's InputMap system
  - **Polymorphic Input System** - The base `InputEvent` class now uses virtual methods for action matching and event comparison, allowing custom virtual events to integrate perfectly into the engine core as first-class citizens.
  - **Editor Integration** - Full inspector support with visual configuration
  - **Performance Optimized** - Minimum overhead with efficient event handling

### In Development

- [ ] **Resource Editor & Library** - Advanced data composition tools

  A dedicated workspace in the Main Panel for editing Resources, treating data as first-class citizens alongside Scripts and Scenes.

  **Resource:**
  - **IDE-like Interface:** Split-view layout with a sidebar for recent resources and a central editing area.
  - **Dual Modes:** Fluid switching between **Visual Mode** (Node/Graph-based) and **Code Mode** (Serialized text view).
  - **Smart Filtering:** Displays only relevant "Editor" properties, filtering internal metadata and storage noise, mirroring the Inspector's clean view.
  - **Script Integration:** Dedicated "Edit Script" button to instantly jump to the resource's logic in the Script Editor.

  **Library:**
  - **Asset Browser:** An asset manager focused on grid view and advanced grouping, replacing the traditional folder tree structure.
  - **Workbench:** A context-aware "Quick Inspector" for immediate adjustments to selected assets without switching context.
  - **CraftTable:** A specialized workbench that filters and displays only Zyris-compatible resources (Registered domains) for streamlined creation.
  - **Context Actions:** Integrated Rename and Delete operations.

- [ ] **Gameplay Ability System (GAS)** - Data-driven gameplay and combat engine
  - Native C++ implementation of `AbilitySystemComponent`, `GameplayAbility`, `GameplayEffect`, and `AttributeSet`.
  - Context-aware execution using high-performance `GameplayTags`.
  - Deterministic execution flow optimized for real-time combat.

- [ ] **Level Streaming System (LSS)** - World orchestration and state management
  - **Yggdrasil Architecture:** Native `LSSServer` managing the engine lifecycle via a Game State Machine (GSM).
  - **StreamingZone System:** Background loading with intelligent spatial pre-caching.
  - **Super Node Architecture:** `LSSRoot` isolates game worlds for advanced transition effects and universe management.

- [ ] **Save System** - High-performance persistence API
  - Native `SaveServer` providing centralized state management and async I/O.
  - Multi-threaded binary serialization with slot-based architecture.
  - Deeply integrated with LSS for automatic world snapshots and state restoration.

- [ ] **Camera System (vCam)** - Cinematic arbitration and dynamic blending
  - **Virtual Camera Architecture:** Priority-based blending integrated natively into `Camera3D` and `Camera2D`.
  - **Native Arbitration:** `CameraServer` acts as the central arbiter for active vCam selection.
  - **Procedural Shake:** Advanced trauma system based on Perlin noise for realistic camera behavior.

- [ ] **Inventory System** - Scalable item and transaction management
  - Centralized `InventoryServer` for item transactions and authoritative loot system.
  - Data-driven `ItemResource` and native `InventoryContainer` for persistent storage.
  - Component-based architecture for seamless UI integration (slots, grids, hotbars).

### Planned Core Systems

- [ ] **Perception System** - Advanced perception engine
  - Physics backend integration
  - Multi-sensory stimulus system (Visual, Auditory, Olfactory, Thermal)
  - Spatial optimization via BVH
  - Blackboard memory integration

- [ ] **Environmental System** - Systemic weather and environment simulation
  - Global wetness and wind simulation
  - Temperature and survival mechanics
  - Biome-based weather systems
  - Day/night cycle with astronomical simulation
  - Shader integration for atmospheric effects

- [ ] **Behavior Tree System** - Hybrid AI system
  - Visual editor (Main Panel)
  - Reinforcement Learning training pipeline (Python backend)
  - AI integration for adaptive difficulty
  - Multi-threaded execution
  - Deterministic multiplayer support

- [ ] **Camera System** - Advanced camera management
  - Virtual camera architecture (Cinemachine-inspired)
  - Procedural camera shake (Perlin noise-based)
  - Priority-based camera blending
  - Dead zone and damping controls
  - Cutscene integration

- [ ] **Audio Engine** - Advanced audio system
  - Visual music sequencer
  - Procedural audio synthesis
  - Adaptive music system (vertical/horizontal transitions)
  - Advanced concurrency and voices management
  - 3D spatial audio with occlusion

- [ ] **Cutscene System** - Cinematic sequencer
  - Timeline-based cutscene system
  - Semantic tracks (Camera, Sound, AI)
  - State restoration
  - Non-destructive editing

- [ ] **Quest System** - Graph-based narrative system
  - Visual quest editor
  - Branching quest graphs
  - Context-aware suggestion engine
  - Objective tracking

- [ ] **Inventory System** - Unified item management
  - Server-based transaction system
  - Loot tables
  - Crafting system
  - UI components (slots, hotbar)

- [ ] **AOT Export System (Zyris AOT Compiler)** - GDScript to native code

  **Zyris AOT Compiler (ZAC)** is a built-in export mode that compiles GDScript directly to machine code (native binary) at export time, ensuring maximum performance and security without altering the development workflow.

  **Core Goals:**
  - **Performance:** Eliminate VM overhead to match or exceed interpreted execution speed.
  - **Security:** Hardening against reverse engineering (removing metadata, symbols, and source code).
  - **Portability:** Enable execution in restricted environments (consoles) prohibiting JIT.

  **Architecture:**
  Integrated directly into the Zyris export pipeline, transforming GDScript -> C++ -> Native Binary transparently.

  **Implementation Roadmap:**
  - **Part 1: Engine Integration**
    - [ ] Export Interface and Flags (UI)
    - [ ] Input Package Generation (`.aot_input`)
    - [ ] Post-processing Hooks
  - **Part 2: Compiler Engine (ZAC)**
    - [ ] Infrastructure (C++, CMake)
    - [ ] Parser and AST (Godot-compatible)
    - [ ] Semantic Validation (AOT-Safe subset)
    - [ ] Static Typed IR and Optimizations
    - [ ] C++ Backend Generation (`gen.cpp`)
    - [ ] Toolchain Integration (MSVC, Clang, NDK)
    - [ ] Hardening and Stripping

## Installation

### Building from Source

Zyris uses the same build system as Godot Engine (SCons):

```bash
# Clone the repository
git clone https://github.com/Kaffyn/ZyrisEngine.git
cd zyris

# Build for your platform
scons platform=windows target=editor
# or
scons platform=linux target=editor
# or
scons platform=macos target=editor
```

For detailed build instructions, see the [official Godot documentation](https://docs.godotengine.org/en/stable/contributing/development/compiling/index.html).

## Contributing

Zyris welcomes contributions from developers who share our vision of creating a world-class game engine.

### Development Philosophy

We follow the **"Godot Way"** for core engine development:

- Strict adherence to Godot's C++ style guidelines
- Use of engine types (`Vector<T>`, `String`, `Ref<T>`)
- Performance-first approach with cache locality optimization
- Comprehensive documentation for all public APIs

See our [Development Manifesto](.github/DEVELOPMENT.md) for detailed guidelines.

### Contribution Workflow

1. **Fork** the repository
2. **Create a feature branch** (`git checkout -b feature/amazing-feature`)
3. **Commit your changes** following our commit message format
4. **Push to your fork** (`git push origin feature/amazing-feature`)
5. **Open a Pull Request** with a clear description

### Commit Message Format

```
Topic: Short description in imperative mood

Detailed explanation of what changed and why.
Reference any related issues or design decisions.
```

**Examples:**

- `Core: Add virtual input device tracking API`
- `Rendering: Optimize shader compilation pipeline`
- `Editor: Implement advanced gesture recognition UI`

## Documentation

- **Engine Documentation**: [Godot Docs](https://docs.godotengine.org) (base engine reference)
- **Zyris-Specific Features**: See `doc/classes/` for XML documentation
- **Development Guide**: `.github/DEVELOPMENT.md`

## Project Structure

```
zyris/
├── core/          # Core engine types, math, memory management
├── scene/         # Scene system, nodes, GUI, 2D/3D
├── servers/       # Low-level rendering, physics, audio servers
├── editor/        # Editor tools, plugins, inspectors
├── modules/       # Optional modules and extensions
├── platform/      # Platform-specific implementations
├── drivers/       # Hardware drivers (rendering, audio, input)
├── doc/           # XML documentation for classes
└── tests/         # Unit and integration tests
```

## License

Zyris Engine is licensed under the **MIT License**, maintaining compatibility with Godot Engine's licensing.

```
Copyright (c) 2025-present Kaffyn and Zyris contributors
Copyright (c) 2014-present Godot Engine contributors
Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur

See LICENSE.txt and COPYRIGHT.txt for full details.
```

## Acknowledgments

Zyris is built upon the incredible work of the **Godot Engine** community. We are deeply grateful to:

- **Juan Linietsky** and **Ariel Manzur** - Godot Engine founders
- All **Godot Engine contributors** - For creating an amazing foundation
- The **open-source community** - For making projects like this possible

For the complete list of Godot contributors, see [AUTHORS.md](AUTHORS.md) and [DONORS.md](DONORS.md).

## Links

- **Official Godot Engine**: <https://godotengine.org>
- **Godot GitHub**: <https://github.com/godotengine/godot>
- **Zyris Repository**: <https://github.com/Kaffyn/ZyrisEngine>
- **Issue Tracker**: <https://github.com/Kaffyn/ZyrisEngine/issues>

---

**Zyris Engine** - Pushing the boundaries of open-source game development
