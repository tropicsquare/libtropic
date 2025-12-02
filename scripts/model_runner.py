import argparse
import pathlib
import subprocess
import yaml
import time
import sys
import socket
import os

def wait_for_server_start(host="127.0.0.1", port=28992, retry_interval=0.2, max_attempts=10) -> bool:
    for i in range(max_attempts):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(1.0)
            try:
                sock.connect((host, port))
                return True
            except (ConnectionRefusedError, socket.timeout):
                time.sleep(retry_interval)
                print(f"Waiting on server, attempt #{i+1}")
    return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="model_runner.py",
        description="Runs the given executable against TROPIC01 model and saves all output to the specified directory."
    )

    parser.add_argument(
        "-e", "--exe",
        help="Path to the executable to run against the model.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "-c", "--model-cfg",
        help="Path to the model configuration YAML file.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "--use-valgrind",
        help="Runs the executable with Valgrind.",
        action="store_true"
    )

    parser.add_argument(
        "-o", "--output-dir",
        help="Path to the directory where output should be saved.",
        type=pathlib.Path,
        required=True
    )

    args = parser.parse_args()

    # Resolve all paths relative to current working directory
    exe_path: pathlib.Path = args.exe.resolve()
    model_cfg_path: pathlib.Path = args.model_cfg.resolve()
    output_path: pathlib.Path = args.output_dir.resolve()
    use_valgrind: bool = args.use_valgrind
    exe_name = exe_path.stem

    # Create destination directory if it doesn't exist yet
    output_path.mkdir(parents=True, exist_ok=True)

    # Get the logging configuration from the model so we can modify it
    dump_logging_cfg_res = subprocess.run(
        ["model_server", "dump-logging-cfg"],
        capture_output=True,
        text=True,
        check=True
    )
    # Transform the YAML output to a dictionary
    model_log_cfg = yaml.safe_load(dump_logging_cfg_res.stdout)
    # Change the default handler to a file
    model_log_cfg["handlers"]["default"]["class"] = "logging.FileHandler"
    model_log_cfg["handlers"]["default"]["filename"] = str(output_path / f"{exe_name}_model_response.log")
    model_log_cfg["handlers"]["default"]["mode"] = "w"
    model_log_cfg["handlers"]["default"]["encoding"] = "utf8"
    # Change logging level to DEBUG
    model_log_cfg["loggers"]["model"]["level"] = "DEBUG"
    model_log_cfg["loggers"]["server"]["level"] = "DEBUG"
    # Disable colors to prevent weird symbols in the .log file
    model_log_cfg["formatters"]["default"]["use_colors"] = False

    model_log_cfg_path = output_path / "model_log_cfg_path.yml"
    with model_log_cfg_path.open("w") as f:
        yaml.dump(model_log_cfg, f, default_flow_style=False)
    
    # Start the model server
    model_process = subprocess.Popen(
        [
            "model_server", "tcp",
            "-c", str(model_cfg_path),
            "-l", str(model_log_cfg_path)
        ],
        env=os.environ
    )

    # Wait for model server to start
    if not wait_for_server_start():
        print("Server did not start.")
        sys.exit(1)

    # Run the executable
    ret = 0
    exe_log_path = output_path / f"{exe_name}.log"
    with exe_log_path.open("w") as f:
        try:
            exe_cmd = []
            if use_valgrind:
                valgrind_log_path = output_path / f"{exe_name}_valgrind_report.log"
                exe_cmd += [
                    "valgrind",
                    "--leak-check=full",
                    "--show-leak-kinds=all",
                    "--track-origins=yes",
                    "--verbose",
                    "--error-exitcode=1",
                    f"--log-file={valgrind_log_path}",
                    "--child-silent-after-fork=yes"
                ]
            exe_cmd += [str(exe_path)]

            subprocess.run(
                args=exe_cmd,
                stdout=f, stderr=f,
                env=os.environ,
                check=True
            )
        except subprocess.CalledProcessError as e:
            ret = e.returncode

    # Clean up
    model_process.terminate()
    try:
        model_process.wait(timeout=10)
    except subprocess.TimeoutExpired:
        model_process.kill()
    sys.exit(ret)