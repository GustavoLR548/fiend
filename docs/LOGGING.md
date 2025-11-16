# Logging System

Fiend now includes a comprehensive logging system to help with debugging and user support.

## Features

- **Multiple Log Levels**: DEBUG, INFO, WARNING, ERROR, NONE
- **File-based Logging**: All logs are written to `logs/fiend_YYYY-MM-DD.log`
- **Console Echo**: Option to also print logs to stdout/stderr
- **Automatic Rotation**: Old log files are automatically deleted after 7 days
- **Timestamps**: All log entries include timestamps
- **Session Markers**: Clear start/end markers for each game session

## Command-Line Usage

### Setting Log Level

You can set the log level when starting the game:

```bash
# Using string names (recommended)
./fiend --log-level debug     # Show all messages (most verbose)
./fiend --log-level info      # Show info, warnings, and errors (default)
./fiend --log-level warning   # Show only warnings and errors
./fiend --log-level error     # Show only errors
./fiend --log-level none      # Disable all logging

# Using numeric values
./fiend --log-level 0         # DEBUG
./fiend --log-level 1         # INFO
./fiend --log-level 2         # WARNING
./fiend --log-level 3         # ERROR
./fiend --log-level 4         # NONE
```

### Other Command-Line Options

Existing command-line options still work:

```bash
./fiend --debug              # Enable debug mode (separate from logging)
./fiend --vsync              # Enable vsync
./fiend --nosound            # Disable sound
./fiend --map <path>         # Load specific map file
```

You can combine options:

```bash
./fiend --log-level debug --vsync --map maps/cellar/cellar.map
```

## Log File Location

Log files are stored in the `logs/` directory inside the game's release folder:

```
release/
  logs/
    fiend_2025-11-16.log
    fiend_2025-11-15.log
    ...
```

Each day gets a new log file. Files older than 7 days are automatically deleted when the game starts.

## Log Levels Explained

### DEBUG (0)
Most verbose level. Shows all internal operations, useful for developers.

**Example:**
```
[2025-11-16 14:12:14] [DEBUG] Loading map: maps/lauder_alive/lauder.map
[2025-11-16 14:12:14] [DEBUG] Map dimensions: 64x64
[2025-11-16 14:12:14] [DEBUG] Allocated 4096 tiles
```

### INFO (1)
General information about what the game is doing. **This is the default level.**

**Example:**
```
[2025-11-16 14:12:14] [INFO] Fiend starting up...
[2025-11-16 14:12:14] [INFO] Audio initialized successfully
[2025-11-16 14:12:31] [INFO] Fiend shutting down...
```

### WARNING (2)
Potential issues that don't prevent the game from running but should be noted.

**Example:**
```
[2025-11-16 14:12:15] [WARNING] Music file not found: music/missing.wav
[2025-11-16 14:12:15] [WARNING] Falling back to silence
```

### ERROR (3)
Serious problems that may cause issues. Errors are **always shown on console** regardless of console_echo setting.

**Example:**
```
[2025-11-16 14:12:14] [ERROR] Failed to load texture: graphic/missing.bmp
[2025-11-16 14:12:14] [ERROR] Could not initialize graphics mode
```

### NONE (4)
Disables all logging. Not recommended for normal use, but can be useful for performance testing.

## Log File Format

Each log entry includes:

1. **Timestamp**: `[YYYY-MM-DD HH:MM:SS]`
2. **Log Level**: `[DEBUG]`, `[INFO]`, `[WARNING]`, or `[ERROR]`
3. **Message**: The actual log message

**Example:**
```
========================================
Session started: 2025-11-16 14:12:14
Log level: INFO
Console echo: enabled
Log rotation: keeping 7 days
========================================

[2025-11-16 14:12:14] [INFO] Fiend starting up...
[2025-11-16 14:12:14] [INFO] Audio initialized successfully
[2025-11-16 14:12:15] [WARNING] Music file not found: music/test.wav
[2025-11-16 14:12:31] [INFO] Fiend shutting down...

========================================
Session ended: 2025-11-16 14:12:31
========================================
```

## For Developers

### Adding Log Messages

To add logging to your code:

1. Include the logger header:
```c
#include "logger.h"
```

2. Use the appropriate log function:
```c
log_debug("Detailed debug info: value=%d", some_value);
log_info("Something happened");
log_warning("Potential issue detected");
log_error("Something went wrong: %s", error_message);
```

### Log Functions

- `log_debug(format, ...)` - Debug messages (only shown at DEBUG level)
- `log_info(format, ...)` - Informational messages
- `log_warning(format, ...)` - Warnings
- `log_error(format, ...)` - Errors

All functions use printf-style formatting.

### Flushing Logs

To ensure logs are written to disk immediately (useful before crashes):

```c
logger_flush();
```

This is done automatically for WARNING and ERROR level messages.

### Runtime Level Changes

You can change the log level while the game is running:

```c
logger_set_level(LOG_LEVEL_DEBUG);
```

## User Support

When users report bugs, ask them to:

1. Run the game with debug logging: `./fiend --log-level debug`
2. Reproduce the issue
3. Send the log file from `logs/fiend_YYYY-MM-DD.log`

The log file will contain detailed information about what the game was doing when the problem occurred.

## Performance Considerations

- Logging has minimal performance impact
- Log files are buffered for efficiency
- DEBUG level logging may produce large files
- Old log files are automatically cleaned up
- Use INFO level for normal gameplay
- Use DEBUG level only when investigating issues

## Troubleshooting

**Q: Logs aren't being written**  
A: Check that the `logs/` directory can be created. The game will print an error if it fails.

**Q: Log file is empty**  
A: The logger may not have been initialized. Check for initialization errors in console output.

**Q: Too many old log files**  
A: The game automatically deletes files older than 7 days on startup. If you need to keep logs longer, back them up before running the game.

**Q: Console is flooded with messages**  
A: Use a higher log level (e.g., `--log-level warning`) or disable console echo (this would require code modification).

**Q: Where are the logs for the map editor?**  
A: The map editor currently doesn't initialize logging, but it uses the same logger infrastructure. This will be added in a future update.
