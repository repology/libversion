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

#ifndef LIBVERSION_PRIVATE_STRING_H
#define LIBVERSION_PRIVATE_STRING_H

#include <stddef.h>

static inline int my_isalpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline int my_isnumber(char c) {
	return c >= '0' && c <= '9';
}

static inline int my_isseparator(char c) {
	return !my_isnumber(c) && !my_isalpha(c) && c != '\0';
}

static inline char my_tolower(char c) {
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	else
		return c;
}

static inline int my_memcasecmp(const char* a, const char* b, size_t len) {
	while (len != 0) {
		unsigned char ua = my_tolower(*a);
		unsigned char ub = my_tolower(*b);

		if (ua != ub)
			return ua - ub;

		a++;
		b++;
		len--;
	}

	return 0;
}

static inline const char* skip_alpha(const char* str) {
	const char* cur = str;
	while (my_isalpha(*cur))
		++cur;
	return cur;
}

static inline const char* skip_number(const char* str) {
	const char* cur = str;
	while (my_isnumber(*cur))
		++cur;
	return cur;
}

static inline const char* skip_zeroes(const char* str) {
	const char* cur = str;
	while (*cur == '0')
		++cur;
	return cur;
}

static inline const char* skip_separator(const char* str) {
	const char* cur = str;
	while (my_isseparator(*cur))
		++cur;
	return cur;
}

#endif /* LIBVERSION_PRIVATE_STRING_H */
