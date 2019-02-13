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

int main() {
	const char version_chars[] = { '0', '1', 'p', 'R', 'e', '.', '-' };
	const size_t num_version_chars = sizeof(version_chars)/sizeof(version_chars[0]);

	const char* samples[] = { "0", "1", "a", "r", "z", "1alpha1", "1patch1" };
	const size_t num_samples = sizeof(samples)/sizeof(samples[0]);

	char buffer[6];
	size_t i0, i1, i2, i3, i4, isample;
	int res;

	buffer[5] = '\0';

	for (i0 = 0; i0 < num_version_chars; i0++) {
		buffer[0] = version_chars[i0];
		for (i1 = 0; i1 < num_version_chars; i1++) {
			buffer[1] = version_chars[i1];
			for (i2 = 0; i2 < num_version_chars; i2++) {
				buffer[2] = version_chars[i2];
				for (i3 = 0; i3 < num_version_chars; i3++) {
					buffer[3] = version_chars[i3];
					for (i4 = 0; i4 < num_version_chars; i4++) {
						buffer[4] = version_chars[i4];

						for (isample = 0; isample < num_samples; isample++) {
							res = version_compare2(buffer, samples[isample]);

							printf("%s %c %s\n", buffer, (res == 0) ? '=' : (res < 0) ? '<' : '>', samples[isample]);
						}
					}
				}
			}
		}
	}

	return 0;
}
