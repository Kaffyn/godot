# Zyris Engine

[![Godot Version](https://img.shields.io/badge/Godot-4.5--stable-blue.svg)](https://github.com/godotengine/godot)
[![Zyris Version](https://img.shields.io/badge/Zyris-0.1.0-orange.svg)](https://github.com/Kaffyn/ZyrisEngine)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

**Zyris** is a powerful game engine distribution powered by [Godot Engine](https://godotengine.org), developed by **Kaffyn** and contributors. Our mission is to push the boundaries of open-source game development by implementing advanced features and optimizations that rival industry-leading engines like Unreal Engine and Unity.

## Vision

Zyris is not a fork intended for upstream contribution to Godot Engine. Instead, we are a **parallel distribution** that:

- **Extends Godot's capabilities** with cutting-edge features
- **Optimizes performance** for high-end game development
- **Implements proprietary innovations** while maintaining open-source principles
- **Targets AAA-level production** workflows and tooling

All credit for the base engine goes to the [official Godot Engine repository](https://github.com/godotengine/godot) and its amazing community of contributors.

## Versioning & Sync Policy

Zyris maintains continuous synchronization with the official Godot Engine repository to ensure compatibility and access to the latest features and bug fixes.

### Version Format

**Zyris Version Format:** `Godot-X.Y.Z-zyris.N`

- **X.Y.Z** - Official Godot Engine version (e.g., `4.5.0`)
- **N** - Zyris sub-version increment (e.g., `zyris.1`, `zyris.2`)

**Current Version:** `4.5.0-zyris.1`

### Sync Strategy

- **Upstream Tracking:** Zyris automatically syncs with the latest stable Godot release
- **Automated Rebasing:** GitHub Actions workflow continuously rebases Zyris changes onto official Godot updates
- **Compatibility First:** All Zyris features are designed to be compatible with upstream changes
- **Release Cadence:** New Zyris sub-versions are released after each major Godot update and when significant features are added

This approach ensures that Zyris users benefit from both the stability of official Godot releases and the advanced features unique to Zyris.

## Roadmap

Zyris is implementing a comprehensive suite of engine systems. Below is our development roadmap:

### Implemented

- [x] **Resource Editor & Library** - Advanced data composition tools

  A dedicated Main Panel workspace for editing Resources, treating data as a first-class citizen alongside Scripts and Scenes.

  **Resource:**
  - **IDE-like Interface:** Split-view layout with a sidebar for recent resources and a central editing area.
  - **Dual Modes:** Seamlessly switch between **Visual Mode** (Graph/Node-based) and **Code Mode** (Text-based serialization view).
  - **Smart Filtering:** Displays only relevant "Editor" properties, filtering out internal metadata and storage noise, mirroring the Inspector's clean view.
  - **Script Integration:** Dedicated "Edit Script" button to instantly jump to the resource's logic in the Script Editor.

  **Library:**
  - **Asset Browser:** A split-view file manager with a Folder Tree and an Asset Grid, specialized for visual resource management.
  - **Context Actions:** Built-in Rename and Delete operations.
  - **CraftTable:** A specialized workbench filters and displays only Zyris-compatible resources (registered Domains) for streamlined creation.

- [x] **Virtual Input Devices** - Cross-platform input abstraction layer

  A complete virtual input system integrated directly into the engine core, providing seamless touch controls for mobile and hybrid devices.

  **Interface Nodes:**
  - `VirtualButton` - Configurable touch button with visual feedback and action mapping
  - `VirtualJoystick` - Analog stick control with customizable dead zones and sensitivity
  - `VirtualDPad` - Directional pad with 4-way and 8-way support
  - `VirtualTouchPad` - Multi-touch gesture area for camera and viewport control

  **Key Features:**
  - **Haptic Feedback Integration** - Vibration support for mobile devices and gamepads
  - **Input Device Tracking** - `LastInputType` API automatically detects and tracks the active input method (Touch, Keyboard/Mouse, Gamepad)
  - **Dynamic UI Adaptation** - Virtual controls automatically show/hide based on detected input device
  - **Visibility Modes** - `ALWAYS`, `TOUCHSCREEN_ONLY`, `WHEN_PAUSED` for flexible UI behavior
  - **Action Mapping** - Direct integration with Godot's InputMap system
  - **Polymorphic Input System** - Base `InputEvent` now uses virtual methods for action matching and event comparisons, enabling custom virtual events to integrate seamlessly with the core engine as first-class citizens.
  - **Editor Integration** - Full inspector support with visual configuration
  - **Performance Optimized** - Minimal overhead with efficient event handling

### In Development

- [ ] **Gameplay Ability System** - Data-driven gameplay engine
  - Context-aware skill execution
  - Component-based character system
  - Hash map inverted candidate selection
  - Multiplayer-ready with deterministic execution

### Planned Core Systems

- [ ] **AI Perception System** - Advanced perception engine
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
  - Multithreaded execution
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
  - Advanced concurrency and voice management
  - 3D spatial audio with occlusion

- [ ] **Cutscene System** - Cinematic sequencer
  - Timeline-based cutscene system
  - Semantic tracks (Camera, Sound, AI)
  - State restoration
  - Non-destructive editing

- [ ] **Level Streaming System** - Game state and level management
  - Portal-based world transitions
  - Chunk loading/unloading
  - Game state machine (Boot → Menu → Gameplay)
  - Smart pre-caching
  - Save system integration

- [ ] **Save System** - Persistent data management
  - Server-based persistence API
  - AES-256 encryption
  - Version migration support
  - Multithreaded I/O

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
    - [ ] Export Interface & Flags (UI)
    - [ ] Input Package Generation (`.aot_input`)
    - [ ] Post-Processing Hooks
  - **Part 2: Compiler Engine (ZAC)**
    - [ ] Infrastructure (C++, CMake)
    - [ ] Parser & AST (Godot-compatible)
    - [ ] Semantic Validation (AOT-Safe subset)
    - [ ] Static Typed IR & Optimizations
    - [ ] C++ Backend Generation (`gen.cpp`)
    - [ ] Toolchain Integration (MSVC, Clang, NDK)
    - [ ] Hardening & Stripping

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
