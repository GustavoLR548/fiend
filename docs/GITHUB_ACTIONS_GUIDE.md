# GitHub Actions Workflow - Quick Reference

## What Happens When You Push

When you push to the `master` branch or create a tag, GitHub Actions will automatically:

### 1. Build Three Versions

#### Build A: System Allegro (Traditional)
- Uses Ubuntu's `liballegro4-dev` package
- Quick build (~10 seconds for Fiend)
- Smaller executables
- **Requires users to install Allegro separately**

#### Build B: Bundled Allegro (Preservation) ⭐
- Clones the `external/allegro4` submodule
- Builds Allegro 4.4.3.1 from source (~30 seconds)
- Builds Fiend linked to bundled Allegro (~10 seconds)
- Bundles the Allegro library with executables
- Creates wrapper scripts (`fiend.sh`, `mapeditor.sh`)
- **No system Allegro required - fully self-contained**

#### Build C: Ubuntu 22.04 (Compatibility)
- Uses Ubuntu 22.04 LTS runner
- System Allegro build
- For older systems

### 2. Create Release Artifacts

Three types of downloadable archives:

#### Recommended: Bundled Release
```
fiend-linux-x64-bundled-<version>.zip
fiend-linux-x64-bundled-<version>.tar.gz
```
**Contents:**
- `fiend` and `mapeditor` executables
- `fiend.sh` and `mapeditor.sh` wrapper scripts
- `lib/` directory with Allegro libraries
- All game assets (data, graphics, maps, music, sound)
- README explaining how to run

**How users run it:**
```bash
unzip fiend-linux-x64-bundled-*.zip
cd fiend-linux-x64-bundled-*
./fiend.sh
```

#### System Release
```
fiend-linux-x64-system-<version>.zip
fiend-linux-x64-system-<version>.tar.gz
```
**Contents:**
- `fiend` and `mapeditor` executables
- All game assets
- README explaining Allegro installation

**How users run it:**
```bash
sudo apt-get install liballegro4-dev  # Must install first
unzip fiend-linux-x64-system-*.zip
cd fiend-linux-x64-system-*
./fiend
```

#### Ubuntu 22.04 Release
```
fiend-linux-ubuntu22-x64-<version>.zip
```
For Ubuntu 22.04 LTS users (glibc compatibility).

### 3. Publish GitHub Release

The workflow automatically creates a GitHub Release with:
- Auto-generated version number (if not tagged)
- All three archive types
- Detailed release notes
- Download instructions
- System requirements

## Version Numbering

### Tagged Releases
```bash
git tag v1.0.0
git push --tags
```
Creates: `Fiend v1.0.0` release

### Automatic Nightly Builds
Push to master without tag:
```
nightly-2024.11.16-build123-abc1234
```
Format: `nightly-<DATE>-build<COUNT>-<SHA>`

## Verifying the Build

After pushing, check:
1. Go to **Actions** tab on GitHub
2. Watch the workflow run (takes ~5-10 minutes total)
3. Three build jobs should complete:
   - ✅ Build on Linux (System Allegro)
   - ✅ Build on Linux (Bundled Allegro)
   - ✅ Build on Ubuntu 22.04
4. Static Analysis job runs in parallel
5. Release is created automatically (if on master)

## Expected Build Times

| Job | Duration | Notes |
|-----|----------|-------|
| System Allegro | ~2 min | Apt install + compile Fiend |
| **Bundled Allegro** | ~5 min | **Build Allegro (~3 min) + Fiend (~1 min)** |
| Ubuntu 22.04 | ~2 min | Same as system build |
| Static Analysis | ~1 min | Runs in parallel |
| Create Release | ~1 min | Package and upload |
| **Total** | ~5-6 min | Parallel execution |

## What Gets Tested

The workflow validates:
- ✅ Submodules work correctly (`submodules: recursive`)
- ✅ Allegro 4.4.3.1 builds from source
- ✅ Fiend compiles against bundled Allegro
- ✅ pkg-config finds the bundled version
- ✅ Executables link correctly (`ldd` check)
- ✅ Wrapper scripts are created with correct paths
- ✅ All game assets are included
- ✅ Archives are created successfully
- ✅ Preservation strategy works end-to-end

## Troubleshooting

### Build Fails on "Checkout code with submodules"
**Issue:** Submodule not initialized
**Fix:** Make sure `.gitmodules` is committed and submodule is pushed

### Build Fails on "Build bundled Allegro"
**Issue:** Missing system dependencies for Allegro
**Fix:** Check if workflow has all required packages:
- libx11-dev, libxext-dev, libxcursor-dev, libxxf86vm-dev
- libasound2-dev, libpulse-dev

### Build Succeeds but ldd Shows Wrong Allegro
**Issue:** pkg-config finding system Allegro instead of bundled
**Fix:** Verify `PKG_CONFIG_PATH` is set correctly in workflow

### Release Not Created
**Issue:** Wrong branch or permissions
**Check:**
- Are you on `master` or `main` branch?
- Does workflow have `contents: write` permission?
- Is `GITHUB_TOKEN` available?

## Manual Testing Locally

To test the exact same build process locally:

```bash
# 1. Clone with submodules (like GitHub Actions does)
git clone --recursive https://github.com/YOUR_USERNAME/fiend.git
cd fiend

# 2. Install dependencies
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake \
  libx11-dev libxext-dev libxcursor-dev libxxf86vm-dev \
  libasound2-dev libpulse-dev

# 3. Build Allegro
cd external/allegro4
mkdir -p build && cd build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/install" \
  -DSHARED=ON \
  -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF \
  -DWANT_TOOLS=OFF -DWANT_TESTS=OFF
make -j$(nproc)
make install
cd ../../..

# 4. Build Fiend
mkdir -p build && cd build
export PKG_CONFIG_PATH="$PWD/../external/allegro4/build/install/lib/pkgconfig:$PKG_CONFIG_PATH"
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

# 5. Bundle library
mkdir -p release/lib
cp external/allegro4/build/install/lib/liballeg*.so* release/lib/

# 6. Create wrapper scripts
cat > release/fiend.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
exec "${SCRIPT_DIR}/fiend" "$@"
EOF

cat > release/mapeditor.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"
exec "${SCRIPT_DIR}/mapeditor" "$@"
EOF

chmod +x release/fiend.sh release/mapeditor.sh

# 7. Test
cd release
./fiend.sh
```

## Next Steps After First Successful Build

1. **Download and test** the bundled release from GitHub
2. **Share** with users who don't want to compile
3. **Monitor** GitHub Actions for any failures
4. **Consider** adding Windows/macOS builds (future)
5. **Update** BUILD.md if any issues found

## Key Success Indicators

After the workflow completes:
- ✅ Three build jobs green
- ✅ Artifacts uploaded successfully
- ✅ Release created with all archives
- ✅ Bundled release includes `lib/` directory
- ✅ Wrapper scripts present (`.sh` files)
- ✅ Can download and extract without errors
- ✅ Running `./fiend.sh` works without installing Allegro

## Preservation Validation

The bundled build proves:
1. **Offline capability** - No external packages needed (except base system)
2. **Version pinning** - Always builds Allegro 4.4.3.1
3. **Reproducibility** - Same submodule commit every time
4. **Self-containment** - Allegro library bundled with game
5. **Long-term viability** - Complete source code in repository

---

**Status:** Ready to test - push to master and watch it work!

Last Updated: November 2024
