# Code Formatter
For the code formatting, we use the [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) tool. Its installation varies dependening on the linux distribution.

To check if `clang-format` is available on your machine, run:
```shell
clang-format --version
```

We use `clang-format` to check code format on pushes and PRs into the *master* and *develop* branches - this is implemented in the action `.github/clang_format_check.yml`. It only checks the format and does not fix it - that is the contributor's responsibility.

## How to Use It
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