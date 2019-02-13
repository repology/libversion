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

#define LIBVERSION_DEPRECATED /* testing deprecated APIs */

#include <libversion/version.h>

#include <stdio.h>

int main() {
	int errors = 0;

	errors += version_compare_simple("1.0.0", "1.0") != 0;
	errors += version_compare_simple("1.0", "1.1") != -1;
	errors += version_compare_simple("1.1", "1.0") != 1;

	errors += version_compare3("1.0", "1.0p1", 0) != 1;
	errors += version_compare3("1.0", "1.0p1", VERSIONFLAG_P_IS_PATCH) != -1;

	errors += version_compare_flags("1.0p1", "1.0p1", 0) != 0;
	errors += version_compare_flags("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH_LEFT) != 1;
	errors += version_compare_flags("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH_RIGHT) != -1;
	errors += version_compare_flags("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH_LEFT|VERSIONFLAG_P_IS_PATCH_RIGHT) != 0;

	errors += version_compare_flags2("1.0p1", "1.0p1", 0, 0) != 0;
	errors += version_compare_flags2("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH, 0) != 1;
	errors += version_compare_flags2("1.0p1", "1.0p1", 0, VERSIONFLAG_P_IS_PATCH) != -1;
	errors += version_compare_flags2("1.0p1", "1.0p1", VERSIONFLAG_P_IS_PATCH, VERSIONFLAG_P_IS_PATCH) != 0;

	if (errors) {
		fprintf(stderr, "Some tests failed!\n", errors);
		return 1;
	}

	fprintf(stderr, "\nAll tests OK!\n");
	return 0;
}
