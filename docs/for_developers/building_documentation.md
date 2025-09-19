# Building the Documentation
Libtropic documentation is built using the two following frameworks, each building a different part of the documentation:

1. [MkDocs](https://www.mkdocs.org/), used to generate the pages you are seeing right now,
2. [Doxygen](https://www.doxygen.nl/), used to generate the API Reference from the libtropic source code.

Normally, you should not need to build the documentation yourself - it is available on our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/), where versions for the [master](https://github.com/tropicsquare/libtropic/tree/master) branch and all [releases](https://github.com/tropicsquare/libtropic/releases) are automatically built and released by our GitHub Actions. However, in the case of contributing to the documentation, it is handy to be able to build it locally and preview the new changes. Refer to the following sections for steps on how to do that.

## Installation of the Dependencies
!!! important
    Before proceeding, you need to have Python 3.x installed on your system.
To install **MkDocs**, do the following:

1. We recommend creating a [Python Virtual Environment](https://docs.python.org/3/library/venv.html), for example with a name `.docs-venv`:
```shell
python3 -m venv .docs-venv
source .docs-venv/bin/activate
```
1. Update `pip` and install the needed `pip` packages using `docs/requirements.txt`:
```shell
pip install --upgrade pip
pip install -r docs/requirements.txt
```

After that, install **Doxygen** and **Graphviz** (used for the diagrams) - installation depends on your system, but we will use Ubuntu in this example:
```shell
sudo apt-get install doxygen graphviz
```

## Building with Doxygen
First, the API Reference has to be built using Doxygen:

1. Switch to `docs/doxygen/`:
```shell
cd docs/doxygen/
```
2. Build:
```shell
doxygen Doxyfile.in
```

The API Reference should be now built in `docs/doxygen/build/html/`.

!!! note
    These steps have to be done each time the contents of `docs/doxygen/` change and you want to preview the changes.

## Building with MkDocs
MkDocs has the ability to run a builtin development server on localhost, where the documentation is automatically deployed. To run it, switch to the root libtropic directory, where `mkdocs.yml` is located, and run:
```shell
mkdocs serve
```
In the terminal, you should see the address of the server. While holding CTRL, left-click the address to open it in your browser (or just manually copy it).

!!! tip
    Each time you edit some files inside `docs/`, the server does not have to be stopped and run again - the server content will be automatically reloaded on each file save.

!!! warning
    MkDocs does not rebuilt the Doxygen documentation automatically - to rebuild it, repeat the steps from section [Building with Doxygen](#building-with-doxygen).

## Versioned Documentation
When you build the documentation using the steps from the section [Building with MkDocs](#building-with-mkdocs), the version selector in the page header is not visible as it is on our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/). That is because for the versioning, we use the [mike](https://github.com/jimporter/mike) plugin for MkDocs. This plugin maintains the `gh-pages` branch, from which the [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/) are deployed.

### Previewing the Versioned Documentation
The most common and safe use case is to locally preview the state of the documentation that is deployed to our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/). Do the following steps to achieve that:

1. Make sure you have the latest version of the `gh-pages` branch from `origin`:
```shell
git fetch origin
git pull origin gh-pages
```
Do not `git checkout gh-pages`, because you will not be able to build the documentation there. Do `git checkout` with `master`, `develop` or any other branch based from one of these.
1. Run a builtin development server with the contents of `gh-pages`:
```shell
mike serve
```
In the terminal, you should see the address of the server. While holding CTRL, left-click the address to open it in your browser (or just manually copy it).

### Editing the Versioned Documentation
!!! danger
    Some of the following commands change the state of the local `git` repository, specifically the `gh-pages` branch, and possibly the `origin` remote!

If you need to locally deploy a new version and preview it, you have to modify the `gh-pages` branch. To do that, run:
```shell
mike deploy <version_name>
```
After running this, `gh-pages` branch will be **created** (if it does not already exist) and the generated documentation will be **pushed** to it.
!!! danger
    If you add the `--push` flag, the `gh-pages` branch will be pushed to `origin` - **we do not recommend doing that!** This applies to most of the `mike` commands.

To see all existing versions, run:
```shell
mike list
```
!!! note
    This command is safe - it does not change `gh-pages` branch.

To remove a specific version, run:
```shell
mike delete 
```
There are more commands available - refer to the [mike repository](https://github.com/jimporter/mike) for more information.