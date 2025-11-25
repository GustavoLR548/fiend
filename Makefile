.PHONY: all build clean run run-log test analyze help asan asan-build release debug valgrind valgrind-build strict gdb gdb-run quick quick-log log editor

# Default target
all: build

# Build the game (defaults to Release)
build: release

# Release build (optimized, -O2)
release:
	@echo "Building Fiend in Release mode..."
	@mkdir -p build
	@cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -j$$(nproc)
	@echo "✓ Release build complete! Binary: release/fiend"

# Debug build (no optimization, -O0)
debug:
	@echo "Building Fiend in Debug mode..."
	@mkdir -p build
	@cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) -j$$(nproc)
	@echo "✓ Debug build complete! Binary: release/fiend"

# Build with AddressSanitizer for memory debugging
asan:
	@echo "Building with AddressSanitizer (memory debugging)..."
	@mkdir -p build
	@cd build && rm -rf * && \
	cmake .. \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_C_FLAGS="-g -fsanitize=address -fno-omit-frame-pointer" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address" && \
	$(MAKE) -j$$(nproc)
	@echo ""
	@echo "========================================="
	@echo "✓ Build complete with AddressSanitizer!"
	@echo "Run 'make asan-run' to test the game"
	@echo "========================================="

# Run the game with AddressSanitizer
asan-run: asan
	@echo "Running Fiend with AddressSanitizer..."
	@cd release && ./fiend 2>&1 | tee asan-test.log
	@echo ""
	@echo "✓ Test complete! Check asan-test.log for any memory errors"

# Build with Valgrind-friendly flags for deep memory checking
valgrind-build:
	@echo "Building with Valgrind-friendly flags..."
	@mkdir -p build
	@cd build && rm -rf * && \
	cmake .. \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_C_FLAGS="-g -O0 -fno-omit-frame-pointer -fno-inline" && \
	$(MAKE) -j$$(nproc)
	@echo "✓ Build complete for Valgrind testing!"

# Run with Valgrind for deep memory checking
valgrind: valgrind-build
	@echo "Running Fiend with Valgrind (memory error detection)..."
	@command -v valgrind >/dev/null 2>&1 || { \
		echo "Error: Valgrind not installed. Install with:"; \
		echo "  sudo apt install valgrind"; \
		exit 1; \
	}
	@cd release && valgrind \
		--leak-check=full \
		--show-leak-kinds=all \
		--track-origins=yes \
		--verbose \
		--log-file=valgrind.log \
		--error-limit=no \
		./fiend 2>&1 | tee valgrind-console.log || true
	@echo ""
	@echo "========================================="
	@echo "✓ Valgrind complete!"
	@echo "Full report: release/valgrind.log"
	@echo "Console output: release/valgrind-console.log"
	@echo "========================================="
	@echo ""
	@echo "Checking for errors..."
	@grep -E "Invalid (read|write)|Invalid free|Source and destination overlap" release/valgrind.log || echo "✓ No critical errors found"

# Build with strict compiler warnings
strict: 
	@echo "Building with strict warnings (catches many bugs at compile time)..."
	@mkdir -p build
	@cd build && rm -rf * && \
	cmake .. \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_C_FLAGS="-g -O0 -Wall -Wextra -Wformat=2 -Wformat-security \
	  -Wnull-dereference -Wstack-protector -Wstrict-overflow=2 \
	  -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Werror" && \
	$(MAKE) -j$$(nproc)
	@echo "✓ Strict build complete!"

# Build with AddressSanitizer (fast memory bug detection)
asan-build:
	@echo "Building with AddressSanitizer..."
	@mkdir -p build/asan
	@cd build/asan && \
	cmake ../.. \
	  -DCMAKE_BUILD_TYPE=Debug \
	  -DCMAKE_C_FLAGS="-g -O1 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fno-optimize-sibling-calls" \
	  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address -fsanitize=undefined" && \
	$(MAKE) -j$$(nproc)
	@echo "✓ ASan build complete! Binary already in release/"

# Run with AddressSanitizer (much faster than Valgrind!)
asan: asan-build
	@echo "Running with AddressSanitizer (finds buffer overflows, use-after-free, etc.)..."
	@cd release && ASAN_OPTIONS=detect_leaks=0:halt_on_error=0:log_path=asan.log ./fiend 2>&1 | tee asan-console.log || true
	@echo ""
	@echo "========================================="
	@echo "✓ ASan run complete!"
	@echo "Check release/asan.log.* for errors"
	@echo "========================================="
	@if ls release/asan.log.* >/dev/null 2>&1; then \
		echo ""; \
		echo "Errors found:"; \
		cat release/asan.log.*; \
	else \
		echo "✓ No errors detected!"; \
	fi

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build
	@rm -f tests/analyze tests/check_sizes tests/verify_trigger tests/scan_messages tests/dump_trigger tests/analyze_trigger tests/check_gap
	@echo "✓ Clean complete!"

