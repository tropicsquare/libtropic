# Building the Documentation
Libtropic documentation is built using the two following frameworks, each building a different part of the documentation:

1. [MkDocs](https://www.mkdocs.org/), used to generate the pages you are seeing right now,
2. [Doxygen](https://www.doxygen.nl/), used to generate the API Reference from the libtropic source code.

Normally, you should not need to build the documentation yourself - it is available on our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/), where versions for the [master](https://github.com/tropicsquare/libtropic/tree/master) branch and all [releases](https://github.com/tropicsquare/libtropic/releases) are automatically built and released by our GitHub Actions. However, in the case of contributing to the documentation, it is handy to be able to build it locally and preview the new changes. Refer to the following sections for steps on how to do that.

## Install the Dependencies
!!! example "Installing dependencies"
    First, instal MkDocs dependencies:

    1. Install Python 3, at least version 3.8.
    2. We recommend creating a [Python Virtual Environment](https://docs.python.org/3/library/venv.html), for example with a name `.docs-venv`:
    ```bash { .copy }
    python3 -m venv .docs-venv
    source .docs-venv/bin/activate
    ```
    3. Update `pip` and install the needed `pip` packages using `docs/requirements.txt`:
    ```bash { .copy }
    pip install --upgrade pip
    pip install -r docs/requirements.txt
    ```
    
    After that, install **Doxygen** and **Graphviz** (used for the diagrams) - installation depends on your system, but we will use Ubuntu in this example:
    ```bash { .copy }
    sudo apt-get install doxygen graphviz
    ```

## Build Doxygen Documentation
First, the API Reference has to be built using Doxygen:

!!! example "Building Doxygen Documentation"
    1. Switch to `docs/doxygen/`:
    ```bash { .copy }
    cd docs/doxygen/
    ```
    2. Build:
    ```bash { .copy }
    doxygen Doxyfile.in
    ```

The API Reference should be now built in `docs/doxygen/build/html/`.

!!! warning
    These steps have to be done each time the contents of `docs/doxygen/` change and you want to preview the changes.

## Build MkDocs Documentation
MkDocs has the ability to run a builtin development server on localhost, where the documentation is automatically deployed. To run it, switch to the root libtropic directory, where `mkdocs.yml` is located, and do:
!!! example "Building MkDocs Documentation"
    ```bash { .copy }
    mkdocs serve
    ```
    In the terminal, you should see the address of the server. To open it in your browser, press <kbd>Ctrl</kbd> + <kbd>:material-mouse-left-click: Left Click</kbd> or just manually copy it.

!!! warning
    MkDocs does not rebuilt the Doxygen documentation automatically - to rebuild it, repeat the steps from section [Build Doxygen Documentation](#build-doxygen-documentation).

!!! tip
    Each time you edit some files inside `docs/`, the server does not have to be stopped and run again - the server content will be automatically reloaded on each file save.

## Versioned Documentation
When you build the documentation using the steps from the section [Build MkDocs Documentation](#build-mkdocs-documentation), the version selector in the page header is not visible as it is on our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/). That is because for the versioning, we use the [mike](https://github.com/jimporter/mike) plugin for MkDocs. This plugin maintains the `gh-pages` branch, from which the [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/) are deployed.

### Preview the Versioned Documentation
The most common and safe use case is to locally preview the state of the documentation that is deployed to our [GitHub Pages](https://tropicsquare.github.io/libtropic/latest/):
!!! example "Previewing the Versioned Documentation"
    1. Make sure you have the latest version of the `gh-pages` branch from `origin`:
    ```bash { .copy }
    git fetch origin
    git pull origin gh-pages
    ```
    Do not `git checkout gh-pages`, because you will not be able to build the documentation there. Do `git checkout` with `master`, `develop` or any other branch based from one of these.
    1. Run a builtin development server with the contents of `gh-pages`:
    ```bash { .copy }
    mike serve
    ```

    In the terminal, you should see the address of the server. To open it in your browser, press <kbd>Ctrl</kbd> + <kbd>:material-mouse-left-click: Left Click</kbd> or just manually copy it.

### Edit the Versioned Documentation
!!! danger
    Some of the following commands change the state of the local `git` repository, specifically the `gh-pages` branch, and possibly the `origin` remote!

If you need to locally deploy a new version and preview it, you have to modify the `gh-pages` branch:
!!! example "Locally Deploying a New Version"
    ```bash { .copy }
    mike deploy <version_name>
    ```
    After running this, `gh-pages` branch will be **created** (if it does not already exist) and the generated documentation will be **pushed** to it.
    !!! danger
        If you add the `--push` flag, the `gh-pages` branch will be pushed to `origin` - **we do not recommend doing that!** This applies to most of the `mike` commands.

To see all existing versions, do:
!!! example "Seeing Existing Versions"
    ```bash { .copy }
    mike list
    ```
    !!! info
        This command is safe - it does not change `gh-pages` branch.

To remove a specific version, do:
!!! example "Deleting Existing Version"
    ```bash { .copy }
    mike delete
    ```

There are more commands available - refer to the [mike repository](https://github.com/jimporter/mike) for more information.