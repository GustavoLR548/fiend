.PHONY: all build clean run run-log test analyze help

# Default target
all: build

# Build the game
build:
	@echo "Building Fiend..."
	@mkdir -p build
	@cd build && cmake .. && make -j$$(nproc)
	@echo "✓ Build complete! Binaries in release/"

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
	@echo "  make build      - Full build (cmake + compile)"
	@echo "  make quick      - Quick rebuild (skip cmake)"
	@echo "  make run        - Build and run the game"
	@echo "  make run-log    - Build and run with logging to debug.log"
	@echo "  make quick-log  - Quick rebuild and run with logging"
	@echo "  make log        - View the last debug.log"
	@echo "  make gdb        - Run the game with GDB debugger"
	@echo "  make gdb-run    - Run the game with GDB (auto-start and backtrace)"
	@echo "  make editor     - Build and run the map editor"
	@echo "  make test       - Compile test files in tests/"
	@echo "  make analyze    - Analyze trigger structure at 0x0001896C"
	@echo "  make clean      - Remove build artifacts"
	@echo "  make help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make run-log    # Most common - build and capture output"
	@echo "  make quick-log  # Fast iteration during development"
	@echo "  make gdb-run    # Debug crashes with automatic backtrace"
	@echo ""
