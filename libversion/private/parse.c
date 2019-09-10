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

static void parse_token_to_component(const char** str, component_t* component, int flags) {
	if (my_isalpha(**str)) {
		component->start = *str;
		component->end = *str = skip_alpha(*str);

		if (flags & VERSIONFLAG_ANY_IS_PATCH) {
			component->metaorder = METAORDER_POST_RELEASE;
		} else {
			component->metaorder = METAORDER_PRE_RELEASE;
		}

		if (component->end - component->start == 5 && my_memcasecmp(component->start, "alpha", 5) == 0)
			component->metaorder = METAORDER_PRE_RELEASE;
		else if (component->end - component->start == 4 && my_memcasecmp(component->start, "beta", 4) == 0)
			component->metaorder = METAORDER_PRE_RELEASE;
		else if (component->end - component->start == 2 && my_memcasecmp(component->start, "rc", 2) == 0)
			component->metaorder = METAORDER_PRE_RELEASE;
		else if (component->end - component->start >= 3 && my_memcasecmp(component->start, "pre", 3) == 0)
			component->metaorder = METAORDER_PRE_RELEASE;
		else if (component->end - component->start >= 4 && my_memcasecmp(component->start, "post", 4) == 0)
			component->metaorder = METAORDER_POST_RELEASE;
		else if (component->end - component->start == 5 && my_memcasecmp(component->start, "patch", 5) == 0)
			component->metaorder = METAORDER_POST_RELEASE;
		else if (component->end - component->start == 2 && my_memcasecmp(component->start, "pl", 2) == 0)  /* patchlevel */
			component->metaorder = METAORDER_POST_RELEASE;
		else if (component->end - component->start == 6 && my_memcasecmp(component->start, "errata", 2) == 0)
			component->metaorder = METAORDER_POST_RELEASE;
		else if (flags & VERSIONFLAG_P_IS_PATCH && component->end - component->start == 1 && (*component->start == 'p' || *component->start == 'P'))
			component->metaorder = METAORDER_POST_RELEASE;

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

	if (my_isalpha(**str) && (my_isseparator(*(*str + 1)) || *(*str + 1) == '\0')) {
		++component;
		component->metaorder = METAORDER_LETTER_SUFFIX;
		component->start = *str;
		component->end = *str + 1;
		++*str;
		return 2;
	}

	return 1;
}
