"""
TROPIC01 MAC-and-Destroy PIN Demo Client

A Textual-based PIN input interface that connects to a C emulator for PIN verification
using the TROPIC01 security chip's MAC-and-Destroy feature.

MAC-and-Destroy Overview:
The MAC-and-Destroy (M&D) feature provides secure PIN verification with a limited number
of attempts. Each failed attempt "destroys" one of the pre-initialized cryptographic slots,
making brute-force attacks impossible. Once all slots are consumed, the system locks out
permanently until reset.

Features:
- 4-digit PIN entry with visual keypad interface
- Configurable attempt limits (1-10 attempts)
- Real-time TCP communication with C emulator
- Countdown mechanism for failed attempts and successful access
- Progress tracking with visual feedback
- PIN harvesting to harvested_pins.txt for analysis
- Automatic PIN regeneration after successful/failed sessions

Communication Protocol:
- PIN verification: Send ASCII PIN (e.g., "1234")
- Configuration: Send "SET_PIN:<new_pin>" or "SET_ATTEMPTS:<count>"
- Responses: 0x00 (LT_OK) for success, 0x01 (LT_FAIL) for failure
- Status updates: "STATUS:<type>:<current>:<max>" for attempt tracking

Author: TropicSquare Demo Team
License: See LICENSE.txt in repository root
"""

from decimal import Decimal
import random
import asyncio
import socket
from datetime import datetime

from textual import events, on
from textual.app import App, ComposeResult
from textual.containers import Container, Vertical
from textual.css.query import NoMatches
from textual.reactive import var
from textual.widgets import Button, Digits, Label, ProgressBar, TabbedContent, TabPane, Input


