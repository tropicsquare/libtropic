# Contributing Guide
We love contributions! To make contributing simple for both sides, please:

- Open an issue and describe how you would like to contribute and discuss details with us.
- Create a branch from the develop branch and do the changes:
    - Make sure to follow specifics in our [Coding Style](#coding-style).
    - Make sure to use [Code Formatter](#code-formatter), otherwise the PR check will fail and cannot be merged.
    - Make sure the branch passes [Tests](https://tropicsquare.github.io/libtropic/latest/for_contributors/tests/) against model -- otherwise, the PR check will fail.
    - Make sure to run [Static Analysis](#static-analysis).
    - Make sure your [Commit Messages](#commit-messages) follow our guidelines.
- Create pull request.

## Coding Style
### Structures and Enums
In the public API (`include/`), we define structured types and enumerations using `typedef`. We do NOT omit
structure (enum) name, to keep possibility to declare using `struct`/`enum` keywords. Example:

```c
typedef struct my_struct {
    ...
} my_struct;

typedef enum my_enum {
    ...
} my_enum;
```

Anywhere else, [we do not use typedefs](https://www.kernel.org/doc/html/latest/process/coding-style.html#typedefs).

## Code Formatter
We use the [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) tool for code formatting. Its installation varies dependening on the linux distribution.
> [!IMPORTANT]
> We recommend using version 16 or higher.

To check if `clang-format` is available on your machine, run:
```shell
clang-format --version
```

We use `clang-format` to check code format on pushes and PRs into the *master* and *develop* branches - this is implemented in the action `.github/clang_format_check.yml`. It only checks the format and does not fix it - that is the contributor's responsibility.

### How to Use It
There are multiple ways to format the code using `clang-format`:

1. For each file with wrong formatting, run:
```shell
clang-format -i <path_to_the_file_to_format>
```
2. If you are using [VSCode](https://code.visualstudio.com/) and the [`cpptools`](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) extension, you can create `.vscode/settings.json` with the following contents (if it does not already exist):
```json
{ // Add this bracket only if your settings.json file is empty
    "[c]": {
        "editor.defaultFormatter": "ms-vscode.cpptools",
        "editor.formatOnSave": true
    },
    "C_Cpp.formatting": "clangFormat"
} // Add this bracket only if your settings.json file is empty
```
This will format the file on each save.
3. There is also the [`git-clang-format`](https://clang.llvm.org/docs/ClangFormat.html#git-integration) tool, which integrates `clang-format` with `git`, but we have not used that yet.
4. Possibly other ways...

## Static Analysis
To run static analysis, follow these steps:

1. Choose a static analysis tool (e.g., cppcheck, clang-tidy).
2. Configure the tool to analyze the library code.
3. Run the analysis and review the reported issues.

## Commit Messages
Our commit message format is inspired by [Conventional Commits guidelines](https://www.conventionalcommits.org/en/v1.0.0/#specification).

The commit messages should fulfill the following:
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
