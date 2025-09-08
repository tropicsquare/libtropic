from pathlib import Path
import serial
import argparse
import sys

class TS04Control:
    """
    TS04Control is a class that provides an interface to communicate with a TS04 device
    via a serial port. It allows sending commands to the device for various operations
    such as connecting to a target, controlling power, resetting, and identifying the device.
    Attributes:
        serial_port (Path): The path to the serial port used for communication.
    Methods:
        connect(target_id: int) -> bool:
            Connects to a specific target on the TS04 device.
            Args:
                target_id (int): The ID of the target to connect to.
            Returns:
                bool: True if the connection is successful, False otherwise.
        power(state: bool) -> bool:
            Sets the power state of the TS04 device.
            Args:
                state (bool): True to turn the power ON, False to turn it OFF.
            Returns:
                bool: True if the power state is successfully set, False otherwise.
        reset() -> bool:
            Resets the TS04 device.
            Returns:
                bool: True if the reset is successful, False otherwise.
        identify() -> bool:
            Retrieves and prints the ID and version information of the TS04 device.
            Returns:
                bool: True if the information is successfully retrieved, False otherwise.
    Exceptions:
        TS04SerialError:
            Custom exception raised when there is an error in serial communication.
    """
    
    def __init__(self, serial_port: Path):
        self.serial_port = serial_port
        pass

    class TS04SerialError(BaseException):
        pass

    def __send_command(self, command: str) -> str:
        with serial.Serial(str(self.serial_port.absolute()), baudrate = 115200, timeout=5) as s:
            if not s.is_open:
                print("Serial port not open!")
                raise self.TS04SerialError

            s.write(bytes(f"{command}\n", encoding='ascii'))
            s.flush()

            resp = s.read_until(b'OK\r\n').decode('ascii')
            
            if len(resp) == 0:
                print("Received no response!")
                raise self.TS04SerialError
            
            if not resp.endswith("OK\r\n"):
                print(f"Command failed! Output: {resp}")
                raise self.TS04SerialError

            return resp

    def connect(self, target_id: int) -> bool:
        print(f"Connecting target {target_id}...")
        try:
            print("Sending MUXALL...")
            self.__send_command(f"MUXALL={target_id}")
            print("Sending CON...")
            self.__send_command(f"CON={target_id}")
        except self.TS04SerialError:
            print("Error!")
            return False
        
        print("OK!")
        return True
    
    def power(self, state: bool) -> bool:
        print(f"Setting power {'ON' if state else 'OFF'}...")
        try:
            self.__send_command(f"PWR={'1' if state else '0'}")
        except self.TS04SerialError:
            print("Error!")
            return False
        
        print("OK!")
        return True

    def reset(self) -> bool:
        print("Resetting...")
        try:
            self.__send_command(f"RESET")
        except self.TS04SerialError:
            print("Error!")
            return False
        
        print("OK!")
        return True

    def identify(self) -> bool:
        print("Getting TS04 information...")
        
        try:
            id  = self.__send_command("ID").rstrip("OK\r\n")
            ver = self.__send_command("VER").rstrip("OK\r\n")
        except self.TS04SerialError:
            print("Error!")
            return False
        
        print(f"ID: \'{id}\'")
        print(f"Version: \'{ver}\'")
        return True

def valid_serial_port(path_str: str) -> Path:
    """Validate that the given path exists and is a file/character device."""
    path = Path(path_str)
    if not path.exists():
        raise argparse.ArgumentTypeError(f"Port path does not exist: {path}")
    if not path.is_char_device() and not path.is_file():
        raise argparse.ArgumentTypeError(f"Port path is not a valid device/file: {path}")
    return path

def main():
    parser = argparse.ArgumentParser(
        description="TS04 control utility",
    )

    # TS04 serial interface.
    parser.add_argument(
        "port",
        type=valid_serial_port,
        help="Path to the TS04 serial interface -- SPI, not TPDI component (e.g. /dev/ttyUSB0, /dev/serial/by-id/usb-TropicSquare_SPI_interface_xxx)"
    )

    subparsers = parser.add_subparsers(
        dest="command", required=True, help="Available commands"
    )

    # Command "identify"
    subparsers.add_parser(
        "identify", help="Identify the TS04 device"
    )

    # Command "connect"
    parser_connect = subparsers.add_parser(
        "connect", help="Connect a chip"
    )
    parser_connect.add_argument(
        "number",
        type=int,
        choices=range(1, 11),
        help="Chip number (1-10)"
    )

    # Command "power"
    parser_power = subparsers.add_parser(
        "power", help="Set TS04 main power state"
    )
    parser_power.add_argument(
        "state",
        choices=["on", "off"],
        help="Power state: 'on' or 'off'"
    )

    args = parser.parse_args()

    # Command handling
    ts04control = TS04Control(args.port)

    if args.command == "identify":
        ret = ts04control.identify()
        sys.exit(not ret) # OK (0) on True, 1 on False return value.
    elif args.command == "connect":
        ret = ts04control.connect(args.number)
        sys.exit(not ret) # OK (0) on True, 1 on False return value.
    elif args.command == "power":
        power = 1 if args.state == "on" else 0
        ret = ts04control.power(power)
        sys.exit(not ret) # OK (0) on True, 1 on False return value.
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()