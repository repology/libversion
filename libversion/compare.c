/*
 * Copyright (c) 2017-2018 Dmitry Marakasov <amdmi3@amdmi3.ru>
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

#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))

#if defined(INT64_MAX)
	typedef int64_t version_component_t;
	#define VERSION_COMPONENT_MAX ((INT64_MAX - 9) / 10)
#elif defined(LLONG_MAX)
	typedef long long version_component_t;
	#define VERSION_COMPONENT_MAX ((LLONG_MAX - 9) / 10)
#else
	typedef long version_component_t;
	#define VERSION_COMPONENT_MAX ((LONG_MAX - 9) / 10)
#endif

static int is_version_char(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static version_component_t parse_number(const char** str) {
	const char* cur = *str;
	version_component_t number = 0;
	while (*cur >= '0' && *cur <= '9') {
		number = number * 10 + (*cur - '0');
		if (number > VERSION_COMPONENT_MAX)
			number = VERSION_COMPONENT_MAX;
		cur++;
	}

	if (cur == *str)
		return -1;

	*str = cur;
	return number;
}

enum {
	ALPHAFLAG_PRERELEASE = 1,
	ALPHAFLAG_POSTRELEASE = 2
};

static int mymemcasecmp(const char* a, const char* b, size_t len) {
	while (len-- != 0) {
		unsigned char ua = (unsigned char)((*a >= 'A' && *a <= 'Z') ? (*a - 'A' + 'a') : (*a));
		unsigned char ub = (unsigned char)((*b >= 'A' && *b <= 'Z') ? (*b - 'A' + 'a') : (*b));

		if (ua != ub)
			return ua - ub;

		a++;
		b++;
	}

	return 0;
}

static version_component_t parse_alpha(const char** str, int* outflags, int flags) {
	char start = **str;

	const char* cur = *str;

	while ((*cur >= 'a' && *cur <= 'z') || (*cur >= 'A' && *cur <= 'Z'))
		cur++;

	*outflags = 0;

	if (cur == *str)
		return -1;
	else if (cur - *str == 5 && mymemcasecmp(*str, "alpha", 5) == 0)
		*outflags = ALPHAFLAG_PRERELEASE;
	else if (cur - *str == 4 && mymemcasecmp(*str, "beta", 4) == 0)
		*outflags = ALPHAFLAG_PRERELEASE;
	else if (cur - *str == 2 && mymemcasecmp(*str, "rc", 2) == 0)
		*outflags = ALPHAFLAG_PRERELEASE;
	else if (cur - *str >= 3 && mymemcasecmp(*str, "pre", 3) == 0)
		*outflags = ALPHAFLAG_PRERELEASE;
	else if (cur - *str >= 4 && mymemcasecmp(*str, "post", 4) == 0)
		*outflags = ALPHAFLAG_POSTRELEASE;
	else if (cur - *str == 5 && mymemcasecmp(*str, "patch", 5) == 0)
		*outflags = ALPHAFLAG_POSTRELEASE;
	else if (cur - *str == 2 && mymemcasecmp(*str, "pl", 2) == 0)  /* patchlevel */
		*outflags = ALPHAFLAG_POSTRELEASE;
	else if (flags & VERSIONFLAG_P_IS_PATCH && cur - *str == 1 && (**str == 'p' || **str == 'P'))
		*outflags = ALPHAFLAG_POSTRELEASE;

	*str = cur;

	if (start >= 'A' && start <= 'Z')
		return start - 'A' + 'a';  /* lowercase */
	else
		return start;
}

static size_t get_next_version_component(const char** str, version_component_t* target, int flags) {
	const char* end;
	version_component_t number, alpha, extranumber;
	int alphaflags = 0;

	/* skip separators */
	while (**str != '\0' && !is_version_char(**str))
		++*str;

	/* EOL, generate empty component */
	if (**str == '\0') {
		*(target++) = 0;
		*(target++) = -1;
		*(target++) = -1;
		return 3;
	}

	end = *str;
	while (is_version_char(*end))
		end++;

	/* parse component from string [str; end) */
	number = parse_number(str);
	alpha = parse_alpha(str, &alphaflags, flags);
	extranumber = parse_number(str);

	/* skip remaining alphanumeric part */
	while (is_version_char(**str))
		++*str;

	if (flags & VERSIONFLAG_ANY_IS_PATCH)
		alphaflags = ALPHAFLAG_POSTRELEASE;

	if (number != -1 && extranumber != -1) {
		/*
		 * `1a1' -> treat as [1  ].[ a1]
		 * `1patch1' -> special case, treat as [1  ].[0p1]
		 */
		*(target++) = number;
		*(target++) = -1;
		*(target++) = -1;
		*(target++) = (alphaflags == ALPHAFLAG_POSTRELEASE) ? 0 : -1;
		*(target++) = alpha;
		*(target++) = extranumber;
		return 6;
	} else if (number != -1 && alpha != -1 && alphaflags) {
		/*
		 * when alpha part is known to mean prerelease,
		 * not a version addendum, unglue it from number
		 *
		 * `1alpha' is treated as [1  ].[ a ], not [1a ]
		 */
		*(target++) = number;
		*(target++) = -1;
		*(target++) = -1;
		*(target++) = (alphaflags == ALPHAFLAG_POSTRELEASE) ? 0 : -1;
		*(target++) = alpha;
		*(target++) = -1;
		return 6;
	} else {
		if (number == -1 && alphaflags == ALPHAFLAG_POSTRELEASE)
			number = 0;
		*(target++) = number;
		*(target++) = alpha;
		*(target++) = extranumber;
		return 3;
	}
}

int version_compare_simple(const char* v1, const char* v2) {
	return version_compare_flags(v1, v2, 0);
}

int version_compare_flags(const char* v1, const char* v2, int flags) {
	const int v1_flags =
		((flags & VERSIONFLAG_P_IS_PATCH_LEFT) ? VERSIONFLAG_P_IS_PATCH : 0) |
		((flags & VERSIONFLAG_ANY_IS_PATCH_LEFT) ? VERSIONFLAG_ANY_IS_PATCH : 0);
	const int v2_flags =
		((flags & VERSIONFLAG_P_IS_PATCH_RIGHT) ? VERSIONFLAG_P_IS_PATCH : 0) |
		((flags & VERSIONFLAG_ANY_IS_PATCH_RIGHT) ? VERSIONFLAG_ANY_IS_PATCH : 0);

	return version_compare_flags2(v1, v2, v1_flags, v2_flags);
}

int version_compare_flags2(const char* v1, const char* v2, int v1_flags, int v2_flags) {
	version_component_t v1_comps[6], v2_comps[6];
	size_t v1_len = 0, v2_len = 0;
	size_t shift, i;

	while (*v1 != '\0' || *v2 != '\0' || v1_len || v2_len) {
		if (v1_len == 0)
			v1_len = get_next_version_component(&v1, v1_comps, v1_flags);
		if (v2_len == 0)
			v2_len = get_next_version_component(&v2, v2_comps, v2_flags);

		shift = MY_MIN(v1_len, v2_len);
		for (i = 0; i < shift; i++) {
			if (v1_comps[i] < v2_comps[i])
				return -1;
			if (v1_comps[i] > v2_comps[i])
				return 1;
		}

		if (v1_len != v2_len) {
			for (i = 0; i < shift; i++) {
				v1_comps[i] = v1_comps[i+shift];
				v2_comps[i] = v2_comps[i+shift];
			}
		}

		v1_len -= shift;
		v2_len -= shift;
	}

	return 0;
}
