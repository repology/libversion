/*
 * Copyright (c) 2017 Dmitry Marakasov <amdmi3@amdmi3.ru>
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

#include <libversion/compare.h>

#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#define VERCOMP_MAX ((LONG_MAX - 9) / 10)

#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MY_MAX(a, b) ((a) > (b) ? (a) : (b))

static int is_version_char(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static long parse_number(const char** str) {
	const char *cur = *str;
	long number = 0;
	while (*cur >= '0' && *cur <= '9') {
		number = number * 10 + (*cur - '0');
		if (number > VERCOMP_MAX)
			number = VERCOMP_MAX;
		cur++;
	}

	if (cur == *str)
		return -1;

	*str = cur;
	return number;
}

static long parse_alpha(const char** str) {
	char start = **str;

	const char *cur = *str;

	while ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z'))
		cur++;

	if (cur == *str)
		return 0;

	*str = cur;

	if (start >= 'A' && start <= 'Z')
		return start - 'A' + 'a'; // lowercase
	else
		return start;
}

static size_t get_next_version_component(const char** str, long* target) {
	// skip separators
	while (**str != '\0' && !is_version_char(**str))
		++*str;

	// EOL, generate empty component
	if (**str == '\0') {
		*(target++) = 0;
		*(target++) = 0;
		*(target++) = -1;
		return 3;
	}

	const char *end = *str;
	while (is_version_char(*end))
		end++;

	// parse component from string [str; end)
	long number = parse_number(str);
	long alpha = parse_alpha(str);
	long extranumber = parse_number(str);

	// skip remaining alphanumeric part
	while (is_version_char(**str))
		++*str;

	// split part with two numbers
	if (number != -1 && extranumber != -1) {
		*(target++) = number;
		*(target++) = 0;
		*(target++) = -1;
		*(target++) = -1;
		*(target++) = alpha;
		*(target++) = extranumber;
		return 6;
	} else {
		*(target++) = number;
		*(target++) = alpha;
		*(target++) = extranumber;
		return 3;
	}
}

int version_compare_simple(const char* v1, const char* v2) {
	long v1_comps[6];
	long v2_comps[6];
	size_t v1_len = 0;
	size_t v2_len = 0;
	while (*v1 != '\0' || *v2 != '\0' || v1_len || v2_len) {
		if (v1_len == 0)
			v1_len = get_next_version_component(&v1, v1_comps);
		if (v2_len == 0)
			v2_len = get_next_version_component(&v2, v2_comps);

		const size_t shift = MY_MIN(v1_len, v2_len);
		for (size_t i = 0; i < shift; i++) {
			if (v1_comps[i] < v2_comps[i])
				return -1;
			if (v1_comps[i] > v2_comps[i])
				return 1;
		}

		if (v1_len != v2_len) {
			for (size_t i = 0; i < shift; i++) {
				v1_comps[i] = v1_comps[i+shift];
				v2_comps[i] = v2_comps[i+shift];
			}
		}

		v1_len -= shift;
		v2_len -= shift;
	}

	return 0;
}
