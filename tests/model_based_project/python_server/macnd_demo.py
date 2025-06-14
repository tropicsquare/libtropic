"""
An implementation of a classic calculator, with a layout inspired by macOS calculator.

Works like a real calculator. Click the buttons or press the equivalent keys.
"""

from decimal import Decimal
import random
import asyncio
import socket
import threading

from textual import events, on
from textual.app import App, ComposeResult
from textual.containers import Container, Vertical
from textual.css.query import NoMatches
from textual.reactive import var
from textual.widgets import Button, Digits, Label, ProgressBar, TabbedContent, TabPane, Input


class MacndDemoApp(App):
    """A PIN input interface."""

    CSS_PATH = "macnd_demo.tcss"

    numbers = var("0")
    value = var("")
    attempts = var(0)
    max_attempts = var(3)
    locked = var(False)
    configured_pin = var("1234")  # Configurable PIN
    countdown_active = var(False)  # Track countdown state
    
    # Server communication variables
    server_socket = None
    client_socket = None
    server_thread = None
    server_running = var(False)
    c_program_connected = var(False)

    # Maps button IDs on to the corresponding key name for PIN input
    NAME_MAP = {
        "underscore": "backspace",
        "minus": "backspace",
        "c": "clear",
    }

    def generate_random_pin(self, digit_count: int = 4) -> str:
        """Generate a random PIN with specified number of digits."""
        return ''.join([str(random.randint(0, 9)) for _ in range(digit_count)])

    async def start_countdown(self, message_prefix: str) -> None:
        """Start a 10-second countdown, then generate new PIN and restart."""
        self.countdown_active = True
        
        # Countdown from 10 to 1
        for i in range(10, 0, -1):
            self.numbers = f"{message_prefix} - {i}"
            await asyncio.sleep(1)
        
        # Generate new PIN and restart session
        current_pin_length = len(self.configured_pin)
        new_pin = self.generate_random_pin(current_pin_length)
        self.configured_pin = new_pin
        
        # Reset all state
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"
        self.countdown_active = False
        
        # Update configuration tab to show new PIN
        pin_input = self.query_one("#pin-input", Input)
        status_label = self.query_one("#config-status", Label)
        pin_input.value = new_pin
        status_label.update(f"🎯 New Challenge! New PIN generated: {new_pin}")
        
        # Reset progress bar to full green
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.progress = self.max_attempts
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        progress_bar.add_class("high-attempts")
        
        # Reset label
        self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter 4-digit PIN 🌴")

    def watch_numbers(self, value: str) -> None:
        """Called when numbers is updated."""
        self.query_one("#numbers", Digits).update(value)

    def watch_attempts_OLD_DISABLED(self, attempts: int) -> None:
        """Called when attempts is updated - COMPLETELY DISABLED."""
        # Only update if not during reset
        if hasattr(self, '_resetting') and self._resetting:
            return
            
        # Update progress bar (show remaining attempts)
        remaining = self.max_attempts - attempts
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.progress = remaining
        
        # Set different colors based on remaining attempts as percentage of max
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        if remaining == self.max_attempts:
            # Full attempts - green
            progress_bar.add_class("high-attempts")
        elif remaining > self.max_attempts / 2:
            # More than half - green
            progress_bar.add_class("high-attempts")
        elif remaining > 1:
            # More than 1 but less than half - gold
            progress_bar.add_class("medium-attempts")
        elif remaining == 1:
            # Only 1 left - red
            progress_bar.add_class("low-attempts")
        else:
            # No attempts left - dark red
            progress_bar.add_class("no-attempts")
        
        # Update label text
        if attempts > 0 and not self.locked:
            self.query_one("#pin-label", Label).update(f"🌴 TropicSquare - Enter PIN ({remaining} attempts left) 🌴")
        elif self.locked:
            self.query_one("#pin-label", Label).update("🚫 LOCKED 🚫")
            progress_bar.progress = 0  # Empty bar when locked
        else:
            self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter 4-digit PIN 🌴")
            progress_bar.progress = self.max_attempts  # Full bar at start

    def compose(self) -> ComposeResult:
        """Add our PIN input with configuration tabs."""
        with TabbedContent(initial="pin-tab"):
            with TabPane("🌴 PIN Entry", id="pin-tab"):
                yield Label("🌴 TropicSquare - Enter 4-digit PIN 🌴", id="pin-label")
                yield ProgressBar(total=3, show_eta=False, show_percentage=False, id="attempts-bar")
                with Container(id="macnd-demo"):
                    yield Digits(id="numbers")
                    yield Button("1", id="number-1", classes="number")
                    yield Button("2", id="number-2", classes="number")
                    yield Button("3", id="number-3", classes="number")
                    yield Button("4", id="number-4", classes="number")
                    yield Button("5", id="number-5", classes="number")
                    yield Button("6", id="number-6", classes="number")
                    yield Button("7", id="number-7", classes="number")
                    yield Button("8", id="number-8", classes="number")
                    yield Button("9", id="number-9", classes="number")
                    yield Button("Clear", id="clear", variant="primary")
                    yield Button("0", id="number-0", classes="number")
                    yield Button("⌫", id="backspace", variant="primary")
                    yield Button("Enter", id="enter", variant="success")
                    yield Button("🔄 New Session", id="new-session", variant="warning")
            
            with TabPane("⚙️ Configuration", id="config-tab"):
                with Vertical(id="config-container"):
                    yield Label("🌴 TropicSquare Configuration 🌴", id="config-title")
                    yield Label("Set PIN (4 digits):", id="pin-config-label")
                    yield Input(placeholder="Enter 4-digit PIN", max_length=4, id="pin-input")
                    yield Label("Max Attempts (1-10):", id="attempts-config-label")
                    yield Input(placeholder="Enter max attempts", max_length=2, id="attempts-input")
                    yield Button("💾 Save Configuration", id="save-config", variant="success")
                    yield Button("🚀 Start Server", id="start-server", variant="success")
                    yield Button("🛑 Stop Server", id="stop-server", variant="error")
                    yield Label("", id="config-status")

    def on_key(self, event: events.Key) -> None:
        """Called when the user presses a key."""

        def press(button_id: str) -> None:
            """Press a button, should it exist."""
            try:
                self.query_one(f"#{button_id}", Button).press()
            except NoMatches:
                pass

        key = event.key
        if key.isdecimal():
            press(f"number-{key}")
        elif key == "c":
            press("clear")
        elif key == "backspace":
            press("backspace")
        elif key == "enter":
            press("enter")
        else:
            button_id = self.NAME_MAP.get(key)
            if button_id is not None:
                press(button_id)

    @on(Button.Pressed, ".number")
    def number_pressed(self, event: Button.Pressed) -> None:
        """Pressed a number."""
        if self.locked or self.countdown_active:
            return  # Don't allow input when locked or during countdown
            
        if len(self.value) < 4:  # Limit to 4 digits for PIN
            assert event.button.id is not None
            number = event.button.id.partition("-")[-1]
            self.numbers = self.value = self.value + number

    @on(Button.Pressed, "#clear")
    def pressed_clear(self) -> None:
        """Pressed Clear"""
        if self.locked or self.countdown_active:
            return  # Don't allow clearing when locked or during countdown
        self.value = ""
        self.numbers = "0"

    @on(Button.Pressed, "#backspace")
    def pressed_backspace(self, event: Button.Pressed) -> None:
        """Pressed Backspace"""
        if self.locked or self.countdown_active:
            return  # Don't allow backspace when locked or during countdown
            
        if len(self.value) > 0:
            self.value = self.value[:-1]
            self.numbers = self.value or "0"

    @on(Button.Pressed, "#enter")
    def pressed_enter(self, event: Button.Pressed) -> None:
        """Pressed Enter - PIN submission"""
        if self.countdown_active:
            return  # Don't allow input during countdown
            
        if self.locked:
            self.numbers = "LOCKED"
            return
            
        if len(self.value) == 4 and self.value.isdigit():
            # Check if connected to C program
            if not self.c_program_connected:
                self.numbers = "NO C PROGRAM"
                return
            
            # Send PIN to C program for verification
            pin_correct = self.send_pin_to_c_program(self.value)
            
            if pin_correct:
                # PIN is correct according to C program - start countdown then generate new PIN
                self.value = ""  # Clear the input value
                
                # Start countdown task
                asyncio.create_task(self.start_countdown("GRANTED"))
                
            else:
                # PIN is incorrect according to C program
                self.attempts += 1
                remaining = self.max_attempts - self.attempts
                
                if self.attempts >= self.max_attempts:
                    self.locked = True
                    
                    # Start countdown for locked state
                    asyncio.create_task(self.start_countdown("LOCKED"))
                    
                else:
                    self.numbers = f"WRONG - {remaining} LEFT"
                    
                    # Manually update progress bar for remaining attempts
                    progress_bar = self.query_one("#attempts-bar", ProgressBar)
                    progress_bar.progress = remaining
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
                        
                    self.query_one("#pin-label", Label).update(f"🌴 TropicSquare - Enter PIN ({remaining} attempts left) 🌴")
                    
                # Clear the input for next attempt
                self.value = ""
        else:
            self.numbers = "NEED 4 DIGITS"

    @on(Button.Pressed, "#new-session")
    def new_session(self) -> None:
        """Start a completely new session - reset everything to defaults."""
        # Reset all variables to defaults
        self.configured_pin = "1234"
        self.max_attempts = 3
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"
        
        # Update progress bar total and reset to full green
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.total = 3
        progress_bar.progress = 3
        progress_bar.remove_class("high-attempts", "medium-attempts", "low-attempts", "no-attempts")
        progress_bar.add_class("high-attempts")
        
        # Reset configuration inputs to defaults
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        status_label = self.query_one("#config-status", Label)
        
        pin_input.value = "1234"
        attempts_input.value = "3"
        
        # Update labels
        self.query_one("#pin-label", Label).update("🌴 TropicSquare - Enter 4-digit PIN 🌴")
        status_label.update("🔄 New session started - All settings reset to defaults")
        
        # Force refresh
        self.refresh()

    @on(Button.Pressed, "#save-config")
    def save_configuration(self) -> None:
        """Save PIN and attempts configuration."""
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        status_label = self.query_one("#config-status", Label)
        
        # Validate PIN input
        new_pin = pin_input.value.strip()
        if len(new_pin) != 4 or not new_pin.isdigit():
            status_label.update("❌ PIN must be exactly 4 digits")
            return
            
        # Validate attempts input
        try:
            new_attempts = int(attempts_input.value.strip())
            if new_attempts < 1 or new_attempts > 10:
                status_label.update("❌ Attempts must be between 1 and 10")
                return
        except ValueError:
            status_label.update("❌ Invalid attempts number")
            return
            
        # Save configuration
        self.configured_pin = new_pin
        self.max_attempts = new_attempts
        
        # Update progress bar total
        progress_bar = self.query_one("#attempts-bar", ProgressBar)
        progress_bar.total = new_attempts
        
        # Reset attempts and unlock if needed
        self.attempts = 0
        self.locked = False
        self.value = ""
        self.numbers = "0"
        
        status_label.update(f"✅ Configuration saved! Current PIN: {new_pin}, Max Attempts: {new_attempts}")
        
        # Clear attempts input field only (keep PIN visible for reference)
        attempts_input.value = ""

    @on(Button.Pressed, "#start-server")
    def start_server_button(self) -> None:
        """Handle start server button press."""
        self.start_server()

    @on(Button.Pressed, "#stop-server")
    def stop_server_button(self) -> None:
        """Handle stop server button press."""
        self.stop_server()

    def on_mount(self) -> None:
        """Initialize configuration inputs with current values."""
        # Set default values in configuration inputs
        pin_input = self.query_one("#pin-input", Input)
        attempts_input = self.query_one("#attempts-input", Input)
        
        pin_input.value = str(self.configured_pin)
        attempts_input.value = str(self.max_attempts)
        
        # Show initial connection status
        status_label = self.query_one("#config-status", Label)
        status_label.update("❌ Not connected to C program")
        
        # Automatically start the server
        self.start_server()

    def on_unmount(self) -> None:
        """Clean up server when app exits."""
        self.stop_server()

    def start_server(self) -> None:
        """Start the socket server to communicate with C program."""
        if self.server_running:
            return
            
        def server_worker():
            try:
                self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                
                host = "127.0.0.1"
                port = 12345
                self.server_socket.bind((host, port))
                self.server_socket.listen(5)
                
                self.server_running = True
                self.log(f"🌴 Server listening on {host}:{port}")
                
                # Wait for C program connection
                self.client_socket, addr = self.server_socket.accept()
                self.c_program_connected = True
                self.log(f"🌴 C program connected from {addr}")
                
                # Update UI to show connection status
                self.call_from_thread(self.update_connection_status, True)
                
            except Exception as e:
                self.log(f"❌ Server error: {e}")
                self.server_running = False
                
        self.server_thread = threading.Thread(target=server_worker, daemon=True)
        self.server_thread.start()

    def stop_server(self) -> None:
        """Stop the socket server."""
        self.server_running = False
        self.c_program_connected = False
        
        if self.client_socket:
            try:
                self.client_socket.close()
            except:
                pass
            self.client_socket = None
            
        if self.server_socket:
            try:
                self.server_socket.close()
            except:
                pass
            self.server_socket = None
            
        self.call_from_thread(self.update_connection_status, False)

    def send_pin_to_c_program(self, pin: str) -> bool:
        """Send PIN to C program and get response."""
        if not self.c_program_connected or not self.client_socket:
            return False
            
        try:
            # Send PIN as 3 ASCII bytes (C program only reads 3 bytes)
            # Take first 3 digits of our 4-digit PIN
            pin_3_digits = pin[:3].zfill(3)
            self.client_socket.sendall(pin_3_digits.encode('ascii'))
            
            # Receive 1-byte response
            response = self.client_socket.recv(1)
            
            # LT_OK is represented as \x00
            return response == b'\x00'
            
        except Exception as e:
            self.log(f"❌ Communication error: {e}")
            self.c_program_connected = False
            self.call_from_thread(self.update_connection_status, False)
            return False

    def update_connection_status(self, connected: bool) -> None:
        """Update UI to show connection status."""
        try:
            status_label = self.query_one("#config-status", Label)
            if connected:
                status_label.update("🔗 Connected to C program - Ready for PIN verification!")
            else:
                status_label.update("❌ Not connected to C program")
        except:
            pass  # UI might not be ready yet


if __name__ == "__main__":
    MacndDemoApp().run(inline=True)
