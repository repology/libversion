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

#define LIBVERSION_NO_DEPRECATED /* disable deprecated APIs */

#include <libversion/version.h>

#include <stdio.h>
#include <string.h>

static char comparison_to_char(int comp) {
	if (comp < 0)
		return '<';
	if (comp > 0)
		return '>';
	return '=';
}

static int version_test(const char* v1, const char* v2, int flags1, int flags2, int expected) {
	int result;

	if (flags1 == 0 && flags2 == 0) {
		result = version_compare2(v1, v2);
	} else {
		result = version_compare4(v1, v2, flags1, flags2);
	}

	if (result == expected) {
		fprintf(stderr, "[ OK ] \"%s\" (0x%x) %c \"%s\" (0x%x)\n", v1, flags1, comparison_to_char(expected), v2, flags2);
		return 0;
	} else {
		fprintf(stderr, "[FAIL] \"%s\" (0x%x) %c \"%s\" (0x%x): got %c\n", v1, flags1, comparison_to_char(expected), v2, flags2, comparison_to_char(result));
		return 1;
	}
}

static int version_test_symmetrical_flags(const char* v1, const char* v2, int flags1, int flags2, int expected) {
	return version_test(v1, v2, flags1, flags2, expected) + version_test(v2, v1, flags2, flags1, -expected);
}

static int version_test_symmetrical(const char* v1, const char* v2, int expected) {
	return version_test_symmetrical_flags(v1, v2, 0, 0, expected);
}

