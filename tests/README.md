# Test Files for Fiend

This directory contains test programs used during debugging and development of the 64-bit Linux port.

## Purpose

These test files were created to debug structure size mismatches between the original 32-bit Windows build and the 64-bit Linux port. They helped identify padding and alignment issues in binary file formats.

## Test Categories

### Structure Size Tests
- `test_struct_size.c` - Verify overall structure sizes
- `test_cond_size.c` - Test CONDITION_DATA structure size
- `test_link_size.c` - Test LINK_DATA structure size  
- `test_trigger_size.c` - Test TRIGGER_DATA structure size
- `test_trigger_unpacked.c` - Test trigger structure without packing
- `test_substructures.c` - Test nested structure sizes

### Binary File Analysis
- `dump_triggers.c` - Hex dump of trigger data from map files
- `find_trigger_start.c` - Locate trigger data position in map files
- `find_set_var_corben.c` - Find specific trigger by name
- `check_104736.c` - Check data at specific file position
- `check_before_nothing.c` - Analyze data before trigger section

### Diagnostic Tools
- `find_link_size.c` - Calculate LINK_DATA size from file layout

## Compiling Tests

From the root directory:
```bash
make test
```

Or compile individually:
```bash
cd tests
gcc test_struct_size.c -I../src -o test_struct_size
./test_struct_size
```

## Key Findings

These tests revealed:
1. **TRIGGER_DATA name field**: 30 bytes (not 40) with 2-byte padding
2. **16-byte gap**: Map file format has 16 mysterious bytes between soundemitors and triggers
3. **Structure alignment**: Explicit padding needed for CONDITION_DATA and EVENT_DATA
4. **File position**: Triggers start at position 104752 (after 16-byte skip from 104736)

## Historical Context

The original game was built with MSVC on 32-bit Windows (2001). The map files contain binary structures with 32-bit alignment. These tests helped reverse-engineer the exact binary layout to make the files compatible with 64-bit GCC on Linux.

## Usage

These files are preserved for reference and future debugging. They are not part of the main build process but may be useful if:
- Porting to other architectures (ARM, big-endian, etc.)
- Debugging save game compatibility
- Understanding the binary file format
- Creating new map editor features

## Related Documentation

See `/home/guara/code/fiend/docs/64BIT_COMPATIBILITY.md` for complete explanation of the structure packing issues and solutions.
