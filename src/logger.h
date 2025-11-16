////////////////////////////////////////////////////
// Logger - Logging system for Fiend
// 
// Provides file-based logging with multiple log levels,
// timestamps, and automatic log rotation
////////////////////////////////////////////////////

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

// Log levels
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_ERROR = 3,
    LOG_LEVEL_NONE = 4
} LogLevel;

// Initialize the logging system
// log_dir: Directory to store log files (e.g., "logs/")
// level: Minimum log level to record
// console_echo: If true, also print to stdout/stderr
// max_log_days: Maximum number of days to keep logs (0 = no rotation)
int logger_init(const char *log_dir, LogLevel level, int console_echo, int max_log_days);

// Clean up and close log file
void logger_cleanup(void);

// Set the minimum log level
void logger_set_level(LogLevel level);

// Enable/disable console echo
void logger_set_console_echo(int enabled);

// Core logging functions
void log_debug(const char *format, ...);
void log_info(const char *format, ...);
void log_warning(const char *format, ...);
void log_error(const char *format, ...);

// Flush log buffer to disk (useful before potential crashes)
void logger_flush(void);

// Get log level from string (for command-line parsing)
LogLevel logger_level_from_string(const char *str);

// Get current log file path
const char* logger_get_filepath(void);

#endif // LOGGER_H
