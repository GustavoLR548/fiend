# Bundled Allegro Builds - Technical Documentation

This document explains how the bundled Allegro build process works in GitHub Actions and how to replicate it locally.

## Overview

The GitHub Actions workflow builds Fiend in two configurations:
1. **System Allegro**: Uses distribution-provided Allegro packages (traditional approach)
2. **Bundled Allegro**: Builds Allegro 4.4.3.1 from the git submodule (preservation approach)

## Bundled Build Process

### Step 1: Clone with Submodules
```bash
git clone --recursive https://github.com/YOUR_USERNAME/fiend.git
cd fiend
```

The `--recursive` flag initializes the `external/allegro4` submodule automatically.

### Step 2: Install Build Dependencies
Note: We install X11, ALSA, etc. but **NOT** liballegro4-dev.

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  cmake \
  libx11-dev \
  libxext-dev \
  libxcursor-dev \
  libxxf86vm-dev \
  libasound2-dev \
  libpulse-dev
```

### Step 3: Build Allegro 4.4.3.1
```bash
cd external/allegro4
mkdir -p build
cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/install" \
  -DSHARED=ON \
  -DWANT_DOCS=OFF \
  -DWANT_EXAMPLES=OFF \
  -DWANT_TOOLS=OFF \
  -DWANT_TESTS=OFF

make -j$(nproc)
make install

cd ../../..
```

This builds Allegro and installs it to `external/allegro4/build/install/`.

### Step 4: Build Fiend
```bash
mkdir -p build
cd build

# Set PKG_CONFIG_PATH to find our bundled Allegro
export PKG_CONFIG_PATH="$PWD/../external/allegro4/build/install/lib/pkgconfig:$PKG_CONFIG_PATH"

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

The key is setting `PKG_CONFIG_PATH` so CMake's `pkg-config` finds our bundled Allegro instead of looking for system packages.

### Step 5: Bundle the Library
```bash
cd ..
mkdir -p release/lib
cp external/allegro4/build/install/lib/liballeg*.so* release/lib/
```

Copy the built Allegro shared libraries into the release directory.

### Step 6: Create Wrapper Scripts
Create `release/fiend.sh`:
```bash
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
exec "${SCRIPT_DIR}/fiend" "$@"
```

Create `release/mapeditor.sh`:
```bash
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
exec "${SCRIPT_DIR}/mapeditor" "$@"
```

Make them executable:
```bash
chmod +x release/fiend.sh release/mapeditor.sh
```

These wrapper scripts set `LD_LIBRARY_PATH` to find the bundled Allegro library before running the game.

## Directory Structure

After bundling, the release directory looks like:
```
release/
├── fiend              # Game executable
├── fiend.sh           # Wrapper script (sets LD_LIBRARY_PATH)
├── mapeditor          # Map editor executable
├── mapeditor.sh       # Wrapper script
├── lib/               # Bundled libraries
│   ├── liballeg.so.4.4
│   ├── liballeg.so.4.4.3.1
│   └── liballeg.so -> liballeg.so.4.4
├── data/              # Game data
├── graphic/           # Graphics assets
├── maps/              # Map files
├── music/             # Music files
├── sound/             # Sound effects
└── readme.txt         # Game readme
```

## Why This Works

### pkg-config Magic
When you set `PKG_CONFIG_PATH`, `pkg-config --libs allegro` returns the path to your bundled Allegro:
```bash
$ export PKG_CONFIG_PATH="$PWD/external/allegro4/build/install/lib/pkgconfig:$PKG_CONFIG_PATH"
$ pkg-config --libs allegro
-L/path/to/fiend/external/allegro4/build/install/lib -lalleg
```

CMake uses `pkg-config` to find Allegro, so it automatically links against the bundled version.

### Runtime Library Loading
Linux executables look for shared libraries in:
1. Paths in `LD_LIBRARY_PATH` environment variable (highest priority)
2. Paths in `/etc/ld.so.conf` 
3. `/lib` and `/usr/lib` (system paths)

By setting `LD_LIBRARY_PATH` in the wrapper scripts, we tell the executable to load Allegro from `release/lib/` first.

## Advantages

1. **No System Dependencies**: Users don't need to install liballegro4-dev
2. **Version Locked**: Always uses Allegro 4.4.3.1, no version conflicts
3. **Portable**: Works across distributions (Ubuntu, Fedora, Arch, etc.)
4. **Preservation**: Complete source code in repository
5. **Offline Build**: No internet required after initial clone

## Disadvantages

1. **Larger Download**: ~5-10 MB larger due to bundled library
2. **More Complex**: Wrapper scripts needed for `LD_LIBRARY_PATH`
3. **Not Fully Static**: Still depends on system X11, ALSA, glibc

## Future: Static Linking

For truly standalone executables, we could build Allegro and Fiend with static linking:

```bash
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DSHARED=OFF \
  -DBUILD_SHARED_LIBS=OFF
```

This would:
- Eliminate wrapper scripts (no `LD_LIBRARY_PATH` needed)
- Create larger executables (~10-20 MB vs ~500 KB)
- Remove the `lib/` directory entirely
- Work without ANY Allegro installation

## Testing the Bundled Build

To verify the bundled build works:

```bash
cd release

# Check what libraries the executable uses
ldd fiend | grep allegro
# Should show: liballeg.so.4.4 => /path/to/release/lib/liballeg.so.4.4

# Test with wrapper
./fiend.sh --help

# Test without wrapper (should fail if system Allegro not installed)
./fiend --help  # May fail if no system Allegro

# Set LD_LIBRARY_PATH manually
export LD_LIBRARY_PATH="$PWD/lib:$LD_LIBRARY_PATH"
./fiend --help  # Should work now
```

## Troubleshooting

### Error: `liballeg.so.4.4: cannot open shared object file`
The bundled Allegro library isn't being found. Solutions:
1. Use the wrapper scripts (`fiend.sh` instead of `fiend`)
2. Set `LD_LIBRARY_PATH` manually: `export LD_LIBRARY_PATH="$PWD/lib:$LD_LIBRARY_PATH"`
3. Install system Allegro: `sudo apt-get install liballegro4-dev`

### Error: `undefined symbol` when running
Allegro version mismatch. Make sure:
1. Fiend was built with `PKG_CONFIG_PATH` pointing to bundled Allegro
2. No system Allegro in `/usr/lib` that might be loaded instead
3. Rebuild both Allegro and Fiend

### Allegro build fails
Check for missing dependencies:
```bash
sudo apt-get install -y \
  libx11-dev libxext-dev libxcursor-dev \
  libxxf86vm-dev libasound2-dev libpulse-dev
```

## GitHub Actions Workflow

The `.github/workflows/build.yml` automates this entire process:

1. `build-linux`: Traditional build with system Allegro
2. `build-linux-bundled`: Complete bundled build as documented here
3. `build-linux-ubuntu-22`: Compatibility build for older Ubuntu

The bundled artifacts include:
- Game executables
- Wrapper scripts
- Bundled Allegro library
- All game assets
- README with instructions

## Summary

Bundled builds represent the preservation strategy in action:
- ✅ Complete source code in repository (git submodule)
- ✅ Reproducible builds (pinned to 4.4.3.1)
- ✅ No external package dependencies
- ✅ Works offline
- ✅ Portable across distributions

This approach ensures Fiend can be built and played decades from now, even if Allegro 4 packages disappear from repositories.

---

Last Updated: November 2024
Maintained for: Long-term preservation
