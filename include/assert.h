/* assert.h
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 */

#pragma once

#define assume(expr)           __attribute__((assume(expr)))
#ifdef DEBUG
_Noreturn void assertion_failed(const char *expr, const char *file, int line);
#define assert(expr)           (__builtin_expect(!!(expr), 1) ? \
					((void) 0) : \
					assertion_failed(#expr, __FILE__, __LINE__))
#else
#define assert(expr)            (assume(expr))
#endif

#define static_assert(expr)     _Static_assert(expr, #expr)