# Analyze trigger structure at specific address
analyze: tests/analyze_trigger tests/check_gap
	@echo "════════════════════════════════════════════════════════════════"
	@echo "  ANALYZING TRIGGER AT 0x0001896C"
	@echo "════════════════════════════════════════════════════════════════"
	@echo ""
	@./tests/analyze_trigger
	@echo ""
	@./tests/check_gap
	@echo ""

tests/analyze_trigger: tests/analyze_trigger.c src/trigger.h
	@echo "Compiling analyze_trigger..."
	@gcc -I./src tests/analyze_trigger.c -o tests/analyze_trigger

tests/check_gap: tests/check_gap.c
	@echo "Compiling check_gap..."
	@gcc tests/check_gap.c -o tests/check_gap

# Run the game
run: build
	@echo "Running Fiend..."
	@cd release && ./fiend

# Run the game with logging to debug.log
run-log: build
	@echo "Running Fiend with logging..."
	@cd release && ./fiend > debug.log 2>&1 || true
	@echo "✓ Log saved to release/debug.log"
	@echo "Last 50 lines:"
	@tail -50 release/debug.log

# Run the game with GDB debugger
gdb: build
	@echo "Running Fiend with GDB..."
	@echo "Type 'run' to start the game"
	@echo "When it crashes, type 'bt' for backtrace"
	@cd release && gdb ./fiend

# Run the game with GDB and auto-run
gdb-run: build
	@echo "Running Fiend with GDB (auto-start)..."
	@cd release && gdb -ex run -ex bt -ex quit --args ./fiend

# Quick rebuild (skip cmake)
quick:
	@echo "Quick rebuild..."
	@cd build && make -j$$(nproc)
	@echo "✓ Quick build complete!"

# Run quick build with logging
quick-log: quick
	@echo "Running Fiend with logging..."
	@cd release && ./fiend > debug.log 2>&1 || true
	@echo "✓ Log saved to release/debug.log"

# View the last run's log
log:
	@if [ -f release/debug.log ]; then \
		less release/debug.log; \
	else \
		echo "No debug.log found. Run 'make run-log' first."; \
	fi

# Run the map editor
editor: build
	@echo "Running Map Editor..."
	@cd release && ./mapeditor

# Test compilation of test files
test:
	@echo "Compiling test files..."
	@cd tests && for f in *.c; do \
		if [ -f "$$f" ]; then \
			echo "  Compiling $$f..."; \
			gcc "$$f" -o "$${f%.c}" -I../src || true; \
		fi \
	done
	@echo "✓ Test compilation complete!"

# Show help
help:
	@echo "Fiend - Build & Run Targets"
	@echo "=============================="
	@echo ""
	@echo "Build Targets:"
	@echo "  make build      - Build in Release mode (default, optimized)"
	@echo "  make release    - Build in Release mode (-O2 optimization)"
	@echo "  make debug      - Build in Debug mode (-O0, full symbols)"
	@echo "  make asan       - Build with AddressSanitizer (memory debugging)"
	@echo "  make quick      - Quick rebuild (skip cmake configuration)"
	@echo "  make clean      - Remove all build artifacts"
	@echo ""
	@echo "Run Targets:"
	@echo "  make run        - Build and run the game"
	@echo "  make run-log    - Build and run with logging to debug.log"
	@echo "  make quick-log  - Quick rebuild and run with logging"
	@echo "  make asan-run   - Build and run with AddressSanitizer"
	@echo "  make valgrind   - Build and run with Valgrind (deep memory check)"
	@echo "  make log        - View the last debug.log"
	@echo ""
	@echo "Debug Targets:"
	@echo "  make gdb        - Run the game with GDB debugger"
	@echo "  make gdb-run    - Run with GDB (auto-start and backtrace)"
	@echo "  make strict     - Build with strict compiler warnings"
	@echo ""
	@echo "Other Targets:"
	@echo "  make editor     - Build and run the map editor"
	@echo "  make test       - Compile test files in tests/"
	@echo "  make analyze    - Analyze trigger structure"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make            # Build in Release mode (most common)"
	@echo "  make run-log    # Build and capture output to debug.log"
	@echo "  make debug      # Build for debugging with GDB"
	@echo "  make asan-run   # Memory debugging with AddressSanitizer"
	@echo "  make valgrind   # Deep memory check (finds ALL bugs)"
	@echo ""
