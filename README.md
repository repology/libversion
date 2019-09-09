# libversion

<a href="https://repology.org/metapackage/libversion/versions">
	<img src="https://repology.org/badge/vertical-allrepos/libversion.svg" alt="libversion packaging status" align="right">
</a>

[![Build Status](https://travis-ci.org/repology/libversion.svg?branch=master)](https://travis-ci.org/repology/libversion)
[![Build status](https://ci.appveyor.com/api/projects/status/n78dvl22i70v379x/branch/master?svg=true)](https://ci.appveyor.com/project/AMDmi3/libversion/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/repology/libversion/badge.svg?branch=master)](https://coveralls.io/github/repology/libversion?branch=master)
[![Github commits (since latest release)](https://img.shields.io/github/commits-since/repology/libversion/latest.svg)](https://github.com/repology/libversion)

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
* Omitting insignificant components: `1.0 == 1.0.0`
* Leading zeroes: `1.001 == 1.1`
* Unusual separators: `1_2~3 == 1.2.3`
* Letter suffixes: `1.2 < 1.2a < 1.2b < 1.3`
* Alphanumeric prerelease components:
  * `1.0alpha1 == 1.0.alpha1 == 1.0a1 == 1.0.a1`
  * `1.0alpha1 < 1.0alpha2 < 1.0beta1 < 1.0rc1 < 1.0`
* Awareness of prerelease keywords: while `1.0 < 1.0a-1` (_a_ treated
  as version addendum), but `1.0alpha-1 < 1.0` (_alpha_ is treated
  as prerelease marker)
* Awareness of _patch_, _post_ and _pl_ keywords: while `1.0alpha1 < 1.0`
  (_alpha_ is pre-release), but `1.0 < 1.0patch1 < 1.1` (_patch_ is post-release)
* Customizable handling of _p_ keyword (it may mean either _patch_ or _pre_,
  and since libversion cannot guess, this is controlled with an external flag)

See [doc/ALGORITHM.md](doc/ALGORITHM.md) for more elaborate description
of inner logic.

## API

### Version comparison

```
int version_compare2(const char* v1, const char* v2);
int version_compare4(const char* v1, const char* v2, int v1_flags, int v2_flags);
```

Compares version strings `v1` and `v2`.

Returns **-1** if `v1` is lower than `v2`, **0** if `v1` is equal to `v2` and **1** if `v1` is higher than `v2`.

Thread safe, does not produce errors, does not allocate dynamic memory,
O(N) computational complexity, O(1) stack memory requirements.

4-argument form allows specifying flags for each version argument to
tune comparison behavior is specific cases. Currently supported `flags`
values are:

* `VERSIONFLAG_P_IS_PATCH` _p_ letter is treated as _patch_
  (post-release) instead of _pre_ (pre-release).
* `VERSIONFLAG_ANY_IS_PATCH` any letter sequence is treated as
  post-release (useful for handling patchsets as in
  `1.2foopatchset3.barpatchset4`).
* `VERSIONFLAG_LOWER_BOUND` derive lowest possible version with
  the given prefix. For example, lower bound for `1.0` is such
  imaginary version `?` that it's higher than any release before
  `1.0` and lower than any prerelease of `1.0`.
  E.g. `0.999` < lower bound(`1.0`) < `1.0alpha0`.
* `VERSIONFLAG_UPPER_BOUND` derive highest possible version with
  the given prefix. Oppisite of `VERSIONFLAG_LOWER_BOUND`.

If both `flags` are zero, `version_compare4` acts exactly the same
as `version_compare2`.

## Example

```c
#include <assert.h>
#include <libversion/version.h>

int main() {
    /* 0.99 < 1.11 */
    assert(version_compare2("0.99", "1.11") == -1);

    /* 1.0 == 1.0.0 */
    assert(version_compare2("1.0", "1.0.0") == 0);

    /* 1.0alpha1 < 1.0.rc1 */
    assert(version_compare2("1.0alpha1", "1.0.rc1") == -1);

    /* 1.0 > 1.0.rc1 */
    assert(version_compare2("1.0", "1.0-rc1") == 1);

    /* 1.2.3alpha4 is the same as 1.2.3~a4 */
    assert(version_compare2("1.2.3alpha4", "1.2.3~a4") == 0);

    /* by default, `p' is treated as `pre'... */
    assert(version_compare2("1.0p1", "1.0pre1") == 0);
    assert(version_compare2("1.0p1", "1.0post1") == -1);
    assert(version_compare2("1.0p1", "1.0patch1") == -1);

    /* ...but this is tunable: here it's handled as `patch` */
    assert(version_compare4("1.0p1", "1.0pre1", VERSIONFLAG_P_IS_PATCH, 0) == 1);
    assert(version_compare4("1.0p1", "1.0post1", VERSIONFLAG_P_IS_PATCH, 0) == 0);
    assert(version_compare4("1.0p1", "1.0patch1", VERSIONFLAG_P_IS_PATCH, 0) == 0);

    /* a way to check that the version belongs to a given release */
    assert(
        version_compare4("1.0alpha1", "1.0", 0, VERSIONFLAG_LOWER_BOUND) == 1) &&
        version_compare4("1.0alpha1", "1.0", 0, VERSIONFLAG_UPPER_BOUND) == -1) &&
        version_compare4("1.0.1", "1.0", 0, VERSIONFLAG_LOWER_BOUND) == 1) &&
        version_compare4("1.0.1", "1.0", 0, VERSIONFLAG_UPPER_BOUND) == -1) &&
        /* 1.0alpha1 and 1.0.1 belong to 1.0 release, e.g. they lie between
           (lowest possible version in 1.0) and (highest possible version in 1.0) */
    );
}
```

```sh
gcc my_code.c `pkg-config --cflags --libs libversion`
# or (for static linking)
gcc my_code.c --static `pkg-config --static --cflags --libs libversion`
```

Using libversion in CMake is very simple (note that this handles
include paths and compiler flags as well):

```cmake
find_package(libversion)
target_link_libraries(my_target libversion::libversion)
# or (for static linking)
target_link_libraries(my_target libversion::libversion_static)
```

## Building

libversion uses [CMake](https://cmake.org/) build system.

To build the library, run `cmake . && cmake --build .`.

To run test suite, run `ctest` after building.

To install the library systemwide, run `make install`.

The project installs library, headers, pkg-config file, CMake import
files and a demo utility, `version_compare`, which may be used to
compare versions from command line:

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
