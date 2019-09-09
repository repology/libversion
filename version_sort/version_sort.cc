// Copyright (c) 2019 Dmitry Marakasov <amdmi3@amdmi3.ru>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <getopt.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <libversion/config.h>
#include <libversion/version.h>

class VersionsList {
private:
	int flags_;
	std::vector<std::string> versions_;

	bool VersionLess(const std::string& a, const std::string& b) const {
		int res = version_compare4(a.c_str(), b.c_str(), flags_, flags_);
		if (res < 0) {
			return true;
		}
		if (res > 0) {
			return false;
		}

		return a < b;  // fallback to stringwise comparison for stable ordering
	}

public:
	VersionsList(int flags) : flags_(flags) {
	}

	void Read(std::istream& stream) {
		std::string line;
		while (std::getline(stream, line)) {
			versions_.push_back(line);
		}
	}

	void Sort() {
		std::sort(
			versions_.begin(),
			versions_.end(),
			[this](const std::string& a, const std::string& b) -> bool {
				return VersionLess(a, b);
			}
		);
	}

	void Dump(std::ostream& stream) const {
		for (const auto& version: versions_) {
			stream << version << '\n';
		}
	}

	void VerboseDump(std::ostream& stream) const {
		std::string prev;
		for (const auto& version: versions_) {
			stream << version << (VersionLess(prev, version) ? " (<)" : " (==)") << '\n';
			prev = version;
		}
	}
};

static void print_version() {
	std::cerr << "libversion " << LIBVERSION_VERSION << std::endl;
}

static void print_usage(const char* progname) {
	std::cerr << "Usage: " << progname << " [-pa] [path]\n";
	std::cerr << "\n";
	std::cerr << " -p       - 'p' letter is treated as 'patch' instead of 'pre'\n";
	std::cerr << " -a       - any alphabetic characters are treated as post-release\n";
	std::cerr << " -v       - verbose mode (display whether version is different from the previous one)\n";
	std::cerr << "\n";
	std::cerr << " -h, -?   - print usage and exit\n";
	std::cerr << " -V       - print version and exit" << std::endl;
}

int main(int argc, char** argv) {
	int ch, flags = 0;
	const char* progname = argv[0];
	bool verbose = false;

	while ((ch = getopt(argc, argv, "pahvV")) != -1) {
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
		case 'V':
			print_version();
			return 0;
		case 'v':
			verbose = true;
			break;
		default:
			print_usage(progname);
			return 1;
		}
	}

	argc -= optind;
	argv += optind;

	VersionsList versions(flags);

	if (argc == 0) {
		versions.Read(std::cin);
	}
	for (int arg = 0; arg < argc; ++arg) {
		std::fstream fs(argv[arg]);
		versions.Read(fs);
	}

	versions.Sort();

	if (verbose) {
		versions.VerboseDump(std::cout);
	} else {
		versions.Dump(std::cout);
	}

	return 0;
}
