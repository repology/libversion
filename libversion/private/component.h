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

#ifndef LIBVERSION_PRIVATE_COMPONENT_H
#define LIBVERSION_PRIVATE_COMPONENT_H

enum {
	METAORDER_LOWER_BOUND = -1000,
	METAORDER_PRE_RELEASE = -1,
	METAORDER_ZERO = 0,
	METAORDER_POST_RELEASE = 2,
	METAORDER_NONZERO = 3,
	METAORDER_LETTER_SUFFIX = 4,
	METAORDER_UPPER_BOUND = 1000,
};

typedef struct {
	int metaorder;
	const char* start;
	const char* end;
} component_t;

#endif /* LIBVERSION_PRIVATE_COMPONENT_H */
