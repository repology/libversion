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

#include <stdint.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

/* helper functions */
#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))

static int my_isalpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int my_isnumber(char c) {
	return c >= '0' && c <= '9';
}

static int my_isseparator(char c) {
	return !my_isnumber(c) && !my_isalpha(c) && c != '\0';
}

static char my_tolower(char c) {
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	else
		return c;
}

static int my_memcasecmp(const char* a, const char* b, size_t len) {
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

const char* skip_alpha(const char* str) {
	const char* cur = str;
	while (my_isalpha(*cur))
		++cur;
	return cur;
}

const char* skip_number(const char* str) {
	const char* cur = str;
	while (my_isnumber(*cur))
		++cur;
	return cur;
}

const char* skip_zeroes(const char* str) {
	const char* cur = str;
	while (*cur == '0')
		++cur;
	return cur;
}

const char* skip_separator(const char* str) {
	const char* cur = str;
	while (my_isseparator(*cur))
		++cur;
	return cur;
}

/* definitions */
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

/* core logic */
static int compare_components(const component_t* u1, const component_t* u2) {
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

	/* numeric comparison */
	const char *p1 = u1->start;
	const char *p2 = u2->start;

	/* skip leading zeroes */
	while (*p1 == '0') {
		++p1;
	}
	while (*p2 == '0') {
		++p2;
	}

	/* compare lengths */
	if (u1->end - p1 < u2->end - p2) {
		return -1;
	}
	if (u1->end - p1 > u2->end - p2) {
		return 1;
	}

	/* numeric comparison itself */
	while (p1 != u1->end) {
		if (*p1 < *p2) {
			return -1;
		}
		if (*p1 > *p2) {
			return 1;
		}

		++p1;
		++p2;
	}

	return 0;
}

static void parse_token_to_component(const char** str, component_t* component, int flags) {
	component->start = *str;

	if (my_isalpha(**str)) {
		if (flags & VERSIONFLAG_ANY_IS_PATCH) {
			component->metaorder = METAORDER_POST_RELEASE;
		} else {
			component->metaorder = METAORDER_PRE_RELEASE;
		}

		component->end = *str = skip_alpha(*str);

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
		component->end = *str = skip_zeroes(*str);
		const char* zeroes_end = *str;
		component->end = *str = skip_number(*str);

		if (zeroes_end == *str) {
			component->metaorder = METAORDER_ZERO;
		} else {
			component->metaorder = METAORDER_NONZERO;
		}
	}
}

static void make_default_component(component_t* component, int flags) {
	static const char* zero = "0";

	if (flags & VERSIONFLAG_LOWER_BOUND) {
		component->metaorder = METAORDER_LOWER_BOUND;
	} else if (flags & VERSIONFLAG_UPPER_BOUND) {
		component->metaorder = METAORDER_UPPER_BOUND;
	} else {
		component->metaorder = METAORDER_ZERO;
	}
	component->start = zero;
	component->end = zero + 1;
}

static size_t get_next_version_component(const char** str, component_t* component, int flags) {
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

/* deprecated */
int version_compare3(const char* v1, const char* v2, int flags) {
	return version_compare4(v1, v2, flags, flags);
}

int version_compare_simple(const char* v1, const char* v2) {
	return version_compare2(v1, v2);
}

int version_compare_flags(const char* v1, const char* v2, int flags) {
	const int v1_flags =
		((flags & VERSIONFLAG_P_IS_PATCH_LEFT) ? VERSIONFLAG_P_IS_PATCH : 0) |
		((flags & VERSIONFLAG_ANY_IS_PATCH_LEFT) ? VERSIONFLAG_ANY_IS_PATCH : 0);
	const int v2_flags =
		((flags & VERSIONFLAG_P_IS_PATCH_RIGHT) ? VERSIONFLAG_P_IS_PATCH : 0) |
		((flags & VERSIONFLAG_ANY_IS_PATCH_RIGHT) ? VERSIONFLAG_ANY_IS_PATCH : 0);

	return version_compare4(v1, v2, v1_flags, v2_flags);
}

int version_compare_flags2(const char* v1, const char* v2, int v1_flags, int v2_flags) {
	return version_compare4(v1, v2, v1_flags, v2_flags);
}
