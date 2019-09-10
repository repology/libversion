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

#include <libversion/private/compare.h>

#include <string.h>

#include <libversion/private/string.h>

int compare_components(const component_t* u1, const component_t* u2) {
	/* metaorder has highest priority */
	if (u1->metaorder < u2->metaorder) {
		return -1;
	}
	if (u1->metaorder > u2->metaorder) {
		return 1;
	}

	/* empty strings come before everything */
	int u1_is_empty = u1->start == u1->end;
	int u2_is_empty = u2->start == u2->end;

	if (u1_is_empty && u2_is_empty) {
		return 0;
	}
	if (u1_is_empty) {
		return -1;
	}
	if (u2_is_empty) {
		return 1;
	}

	/* alpha come before numbers  */
	int u1_is_alpha = my_isalpha(*u1->start);
	int u2_is_alpha = my_isalpha(*u2->start);

	if (u1_is_alpha && u2_is_alpha) {
		if (my_tolower(*u1->start) < my_tolower(*u2->start)) {
			return -1;
		}
		if (my_tolower(*u1->start) > my_tolower(*u2->start)) {
			return 1;
		}
		return 0;
	}
	if (u1_is_alpha) {
		return -1;
	}
	if (u2_is_alpha) {
		return 1;
	}

	/* numeric comparison (note that leading zeroes are already trimmed here) */
	if (u1->end - u1->start < u2->end - u2->start) {
		return -1;
	}
	if (u1->end - u1->start > u2->end - u2->start) {
		return 1;
	}

	int res = memcmp(u1->start, u2->start, u1->end - u1->start);
	if (res < 0) {
		return -1;
	}
	if (res > 0) {
		return 1;
	}
	return 0;
}
