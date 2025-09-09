# Code Formatter
Following instructions are applicable on linux based machines:

## Download `clang-format-17.0.6`

Choose version for your OS [here](https://github.com/llvm/llvm-project/releases), download it and extract it.
Then add its path into your `.bashrc` file:

```
export PATH="$PATH:/opt/clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04/bin"
```
Reload `.bashrc` file:

```
$ source ~/.bashrc
```

Installed version might be checked like this:

```
$ clang-format --version
clang-format version 17.0.6 (https://github.com/llvm/llvm-project 6009708b4367171ccdbf4b5905cb6a803753fe18)
```

For usage within VSCode, install `clang-format` extension for VSCode and restart IDE. You can then run formatter by right clicking on the code and choosing `Format document->Clang Format`.

To format particular folder use following prompt:

```
clang-format -i -- src/*.c src/*.h
```