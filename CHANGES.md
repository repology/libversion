# Change Log

All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## 2.6.0 - 2018-03-27
* Implemented `version_compare_flags2()` 4-argument function which allows
  to specify flags for either compared version independently, without
  the need to squish them into the single argument
* Added shared library

## 2.5.0 - 2018-02-02
* Implemented `VERSIONFLAG_ANY_IS_PATCH` flag which makes any alphabetic
  characters encountered in versions treated as post-release.

## 2.4.0 - 2018-01-19
* Allow to specify `P_IS_PATCH` flag to each comparison argument separately

## 2.3.0 - 2017-11-13
* Handle `pl` ("patchlevel") keyword as post-release.

## 2.2.0 - 2017-09-28
* Handle any keyword which starts with `post` equally (as post-release). This
  is now also consistent with how keywords starting with `pre` are handled.

## 2.1.0 - 2017-09-28
* Added special handling of `post` keyword (as post-release, similar to `patch`)

## 2.0.0 - 2017-09-15
* Add `version_signature_single()` function to generate format signature
  for a version string
* Header file renamed to `libversion/version.h`

## 1.3.0 - 2017-09-14
* Add `version_compare_flags()`, equivalent of `version_compare_simple()`
  which takes additional `flags` argument used to fine tune its
  behavior.
* Implemented `VERSIONFLAG_P_IS_PATCH` flag which affects the handling
  of `p` letter encountered in versions.

## 1.2.0 - 2017-05-23
* Enable position independent code so libversion can be used with shared libraries

## 1.1.0 - 2017-03-30
* Improved handling of keywords, including fixed handling of `patch`
* Improved documentation

## 1.0.0 - 2017-03-16
* Initial release featuring single function `version_compare_simple(v1, v2)`
