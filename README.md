# libversion

<a href="https://repology.org/metapackage/libversion">
    <img src="https://repology.org/badge/vertical-allrepos/libversion.svg" alt="Packaging status" align="right">
</a>

[![Build Status](https://travis-ci.org/repology/libversion.svg?branch=master)](https://travis-ci.org/repology/libversion)
[![Build status](https://ci.appveyor.com/api/projects/status/n78dvl22i70v379x/branch/master?svg=true)](https://ci.appveyor.com/project/AMDmi3/libversion/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/repology/libversion/badge.svg?branch=master)](https://coveralls.io/github/repology/libversion?branch=master)

Advanced version string comparison library.

Need to compare software, package or whatever versions? Comparing
`1.0` and `1.1` could be easy, but are you ready for more
complex cases like `1.2-x.3~alpha4`? **libversion** is, which
is proven by using the library in [Repology](https://repology.org/)
project which relies on comparing software version strings, even
if they are written in different formats.

## Features

A short list of version features libversion handles for you:

* Simple versions, obviously: `0.9 < 1.0 < 1.1`
* Missing components: `1.0 == 1.0.0`
* Leading zeroes: `1.001 == 1.1`
* Unusual separators: `1_2~3 == 1.2.3`
* Letter suffixes: `1.2 < 1.2a < 1.2b < 1.3`
* Alphanumeric prerelease components:
  * `1.0alpha1 == 1.0.alpha1 == 1.0a1 == 1.0.a1`
  * `1.0alpha1 < 1.0alpha2 < 1.0beta1 < 1.0rc1 < 1.0`
* Awareness of prerelease keywords: while `1.0 < 1.0a-1` (_a_ treated as version addendum), but `1.0alpha-1 < 1.0` (_alpha_ is treated as prerelease marker)
* Awareness of _patch_ keyword: while `1.0alpha1 < 1.0` (_alpha_ is pre-release), but `1.0 < 1.0patch1 < 1.1` (_patch_ is post-release)
* Customizable handling of _p_ keyword (it may mean either _patch_ or _pre_, and since libversion cannot guess, this is controlled with an external flag)

See [doc/ALGORITHM.md](doc/ALGORITHM.md) for more elaborate description
of inner logic.

## API

```
int version_compare_simple(const char* v1, const char* v2);
```

Compares version strings `v1` and `v2`.

Returns **-1** if `v1` is lower than `v2`, **0** if `v1` is equal to `v2` and **1** if `v1` is higher than `v2`.

Thread safe, does not produce errors, does not allocate dynamic memory.

```
int version_compare_flags(const char* v1, const char* v2, int flags);
```

Compares version strings `v1` and `v2`, allowing to fine tune
comparison behavior.

Available `flags` values:

* `VERSIONFLAG_P_IS_PATCH` _p_ keyword is treated as _patch_ (post-release) instead of _pre_ (pre-release)

If flags is 0, acts exactly the same way as `version_compare_simple`.

Returns **-1** if `v1` is lower than `v2`, **0** if `v1` is equal to `v2` and **1** if `v1` is higher than `v2`.

Thread safe, does not produce errors, does not allocate dynamic memory.

## Example

```c
#include <assert.h>
#include <libversion/compare.h>

int main() {
    /* 0.99 < 1.11 */
    assert(version_compare_simple("0.99", "1.11") == -1);

    /* 1.0 == 1.0.0 */
    assert(version_compare_simple("1.0", "1.0.0") == 0);

    /* 1.0alpha1 < 1.0.rc1 */
    assert(version_compare_simple("1.0alpha1", "1.0.rc1") == -1);

    /* 1.0 > 1.0.rc1 */
    assert(version_compare_simple("1.0", "1.0-rc1") == 1);

    /* 1.2.3alpha4 is the same as 1.2.3~a4 */
    assert(version_compare_simple("1.2.3alpha4", "1.2.3~a4") == 0);

    /* by default, `p' is treated as `pre'... */
    assert(version_compare_simple("1.0p0", "1.0", 0) == -1);
    assert(version_compare_flags("1.0p0", "1.0", 0) == -1);

    /* ...but this is tunable: here it's handled as `patch` */
    assert(version_compare_flags("1.0p0", "1.0", VERSIONFLAG_P_IS_PATCH) == 1);
}
```

```sh
gcc `pkg-config --libs --cflags libversion` test.c
```

## Building

libversion uses [CMake](https://cmake.org/) build system.

To build the library, run `cmake . && make`.

To run test suite, run `make test` after building.

To install the library systemwide, run `make install`.

The project installs library, headers, pkg-config file and a demo utility, `version_compare`, which may be used to compare versions from command line:

```
$ ./version_compare
Usage: ./version_compare version1 version2
$ ./version_compare 0.99 1.0
<
```

## Author

* [Dmitry Marakasov](https://github.com/AMDmi3) <amdmi3@amdmi3.ru>

## License

[MIT](COPYING)
