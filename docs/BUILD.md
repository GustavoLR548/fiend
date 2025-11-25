# Building Fiend - Preservation Guide

This document describes how to build Fiend from source, with special attention to long-term preservation and self-contained builds.

## Quick Start (System Dependencies)

### Linux (Debian/Ubuntu)
```bash
sudo apt-get install build-essential cmake git liballegro4-dev
git clone --recursive https://github.com/YOUR_USERNAME/fiend.git
cd fiend
mkdir build && cd build
cmake ..
make -j$(nproc)
./release/fiend
```

### Linux (Fedora/RHEL)
```bash
sudo dnf install gcc gcc-c++ cmake git allegro-devel
git clone --recursive https://github.com/YOUR_USERNAME/fiend.git
cd fiend
mkdir build && cd build
cmake ..
make -j$(nproc)
./release/fiend
```

## Self-Contained Build (Maximum Preservation)

For maximum preservation and offline building capability, Fiend includes Allegro 4.4.3.1 source code as a git submodule. This ensures the game can be built even if external repositories become unavailable.

### Step 1: Clone with Submodules

```bash
git clone --recursive https://github.com/YOUR_USERNAME/fiend.git
cd fiend
```

If you already cloned without `--recursive`:
```bash
git submodule update --init --recursive
```

### Step 2: Build Bundled Allegro

The bundled Allegro must be built separately first:

```bash
cd external/allegro4
mkdir build && cd build

# Configure Allegro
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DSHARED=ON \
    -DWANT_DOCS=OFF \
    -DWANT_EXAMPLES=OFF \
    -DWANT_TOOLS=OFF \
    -DWANT_TESTS=OFF

# Build
make -j$(nproc)

# Optional: Install to system
sudo make install
sudo ldconfig

# Or: Install to local prefix for self-contained builds
cmake .. -DCMAKE_INSTALL_PREFIX="$PWD/install"
make install
```

### Step 3: Build Fiend with Bundled Allegro

#### Option A: System-installed bundled Allegro
```bash
cd ../../..  # Back to fiend root
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Option B: Local prefix (fully self-contained)
```bash
cd ../../..  # Back to fiend root
mkdir build && cd build

# Point CMake to your local Allegro install
export PKG_CONFIG_PATH="$PWD/../external/allegro4/build/install/lib/pkgconfig:$PKG_CONFIG_PATH"
cmake ..
make -j$(nproc)

# You may need to set LD_LIBRARY_PATH to run
export LD_LIBRARY_PATH="$PWD/../external/allegro4/build/install/lib:$LD_LIBRARY_PATH"
./release/fiend
```

## Build Options

### CMake Options

- `CMAKE_BUILD_TYPE`: `Debug` or `Release` (default: Release)
- `USE_BUNDLED_ALLEGRO`: `ON` or `OFF` (default: OFF) - Experimental, requires pre-built Allegro in external/allegro4/build/lib

Example:
```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_BUNDLED_ALLEGRO=OFF
```

## Troubleshooting

### Missing Allegro Headers

**Error:** `allegro.h: No such file or directory`

**Solution:** Install allegro development package:
```bash
# Debian/Ubuntu
sudo apt-get install liballegro4-dev

# Fedora/RHEL
sudo dnf install allegro-devel

# Or build bundled version (see above)
```

### Submodule Not Initialized

**Error:** `external/allegro4` directory is empty

**Solution:**
```bash
git submodule update --init --recursive
```

### Allegro Build Failures

The bundled Allegro 4.4.3.1 may require additional development packages:

```bash
# Debian/Ubuntu
sudo apt-get install \
    libx11-dev \
    libxext-dev \
    libxcursor-dev \
    libxxf86vm-dev \
    libasound2-dev \
    libpulse-dev

# Fedora/RHEL
sudo dnf install \
    libX11-devel \
    libXext-devel \
    libXcursor-devel \
    alsa-lib-devel \
    pulseaudio-libs-devel
```

## Verifying Your Build

After building, test both executables:

```bash
# From build directory
cd ../release

# Test the game
./fiend

# Test the map editor
./mapeditor
```

Check the logs in `release/logs/` for any runtime errors.

## Cross-Platform Notes

### Windows
Windows builds require MinGW or Visual Studio. The bundled Allegro approach is recommended for Windows to avoid DLL dependency issues.

### macOS
macOS builds may require additional frameworks. System Allegro via Homebrew is recommended:
```bash
brew install allegro@4
```

## Static Linking (Future Enhancement)

For maximum portability, static linking is planned. This will create standalone executables with no external dependencies. Track issue #XX for progress.

## Preservation Philosophy

This project prioritizes long-term preservation:

1. **Bundled Dependencies**: All core dependencies (Allegro 4.4.3.1) are included as git submodules
2. **Pinned Versions**: Submodules are pinned to specific release tags, not branches
3. **Offline Building**: Complete source code is version-controlled; no internet required after initial clone
4. **Documentation**: Comprehensive build instructions for when package managers change
5. **Self-Contained**: Goal is executables that run without system dependencies

## Getting Help

If you encounter build issues:

1. Check `CMakeLists.txt` for current dependency requirements
2. Review `DEPENDENCIES.txt` for exact version information
3. Check GitHub Issues for similar problems
4. The game was originally built with Allegro 4.4.3.1 on Linux - this is the reference platform

## Future Enhancements

- [ ] Fully automated bundled Allegro build integrated into CMake
- [ ] Static linking support for standalone executables
- [ ] Windows cross-compilation from Linux
- [ ] macOS application bundle generation
- [ ] Flatpak/AppImage packaging
- [ ] Pre-built binaries via GitHub Actions

---

Last Updated: November 2024
Allegro Version: 4.4.3.1 (bundled)
Target Platforms: Linux (primary), Windows, macOS