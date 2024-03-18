# Versioning
Versions are structured as follows:

`release_type` `major`.`minor`.`patch`-`type`

`release_type` is the release type.

`major` this only increases on breaking changes.

`minor` this increases when only features are added.

`patch` this increases when only bugs are fixed.

`type` this is the kernel build type

## Release types

Release types are:

- `dev`: this isn't a release, this is the git repo
- `stable`: this is a **stable** release
- `beta`: this is in the main bugfixing phase
- `alpha`: this is in the feature phase

### Dev

The version number is that of the next planned version.

### Alpha

The version number for this works slightly different.

`release_type` `major`.`minor`.`feature_name`-`type`

`feature_name` is the feature beign developed.

### Beta

The version number for this also works slightly different.

`patch` starts with a `b` to indicate the beta fixes.

## Kernel Types

Only `core` is currently in existence.

## Feature addition

Optimizing existing code is not a feature and is counted as a Bugfix.

## Breaking changes

If syscall parameters change, the sheduler get rewritten or old features get removed.

## Cycle

Dev (half-baked features) -> Alpha (usable features)

Alpha (usable features) -> Beta (remove bugs as good as possible)

Beta (remove bugs as good as possible) -> Stable (it works reliably)
