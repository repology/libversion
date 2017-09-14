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

#include <libversion/version.h>

#include <stdio.h>
#include <string.h>

/* long enough version to make sure its signature overflows 64 bits */
#define VERYLONGVERSION \
	"0.1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16." \
	"17.18.19.20.21.22.23.24.25.26.27.28.29.30.31." \
	"32.33.34.35.36.37.38.39.40.41.42.43.44.45.46.47." \
	"48.49.50.51.52.53.54.55.56.57.58.59.60.61.62.63." \
	"64"

static int signature_test(const char* v1, const char* v2, int expected) {
	version_signature_t s1 = version_signature_simple(v1);
	version_signature_t s2 = version_signature_simple(v2);

	int result = s1 == s2;

	const char* c = expected ? "==" : "!=";

	if (result != expected) {
		fprintf(stderr, "[FAIL] signature of \"%s\" %s signature of \"%s\": 0x%0lx vs. 0x%0lx\n", v1, c, v2, (unsigned long)s1, (unsigned long)s2);
		return 1;
	} else {
		fprintf(stderr, "[ OK ] signature of \"%s\" %s signature of \"%s\": 0x%0lx vs. 0x%0lx\n", v1, c, v2, (unsigned long)s1, (unsigned long)s2);
		return 0;
	}
}

