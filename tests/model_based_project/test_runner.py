import argparse
import pathlib
import subprocess
import yaml
import time
import sys
import socket

MODEL_LOGGING_CFG_PATH = pathlib.Path(__file__).parent.joinpath("model_logging_cfg.yml")
MODEL_CFG_PATH         = pathlib.Path(__file__).parent.joinpath("model_cfg.yml")

def wait_for_server_start(host="127.0.0.1", port=28992, retry_interval=0.2, max_attempts=10) -> bool:
    for i in range(max_attempts):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(1.0)
            try:
                sock.connect((host, port))
                return True
            except (ConnectionRefusedError, socket.timeout):
                time.sleep(retry_interval)
                print(f"Waiting on server, attempt #{i}")
    return False

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog = "test_runner.py",
        description = "Runs the specified test against TROPIC01 model and saves all output to specified directory."
    )

    parser.add_argument(
        "-t", "--test",
        help="Path to the test executable.",
        type=pathlib.Path,
        required=True
    )

    parser.add_argument(
        "-o", "--output-dir",
        help="Path to the directory where output should be saved.",
        type=pathlib.Path,
        required=True
    )

    args = parser.parse_args()

    # Save args
    test_path: pathlib.Path = args.test
    output_path: pathlib.Path = args.output_dir
    test_name = test_path.stem

    # Create destination directory if it doesn't exist yet
    output_path.mkdir(parents=True, exist_ok=True)

    # Load model logging configuration, modify logging file name and save
    with MODEL_LOGGING_CFG_PATH.open("r") as f:
        model_config = yaml.safe_load(f)
    
    model_logging_file_path = output_path.joinpath(f"{test_name}_model_response").with_suffix(".log")
    model_config["handlers"]["file"]["filename"] = str(model_logging_file_path)

    model_logging_cfg_modified_path = pathlib.Path("model_logging_cfg_modified.yml")
    with model_logging_cfg_modified_path.open("w") as f:
        yaml.dump(model_config, f, default_flow_style=False)

    # Start the model server
    model_process = subprocess.Popen(
        [
            "model_server", "tcp",
            "-c", f"{str(MODEL_CFG_PATH)}",
            "-l", f"{str(model_logging_cfg_modified_path)}"
        ]
    )

    # Wait for model server to start
    if wait_for_server_start() == False:
        model_logging_cfg_modified_path.unlink()
        print("Server did not start.")
        sys.exit(1)

    # Execute the test
    ret = 0
    with output_path.joinpath(test_name).with_suffix(".log").open("w") as f:
        try: 
            subprocess.run(
                [
                    "stdbuf", "-o0", "-e0",  # Disable buffering of stdout and stderr
                    str(test_path)
                ],
                stdout=f,
                stderr=f,
                check=True
            )
        except subprocess.CalledProcessError as e:
            ret = e.returncode

    # Clean up
    model_process.terminate()
    model_process.wait()
    model_logging_cfg_modified_path.unlink()
    sys.exit(ret)