#!/usr/bin/env python3
"""
Generate resource list files for Fiend game assets.
This preprocesses all resource paths to avoid runtime directory scanning issues on Windows.
"""

import os
import sys
from pathlib import Path


def to_relative_path(path, base_path):
    """Return a relative path from base_path to path robustly across Windows/MSYS.

    Uses os.path.relpath as primary method, but also handles cases where
    path and base_path are expressed with different separators or drive
    letters (common in MSYS/MinGW CI).
    Returns a path using forward slashes.
    """
    # Convert to str and normalize separators
    p = os.path.normpath(str(path))
    b = os.path.normpath(str(base_path))

    # On Windows, os.path.relpath handles drive letters; on POSIX it works too.
    try:
        rel = os.path.relpath(p, b)
    except Exception:
        # Fallback: if relpath fails, try to strip prefix case-insensitively
        lp = p.lower()
        lb = b.lower()
        if lp.startswith(lb):
            rel = p[len(b):].lstrip('/\\')
        else:
            # As a last resort, return the basename
            rel = os.path.basename(p)

    # Use forward slashes for consistency in resource lists
    rel = rel.replace('\\', '/').replace(os.sep, '/')
    return rel


def scan_bmp_array_directory(base_path, rel_dir, output_file):
    """Scan a directory for BMP files and write the list to output file."""
    # Accept rel_dir as either a Path or a relative string
    full_dir = Path(base_path) / rel_dir

    if not full_dir.exists():
        print(f"Warning: Directory not found: {full_dir}")
        return 0

    # Find all .bmp files
    bmp_files = sorted(full_dir.glob("*.bmp"))

    if not bmp_files:
        print(f"Warning: No BMP files found in {full_dir}")
        return 0

    # Write relative paths (with forward slashes for consistency)
    for bmp_file in bmp_files:
        rel_path = to_relative_path(bmp_file, base_path)
        output_file.write(f"{rel_path}\n")

    return len(bmp_files)


def generate_character_list(base_path, output_path):
    """Generate list of character sprite directories."""
    output_file = output_path / "characters.txt"
    char_dir = base_path / "graphic" / "characters"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        # List all character subdirectories
        if char_dir.exists():
            for subdir in sorted(char_dir.iterdir()):
                if subdir.is_dir():
                    count = scan_bmp_array_directory(base_path, subdir.relative_to(base_path), f)
                    print(f"  {subdir.name}: {count} files")
        else:
            print(f"Warning: {char_dir} not found")
    
    print(f"Created {output_file}")


def generate_enemy_list(base_path, output_path):
    """Generate list of enemy sprite directories."""
    output_file = output_path / "enemies.txt"
    enemy_dir = base_path / "graphic" / "enemies"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        if enemy_dir.exists():
            for subdir in sorted(enemy_dir.iterdir()):
                if subdir.is_dir():
                    count = scan_bmp_array_directory(base_path, subdir.relative_to(base_path), f)
                    print(f"  {subdir.name}: {count} files")
        else:
            print(f"Warning: {enemy_dir} not found")
    
    print(f"Created {output_file}")


def generate_object_list(base_path, output_path):
    """Generate list of object sprite directories (recursive for house, etc)."""
    output_file = output_path / "objects.txt"
    object_dir = base_path / "graphic" / "objects"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        if object_dir.exists():
            # Recursively find all directories with BMP files
            for root, dirs, files in sorted(os.walk(object_dir)):
                root_path = Path(root)
                bmp_files = [f for f in files if f.lower().endswith('.bmp')]
                
                if bmp_files:
                    rel_dir = root_path.relative_to(base_path)
                    count = scan_bmp_array_directory(base_path, rel_dir, f)
                    print(f"  {rel_dir}: {count} files")
        else:
            print(f"Warning: {object_dir} not found")
    
    print(f"Created {output_file}")


def generate_face_list(base_path, output_path):
    """Generate list of message face images."""
    output_file = output_path / "faces.txt"
    face_dir = base_path / "graphic" / "faces"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        if face_dir.exists():
            count = scan_bmp_array_directory(base_path, face_dir.relative_to(base_path), f)
            print(f"  faces: {count} files")
        else:
            print(f"Warning: {face_dir} not found")
    
    print(f"Created {output_file}")


def generate_tile_list(base_path, output_path):
    """Generate list of tile directories."""
    output_file = output_path / "tiles.txt"
    tile_dir = base_path / "graphic" / "tiles"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        if tile_dir.exists():
            for subdir in sorted(tile_dir.iterdir()):
                if subdir.is_dir():
                    count = scan_bmp_array_directory(base_path, subdir.relative_to(base_path), f)
                    print(f"  {subdir.name}: {count} files")
        else:
            print(f"Warning: {tile_dir} not found")
    
    print(f"Created {output_file}")


def generate_particle_list(base_path, output_path):
    """Generate list of particle sprite directories."""
    output_file = output_path / "particles.txt"
    particle_dir = base_path / "graphic" / "particles"
    
    print(f"\nGenerating {output_file}...")
    
    with open(output_file, 'w') as f:
        if particle_dir.exists():
            for subdir in sorted(particle_dir.iterdir()):
                if subdir.is_dir():
                    count = scan_bmp_array_directory(base_path, subdir.relative_to(base_path), f)
                    print(f"  {subdir.name}: {count} files")
        else:
            print(f"Warning: {particle_dir} not found")
    
    print(f"Created {output_file}")


def main():
    # Determine base path (should be 'release' directory)
    if len(sys.argv) > 1:
        base_path = Path(sys.argv[1])
    else:
        # Default to release directory relative to script location
        script_dir = Path(__file__).parent
        base_path = script_dir.parent / "release"
    
    if not base_path.exists():
        print(f"Error: Base path not found: {base_path}")
        sys.exit(1)
    
    # Output directory for resource lists
    output_path = base_path / "data" / "resource_lists"
    output_path.mkdir(parents=True, exist_ok=True)
    
    print(f"Scanning resources in: {base_path}")
    print(f"Output directory: {output_path}")
    
    # Generate all resource lists
    generate_character_list(base_path, output_path)
    generate_enemy_list(base_path, output_path)
    generate_object_list(base_path, output_path)
    generate_face_list(base_path, output_path)
    generate_tile_list(base_path, output_path)
    generate_particle_list(base_path, output_path)
    
    print(f"\nAll resource lists generated successfully in {output_path}")


if __name__ == "__main__":
    main()