int main() {
	int errors = 0;

	fprintf(stderr, "Test group: equality\n");
	errors += version_test_symmetrical("0", "0", 0);
	errors += version_test_symmetrical("0a", "0a", 0);
	errors += version_test_symmetrical("a", "a", 0);
	errors += version_test_symmetrical("a0", "a0", 0);
	errors += version_test_symmetrical("0a1", "0a1", 0);
	errors += version_test_symmetrical("0a1b2", "0a1b2", 0);
	errors += version_test_symmetrical("1alpha1", "1alpha1", 0);
	errors += version_test_symmetrical("foo", "foo", 0);
	errors += version_test_symmetrical("1.2.3", "1.2.3", 0);
	errors += version_test_symmetrical("hello.world", "hello.world", 0);

	fprintf(stderr, "\nTest group: different number of components\n");
	errors += version_test_symmetrical("1", "1.0", 0);
	errors += version_test_symmetrical("1", "1.0.0", 0);
	errors += version_test_symmetrical("1.0", "1.0.0", 0);
	errors += version_test_symmetrical("1.0", "1.0.0.0.0.0.0.0", 0);

	fprintf(stderr, "\nTest group: leading zeroes\n");
	errors += version_test_symmetrical("00100.00100", "100.100", 0);
	errors += version_test_symmetrical("0", "00000000000000000", 0);

	fprintf(stderr, "\nTest group: simple comparisons\n");
	errors += version_test_symmetrical("0.0.0", "0.0.1", -1);
	errors += version_test_symmetrical("0.0.1", "0.0.2", -1);
	errors += version_test_symmetrical("0.0.2", "0.0.10", -1);
	errors += version_test_symmetrical("0.0.2", "0.1.0", -1);
	errors += version_test_symmetrical("0.0.10", "0.1.0", -1);
	errors += version_test_symmetrical("0.1.0", "0.1.1", -1);
	errors += version_test_symmetrical("0.1.1", "1.0.0", -1);
	errors += version_test_symmetrical("1.0.0", "10.0.0", -1);
	errors += version_test_symmetrical("10.0.0", "100.0.0", -1);
	errors += version_test_symmetrical("10.10000.10000", "11.0.0", -1);

	fprintf(stderr, "\nTest group: long numbers\n");
	errors += version_test_symmetrical("20160101", "20160102", -1);
	errors += version_test_symmetrical("999999999999999999", "1000000000000000000", -1);

	fprintf(stderr, "\nTest group: very long numbers\n");
	errors += version_test_symmetrical("99999999999999999999999999999999999998", "99999999999999999999999999999999999999", -1);

	fprintf(stderr, "\nTest group: letter addendum\n");
	errors += version_test_symmetrical("1.0", "1.0a", -1);
	errors += version_test_symmetrical("1.0a", "1.0b", -1);
	errors += version_test_symmetrical("1.0b", "1.1", -1);

	fprintf(stderr, "\nTest group: letter vs. number\n");
	errors += version_test_symmetrical("a", "0", -1);
	errors += version_test_symmetrical("1.a", "1.0", -1);

	fprintf(stderr, "\nTest group: letter-only component\n");
	errors += version_test_symmetrical("1.0.a", "1.0.b", -1);
	errors += version_test_symmetrical("1.0.b", "1.0.c", -1);
	errors += version_test_symmetrical("1.0.c", "1.0", -1);
	errors += version_test_symmetrical("1.0.c", "1.0.0", -1);

	fprintf(stderr, "\nTest group: letter component split\n");
	errors += version_test_symmetrical("1.0a0", "1.0.a0", 0);
	errors += version_test_symmetrical("1.0beta3", "1.0.b3", 0);

	fprintf(stderr, "\nTest group: case is ignored\n");
	errors += version_test_symmetrical("a", "A", 0);
	errors += version_test_symmetrical("1alpha", "1ALPHA", 0);
	errors += version_test_symmetrical("alpha1", "ALPHA1", 0);

	fprintf(stderr, "\nTest group: strings are shortened to one letter\n");
	errors += version_test_symmetrical("a", "alpha", 0);
	errors += version_test_symmetrical("b", "beta", 0);
	errors += version_test_symmetrical("p", "prerelease", 0);

	fprintf(stderr, "\nTest group: unusial component separators\n");
	errors += version_test_symmetrical("1.0.alpha.2", "1_0_alpha_2", 0);
	errors += version_test_symmetrical("1.0.alpha.2", "1-0-alpha-2", 0);
	errors += version_test_symmetrical("1.0.alpha.2", "1,0:alpha~2", 0);

	fprintf(stderr, "\nTest group: multiple consequentional separators\n");
	errors += version_test_symmetrical("..1....2....3..", "1.2.3", 0);
	errors += version_test_symmetrical(".-~1~-.-~2~-.", "1.2", 0);
	errors += version_test_symmetrical(".,:;~+-_", "0", 0);

	fprintf(stderr, "\nTest group: empty string\n");
	errors += version_test_symmetrical("", "", 0);
	errors += version_test_symmetrical("", "0", 0);
	errors += version_test_symmetrical("", "1", -1);

	fprintf(stderr, "\nTest group: prerelease sequence\n");
	/* XXX: is rc/pre ordering defined? */
	errors += version_test_symmetrical("1.0alpha1", "1.0alpha2", -1);
	errors += version_test_symmetrical("1.0alpha2", "1.0beta1", -1);
	errors += version_test_symmetrical("1.0beta1", "1.0beta2", -1);
	errors += version_test_symmetrical("1.0beta2", "1.0rc1", -1);
	errors += version_test_symmetrical("1.0beta2", "1.0pre1", -1);
	errors += version_test_symmetrical("1.0rc1", "1.0", -1);
	errors += version_test_symmetrical("1.0pre1", "1.0", -1);

	errors += version_test_symmetrical("1.0.alpha1", "1.0.alpha2", -1);
	errors += version_test_symmetrical("1.0.alpha2", "1.0.beta1", -1);
	errors += version_test_symmetrical("1.0.beta1", "1.0.beta2", -1);
	errors += version_test_symmetrical("1.0.beta2", "1.0.rc1", -1);
	errors += version_test_symmetrical("1.0.beta2", "1.0.pre1", -1);
	errors += version_test_symmetrical("1.0.rc1", "1.0", -1);
	errors += version_test_symmetrical("1.0.pre1", "1.0", -1);

	errors += version_test_symmetrical("1.0alpha.1", "1.0alpha.2", -1);
	errors += version_test_symmetrical("1.0alpha.2", "1.0beta.1", -1);
	errors += version_test_symmetrical("1.0beta.1", "1.0beta.2", -1);
	errors += version_test_symmetrical("1.0beta.2", "1.0rc.1", -1);
	errors += version_test_symmetrical("1.0beta.2", "1.0pre.1", -1);
	errors += version_test_symmetrical("1.0rc.1", "1.0", -1);
	errors += version_test_symmetrical("1.0pre.1", "1.0", -1);

	errors += version_test_symmetrical("1.0.alpha.1", "1.0.alpha.2", -1);
	errors += version_test_symmetrical("1.0.alpha.2", "1.0.beta.1", -1);
	errors += version_test_symmetrical("1.0.beta.1", "1.0.beta.2", -1);
	errors += version_test_symmetrical("1.0.beta.2", "1.0.rc.1", -1);
	errors += version_test_symmetrical("1.0.beta.2", "1.0.pre.1", -1);
	errors += version_test_symmetrical("1.0.rc.1", "1.0", -1);
	errors += version_test_symmetrical("1.0.pre.1", "1.0", -1);

	fprintf(stderr, "\nTest group: long word awareness\n");
	/* this should not be treated as 1.0a-1 */
	errors += version_test_symmetrical("1.0alpha-1", "0.9", 1);
	errors += version_test_symmetrical("1.0alpha-1", "1.0", -1);
	errors += version_test_symmetrical("1.0alpha-1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0alpha-1", "1.1", -1);

	errors += version_test_symmetrical("1.0beta-1", "0.9", 1);
	errors += version_test_symmetrical("1.0beta-1", "1.0", -1);
	errors += version_test_symmetrical("1.0beta-1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0beta-1", "1.1", -1);

	errors += version_test_symmetrical("1.0pre-1", "0.9", 1);
	errors += version_test_symmetrical("1.0pre-1", "1.0", -1);
	errors += version_test_symmetrical("1.0pre-1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0pre-1", "1.1", -1);

	errors += version_test_symmetrical("1.0prerelease-1", "0.9", 1);
	errors += version_test_symmetrical("1.0prerelease-1", "1.0", -1);
	errors += version_test_symmetrical("1.0prerelease-1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0prerelease-1", "1.1", -1);

	errors += version_test_symmetrical("1.0rc-1", "0.9", 1);
	errors += version_test_symmetrical("1.0rc-1", "1.0", -1);
	errors += version_test_symmetrical("1.0rc-1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0rc-1", "1.1", -1);

	fprintf(stderr, "\nTest group: post-release keyword awareness\n");
	/* this should not be treated as 1.0a-1 */
	errors += version_test_symmetrical("1.0patch1", "0.9", 1);
	errors += version_test_symmetrical("1.0patch1", "1.0", 1);
	errors += version_test_symmetrical("1.0patch1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0patch1", "1.1", -1);

	errors += version_test_symmetrical("1.0.patch1", "0.9", 1);
	errors += version_test_symmetrical("1.0.patch1", "1.0", 1);
	errors += version_test_symmetrical("1.0.patch1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0.patch1", "1.1", -1);

	errors += version_test_symmetrical("1.0patch.1", "0.9", 1);
	errors += version_test_symmetrical("1.0patch.1", "1.0", 1);
	errors += version_test_symmetrical("1.0patch.1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0patch.1", "1.1", -1);

	errors += version_test_symmetrical("1.0.patch.1", "0.9", 1);
	errors += version_test_symmetrical("1.0.patch.1", "1.0", 1);
	errors += version_test_symmetrical("1.0.patch.1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0.patch.1", "1.1", -1);

	errors += version_test_symmetrical("1.0post1", "0.9", 1);
	errors += version_test_symmetrical("1.0post1", "1.0", 1);
	errors += version_test_symmetrical("1.0post1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0post1", "1.1", -1);

	errors += version_test_symmetrical("1.0postanythinggoeshere1", "0.9", 1);
	errors += version_test_symmetrical("1.0postanythinggoeshere1", "1.0", 1);
	errors += version_test_symmetrical("1.0postanythinggoeshere1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0postanythinggoeshere1", "1.1", -1);

	errors += version_test_symmetrical("1.0pl1", "0.9", 1);
	errors += version_test_symmetrical("1.0pl1", "1.0", 1);
	errors += version_test_symmetrical("1.0pl1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0pl1", "1.1", -1);

	errors += version_test_symmetrical("1.0errata1", "0.9", 1);
	errors += version_test_symmetrical("1.0errata1", "1.0", 1);
	errors += version_test_symmetrical("1.0errata1", "1.0.1", -1);
	errors += version_test_symmetrical("1.0errata1", "1.1", -1);

	fprintf(stderr, "\nTest group: p is patch flag\n");
	errors += version_test_symmetrical_flags("1.0p1", "1.0p1", 0, 0, 0);
	errors += version_test_symmetrical_flags("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH, VERSIONFLAG_P_IS_PATCH, 0);
	errors += version_test_symmetrical_flags("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0p1", "1.0p1", 0, VERSIONFLAG_P_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0p1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0p1", VERSIONFLAG_P_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0p1", 0, VERSIONFLAG_P_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0.p1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.p1", VERSIONFLAG_P_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.p1", 0, VERSIONFLAG_P_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0.p.1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.p.1", VERSIONFLAG_P_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.p.1", 0, VERSIONFLAG_P_IS_PATCH, -1);

	/* this case is not affected */
	errors += version_test_symmetrical_flags("1.0", "1.0p.1", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0p.1", VERSIONFLAG_P_IS_PATCH, 0, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0p.1", 0, VERSIONFLAG_P_IS_PATCH, -1);

	fprintf(stderr, "\nTest group: any is patch flag\n");
	errors += version_test_symmetrical_flags("1.0a1", "1.0a1", 0, 0, 0);
	errors += version_test_symmetrical_flags("1.0a1", "1.0a1", VERSIONFLAG_ANY_IS_PATCH, VERSIONFLAG_ANY_IS_PATCH, 0);
	errors += version_test_symmetrical_flags("1.0a1", "1.0a1", VERSIONFLAG_ANY_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0a1", "1.0a1", 0, VERSIONFLAG_ANY_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0a1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0a1", VERSIONFLAG_ANY_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0a1", 0, VERSIONFLAG_ANY_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0.a1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.a1", VERSIONFLAG_ANY_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.a1", 0, VERSIONFLAG_ANY_IS_PATCH, -1);

	errors += version_test_symmetrical_flags("1.0", "1.0.a.1", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.a.1", VERSIONFLAG_ANY_IS_PATCH, 0, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0.a.1", 0, VERSIONFLAG_ANY_IS_PATCH, -1);

	/* this case is not affected */
	errors += version_test_symmetrical_flags("1.0", "1.0a.1", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0a.1", VERSIONFLAG_ANY_IS_PATCH, 0, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0a.1", 0, VERSIONFLAG_ANY_IS_PATCH, -1);

	fprintf(stderr, "\nTest group: p/patch compatibility\n");
	errors += version_test_symmetrical_flags("1.0p1", "1.0pre1", 0, 0, 0);
	errors += version_test_symmetrical_flags("1.0p1", "1.0patch1", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0p1", "1.0post1", 0, 0, -1);

	errors += version_test_symmetrical_flags("1.0p1", "1.0pre1", VERSIONFLAG_P_IS_PATCH, VERSIONFLAG_P_IS_PATCH, 1);
	errors += version_test_symmetrical_flags("1.0p1", "1.0patch1", VERSIONFLAG_P_IS_PATCH, VERSIONFLAG_P_IS_PATCH, 0);
	errors += version_test_symmetrical_flags("1.0p1", "1.0post1", VERSIONFLAG_P_IS_PATCH, VERSIONFLAG_P_IS_PATCH, 0);

	fprintf(stderr, "\nTest group: prerelease words without numbers\n");
	errors += version_test_symmetrical("1.0alpha", "1.0", -1);
	errors += version_test_symmetrical("1.0.alpha", "1.0", -1);

	errors += version_test_symmetrical("1.0beta", "1.0", -1);
	errors += version_test_symmetrical("1.0.beta", "1.0", -1);

	errors += version_test_symmetrical("1.0rc", "1.0", -1);
	errors += version_test_symmetrical("1.0.rc", "1.0", -1);

	errors += version_test_symmetrical("1.0pre", "1.0", -1);
	errors += version_test_symmetrical("1.0.pre", "1.0", -1);

	errors += version_test_symmetrical("1.0prerelese", "1.0", -1);
	errors += version_test_symmetrical("1.0.prerelese", "1.0", -1);

	errors += version_test_symmetrical("1.0patch", "1.0", 1);
	errors += version_test_symmetrical("1.0.patch", "1.0", 1);

	fprintf(stderr, "\nTest group: release bounds\n");
	errors += version_test_symmetrical_flags("0.99999", "1.0", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0alpha", "1.0", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0alpha0", "1.0", 0, 0, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0", 0, 0, 0);
	errors += version_test_symmetrical_flags("1.0patch", "1.0", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0patch0", "1.0", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.0.1", "1.0", 0, 0, 1);
	errors += version_test_symmetrical_flags("1.1", "1.0", 0, 0, 1);

	errors += version_test_symmetrical_flags("0.99999", "1.0", 0, VERSIONFLAG_LOWER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0alpha", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0alpha0", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0patch", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0patch0", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0a", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.0.1", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);
	errors += version_test_symmetrical_flags("1.1", "1.0", 0, VERSIONFLAG_LOWER_BOUND, 1);

	errors += version_test_symmetrical_flags("0.99999", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0alpha", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0alpha0", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0patch", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0patch0", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0a", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.0.1", "1.0", 0, VERSIONFLAG_UPPER_BOUND, -1);
	errors += version_test_symmetrical_flags("1.1", "1.0", 0, VERSIONFLAG_UPPER_BOUND, 1);

	errors += version_test_symmetrical_flags("1.0", "1.0", VERSIONFLAG_LOWER_BOUND, VERSIONFLAG_LOWER_BOUND, 0);
	errors += version_test_symmetrical_flags("1.0", "1.0", VERSIONFLAG_UPPER_BOUND, VERSIONFLAG_UPPER_BOUND, 0);
	errors += version_test_symmetrical_flags("1.0", "1.0", VERSIONFLAG_LOWER_BOUND, VERSIONFLAG_UPPER_BOUND, -1);

	errors += version_test_symmetrical_flags("1.0", "1.1", VERSIONFLAG_UPPER_BOUND, VERSIONFLAG_LOWER_BOUND, -1);

	errors += version_test_symmetrical_flags("0", "0.0", VERSIONFLAG_UPPER_BOUND, VERSIONFLAG_UPPER_BOUND, 1);
	errors += version_test_symmetrical_flags("0", "0.0", VERSIONFLAG_LOWER_BOUND, VERSIONFLAG_LOWER_BOUND, -1);

	fprintf(stderr, "\nTest group: uniform component splitting\n");
	errors += version_test_symmetrical("1.0alpha1", "1.0alpha1", 0);
	errors += version_test_symmetrical("1.0alpha1", "1.0.alpha1", 0);
	errors += version_test_symmetrical("1.0alpha1", "1.0alpha.1", 0);
	errors += version_test_symmetrical("1.0alpha1", "1.0.alpha.1", 0);

	errors += version_test_symmetrical("1.0patch1", "1.0patch1", 0);
	errors += version_test_symmetrical("1.0patch1", "1.0.patch1", 0);
	errors += version_test_symmetrical("1.0patch1", "1.0patch.1", 0);
	errors += version_test_symmetrical("1.0patch1", "1.0.patch.1", 0);

	/* controversial - TBD
	fprintf(stderr, "\nTest group: letter vs. numeric component ordering\n");
	errors += version_test_symmetrical("1.0", "1.0a", -1);
	errors += version_test_symmetrical("1.0a", "1.0.1", -1);
	*/

	if (errors) {
		fprintf(stderr, "\n%d test(s) failed!\n", errors);
		return 1;
	}

	fprintf(stderr, "\nAll tests OK!\n");
	return 0;
}
