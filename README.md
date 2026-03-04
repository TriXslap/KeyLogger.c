# KeyLogger.c

A simple keylogger project for Linux systems written in C. This program captures keyboard input events directly from the device file, translates them into human-readable characters, and logs them to a file. It is designed to run as a background daemon process.

## Disclamer
The project is still in progress, and I intend to add a few details before finishing it(running on boot, finding the keyboard device alone, trying to keep Root privilege...)

## Features

-   **Daemonization**: Runs silently in the background, detached from the terminal.
-   **Keystroke Logging**: Captures all key presses, including alphanumeric characters and special keys like `[SHIFT]`, `[ENTER]`, and `[BACKSPACE]`.
-   **Shift & Caps Lock Handling**: Correctly logs uppercase characters and symbols when the Shift or Caps Lock keys are active.
-   **Stealthy Log File**: Saves the captured keystrokes to a hidden file in the `/tmp` directory (`/tmp/.keylog.txt`).
-   **Graceful Shutdown**: Uses signal handling to close files and exit cleanly upon receiving `SIGINT` or `SIGTERM`.

## Prerequisites

-   A Linux-based operating system.
-   Root privileges are required to read from the `/dev/input/` device files.
-   `gcc` compiler to build the program from source.

## Setup and Usage

### 1. Identify Your Keyboard Device

The program is hardcoded to use `/dev/input/event2`. This may not be correct for your system. You need to find the event file corresponding to your keyboard.

You can list all input devices and their handlers with the following command:

```bash
grep -E 'Handlers|EV' /proc/bus/input/devices
```

Look for the device with `EV=120013` (a common event mask for keyboards) and note its `eventX` handler. Then, update the `DEVICE_INPUT_PATH` macro in `keyLogger.c` with the correct path.

For example, if your keyboard is `event4`, change the line to:
```c
#define DEVICE_INPUT_PATH "/dev/input/event4"
```

### 2. Compile the Program

Use `gcc` to compile the source code into an executable file named `KeyLogger`.

```bash
gcc keyLogger.c -o KeyLogger
```

### 3. Run the Keylogger

Execute the compiled program with `sudo` to grant it the necessary root privileges.

```bash
sudo ./KeyLogger
```

The program will fork into the background, and you can continue using your terminal.

### 4. View the Logs

The keystrokes are logged to `/tmp/.keylog.txt`. You can view the contents of this file at any time.

```bash
cat /tmp/.keylog.txt
```
To monitor the log file in real-time:
```bash
tail -f /tmp/.keylog.txt
```

### 5. Stop the Keylogger

To stop the keylogger, you need to find its Process ID (PID) and send it a termination signal.

```bash
# Find the PID
ps aux | grep KeyLogger

# Terminate the process using its PID
sudo kill <PID>
```

Upon termination, the process will log a "Process Terminated" message to the log file and shut down cleanly.

## Disclaimer

This tool is intended for educational and research purposes only. Using this keylogger on any computer without the owner's explicit permission is illegal and unethical. The developer is not responsible for any malicious use of this software.
