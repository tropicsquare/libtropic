# CodeChecker

The CodeChecker tool is used to perform static code analysis and generate reports.

You need to install CodeChecker and dependencies. Check out the [official repository](https://github.com/Ericsson/codechecker) for guidance.

## Generating reports

There are 2 options to get reports:

1. Generate HTML report using CLI.
    - Run following commands:

    ```sh
    CodeChecker check -b "./scripts/codechecker/codechecker_build.sh" --config ./scripts/codechecker/codechecker_config.json --output ./.codechecker/reports
    CodeChecker parse -e html ./.codechecker/reports -o ./.codechecker/reports_html
    ```
    
    - Open `./.codechecker/reports_html/index.html` in your favorite web browser.

2. Use VS Code CodeChecker add-on.
    - Install the add-on from [marketplace](https://marketplace.visualstudio.com/items?itemName=codechecker.vscode-codechecker).
    - Add these lines to your VS Code's workspace settings (`.vscode/settings.json`):

    ```json
    "codechecker.executor.executablePath": <path to CodeChecker>,
    "codechecker.analyze.arguments": "--config ./scripts/codechecker/codechecker_config.json",
    "codechecker.log.buildCommand": "./scripts/codechecker/codechecker_build.sh",
    "codechecker.backend.compilationDatabasePath": "${workspaceFolder}/.codechecker/compile_commands.json"
    ```

    - Note: `<path to CodeChecker>` can be replaced with "CodeChecker" if the CodeChecker is available in your `$PATH`. Otherwise, you need to specify full path to the CodeChecker executable.
