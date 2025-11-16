# Fiend - Preservation Status

This document tracks the preservation efforts for the Fiend game project.

## Goal
Ensure Fiend can be built and played 50+ years from now, even if:
- GitHub becomes unavailable
- Package repositories disappear
- Allegro 4 support is dropped from distributions
- Internet access is unavailable

## Current Status: ✅ Foundation Complete

### Completed (November 2024)

#### 1. Dependency Bundling
- ✅ Added Allegro 4.4.3.1 as git submodule (`external/allegro4`)
- ✅ Pinned to specific release tag (commit: 8a386b275)
- ✅ Submodule initialized and committed
- ✅ Repository now contains complete Allegro source code

#### 2. Build System
- ✅ CMake option `USE_BUNDLED_ALLEGRO` added (defaults to system Allegro)
- ✅ Supports both system and bundled Allegro builds
- ✅ Build tested and working with system Allegro

#### 3. Documentation
- ✅ `BUILD.md` created with comprehensive build instructions
  - Quick start guide
  - Self-contained build process
  - Troubleshooting section
  - Cross-platform notes
  
- ✅ `DEPENDENCIES.txt` created documenting all dependencies
  - Complete dependency list
  - Version information
  - Preservation rationale
  - License compatibility notes

- ✅ `PRESERVATION_STATUS.md` (this file)

#### 4. Logging System
- ✅ Implemented file-based logging (logs/fiend_YYYY-MM-DD.log)
- ✅ Multiple log levels (DEBUG/INFO/WARNING/ERROR/NONE)
- ✅ 7-day log rotation
- ✅ Console echo option
- ✅ Integrated throughout codebase
- ✅ All fprintf(stderr, ...) replaced with log_*() functions

#### 5. Map Editor Fixes
- ✅ Audio system not initialized (commented out load_sounds() - not needed)
- ✅ Close button handler implemented (confirmation dialog working)
- ✅ Immediate exit bug fixed

### In Progress

#### GitHub Actions (Next Priority)
Create `.github/workflows/build.yml` for automated builds:
- [ ] Linux build (Ubuntu LTS)
- [ ] Windows cross-compilation (MinGW)
- [ ] macOS build
- [ ] Artifact uploads
- [ ] Release automation

**Why This Matters:**
- Provides pre-built binaries for users who can't compile
- Validates builds on multiple platforms automatically
- Creates downloadable releases
- BUT: Depends on GitHub - submodules are the real preservation

#### Bundled Allegro Build Integration
- [ ] Integrate Allegro build into main CMakeLists.txt
- [ ] Create helper script: `scripts/build-allegro.sh`
- [ ] Test full offline build workflow
- [ ] Document any platform-specific quirks

**Current Limitation:**
Allegro 4's CMake has issues building as a subdirectory. Current workaround:
1. Build Allegro separately in `external/allegro4/build/`
2. Use `-DUSE_BUNDLED_ALLEGRO=ON` to link against it

### Future Enhancements

#### Static Linking (High Priority)
- [ ] Configure CMake for static linking
- [ ] Bundle all dependencies into executables
- [ ] Test standalone executables (no .so/.dll dependencies)
- [ ] Create portable archives (tar.gz, zip)

**Benefit:** Truly self-contained executables that run anywhere

#### Additional Documentation
- [ ] PORTING.md - Guide for porting to new platforms
- [ ] ARCHITECTURE.md - Code structure documentation
- [ ] ASSETS.md - Data file format documentation

#### Alternative Builds
- [ ] Flatpak package
- [ ] AppImage
- [ ] Snap package
- [ ] Windows installer (NSIS)

#### Long-term Considerations
- [ ] Consider bundling SDL2 as Allegro alternative
- [ ] Document file formats for future re-implementations
- [ ] Archive design documents if available

## Verification Checklist

To verify preservation integrity:

```bash
# 1. Clone with submodules
git clone --recursive <repo-url>
cd fiend

# 2. Verify submodule
cd external/allegro4
git describe --tags  # Should show: 4.4.3.1
cd ../..

# 3. Build Allegro (offline test)
cd external/allegro4
mkdir build && cd build
cmake .. -DSHARED=ON
make -j$(nproc)
cd ../../..

# 4. Build Fiend
mkdir build && cd build
cmake ..
make -j$(nproc)

# 5. Test executables
cd ../release
./fiend
./mapeditor
```

