#!/bin/bash
# Test script to debug crashes

echo "========================================="
echo "Fiend Crash Debugging Script"
echo "========================================="
echo ""
echo "This will run the game with GDB and catch crashes."
echo ""
echo "Game Instructions:"
echo "1. Press ENTER to skip intro"
echo "2. Move to George's house (the building near spawn)"
echo "3. Press ALT to enter the door"
echo "4. Try to go upstairs (second floor trigger)"
echo "5. If it crashes, we'll see the backtrace"
echo ""
echo "Press ENTER to start..."
read

cd release

# Run with GDB and automatic backtrace
echo ""
echo "Starting game with GDB..."
echo ""

gdb -batch \
    -ex "set pagination off" \
    -ex "run" \
    -ex "thread apply all bt" \
    -ex "quit" \
    ./fiend 2>&1 | tee ../crash_debug.log

echo ""
echo "========================================="
echo "Crash log saved to: crash_debug.log"
echo "========================================="
echo ""

if grep -q "Segmentation fault\|Aborted\|signal SIG" ../crash_debug.log; then
    echo "❌ CRASH DETECTED!"
    echo ""
    echo "Backtrace:"
    grep -A 20 "^#0" ../crash_debug.log | head -30
else
    echo "✅ No crash detected (game exited normally)"
fi

