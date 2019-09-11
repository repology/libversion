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

#include <libversion/version.h>

#include <libversion/private/parse.h>
#include <libversion/private/compare.h>

#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))

int version_compare4(const char* v1, const char* v2, int v1_flags, int v2_flags) {
	component_t v1_components[2], v2_components[2];
	size_t v1_len = 0, v2_len = 0;
	size_t shift, i;

	int v1_extra_components = (v1_flags & (VERSIONFLAG_LOWER_BOUND|VERSIONFLAG_UPPER_BOUND)) ? 1 : 0;
	int v2_extra_components = (v2_flags & (VERSIONFLAG_LOWER_BOUND|VERSIONFLAG_UPPER_BOUND)) ? 1 : 0;

	int v1_exhausted, v2_exhausted;

	int res;

	do {
		if (v1_len == 0)
			v1_len = get_next_version_component(&v1, v1_components, v1_flags);
		if (v2_len == 0)
			v2_len = get_next_version_component(&v2, v2_components, v2_flags);

		shift = MY_MIN(v1_len, v2_len);
		for (i = 0; i < shift; i++) {
			res = compare_components(&v1_components[i], &v2_components[i]);
			if (res != 0)
				return res;
		}

		if (v1_len != v2_len) {
			for (i = 0; i < shift; i++) {
				v1_components[i] = v1_components[i+shift];
				v2_components[i] = v2_components[i+shift];
			}
		}

		v1_len -= shift;
		v2_len -= shift;

		v1_exhausted = *v1 == '\0' && v1_len == 0;
		v2_exhausted = *v2 == '\0' && v2_len == 0;

		if (v1_exhausted && v1_extra_components > 0) {
			v1_extra_components--;
			v1_exhausted = 0;
		}
		if (v2_exhausted && v2_extra_components > 0) {
			v2_extra_components--;
			v2_exhausted = 0;
		}
	} while (!v1_exhausted || !v2_exhausted);

	return 0;
}

int version_compare2(const char* v1, const char* v2) {
	return version_compare4(v1, v2, 0, 0);
}
