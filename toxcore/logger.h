/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright © 2016-2018 The TokTok team.
 * Copyright © 2013 Tox project.
 */

/**
 * Logger abstraction backed by callbacks for writing.
 */
#ifndef C_TOXCORE_TOXCORE_LOGGER_H
#define C_TOXCORE_TOXCORE_LOGGER_H

#include <stdint.h>
#include <stdarg.h>
#include <pthread.h>

#include "attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MUTEXLOCKINGDEBUG
/*
 * hook mutex function so we can nicely log them (to the NULL logger!)
 */
#define pthread_mutex_lock(MTX) my_pthread_mutex_lock(MTX, #MTX, __FILE__, __LINE__, __func__)
#define pthread_mutex_unlock(MTX) my_pthread_mutex_unlock(MTX, #MTX, __FILE__, __LINE__, __func__)

int my_pthread_mutex_lock(pthread_mutex_t *mutex, const char *mutex_name, const char *file, int line, const char *func);
int my_pthread_mutex_unlock(pthread_mutex_t *mutex, const char *mutex_name, const char *file, int line, const char *func);
#endif

#ifndef MIN_LOGGER_LEVEL
#define MIN_LOGGER_LEVEL LOGGER_LEVEL_INFO
#endif

// NOTE: Don't forget to update build system files after modifying the enum.
typedef enum Logger_Level {
    LOGGER_LEVEL_TRACE,
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_ERROR,
} Logger_Level;

typedef struct Logger Logger;

typedef void logger_cb(void *context, Logger_Level level, const char *file, int line,
                       const char *func, const char *message, void *userdata);

/**
 * Creates a new logger with logging disabled (callback is NULL) by default.
 */
Logger *logger_new(void);

/**
 * Frees all resources associated with the logger.
 */
nullable(1)
void logger_kill(Logger *log);

/**
 * Sets the logger callback. Disables logging if set to NULL.
 * The context parameter is passed to the callback as first argument.
 */
non_null(1) nullable(2, 3, 4)
void logger_callback_log(Logger *log, logger_cb *function, void *context, void *userdata);

/** @brief Main write function. If logging is disabled, this does nothing.
 *
 * If the logger is NULL and `NDEBUG` is not defined, this writes to stderr.
 * This behaviour should not be used in production code, but can be useful for
 * temporarily debugging a function that does not have a logger available. It's
 * essentially `fprintf(stderr, ...)`, but with source location.
 *
 * If `NDEBUG` is defined, the NULL logger does nothing.
 */
non_null(3, 5, 6) nullable(1) GNU_PRINTF(6, 7)
void logger_write(
    const Logger *log, Logger_Level level, const char *file, int line, const char *func,
    const char *format, ...);

void logger_api_write(const Logger *log, Logger_Level level, const char *file, int line, const char *func,
                      const char *format, va_list args) GNU_PRINTF(6, 0);

/**
 * ==================================================================================================
 * toxcore public api call to logger, used by ToxAV
 */
#ifndef TOX_DEFINED
#define TOX_DEFINED
typedef struct Tox Tox;
#endif /* TOX_DEFINED */

void tox_logmsg(const Tox *tox, Logger_Level level, const char *file, int line, const char *func, const char *fmt,
                ...) GNU_PRINTF(6, 7);

#define LOGGER_API_TRACE(tox, ...)   LOGGER_API_WRITE(tox, LOGGER_LEVEL_TRACE  , __VA_ARGS__)
#define LOGGER_API_DEBUG(tox, ...)   LOGGER_API_WRITE(tox, LOGGER_LEVEL_DEBUG  , __VA_ARGS__)
#define LOGGER_API_INFO(tox, ...)    LOGGER_API_WRITE(tox, LOGGER_LEVEL_INFO   , __VA_ARGS__)
#define LOGGER_API_WARNING(tox, ...) LOGGER_API_WRITE(tox, LOGGER_LEVEL_WARNING, __VA_ARGS__)
#define LOGGER_API_ERROR(tox, ...)   LOGGER_API_WRITE(tox, LOGGER_LEVEL_ERROR  , __VA_ARGS__)

#define LOGGER_API_WRITE(tox, level, ...) \
    do { \
        if (level >= MIN_LOGGER_LEVEL) { \
            tox_logmsg(tox, level, __FILE__, __LINE__, __func__, __VA_ARGS__); \
        } \
    } while (0)

/**
 * ==================================================================================================
 */

#define LOGGER_WRITE(log, level, ...)                                            \
    do {                                                                         \
        if (level >= MIN_LOGGER_LEVEL) {                                         \
            logger_write(log, level, __FILE__, __LINE__, __func__, __VA_ARGS__); \
        }                                                                        \
    } while (0)

/* To log with an logger */
#define LOGGER_TRACE(log, ...)   LOGGER_WRITE(log, LOGGER_LEVEL_TRACE, __VA_ARGS__)
#define LOGGER_DEBUG(log, ...)   LOGGER_WRITE(log, LOGGER_LEVEL_DEBUG, __VA_ARGS__)
#define LOGGER_INFO(log, ...)    LOGGER_WRITE(log, LOGGER_LEVEL_INFO, __VA_ARGS__)
#define LOGGER_WARNING(log, ...) LOGGER_WRITE(log, LOGGER_LEVEL_WARNING, __VA_ARGS__)
#define LOGGER_ERROR(log, ...)   LOGGER_WRITE(log, LOGGER_LEVEL_ERROR, __VA_ARGS__)

#define LOGGER_FATAL(log, ...)          \
    do {                                \
        LOGGER_ERROR(log, __VA_ARGS__); \
        abort();                        \
    } while (0)

#define LOGGER_ASSERT(log, cond, ...)              \
    do {                                           \
        if (!(cond)) {                             \
            LOGGER_ERROR(log, "Assertion failed"); \
            LOGGER_FATAL(log, __VA_ARGS__);        \
        }                                          \
    } while (0)

#ifdef __cplusplus
}  // extern "C"
#endif

#endif // C_TOXCORE_TOXCORE_LOGGER_H
