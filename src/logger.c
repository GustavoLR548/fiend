////////////////////////////////////////////////////
// Logger - Logging system for Fiend
// 
// Provides file-based logging with multiple log levels,
// timestamps, and automatic log rotation
////////////////////////////////////////////////////

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

#ifdef _WIN32
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
#endif

static FILE *log_file = NULL;
static LogLevel current_log_level = LOG_LEVEL_INFO;
static int console_echo_enabled = 1;
static char log_filepath[512] = {0};
static char log_directory[256] = {0};
static int max_log_days = 7;

// Create directory if it doesn't exist
static int ensure_directory_exists(const char *path) {
    struct stat st = {0};
    
    if (stat(path, &st) == -1) {
        if (mkdir(path, 0755) == -1) {
            fprintf(stderr, "Failed to create log directory '%s': %s\n", path, strerror(errno));
            return 0;
        }
    }
    return 1;
}

// Get current timestamp string
static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Get log level string
static const char* get_level_string(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG:   return "DEBUG";
        case LOG_LEVEL_INFO:    return "INFO";
        case LOG_LEVEL_WARNING: return "WARNING";
        case LOG_LEVEL_ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

// Delete old log files (log rotation)
static void rotate_logs(const char *log_dir, int max_days) {
    if (max_days <= 0) {
        return; // No rotation
    }
    
    DIR *dir = opendir(log_dir);
    if (!dir) {
        return;
    }
    
    time_t now = time(NULL);
    time_t cutoff_time = now - (max_days * 24 * 60 * 60);
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Check if filename matches pattern "fiend_YYYY-MM-DD.log"
        if (strncmp(entry->d_name, "fiend_", 6) == 0 && 
            strstr(entry->d_name, ".log") != NULL) {
            
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", log_dir, entry->d_name);
            
            struct stat st;
            if (stat(filepath, &st) == 0) {
                if (st.st_mtime < cutoff_time) {
                    // Delete old log file
                    if (remove(filepath) == 0) {
                        fprintf(stderr, "Rotated old log file: %s\n", entry->d_name);
                    }
                }
            }
        }
    }
    
    closedir(dir);
}

int logger_init(const char *log_dir, LogLevel level, int console_echo, int max_days) {
    char timestamp[64];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    // Ensure log directory exists
    if (!ensure_directory_exists(log_dir)) {
        return 0;
    }
    
    // Store log directory for rotation
    strncpy(log_directory, log_dir, sizeof(log_directory) - 1);
    log_directory[sizeof(log_directory) - 1] = '\0';
    max_log_days = max_days;
    
    // Rotate old logs
    rotate_logs(log_dir, max_days);
    
    // Create log filename with date
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d", tm_info);
    snprintf(log_filepath, sizeof(log_filepath), "%s/fiend_%s.log", log_dir, timestamp);
    
    // Open log file in append mode
    log_file = fopen(log_filepath, "a");
    if (!log_file) {
        fprintf(stderr, "Failed to open log file '%s': %s\n", log_filepath, strerror(errno));
        return 0;
    }
    
    current_log_level = level;
    console_echo_enabled = console_echo;
    
    // Write session start marker
    get_timestamp(timestamp, sizeof(timestamp));
    fprintf(log_file, "\n========================================\n");
    fprintf(log_file, "Session started: %s\n", timestamp);
    fprintf(log_file, "Log level: %s\n", get_level_string(level));
    fprintf(log_file, "Console echo: %s\n", console_echo ? "enabled" : "disabled");
    if (max_days > 0) {
        fprintf(log_file, "Log rotation: keeping %d days\n", max_days);
    }
    fprintf(log_file, "========================================\n\n");
    fflush(log_file);
    
    return 1;
}

void logger_cleanup(void) {
    if (log_file) {
        char timestamp[64];
        get_timestamp(timestamp, sizeof(timestamp));
        
        fprintf(log_file, "\n========================================\n");
        fprintf(log_file, "Session ended: %s\n", timestamp);
        fprintf(log_file, "========================================\n\n");
        
        fclose(log_file);
        log_file = NULL;
    }
}

void logger_set_level(LogLevel level) {
    current_log_level = level;
    log_info("Log level changed to: %s", get_level_string(level));
}

void logger_set_console_echo(int enabled) {
    console_echo_enabled = enabled;
}

void logger_flush(void) {
    if (log_file) {
        fflush(log_file);
    }
}

LogLevel logger_level_from_string(const char *str) {
    if (strcasecmp(str, "debug") == 0 || strcmp(str, "0") == 0) {
        return LOG_LEVEL_DEBUG;
    } else if (strcasecmp(str, "info") == 0 || strcmp(str, "1") == 0) {
        return LOG_LEVEL_INFO;
    } else if (strcasecmp(str, "warning") == 0 || strcasecmp(str, "warn") == 0 || strcmp(str, "2") == 0) {
        return LOG_LEVEL_WARNING;
    } else if (strcasecmp(str, "error") == 0 || strcmp(str, "3") == 0) {
        return LOG_LEVEL_ERROR;
    } else if (strcasecmp(str, "none") == 0 || strcmp(str, "4") == 0) {
        return LOG_LEVEL_NONE;
    }
    return LOG_LEVEL_INFO; // Default
}

const char* logger_get_filepath(void) {
    return log_filepath;
}

// Core logging function
static void log_message(LogLevel level, const char *format, va_list args) {
    if (level < current_log_level) {
        return;
    }
    
    char timestamp[64];
    get_timestamp(timestamp, sizeof(timestamp));
    
    // Make a copy of args for console output (must do this before first use)
    va_list args_copy;
    int need_console = console_echo_enabled || level >= LOG_LEVEL_WARNING;
    if (need_console) {
        va_copy(args_copy, args);
    }
    
    // Write to log file
    if (log_file) {
        fprintf(log_file, "[%s] [%s] ", timestamp, get_level_string(level));
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
        
        // Flush on errors and warnings for immediate feedback
        if (level >= LOG_LEVEL_WARNING) {
            fflush(log_file);
        }
    }
    
    // Echo to console if enabled (or always for errors/warnings)
    if (need_console) {
        FILE *output = (level >= LOG_LEVEL_WARNING) ? stderr : stdout;
        fprintf(output, "[%s] ", get_level_string(level));
        vfprintf(output, format, args_copy);
        va_end(args_copy);
        fprintf(output, "\n");
        
        if (level >= LOG_LEVEL_WARNING) {
            fflush(output);
        }
    }
}

void log_debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_DEBUG, format, args);
    va_end(args);
}

void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_INFO, format, args);
    va_end(args);
}

void log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_WARNING, format, args);
    va_end(args);
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message(LOG_LEVEL_ERROR, format, args);
    va_end(args);
}