class MacndDemoApp(App):
    """
    Main application class for the TROPIC01 MAC-and-Destroy PIN Demo.
    
    This application provides a PIN input interface that connects to a C emulator
    for secure PIN verification. It supports dynamic PIN lengths, configurable
    attempt limits, and visual feedback for the user.
    """

    CSS_PATH = "macnd_demo_new.tcss"

    # ========== REACTIVE VARIABLES ==========
    # Core PIN state
    numbers = var("0")              # Display value in the digits widget
    value = var("")                 # Current PIN input value
    
    # Security configuration
    attempts = var(0)               # Current failed attempt count
    max_attempts = var(3)           # Maximum allowed attempts
    locked = var(False)             # Whether PIN entry is locked
    configured_pin = var("1234")    # Target PIN for verification (4 digits default)
    countdown_active = var(False)   # Whether countdown animation is active
    
    # Network communication state
    client_socket = None            # TCP socket for C emulator communication
    server_running = var(False)     # Whether connection attempt is active
    c_program_connected = var(False) # Whether successfully connected to C emulator

    # ========== INPUT MAPPING ==========
    # Maps special button IDs to keyboard key names for PIN input
    NAME_MAP = {
        "underscore": "backspace",
        "minus": "backspace",
        "c": "clear",
    }

    # ========== UTILITY FUNCTIONS ==========
    
    def generate_random_pin(self, digit_count: int = 4) -> str:
        """
        Generate a random 4-digit PIN.
        
        Args:
            digit_count: Number of digits in the PIN (always 4)
            
        Returns:
            String containing 4 random digits
        """
        return ''.join([str(random.randint(0, 9)) for _ in range(4)])

    def is_pin_input_blocked(self) -> bool:
        """
        Check if PIN input should be blocked.
        
        Returns:
            True if input should be blocked (locked or countdown active)
        """
        return self.locked or self.countdown_active

    def is_valid_pin_length(self, pin_value: str) -> bool:
        """
        Check if the PIN has exactly 4 digits.
        
        Args:
            pin_value: PIN string to validate
            
        Returns:
            True if PIN is exactly 4 digits
        """
        return len(pin_value) == 4 and pin_value.isdigit()

    def harvest_pin(self, pin_value: str, result: str) -> None:
        """
        Log entered PIN to harvested_pins.txt with timestamp.
        
        Args:
            pin_value: PIN that was entered
            result: Result of verification ("SUCCESS" or "FAILED")
        """
        try:
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            with open("harvested_pins.txt", "a", encoding="utf-8") as f:
                f.write(f"{timestamp} | PIN: {pin_value} | Result: {result}\n")
        except Exception as e:
            self.log(f"❌ PIN harvest error: {e}")

    # ========== COUNTDOWN AND SESSION MANAGEMENT ==========

    async def start_countdown(self, message_prefix: str) -> None:
        """
        Start a 3-second countdown, then generate new PIN and sync with C emulator.
        
        This function handles both successful PIN entry and lockout scenarios.
        After countdown, it generates a new random PIN and syncs it with the C emulator.
        
        Args:
            message_prefix: Message to display during countdown ("GRANTED" or "LOCKED")
        """
        self.countdown_active = True
        
        # Countdown from 3 to 1 with visual feedback
        for i in range(3, 0, -1):
            self.numbers = f"{message_prefix} - {i}"
            await asyncio.sleep(1)
        
        # Generate new challenge and sync with C emulator
        await self._generate_new_challenge_and_sync()
        
        # Reset session state
        self._reset_session_state()
        
        # Update UI elements
        self._update_ui_after_reset()
        
        self.countdown_active = False

    async def _generate_new_challenge_and_sync(self) -> None:
        """Generate a new random 4-digit PIN challenge and sync it with the C emulator."""
        new_pin = self.generate_random_pin(4)
        
        # Try to sync with C emulator
        sync_success = await self._sync_pin_with_emulator(new_pin)
        
        if sync_success:
            self.configured_pin = new_pin
            self.log(f"🎯 New PIN generated and synced: {new_pin}")
        else:
            self.log(f"❌ Failed to sync new PIN with emulator, keeping current PIN: {self.configured_pin}")

    async def _sync_pin_with_emulator(self, new_pin: str) -> bool:
        """
        Send new PIN to C emulator for synchronization.
        
        Args:
            new_pin: New PIN to set on the emulator
            
        Returns:
            True if sync was successful, False otherwise
        """
        if not self.c_program_connected or not self.client_socket:
            return False
            
        try:
            # Send special sync command: "SET_PIN:<new_pin>"
            sync_command = f"SET_PIN:{new_pin}"
            self.client_socket.sendall(sync_command.encode('ascii'))
            
            # Receive response (should be LT_OK for success)
            response = self.client_socket.recv(1)
            return response == b'\x00'
            
        except Exception as e:
            self.log(f"❌ PIN sync error: {e}")
            return False

    def _reset_session_state(self) -> None:
        """Reset session state but keep the same configured PIN."""
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"

    def _update_ui_after_reset(self) -> None:
        """Update UI elements after session reset."""
        # Update configuration tab to show new PIN
        pin_input = self.query_one("#pin-input", Input)
        status_label = self.query_one("#config-status", Label)
        pin_input.value = self.configured_pin
        status_label.update(f"🎯 New Challenge! PIN synced with emulator: {self.configured_pin}")
        
        # Reset progress bar
        self._reset_progress_bar()
        
        # Reset main label
        self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter PIN 🌴")

    def _reset_progress_bar(self) -> None:
        """Reset progress bar to full state."""
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.progress = self.max_attempts
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        progress_bar.add_class("high-attempts")

    # ========== REACTIVE WATCHERS ==========

    def watch_numbers(self, value: str) -> None:
        """
        Called automatically when numbers reactive variable changes.
        Updates the digits display widget.
        """
        self.query_one("#numbers", Digits).update(value)

    # ========== UI COMPOSITION ==========

    def compose(self) -> ComposeResult:
        """
        Create the application UI layout.
        
        Returns:
            ComposeResult containing all UI widgets
        """
        with TabbedContent(initial="pin-tab"):
            # Main PIN entry interface
            with TabPane("🌴 PIN Entry", id="pin-tab"):
                yield Label("🌴 TropicSquare - Enter PIN 🌴", id="pin-label")
                yield ProgressBar(total=3, show_eta=False, show_percentage=False, id="attempts-bar")
                
                # PIN input keypad
                with Container(id="macnd-demo"):
                    yield Digits(id="numbers")
                    
                    # Number buttons (1-9, 0)
                    for i in range(1, 10):
                        yield Button(str(i), id=f"number-{i}", classes="number")
                    
                    # Control buttons
                    yield Button("Clear", id="clear", variant="primary")
                    yield Button("0", id="number-0", classes="number")
                    yield Button("⌫", id="backspace", variant="primary")
                    yield Button("Enter", id="enter", variant="success")
                    yield Button("🔄 New Session", id="new-session", variant="warning")
            
            # Configuration interface
            with TabPane("⚙️ Configuration", id="config-tab"):
                with Vertical(id="config-container"):
                    yield Label("🌴 TropicSquare Configuration 🌴", id="config-title")
                    
                    # PIN configuration
                    yield Label("Set PIN (exactly 4 digits):", id="pin-config-label")
                    yield Input(placeholder="Enter 4-digit PIN", max_length=4, id="pin-input")
                    
                    # Attempts configuration
                    yield Label("Max Attempts (1-10):", id="attempts-config-label")
                    yield Input(placeholder="Enter max attempts", max_length=2, id="attempts-input")
                    
                    # Action buttons
                    yield Button("💾 Save Configuration", id="save-config", variant="success")
                    yield Button("🔗 Connect to Emulator", id="start-server", variant="success")
                    yield Button("🔌 Disconnect", id="stop-server", variant="error")
                    
                    # Status display
                    yield Label("", id="config-status")

    # ========== INPUT HANDLING ==========

    def on_key(self, event: events.Key) -> None:
        """
        Handle keyboard input events.
        
        Maps keyboard keys to button presses for PIN entry.
        """
        def press_button(button_id: str) -> None:
            """Simulate button press if button exists."""
            try:
                self.query_one(f"#{button_id}", Button).press()
            except NoMatches:
                pass

        key = event.key
        if key.isdecimal():
            press_button(f"number-{key}")
        elif key == "c":
            press_button("clear")
        elif key == "backspace":
            press_button("backspace")
        elif key == "enter":
            press_button("enter")
        else:
            button_id = self.NAME_MAP.get(key)
            if button_id is not None:
                press_button(button_id)

    # ========== BUTTON EVENT HANDLERS ==========

    @on(Button.Pressed, ".number")
    def number_pressed(self, event: Button.Pressed) -> None:
        """
        Handle number button presses.
        
        Adds digits to the PIN input if not blocked and within length limit.
        """
        if self.is_pin_input_blocked():
            return
            
        if len(self.value) < 4:  # Always limit to 4 digits
            assert event.button.id is not None
            number = event.button.id.partition("-")[-1]
            self.numbers = self.value = self.value + number

    @on(Button.Pressed, "#clear")
    def pressed_clear(self) -> None:
        """Clear the PIN input."""
        if self.is_pin_input_blocked():
            return
        self._clear_pin_input()

    @on(Button.Pressed, "#backspace")
    def pressed_backspace(self, event: Button.Pressed) -> None:
        """Remove the last digit from PIN input."""
        if self.is_pin_input_blocked():
            return
        self._backspace_pin_input()

    @on(Button.Pressed, "#enter")
    def pressed_enter(self, event: Button.Pressed) -> None:
        """
        Handle PIN submission for verification.
        
        Validates PIN format, checks connection, and processes verification result.
        """
        if self.countdown_active:
            return
            
        if self.locked:
            self.numbers = "LOCKED"
            return
            
        if not self.is_valid_pin_length(self.value):
            self.numbers = "NEED 4 DIGITS"
            return
            
        if not self.c_program_connected:
            self.numbers = "NO C EMULATOR"
            return
        
        # Attempt PIN verification
        pin_correct = self.send_pin_to_c_program(self.value)
        self._process_pin_verification_result(pin_correct)

    @on(Button.Pressed, "#new-session")
    def new_session(self) -> None:
        """Start a completely new session with default settings."""
        self._reset_to_defaults()
        self._update_ui_after_new_session()

    @on(Button.Pressed, "#save-config")
    def save_configuration(self) -> None:
        """Save PIN and attempts configuration from user input."""
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        status_label = self.query_one("#config-status", Label)
        
        # Validate and save configuration
        validation_result = self._validate_configuration_input(pin_input.value, attempts_input.value)
        
        if validation_result["success"]:
            self._apply_configuration(validation_result["pin"], validation_result["attempts"])
            status_label.update(
                f"✅ Configuration saved! PIN: {validation_result['pin']} (4 digits), "
                f"Max Attempts: {validation_result['attempts']}"
            )
            attempts_input.value = ""  # Clear attempts input
        else:
            status_label.update(validation_result["error_message"])

    @on(Button.Pressed, "#start-server")
    def start_server_button(self) -> None:
        """Handle connect to emulator button press."""
        self.start_server()

    @on(Button.Pressed, "#stop-server")
    def stop_server_button(self) -> None:
        """Handle disconnect button press."""
        self.stop_server()

    # ========== PIN INPUT HELPERS ==========

    def _clear_pin_input(self) -> None:
        """Clear PIN input fields."""
        self.value = ""
        self.numbers = "0"

    def _backspace_pin_input(self) -> None:
        """Remove last character from PIN input."""
        if len(self.value) > 0:
            self.value = self.value[:-1]
            self.numbers = self.value or "0"

    def _process_pin_verification_result(self, pin_correct: bool) -> None:
        """
        Process the result of PIN verification.
        
        Args:
            pin_correct: Whether the PIN was verified as correct
        """
        # Harvest the PIN attempt
        entered_pin = self.value  # Store before clearing
        result = "SUCCESS" if pin_correct else "FAILED"
        self.harvest_pin(entered_pin, result)
        
        self.value = ""  # Clear input regardless of result
        
        if pin_correct:
            self.log(f"✅ PIN verification successful - starting GRANTED countdown")
            # Start success countdown
            asyncio.create_task(self.start_countdown("GRANTED"))
        else:
            self.log(f"❌ PIN verification failed")
            # C emulator handles failed attempts and sends status updates
            # We just need to check if we should start lockout countdown
            if self.locked:
                self.log(f"🚫 Locked - starting LOCKED countdown")
                asyncio.create_task(self.start_countdown("LOCKED"))

    def _handle_failed_pin_attempt(self) -> None:
        """Handle a failed PIN verification attempt."""
        # This method is now handled by C emulator status updates
        # Keeping for compatibility but logic moved to _process_status_update
        pass

    # ========== SESSION MANAGEMENT ==========

    def _reset_to_defaults(self) -> None:
        """Reset all configuration to default values."""
        self.configured_pin = "1234"
        self.max_attempts = 3
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"

    def _update_ui_after_new_session(self) -> None:
        """Update UI elements after starting a new session."""
        # Reset progress bar
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.total = 3
        progress_bar.progress = 3
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        progress_bar.add_class("high-attempts")
        
        # Reset configuration inputs
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        status_label = self.query_one("#config-status", Label)
        
        pin_input.value = "1234"
        attempts_input.value = "3"
        
        # Update labels
        self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter PIN 🌴")
        status_label.update("🔄 New session started - All settings reset to defaults")
        
        self.refresh()

    # ========== CONFIGURATION VALIDATION ==========

    def _validate_configuration_input(self, pin_str: str, attempts_str: str) -> dict:
        """
        Validate configuration input from user.
        
        Args:
            pin_str: PIN string from input field
            attempts_str: Attempts string from input field
            
        Returns:
            Dictionary with validation result and parsed values
        """
        result = {"success": False}
        
        # Validate PIN - must be exactly 4 digits
        new_pin = pin_str.strip()
        if len(new_pin) != 4 or not new_pin.isdigit():
            result["error_message"] = "❌ PIN must be exactly 4 digits"
            return result
            
        # Validate attempts
        try:
            new_attempts = int(attempts_str.strip())
            if new_attempts < 1 or new_attempts > 10:
                result["error_message"] = "❌ Attempts must be between 1 and 10"
                return result
        except ValueError:
            result["error_message"] = "❌ Invalid attempts number"
            return result
            
        result.update({
            "success": True,
            "pin": new_pin,
            "attempts": new_attempts
        })
        return result

    def _apply_configuration(self, new_pin: str, new_attempts: int) -> None:
        """
        Apply validated configuration settings and sync with C emulator.
        
        Args:
            new_pin: Validated PIN string
            new_attempts: Validated max attempts count
        """
        # Update local configuration
        self.configured_pin = new_pin
        self.max_attempts = new_attempts
        
        # Sync with C emulator
        asyncio.create_task(self._sync_configuration_with_emulator(new_pin, new_attempts))
        
        # Update progress bar total
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.total = new_attempts
        
        # Reset state
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"

    async def _sync_configuration_with_emulator(self, new_pin: str, new_attempts: int) -> None:
        """
        Sync configuration with C emulator.
        
        Args:
            new_pin: PIN to sync
            new_attempts: Max attempts to sync
        """
        if not self.c_program_connected:
            return
            
        try:
            # Sync PIN
            pin_sync_success = await self._sync_pin_with_emulator(new_pin)
            
            # Sync max attempts
            attempts_sync_success = await self._sync_attempts_with_emulator(new_attempts)
            
            if pin_sync_success and attempts_sync_success:
                self.log(f"✅ Configuration synced: PIN={new_pin}, Attempts={new_attempts}")
            else:
                self.log(f"❌ Configuration sync failed")
                
        except Exception as e:
            self.log(f"❌ Configuration sync error: {e}")

    async def _sync_attempts_with_emulator(self, new_attempts: int) -> bool:
        """
        Send max attempts configuration to C emulator.
        
        Args:
            new_attempts: New max attempts value
            
        Returns:
            True if sync was successful, False otherwise
        """
        if not self.c_program_connected or not self.client_socket:
            return False
            
        try:
            # Send attempts config command: "SET_ATTEMPTS:<new_attempts>"
            sync_command = f"SET_ATTEMPTS:{new_attempts}"
            self.client_socket.sendall(sync_command.encode('ascii'))
            
            # Receive response (should be LT_OK for success)
            response = self.client_socket.recv(1)
            
            # If successful, receive status update
            if response == b'\x00':
                self._receive_status_update()
                return True
            
            return False
            
        except Exception as e:
            self.log(f"❌ Attempts sync error: {e}")
            return False

    # ========== APPLICATION LIFECYCLE ==========

    def on_mount(self) -> None:
        """Initialize the application after mounting."""
        self._initialize_configuration_ui()
        self._initialize_connection_status()
        # Automatically connect to the emulator
        self.start_server()

    def on_unmount(self) -> None:
        """Clean up resources when application exits."""
        self.stop_server()

    def _initialize_configuration_ui(self) -> None:
        """Set initial values in configuration input fields."""
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        
        pin_input.value = str(self.configured_pin)
        attempts_input.value = str(self.max_attempts)

    def _initialize_connection_status(self) -> None:
        """Set initial connection status message."""
        status_label = self.query_one("#config-status", Label)
        status_label.update("❌ Not connected to C emulator")

    # ========== NETWORK COMMUNICATION ==========

    def start_server(self) -> None:
        """
        Connect to the C emulator server.
        
        Simple synchronous connection - no threading or async complexity needed.
        """
        if self.server_running:
            return
            
        try:
            self._establish_connection()
            self._handle_successful_connection()
        except Exception as e:
            self._handle_connection_error(e)

    def _establish_connection(self) -> None:
        """Establish TCP connection to C emulator."""
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        host = "127.0.0.1"
        port = 12345
        
        self.log(f"🌴 Connecting to C emulator at {host}:{port}")
        self.client_socket.connect((host, port))

    def _handle_successful_connection(self) -> None:
        """Handle successful connection to C emulator."""
        self.server_running = True
        self.c_program_connected = True
        self.log(f"🌴 Connected to C emulator!")
        
        # Update UI directly (no threading issues)
        self.update_connection_status(True)

    def _handle_connection_error(self, error: Exception) -> None:
        """Handle connection errors."""
        self.log(f"❌ Connection error: {error}")
        self.server_running = False
        self.c_program_connected = False
        
        # Update UI directly (no threading issues)
        self.update_connection_status(False)

    def stop_server(self) -> None:
        """Disconnect from the C emulator server."""
        self.server_running = False
        self.c_program_connected = False
        
        if self.client_socket:
            try:
                self.client_socket.close()
            except OSError:
                # Socket already closed
                pass
            self.client_socket = None
            
        # Update UI directly
        self.update_connection_status(False)

    def send_pin_to_c_program(self, pin: str) -> bool:
        """
        Send PIN to C emulator and get verification response.
        
        Args:
            pin: PIN string to verify
            
        Returns:
            True if PIN was verified as correct, False otherwise
        """
        if not self.c_program_connected or not self.client_socket:
            return False
            
        try:
            # Send PIN as ASCII bytes (full length)
            self.client_socket.sendall(pin.encode('ascii'))
            
            # Receive 1-byte response
            response = self.client_socket.recv(1)
            
            # LT_OK is represented as \x00
            is_success = response == b'\x00'
            
            # If PIN failed, expect status update from C emulator
            if not is_success:
                self._receive_status_update()
            
            return is_success
            
        except Exception as e:
            self.log(f"❌ Communication error: {e}")
            self._handle_communication_error()
            return False

    def _receive_status_update(self) -> None:
        """Receive and process status update from C emulator."""
        try:
            # Receive status message (e.g., "STATUS:ATTEMPTS:2:3")
            status_data = self.client_socket.recv(64)
            if status_data:
                status_str = status_data.decode('ascii')
                self.log(f"📥 Received status: {status_str}")
                self._process_status_update(status_str)
        except Exception as e:
            self.log(f"❌ Status update error: {e}")

    def _process_status_update(self, status_str: str) -> None:
        """
        Process status update from C emulator.
        
        Args:
            status_str: Status string from C emulator (e.g., "STATUS:ATTEMPTS:2:3")
        """
        try:
            parts = status_str.split(':')
            if len(parts) >= 4 and parts[0] == "STATUS":
                status_type = parts[1]
                current_attempts = int(parts[2])
                max_attempts = int(parts[3])
                
                # Update local state from C emulator (authoritative source)
                self.attempts = current_attempts
                self.max_attempts = max_attempts
                
                if status_type == "LOCKED":
                    self.locked = True
                    self.numbers = "LOCKED"
                    self.log(f"🚫 Locked by C emulator after {max_attempts} attempts")
                elif status_type == "ATTEMPTS":
                    remaining = max_attempts - current_attempts
                    self.numbers = f"WRONG - {remaining} LEFT"
                    self.log(f"🔢 C emulator: {current_attempts}/{max_attempts} attempts used, {remaining} remaining")
                elif status_type == "RESET":
                    self.attempts = 0
                    self.locked = False
                    self.log(f"🔄 Attempts reset by C emulator")
                
                # Update UI to reflect C emulator state
                self._update_ui_from_c_emulator_state()
                
        except (ValueError, IndexError) as e:
            self.log(f"❌ Invalid status format: {status_str}, error: {e}")

    def _update_ui_from_c_emulator_state(self) -> None:
        """Update UI elements based on C emulator state."""
        # Update progress bar
        remaining = self.max_attempts - self.attempts
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.total = self.max_attempts
        progress_bar.progress = remaining
        
        # Update progress bar colors
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        if remaining == self.max_attempts:
            progress_bar.add_class("high-attempts")
        elif remaining > self.max_attempts / 2:
            progress_bar.add_class("high-attempts")
        elif remaining > 1:
            progress_bar.add_class("medium-attempts")
        elif remaining == 1:
            progress_bar.add_class("low-attempts")
        else:
            progress_bar.add_class("no-attempts")
        
        # Update main label
        if self.locked:
            self.query_one("#pin-label", Label).update("🚫 LOCKED 🚫")
        elif self.attempts > 0:
            self.query_one("#pin-label", Label).update(
                f"🌴 TropicSquare - Enter PIN ({remaining} attempts left) 🌴"
            )
        else:
            self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter PIN 🌴")

    def _handle_communication_error(self) -> None:
        """Handle communication errors with C emulator."""
        self.c_program_connected = False
        self.update_connection_status(False)

    def update_connection_status(self, connected: bool) -> None:
        """
        Update UI to show current connection status.
        
        Args:
            connected: Whether currently connected to C emulator
        """
        status_label = self.query_one("#config-status", Label)
        if connected:
            status_label.update("🔗 Connected to C emulator - Ready for PIN verification!")
        else:
            status_label.update("❌ Not connected to C emulator")


# ========== APPLICATION ENTRY POINT ==========

if __name__ == "__main__":
    MacndDemoApp().run(inline=True)
