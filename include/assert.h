/*
 * assert.h
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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