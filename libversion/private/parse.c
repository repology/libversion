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

#include <libversion/private/parse.h>

#include <libversion/private/string.h>
#include <libversion/version.h>

enum {
	KEYWORD_UNKNOWN,
	KEYWORD_PRE_RELEASE,
	KEYWORD_POST_RELEASE,
};

static int classify_keyword(const char* start, const char* end, int flags) {
	if (end - start == 5 && my_memcasecmp(start, "alpha", 5) == 0)
		return KEYWORD_PRE_RELEASE;
	else if (end - start == 4 && my_memcasecmp(start, "beta", 4) == 0)
		return KEYWORD_PRE_RELEASE;
	else if (end - start == 2 && my_memcasecmp(start, "rc", 2) == 0)
		return KEYWORD_PRE_RELEASE;
	else if (end - start >= 3 && my_memcasecmp(start, "pre", 3) == 0)
		return KEYWORD_PRE_RELEASE;
	else if (end - start >= 4 && my_memcasecmp(start, "post", 4) == 0)
		return KEYWORD_POST_RELEASE;
	else if (end - start >= 5 && my_memcasecmp(start, "patch", 5) == 0)
		return KEYWORD_POST_RELEASE;
	else if (end - start == 2 && my_memcasecmp(start, "pl", 2) == 0)  /* patchlevel */
		return KEYWORD_POST_RELEASE;
	else if (end - start == 6 && my_memcasecmp(start, "errata", 6) == 0)
		return KEYWORD_POST_RELEASE;
	else if (flags & VERSIONFLAG_P_IS_PATCH && end - start == 1 && (*start == 'p' || *start == 'P'))
		return KEYWORD_POST_RELEASE;

	return KEYWORD_UNKNOWN;
}

static void parse_token_to_component(const char** str, component_t* component, int flags) {
	if (my_isalpha(**str)) {
		component->start = *str;
		component->end = *str = skip_alpha(*str);

		switch (classify_keyword(component->start, component->end, flags)) {
		case KEYWORD_UNKNOWN:
			component->metaorder = (flags & VERSIONFLAG_ANY_IS_PATCH) ? METAORDER_POST_RELEASE : METAORDER_PRE_RELEASE;
			break;
		case KEYWORD_PRE_RELEASE:
			component->metaorder = METAORDER_PRE_RELEASE;
			break;
		case KEYWORD_POST_RELEASE:
			component->metaorder = METAORDER_POST_RELEASE;
			break;
		}
	} else {
		component->start = *str = skip_zeroes(*str);
		component->end = *str = skip_number(*str);

		if (component->start == component->end) {
			component->metaorder = METAORDER_ZERO;
		} else {
			component->metaorder = METAORDER_NONZERO;
		}
	}
}

static void make_default_component(component_t* component, int flags) {
	static const char* empty = "";

	if (flags & VERSIONFLAG_LOWER_BOUND) {
		component->metaorder = METAORDER_LOWER_BOUND;
	} else if (flags & VERSIONFLAG_UPPER_BOUND) {
		component->metaorder = METAORDER_UPPER_BOUND;
	} else {
		component->metaorder = METAORDER_ZERO;
	}
	component->start = empty;
	component->end = empty;
}

size_t get_next_version_component(const char** str, component_t* component, int flags) {
	*str = skip_separator(*str);

	if (**str == '\0') {
		make_default_component(component, flags);
		return 1;
	}

	parse_token_to_component(str, component, flags);

	/* Special case for letter suffix:
	 * - We taste whether the next component is alpha not followed by a number,
	 *   e.g 1a, 1a.1, but not 1a1
	 * - We check whether it's known keyword (in which case it's treated normally)
	 * - Otherwise, it's treated as letter suffix
	 */
	if (my_isalpha(**str)) {
		++component;

		component->start = *str;
		component->end = skip_alpha(*str);

		if (!my_isnumber(*component->end)) {
			switch (classify_keyword(component->start, component->end, flags)) {
			case KEYWORD_UNKNOWN:
				component->metaorder = METAORDER_LETTER_SUFFIX;
				break;
			case KEYWORD_PRE_RELEASE:
				component->metaorder = METAORDER_PRE_RELEASE;
				break;
			case KEYWORD_POST_RELEASE:
				component->metaorder = METAORDER_POST_RELEASE;
				break;
			}

			*str = component->end;
			return 2;
		}
	}

	return 1;
}
