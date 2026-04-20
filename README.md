# ft_vox

![C++](https://img.shields.io/badge/C++-23-blue?style=flat-square&logo=cplusplus&logoColor=white)
![Vulkan](https://img.shields.io/badge/Vulkan-1.3-red?style=flat-square&logo=vulkan&logoColor=white)
![42 Project](https://img.shields.io/badge/42-ft__vox-black?style=flat-square&logo=42&logoColor=white)


## Overview

**ft_vox** is a voxel-based terrain engine inspired by Minecraft, built from scratch in C++ with Vulkan. The project focuses on procedural world generation, real-time rendering performance, and multi threaded management.

The core challenge is not just generating terrain, but doing so efficiently: culling invisible faces, batching geometry, and streaming chunks in and out as the player moves through the world.


---


## Controls

| Input | Action |
|---|---|
| `W A S D` | Move forward / left / backward / right |
| `Q E` | Move up / down (y axis) |
| `T` | Toggle fps mouse camera mode |
| `up / bottom / left / right` | Turn around |
| `Escape` | Quit |

---

## Requirements
- `g++`/`clang` with C++23 support
- Vulkan 1.4
- GLFW3

On Ubuntu/Debian:
```
bash sudo apt-get install libglfw3-dev libglew-dev libglm-dev
```

On Fedora:
```
sudo dnf install vulkan-loader vulkan-loader-devel vulkan-validation-layers vulkan-tools shaderc glfw glfw-devel libX11-devel libXrandr-devel libXi-devel mesa-libGL-devel pkgconf-pkg-config
```

On macOS (with Homebrew):
```bash
brew install glfw glew
```

---

## Build & Run

```bash
# Clone repository
git clone https://github.com/Soepgroente/ft_vox.git
cd ft_vox

# Build
source /opt/vulkan/current/setup-env.sh
make

# Run
make run

# Run with a specific seed (todo)
./ft_vox --seed 42

# Run with custom render distance (todo)
./ft_vox --render-distance 12
```

To clean build artifacts:
```bash
make clean   # remove object files
make fclean  # remove object files and binary
make re      # full rebuild
```

---

## Project Structure

```
ft_vox/
├── include/        # Header files
├── lib/            # Static libraries (vectors/math and Vulkan wrapper)
├── shaders/        # Shaders
├── source/         # Source files
└── textures/       # images
```


## Technical Details

### Chunk system

The world is divided into fixed-size chunks (typically 16×256×16 blocks). Only chunks within the configured render distance are loaded into memory. As the player moves, chunks at the edge are unloaded and new ones are generated and uploaded to the GPU.

### Procedural generation

Terrain height is determined by layered noise functions (fractal Brownian motion).

### Rendering

Each chunk builds a single VAO/VBO containing only its visible faces. On each frame, visible chunks (after frustum culling) are drawn with a single draw call per chunk using a texture atlas to avoid switching textures between blocks.

---

| Authors |
|---|
| [Fra](https://github.com/Orpheus-3145) |
| [Vincent](https://github.com/Soepgroente) |

