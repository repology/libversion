/*
 * Copyright (c) 2019 Dmitry Marakasov <amdmi3@amdmi3.ru>
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
#include <string.h>
#include <unistd.h>

#include <libversion/config.h>
#include <libversion/version.h>
#include <libversion/private/parse.h>

void version_explain(const char* v, int flags) {
    component_t components[2];

    fprintf(stderr, "%3s %s\n", "M/O", "Data");
    while (*v != '\0') {
        size_t len = get_next_version_component(&v, components, flags);

        for (size_t i = 0; i < len; i++) {
            fprintf(stderr, "%3d \"%.*s\"\n", components[i].metaorder, (int)(components[i].end - components[i].start), components[i].start);
        }
    }
}

static void print_version() {
	fprintf(stderr, "libversion %s\n", LIBVERSION_VERSION);
}

static void print_usage(const char* progname) {
	fprintf(stderr, "Usage: %s [-pa] version\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, " -p       - 'p' letter is treated as 'patch' instead of 'pre'\n");
	fprintf(stderr, " -a       - any alphabetic characters are treated as post-release\n");
	fprintf(stderr, "\n");
	fprintf(stderr, " -h, -?   - print usage and exit\n");
	fprintf(stderr, " -v       - print version and exit\n");
}

int main(int argc, char** argv) {
	int ch, flags = 0;
	const char* progname = argv[0];

	while ((ch = getopt(argc, argv, "pahv")) != -1) {
		switch (ch) {
		case 'p':
			flags |= VERSIONFLAG_P_IS_PATCH;
			break;
		case 'a':
			flags |= VERSIONFLAG_ANY_IS_PATCH;
			break;
		case 'h':
		case '?':
			print_usage(progname);
			return 0;
		case 'v':
			print_version();
			return 0;
		default:
			print_usage(progname);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 1) {
		version_explain(argv[0], flags);
		return 0;
	}

	print_usage(progname);
    return 1;
}