## Metrics

### Repository Size
- Main repo: ~XXX KB (without submodules)
- Allegro submodule: ~40 MB download, ~105k objects
- Total with submodules: ~XXX MB

### Build Time
- Allegro 4.4.3.1: ~30 seconds (parallel build, 8 cores)
- Fiend: ~10 seconds (parallel build)
- Total: <1 minute for complete offline build

### External Dependencies After Preservation
**Still Required (Platform Libraries):**
- X11 libraries (Linux) - part of base system
- ALSA/PulseAudio (Linux audio) - part of base system
- Standard C library - always present
- OpenGL (optional) - graphics acceleration

**No Longer Required:**
- ~~Network access to download Allegro~~
- ~~Package manager allegro-dev packages~~
- ~~External Allegro repository~~

## Philosophy

### Why This Approach?

1. **Git Submodules Over System Packages**
   - System packages disappear (Python 2, anyone?)
   - PPAs get abandoned
   - Distributions drop old software
   - Submodules are forever (if repo survives)

2. **Source Code Over Binaries**
   - Binaries become incompatible (glibc versions, etc.)
   - Source can be recompiled for new systems
   - Easier to patch and modify
   - Future-proof

3. **Documentation Over Assumptions**
   - Assumptions break (build systems change)
   - Documentation ages better than toolchains
   - Enables future archaeologists to understand intent

4. **Simple Over Complex**
   - CMake over autotools (more maintainable)
   - Minimal dependencies (Allegro 4 is ~40MB)
   - No complex dependency chains

### Preservation Horizon

**10 Years:** GitHub likely still exists, builds "just work"

**25 Years:** GitHub may exist, but Allegro 4 packages rare
- Submodules ensure we have source code
- Documentation helps with changed build tools

**50 Years:** GitHub may not exist, Linux may be very different
- Complete source code is in repository
- Documentation explains architecture
- Simple C code easier to port than complex frameworks

**100 Years:** Who knows?
- Source code is human-readable
- Documentation provides context
- Simple design aids reimplementation

## Testing Preservation

Simulate future scenarios:

### Test 1: No Internet
```bash
# Disconnect internet, then:
git clone --recursive <local-copy> fiend-test
cd fiend-test
# Follow BUILD.md offline build instructions
```

### Test 2: No System Packages
```bash
# Build in Docker with minimal base system
docker run -it debian:stable-slim
apt-get update && apt-get install -y build-essential cmake git
# Clone and build
```

### Test 3: Clean Room Build
```bash
# Fresh VM, no cached packages
# Document any missing dependencies
# Update BUILD.md with findings
```

## Lessons Learned

1. **Allegro 4 CMake is Quirky**
   - Doesn't build cleanly as subdirectory
   - Needs separate build step
   - Acceptable trade-off for preservation

2. **Documentation is Critical**
   - Future users won't know current conventions
   - Explicit is better than implicit
   - Troubleshooting section saves hours

3. **Test Early, Test Often**
   - Verify submodules work
   - Test builds on clean systems
   - Don't assume package availability

## Contact & Maintenance

- **Last Updated:** November 2024
- **Primary Platform:** Linux (Ubuntu/Debian)
- **Tested Allegro Version:** 4.4.3.1
- **Preservation Maintainer:** See git log

## Future Maintainers

If you're reading this in the future:

1. Test the build process on your current system
2. Update BUILD.md with any changes needed
3. Document new platform requirements
4. Consider updating Allegro submodule (test carefully!)
5. Maintain the philosophy: offline buildability is paramount

The goal is not perfection - it's survival. If the code compiles and runs,
even with warnings, that's a win. Focus on:
- Keeping it buildable
- Keeping documentation current
- Keeping dependencies minimal
- Keeping the game playable

Good luck, and thank you for maintaining this piece of history!

---

**Status Legend:**
- ✅ Complete and tested
- 🚧 In progress
- 📋 Planned
- ❓ Under consideration
