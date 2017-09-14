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

#include <stdio.h>
#include <unistd.h>

#include <libversion/compare.h>

static void usage(const char* progname) {
    fprintf(stderr, "Usage: %s [-p] version1 version2\n", progname);
    fprintf(stderr, "\n");
    fprintf(stderr, " -p       - 'p' letter is treated as 'patch' instead of 'pre'\n");
    fprintf(stderr, "\n");
    fprintf(stderr, " -h, -?   - print usage and exit\n");
}

int main(int argc, char** argv) {
    int ch, pflag, result, flags = 0;
    const char* progname = argv[0];

    while ((ch = getopt(argc, argv, "ph")) != -1) {
        switch (ch) {
        case 'p':
            flags |= VERSIONFLAG_P_IS_PATCH;
            pflag = 1;
            break;
        case 'h':
        case '?':
            usage(progname);
            return 0;
        default:
            usage(progname);
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

	if (argc != 2) {
        usage(progname);
        return 1;
    }

	result = version_compare_flags(argv[0], argv[1], flags);
	if (result < 0)
		printf("<\n");
	else if (result > 0)
		printf(">\n");
	else
		printf("=\n");

	return 0;
}
