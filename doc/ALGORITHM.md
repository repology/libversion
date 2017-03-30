# Repology version comparison algorithm

1. Version is split into alphanumeric components, all other characters
   are treated as separators. Empty components are ignored.

   * ```1.2.3a``` → ```1```, ```2```, ```3a```
   * ```~1...2-3a~``` → ```1```, ```2```, ```3a```

2. Each component is decomposed into one or more (currently no more
   than two) triplets of ```{number, string, number}``` format. Each
   part of such triple either holds some value parsed from version
   or a placeholder value crafted for proper comparison. The set of
   rules on how to convert a version component into a set of triples
   is the core of this algorithm, but before explaining it, some
   implementation details:

   * Tuples are stored as three 64bit signed integers
     * For numeric parts, 64 bits are needed to represent long numeric
       version components such as 20160328234507 (real-world case).
     * Numbers which do not fit into 64 bits are clamped to maximal
       representable value
   * Alphabetic parts are currently clamped to a single character
     * This works correctly with both known cases where alphabetic
       parts are used: either as an addendum to a number (```1.49c```)
       which is single character naturally, or a prerelease keyword
       (```alpha2```, ```beta3```, ...) which are thankfully already
       ordered alphabetically and this does not change after clamping
     * Alphabetic values are converted to lowercase.
   * -1 is the default placeholder value.

   Now, basic rules on how we split component into tuples:

   * We try to parse number, then alphabetic part, then another
     number from a version component. Additional contents are
     discarded (```1a2b3``` is treated as ```1a2```).
   * We only allow a single numeric part in a triple, so component
     with two numeric parts generates two triples.
   * Summarizing, here are all possible variants of triple generation:
     * ```2``` → ```{2, -1, -1}```
     * ```2a``` → ```{2, 'a', -1}```
     * ```a2``` → ```{-1, 'a', 2}```
     * ```2a2``` → ```{2, -1, -1}, {-1, 'a', 2}```

   The result of this logic is that the comparison of tuples gives
   correct results for both cases involving alphabetic parts:

   * ```1.0 < 1.0a < 1.0b``` (addendum letter)
   * ```1alpha1 == 1.alpha1 == 1a1 == 1.a1 < 1 == 1.0``` (prerelease version)
   * As a side effect, ```1alpha1 == 1.alpha1``` which also handles some cases
     of different formats of a same version, without giving false positives

   This base algorithm was later extended with more rules for specific cases:

   * Before cropping alphabetic part to a single character, it's checked to
     be ```alpha```, ```beta```, ```pre```, ```prerelease```, ```rc``` or
     ```patch```.
     * If the check succeeds, we now have additional information that this is
       definitely not a version addendum, so we always unglue this from
       preceding number by generating additional triple. Compare:
       * ```2.0 < 2.0a-3 = 2.0a.3```
       * ```2.0alpha-3 = 2.0.a.3 < 2.0```
     * ```patch``` is handled even more specially, as it's a
       __post__-release keyword as opposed to pre-release. For it, tuple
       with different placeholder numeric part is generated. Compare:
       * ```1.0prerelease1 = 1.0.p1 < 1.0```
       * ```1.0 < 1.0patch1 = 1.0.0p1```

3. To compare two versions, generate triples for both of them and compare
   as simple integer lists. If one of lists is shorter, it's padded with
   ```{0, -1, -1}``` triple, which is an equivalent to ```0``` version
   component. Therefore, ```1``` is equal to ```1.0``` and ```1.0.0```.

## Possible room for improvement

* :warning: extra alphabetic components are dropped (```1ab2cd3e```)

  These are rare, and probably come from git commit hashes. We can't
  compare them anyway.

* :warning: strings are clamped to a single character (```1aa == 1ab```)

  As mentioned, it works well on practice. But if there are real
  world cases where longer strings are used, these may be packed
  into integers

* :warning: specific case ```0.21-alpha```

  There is a prerelease keyword without numeric part. In some cases, it
  may be dropped (e.g. whole ```0.``` branch is alpha), in other cases
  it may be a version part.

* :warning: are there more keywords?

  Can ```rev```, ```git```, be handled in some sane way? What about
  ```post```, ```dev```, ```final```, ```release```, ```prealpha```,
  ```build```?