int main() {
	int errors = 0;

	fprintf(stderr, "Test group: lowlevel 1\n");
	errors += signature_test("", "", 1);
	errors += signature_test("1", "2", 1);
	errors += signature_test("a", "b", 1);
	errors += signature_test("", "b", 0);
	errors += signature_test("", "2", 0);
	errors += signature_test("a", "2", 0);

	fprintf(stderr, "\nTest group: lowlevel 2\n");
	errors += signature_test("01", "23", 1);
	errors += signature_test("a1", "23", 0);
	errors += signature_test("0b", "23", 0);
	errors += signature_test("ab", "23", 0);

	errors += signature_test("01", "c3", 0);
	errors += signature_test("a1", "c3", 1);
	errors += signature_test("0b", "c3", 0);
	errors += signature_test("ab", "c3", 0);

	errors += signature_test("01", "2d", 0);
	errors += signature_test("a1", "2d", 0);
	errors += signature_test("0b", "2d", 1);
	errors += signature_test("ab", "2d", 0);

	errors += signature_test("01", "cd", 0);
	errors += signature_test("a1", "cd", 0);
	errors += signature_test("0b", "cd", 0);
	errors += signature_test("ab", "cd", 1);

	fprintf(stderr, "\nTest group: lowlevel 2.5\n");
	errors += signature_test("01", "2.3", 0);
	errors += signature_test("a1", "2.3", 0);
	errors += signature_test("0b", "2.3", 0);
	errors += signature_test("ab", "2.3", 0);

	errors += signature_test("01", "c.3", 0);
	errors += signature_test("a1", "c.3", 0);
	errors += signature_test("0b", "c.3", 0);
	errors += signature_test("ab", "c.3", 0);

	errors += signature_test("01", "2.d", 0);
	errors += signature_test("a1", "2.d", 0);
	errors += signature_test("0b", "2.d", 0);
	errors += signature_test("ab", "2.d", 0);

	errors += signature_test("01", "c.d", 0);
	errors += signature_test("a1", "c.d", 0);
	errors += signature_test("0b", "c.d", 0);
	errors += signature_test("ab", "c.d", 0);

	fprintf(stderr, "\nTest group: lowlevel 3\n");
	errors += signature_test("0.1", "2.3", 1);
	errors += signature_test("a.1", "2.3", 0);
	errors += signature_test("0.b", "2.3", 0);
	errors += signature_test("a.b", "2.3", 0);

	errors += signature_test("0.1", "c.3", 0);
	errors += signature_test("a.1", "c.3", 1);
	errors += signature_test("0.b", "c.3", 0);
	errors += signature_test("a.b", "c.3", 0);

	errors += signature_test("0.1", "2.d", 0);
	errors += signature_test("a.1", "2.d", 0);
	errors += signature_test("0.b", "2.d", 1);
	errors += signature_test("a.b", "2.d", 0);

	errors += signature_test("0.1", "c.d", 0);
	errors += signature_test("a.1", "c.d", 0);
	errors += signature_test("0.b", "c.d", 0);
	errors += signature_test("a.b", "c.d", 1);

	fprintf(stderr, "\nTest group: values are ignored\n");
	errors += signature_test("1.0", "0.1", 1);
	errors += signature_test("1.0", "1.00", 1);
	errors += signature_test("1.1", "1.001", 1);
	errors += signature_test("1.0", "9999.9999", 1);
	errors += signature_test("1asdfdhasfkjahsdkjf2", "9999x9999", 1);

	fprintf(stderr, "\nTest group: extra separators are ignored\n");
	errors += signature_test(".~.1.0", "1.0", 1);
	errors += signature_test("1.~.0", "1.0", 1);
	errors += signature_test("1.0.~.", "1.0", 1);

	fprintf(stderr, "\nTest group: number of components\n");
	errors += signature_test("1.0", "1", 0);
	errors += signature_test("1.0", "1.0.0", 0);

	fprintf(stderr, "\nTest group: alpha component\n");
	errors += signature_test("1.0alpha1", "2.3a2", 1);
	errors += signature_test("1.0alpha1", "2.3p2", 1);
	errors += signature_test("1.0alpha1", "2.3beta2", 1);
	errors += signature_test("1.0alpha1", "2.3patch2", 1);
	errors += signature_test("1.0alpha1", "2.3pre2", 1);
	errors += signature_test("1.0alpha1", "2.3pre2", 1);
	errors += signature_test("1.1alpha1", "1.1", 0);
	errors += signature_test("1.1alpha1", "1alpha1", 0);
	errors += signature_test("1.1alpha1", "1alpha1.1", 0);

	fprintf(stderr, "\nTest group: separated alpha component\n");
	errors += signature_test("1.0a1", "2.3.b2", 0);

	fprintf(stderr, "\nTest group: alpha suffix\n");
	errors += signature_test("1.0a", "1.0b", 1);
	errors += signature_test("1.0a", "1.0blahblahblah", 1);
	errors += signature_test("1.0a", "1.0", 0);

	fprintf(stderr, "\nTest group: should support versions at least this long (signature fits into 32 bits)\n");
	errors += signature_test("1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", 1);
	errors += signature_test("1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1a", 0);
	errors += signature_test("1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "1a.1a1.1a1.1a1.1a1.1a1.1a1.1a", 0);
	errors += signature_test("1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1.a", 0);
	errors += signature_test("1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "1a.1a1.1a1.1a1.1a1.1a1.1a1.1a1.1", 0);
	errors += signature_test("a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", 1);
	errors += signature_test("a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "a.1a1.1a1.1a1.1a1.1a1.1a1.1a1a", 0);
	errors += signature_test("a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "a.1a1.1a1.1a1.1a1.1a1.1a1.1a", 0);
	errors += signature_test("a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "a.1a1.1a1.1a1.1a1.1a1.1a1.1a1.a", 0);
	errors += signature_test("a.1a1.1a1.1a1.1a1.1a1.1a1.1a1", "a.1a1.1a1.1a1.1a1.1a1.1a1.1a1.1", 0);

	fprintf(stderr, "\nTest group: very long versions comparison is not reliable, but it should still work on the best effort basis\n");
	errors += signature_test(VERYLONGVERSION, VERYLONGVERSION, 1);
	errors += signature_test(VERYLONGVERSION, VERYLONGVERSION ".0", 0);
	errors += signature_test(VERYLONGVERSION, VERYLONGVERSION ".0.0", 0);
	errors += signature_test(VERYLONGVERSION, VERYLONGVERSION ".0.0.0", 0);
	errors += signature_test(VERYLONGVERSION, VERYLONGVERSION "a", 0);
	errors += signature_test(VERYLONGVERSION, "a" VERYLONGVERSION, 0);

	if (errors) {
		fprintf(stderr, "\n%d test(s) failed!\n", errors);
		return 1;
	}

	fprintf(stderr, "\nAll tests OK!\n");
	return 0;
}
