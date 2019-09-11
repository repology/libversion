# Repology version comparison algorithm

## Core algorithm

1. Version is split into separate all-alphabetic or all-numeric
   components. All other characters are treated as separators.
   Empty components are not generated.

   * `10.2alpha3..patch.4.` → `10`, `2`, `alpha`, `3`, `patch`, `4`

2. Components are assigned ranks by their value:

   * **PRE_RELEASE** - known pre-release keyword (`alpha`, `beta`, `rc`).
   * **ZERO** - numeric component equal to zero.
   * **POST_RELEASE** - known post-release keyword (`patch`, `post`, `pl`).
   * **NONZERO** - numeric component not equal to zero.

   * Unclassified alphabetic components are assigned **PRE_RELEASE**
     rank (choice of this behavior explained below).

   A special case exists for alphabetic component which follows
   numeric component, and is not followed by another numeric
   component (`1.0a`, `1.0a.1`, but not `1.0a1`). Such alphabetic
   component is assigned a different rank, **LETTER_SUFFIX**, which
   follows **NONZERO** (choice of this behavior explained below).

3. Versions are compared component-wise.

   * Ranks are compared first, and are ordered the same way as
     introduced above (**PRE_RELEASE** < **ZERO** < **POST_RELEASE**
     < **NONZERO** < **LETTER_SUFFIX**).
   * Alphabetic components are compared by case insensitively comparing
     their first letters (choice of this behavior explained below).
   * Numeric components are compared numerically.

   Note that because of how ranks are assigned, there's no case
   where alphabetic components are compared with numeric ones.

   * If one of version component sequences is exhausted during
     comparison, it is padded with component equal to `0` (e.g.
     rank **ZERO**).

## Example

Here's an example of internal representation of some versions,
in ascending order, split into components, rank and value shown
for each compoment, padding components included.

| Version     | C1    | C2    | C3        | C4    | Why greater the previous |
|-------------|-------|-------|-----------|-------|--------------------------|
| `1.0alpha1` | 3 `1` | 1 `0` | 0 `alpha` | 3 `1` |
| `1.0beta1`  | 3 `1` | 1 `0` | 0 `beta`  | 3 `1` | Third component is alphabetically greater
| `1.0`       | 3 `1` | 1 `0` | 1 `0`     | 1 `0` | **ZERO** rank > **PRE_RELEASE** rank
| `1.0patch1` | 3 `1` | 1 `0` | 2 `patch` | 3 `1` | **POST_RELEASE** rank > **ZERO** rank
| `1.0.1`     | 3 `1` | 1 `0` | 3 `1`     | 1 `0` | **NONZERO** rank > **POST_RELEASE** rank
| `1.0a`      | 3 `1` | 1 `0` | 4 `a`     | 1 `0` | **LETTER_SUFFIX** rank > **NONZERO** rank
| `1.0b`      | 3 `1` | 1 `0` | 4 `b`     | 1 `0` | Third component is alphabetically greater
| `1.1`       | 3 `1` | 3 `1` | 1 `0`     | 1 `0` | **NONZERO** rank > **ZERO** rank
| `1.2`       | 3 `1` | 3 `2` | 1 `0`     | 1 `0` | Second component is numerically greater

## Choices

Some parts of the algorithm may be implemented differently, here
are the options and explanation.

### Treating unclassified alphabetic components

Unclassified alphabetic components may be treated as either pre-release
and post-release:

* `1.0custom1` < `1.0`
* `1.0` < `1.0custom1`

We chose the former variant by default because:

* In many cases is actually abbreviated prerelease suffix `a`/`b`/`r`
  (for `alpha`/`beta`/`rc`), and we want `1.0a(lpha)1` < `1.0`.
* In other cases it's some snapshot pattern like `1.0git20190911`, where
  it's fundamentally not known if it's a snapshot pre- or post- 1.0. We
  prefer to treat it lower than `1.0` to favor clean versions over
  ambigious case and to prevent snapshots and many other kinds of
  incorrect version patterns to be reported as the latest version.

libversion provides a way to tweak this behavior with
`VERSIONFLAG_P_IS_PATCH` and `VERSIONFLAG_ANY_IS_PATCH` flags.

### Comparing alphabetic components by first letter

Strings are compared by first letter, which implies `a` being equal
to `alpha`. A benefit of this is that we handle a very common case
of upstreams and maintainers abbreviating `alpha`, `beta` to just
`a` and `b` by comparing abbreviated and full keywords as equal.
On the downside, we're not able to distinguish e.g. `0.9.8za` and
`0.9.8zb`, but the practice shows that cases like this are much
less common.

However, another option is to trim known suffixes to the first
letter, and switch to full string comparison otherwise, which would
give benefits of both approaches. This should be investigated.

### Ordering of letter suffixes

I'd say that intuitively `1.0` < `1.0a` < `1.0.1` because `a` seems
to be tighter "variant" of `1.0` than the "next release" `1.0.1`, and
it would also correspond to how general alphabetic parts are ordered
(`a` < `1`), but we still stick to `1.0` < `1.0.1` < `1.0a` order, because
in practice no valid cases were found to be broken by this, but a number
of cases where incorrectly written versions (`1.0a` as `1.0.1`) were
favored upon genuine ones appeared.

In future this behavior may change, or a flag to tune it may be
introduced.

Technically it may be switched by changing order of **LETTER_SUFFIX**
rank from after **NONZERO** to before.

## Extensions

There may be some extensions to this algorithm which do not affect
core logic.

### Upper/lower bounds

Sometimes it's useful to check whether a given version logically
belongs to a given release. It's not possible to check this with
just comparison. For instance, `1.0alpha1` belongs to `1.0`, but
it's compared less than `1.0`. You could compare with `0.9` but
then again there's `0.99` which compares higher, but is still not
`1.0`.

The solution is to introduce two more ranks, *lower bound* which is
lesser than any other rank, and *upper bound* which is greater than
any other rank, and to tweak version padding algorithm to use these
instead of `0` filler. This way for a given version a component sequence
would be produced which always compares less than or greater than
(correspondingly) any natural post-or pre-release version with the
same prefix.

This is implemented in libversion with `VERSIONFLAG_LOWER_BOUND`
and `VERSIONFLAG_UPPER_BOUND` flags.
