# Contributing

We love contributions! To make contributing simple for both sides, please:

- Open an issue and describe how you would like to contribute and discuss details with us.
- Create a branch from develop and do the changes:
    - Make sure your commit messages follow our guidelines -- [see below](#commit-messages).
    - Make sure to follow specifics in our coding style -- [see below](#coding-style).
    - Make sure your code is properly formatted with `clang-format` version >16! Otherwise the PR check will fail and cannot be merged.
        - install clang-format
        - run `clang-format -i -- path_to_c_or_h_source_file`.
    - Make sure the branch passes tests against model -- otherwise, again, the PR check will fail.
        - Guide about testing against the model is in the [`tests/model_based_project/README.md`](tests/model_based_project/README.md).
- Create pull request.

## Coding style
### Structures and enums
In the public API (`include/`) we define structured types and enumerations using typedef. We do NOT omit
structure (enum) name, to keep possibility to declare using struct/enum keywords. Example:

```c
typedef struct my_struct {
    ...
} my_struct;

typedef enum my_enum {
    ...
} my_enum;
```

Anywhere else [we do not use typedefs](https://www.kernel.org/doc/html/latest/process/coding-style.html#typedefs).

## Commit messages
Our commit message format is inspired by [Conventional Commits guidelines](https://www.conventionalcommits.org/en/v1.0.0/#specification).

The commit messages should look like following:
```
< type >[ optional scope ]: < description >
[ optional JIRA REF ]
[ optional body ]
[ optional footer ( s ) ]
```

Where the meaning of individual ﬁelds is:

```
<type> - Type of commit. Can be one of following:
    - feat     - A new feature.
    - build    - A change to build or compile scripts.
    - ci       - A change to continuous integration setup and scripting.
    - doc      - A change to documentation.
    - refactor - A refactoring of code. Shall not change functionality.
    - test     - A change in tests or test-bench environment.
    - ﬁx       - Fix of incorrect functionality.
    - perf     - Performance enhancement.
    - deps     - A change to dependency settings

<description>  - Part of the repository or ﬂow where the change is made. The ﬁrst line of the commit message shall be at most 72 characters long.

scope          - Optional part of the repository or ﬂow where the change is made.

JIRA REF       - Optional reference to a JIRA issue

body           - Optional arbitrary number of paragraphs describing what the commit does.

footer         - Optional footer (see https://www.conventionalcommits.org/en/v1.0.0/#specification)
```
