# GitHub Actions Workflows

This directory contains automated CI/CD workflows for the Fiend project.

## Workflows

### Build (`build.yml`)

Main CI/CD workflow that runs on every push and pull request.

**Triggers:**
- Push to any branch
- Pull requests to any branch

**Jobs:**

1. **build-linux** - Build on Ubuntu 24.04
   - Installs Allegro 4 and FMOD dependencies
   - Compiles the game and map editor
   - Runs cppcheck static analysis
   - Creates build artifacts

2. **build-linux-ubuntu22** - Build on Ubuntu 22.04
   - Same as above but for older Ubuntu compatibility
   - Uses liballegro4-dev package

3. **create-release** - Create GitHub Release
   - **Only runs on:** Pushes to `main` branch or version tags
   - **Nightly Builds (main branch):**
     - Auto-generates version: `nightly-YYYY.MM.DD-buildNNN-HASH`
     - Build number (NNN) = total commit count
     - Hash = short commit SHA (7 chars)
     - Marked as pre-release
     - Example: `nightly-2024.01.15-build287-a3f2c1b`
   
   - **Stable Releases (tags):**
     - Triggered by pushing a tag: `git tag v1.0.0 && git push origin v1.0.0`
     - Version = tag name
     - Marked as stable release
     - Example: `v1.0.0`
   
   - Creates release archives with version suffix:
     - `fiend-linux-x64-VERSION.zip` (Ubuntu 24.04+)
     - `fiend-linux-x64-VERSION.tar.gz` (Ubuntu 24.04+)
     - `fiend-linux-ubuntu22-x64-VERSION.zip` (Ubuntu 22.04)

## Artifacts

Build artifacts are available for 90 days:
- `fiend-linux-x64` - Ubuntu 24.04+ build
- `fiend-linux-ubuntu22-x64` - Ubuntu 22.04 build

## Versioning

### Automatic Nightly Builds
Every push to `main` automatically creates a nightly release:
```
nightly-<DATE>-build<COUNT>-<HASH>
```
- **DATE**: Build date in YYYY.MM.DD format
- **COUNT**: Total number of commits in repository history
- **HASH**: 7-character git commit SHA

This ensures:
- Chronological sorting (by date)
- Unique version numbers (by commit count)
- Exact commit traceability (by SHA)

### Manual Stable Releases
Create a version tag to trigger a stable release:
```bash
git tag v1.0.0
git push origin v1.0.0
```

Tag format should follow semantic versioning: `v<major>.<minor>.<patch>`

## Static Analysis

cppcheck runs on every build with:
- All checks enabled
- Warning level 3
- Inline suppression allowed
- Errors fail the build
