/*
 * Copyright (c) 2017-2019 Dmitry Marakasov <amdmi3@amdmi3.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef LIBVERSION_VERSION_H
#define LIBVERSION_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libversion/config.h>
#include <libversion/export.h>

#define LIBVERSION_VERSION_ATLEAST(x, y, z) \
	( \
		(LIBVERSION_VERSION_MAJOR > (x)) || \
		(LIBVERSION_VERSION_MAJOR == (x) && LIBVERSION_VERSION_MINOR > (y)) || \
		(LIBVERSION_VERSION_MAJOR == (x) && LIBVERSION_VERSION_MINOR == (y) && LIBVERSION_VERSION_PATCH >= (z)) \
	)

enum {
	VERSIONFLAG_P_IS_PATCH = 0x1,
	VERSIONFLAG_ANY_IS_PATCH = 0x2,
	VERSIONFLAG_LOWER_BOUND = 0x4,
	VERSIONFLAG_UPPER_BOUND = 0x8,
};

extern LIBVERSION_EXPORT int version_compare2(const char* v1, const char* v2);
extern LIBVERSION_EXPORT int version_compare4(const char* v1, const char* v2, int v1_flags, int v2_flags);

#ifdef __cplusplus
}
#endif

#endif /* LIBVERSION_VERSION_H */
