# Contributing guidelines

## Project structure

- `.github/ISSUE_TEMPLATE.md`: template for GitHub issues
- `.github/PULL_REQUEST_TEMPLATE.md`: template for GitHub pull requests
- `build/`: CMake out-of-source build location
- `docs/`: documentation
- `include/ametsuchi`: headers
- `scripts/`: dev scripts
- `src/`: sources
- `.clang-format`: [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) config, based on [Google C++ style guide](https://google.github.io/styleguide/cppguide.html)
- `.editorconfig`: [EditorConfig](http://editorconfig.org/) config
- `.gitignore`: [gitignore](https://git-scm.com/docs/gitignore) config
- `CMakeLists.txt`: [CMake](https://cmake.org/) project definition
- `CONTRIBUTING.md`: contains information for contributors
- `LICENSE`: project license
- `README.md`: The main introductory document for the project

## How to build

Requirements:

- gcc or clang
- CMake
- clang-format

Run `scripts/init.sh`.

## Versioning policy

Releases are numbered according to [semantic versioning](http://semver.org/).

## Branch policy

We use [GitHub flow](https://guides.github.com/introduction/flow/) with [squash-and-merge pull requests](https://help.github.com/articles/about-pull-request-merges/#squash-and-merge-your-pull-request-commits).

### Branch types

#### Master branch

`master` is the main development branch.
It must _always_ build successfully and pass unit, integration and other tests.

#### Feature branches

Feature branches are devoted for implementing a single, well-defined feature or task.
They are named as `feature/dash-separated-feature-name`.
Feature branches are _always_ created from `master`.
It is OK to fail builds and have red tests in these branches.

Feature branches are merged with `master` _only_ by squash-merging.
They can stay around for a while after a successful squash-merge (say, for 1 week or for a single sprint), but eventually should be deleted.

In order to avoid painful squash-merging in the end, feature branches should be occasionally rebased onto `master`.

#### Release branches

Release branches are created for every minor release.
They are named as `release/vA.B`, where `A` is the major version and `B` is the minor version according to semantic versioning.
Release branches are created from `master`.

#### Latest release branch

The latest release must _always_ point to the most recent release branch.
It is indented to represent the freshest shippable code.

#### Hotfix branches

When an emergency code change is required in release branches, hotfix branches should be used.
They are named as `hotfix/dash-separated-fix-name`.
Hotfix branches are similar to feature branches, but but is created only for emergency changes indented for a set of specific releases.
They should be used only in cases when the fix is too complex for a single commit, and does not represent a feature.

## Gotchas

- Use `#praga once` in headers instead of include guards: even though it's not in the standard, this pragma is widely supported and requires less maintenance.
