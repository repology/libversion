# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## 3.0.1
* CMake corrections

## 3.0.0
* Core algorithm reworked and simplified, which brings many small improvements
  * Numbers of arbitrary lengths are now compared correctly
  * Versions are now split more uniformly, so now `1.0alpha1` == `1.0.alpha1` == `1.0alpha.1` == `1.0.alpha.1`
  * Any number of alphabetic/numeric compoments are now allowed (`1a2b3c4` is no longer effectively trimmed to `1a2`)
* Implement `VERSIONFLAG_LOWER_BOUND` and `VERSIONFLAG_UPPER_BOUND` flags
* Added `version_sort` and `version_explain` (not installed) utilities,
  useful for debugging
* Removed deprecated `version_signature` API

## 2.9.1
* Add support for `errata` keyword as post-release.

## 2.9.0
* Modernized CMake code, added support for CMake export mechanism, so
  it's now super easy to use libversion from CMake
* Added export header so shared library can now be used on Windows
* Better handling of deprecated APIs
* Fixed library name conflicts on Windows

## 2.8.2
* Fixed undefined behavior in string comparison

## 2.8.1
* Fixed installation for out-of-source builds
* Documentation updates

## 2.8.0
* Deprecated `version_compare3()` as well, as it would create ambiguities
  for higher level language wrappers
* Exposed libversion version to consumer code via `LIBVERSION_VERSION_ATLEAST`
  macro

## 2.7.0
* API cleanup: all `version_compare*` methods are now replaced with
  `version_compare2()`, `version_compare3()`, and `version_compare4()`
  which are consistent and suggest the number of arguments.
  `version_compare_simple()`, `version_compare_flags()`, and
  `version_compare_flags2()` are thus deprecated and will be removed in
  further releases.
* Also deprecate `version_signature_simple()` which had no practical use.

## 2.6.0
* Implemented `version_compare_flags2()` 4-argument function which allows
  to specify flags for either compared version independently, without
  the need to squish them into the single argument
* Added shared library

## 2.5.0
* Implemented `VERSIONFLAG_ANY_IS_PATCH` flag which makes any alphabetic
  characters encountered in versions treated as post-release.

## 2.4.0
* Allow to specify `P_IS_PATCH` flag to each comparison argument separately

## 2.3.0
* Handle `pl` ("patchlevel") keyword as post-release.

## 2.2.0
* Handle any keyword which starts with `post` equally (as post-release). This
  is now also consistent with how keywords starting with `pre` are handled.

## 2.1.0
* Added special handling of `post` keyword (as post-release, similar to `patch`)

## 2.0.0
* Add `version_signature_single()` function to generate format signature
  for a version string
* Header file renamed to `libversion/version.h`

## 1.3.0
* Add `version_compare_flags()`, equivalent of `version_compare_simple()`
  which takes additional `flags` argument used to fine tune its
  behavior.
* Implemented `VERSIONFLAG_P_IS_PATCH` flag which affects the handling
  of `p` letter encountered in versions.

## 1.2.0
* Enable position independent code so libversion can be used with shared libraries

## 1.1.0
* Improved handling of keywords, including fixed handling of `patch`
* Improved documentation

## 1.0.0
* Initial release featuring single function `version_compare_simple(v1, v2)`
