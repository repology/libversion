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

#include <limits.h>
#include <stdio.h>

#include <libversion/version.h>

typedef enum {
	STATE_SEPARATOR = 0x0,
	STATE_NUMBER    = 0x1,
	STATE_ALPHA     = 0x2,
	STATE_END
} signature_parser_state_t;

#define ROTARY_SHIFT_LEFT(arg, bits) \
	(((arg) << (bits)) | ((arg) >> (sizeof(arg) * CHAR_BIT - 1)))

#define ROTARY_SHIFT_RIGHT(arg, bits) \
	(((arg) >> (bits)) | ((arg) << (sizeof(arg) * CHAR_BIT - 1)))

version_signature_t version_signature_simple(const char* v) {
	signature_parser_state_t state, prev_state = STATE_SEPARATOR;
	version_signature_t result = 1;
	unsigned int bit = 0;

	/*
	 * The idea here is the following:
	 *
	 * 1) We represent our version string as a sequence of objects
	 *    of types {separator, number, alphabetic} ignoring their
	 *    actual values
	 *
	 * 2) We generate a bit string representing transitions from one
	 *    object type to another. Note that since there cannot be
	 *    two consequential objects of the same type, the difference
	 *    is always representable by a single bit, e.g.
	 *
	 *      > | A | B | C
	 *     ---+---+---+---
	 *      A | - | 1 | 0
	 *      B | 0 | - | 1
	 *      C | 1 | 0 | -
	 *
	 * 3) We represent signature as an integer of limited size. Although
	 *    it is long enough to store distinct signature for any sane
	 *    real world version, it may still overflow, so if we just append
	 *    bits as in `result = (result << 1) | bit`, the signature will be
	 *    trimmed. To act on best effort basis, we instead use rotary shift
	 *    and XOR, so extra bits will not be lost and there's a chance we'll
	 *    get different signatures for different versions. The comparison
	 *    is, of couse, still not reliable in this case.
	 */
	while (1) {
		if (*v >= '0' && *v <= '9')
			state = STATE_NUMBER;
		else if ((*v >= 'a' && *v <= 'z') || (*v >= 'A' && *v <= 'Z'))
			state = STATE_ALPHA;
		else if (*v == '\0')
			break;
		else
			state = STATE_SEPARATOR;

		if (state != prev_state) {
			bit = (((2 + state - prev_state) % 3) & 1);

			result = ROTARY_SHIFT_LEFT(result, 1) ^ bit;
		}

		++v;
		prev_state = state;
	}

	/* trailing separator should be ignored. Since we've already
	 * recorded it, undo it */
	if (prev_state == STATE_SEPARATOR)
		result = ROTARY_SHIFT_RIGHT(result ^ bit, 1);

	return result;
}
