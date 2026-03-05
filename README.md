# KeyLogger.c

A keylogger for Linux written in C — a self-taught learning project built to get hands-on experience with low-level Linux concepts like input devices, signals, file descriptors, and `systemd` services.

This is still a work in progress, and I'm actively learning as I go. Feel free to look through the code, suggest improvements, or use it as a reference for your own learning!

---

## What I Learned Building This

This project ended up being a great way to explore a bunch of C and Linux concepts I hadn't worked with before:

- Reading raw input events from `/dev/input/` using `input_event` structs
- Parsing `/proc/bus/input/devices` to find the right keyboard device at runtime
- Writing and registering a `systemd` service so the program survives reboots
- Handling Unix signals (`SIGINT`, `SIGTERM`) for a clean shutdown
- Working with file descriptors, `fopen`, `fork`, and process management

---

## How It Works

### First Run — Auto-Install
When you run the program for the first time, it checks if it's already installed at `/usr/local/bin/KeyLogger`. If not, it will:

1. Copy itself to `/usr/local/bin/KeyLogger`
2. Create a `systemd` service file at `/etc/systemd/system/KeyLogger.service`
3. Enable and start the service automatically via `systemctl`

From that point on, the keylogger starts on every boot by itself — no need to run it again manually.

### Keyboard Device Detection
Instead of hardcoding a device path (which would break on different machines), the program reads `/proc/bus/input/devices` and looks for a device with the event mask `EV=120013` — the standard identifier for keyboards on Linux. It then builds the correct `/dev/input/eventX` path on its own.

### Keystroke Logging
The program reads raw `input_event` structs from the keyboard device in a loop and translates key codes into readable characters. It correctly handles:

- All standard letters, numbers, and symbols
- `[SHIFT]` (left & right) for uppercase letters and symbols like `!`, `@`, `#`, etc.
- `[CAPS LOCK]` toggling
- Special keys like `[ESC]`, `[TAB]`, `[BACKSPACE]`, `[LCTRL]`, and `[ENTER]`

Everything gets written to `/var/log/.keylog.txt` with buffering turned off, so keystrokes hit the file immediately.

### Clean Shutdown
When the program receives a `SIGINT` or `SIGTERM` signal, it logs a termination message (including the signal number) to the file, closes everything properly, and exits.

---

## Requirements

- A Linux system with `systemd`
- Root privileges (required to access `/dev/input/` device files)
- `gcc` to compile

---

## Build & Usage

### Compile
```bash
gcc keyLogger.c -o KeyLogger
```

### First Run
```bash
sudo ./KeyLogger
```

That's it! The program installs itself and sets up the service. You won't need to run it manually again.

### View the Logs
```bash
cat /var/log/.keylog.txt
```

Or watch it live:
```bash
tail -f /var/log/.keylog.txt
```

### Managing the Service
```bash
# Check if it's running
sudo systemctl status KeyLogger.service

# Stop it
sudo systemctl stop KeyLogger.service

# Remove it from startup
sudo systemctl disable KeyLogger.service
```

---

## Disclaimer

This project was built purely for **educational purposes** as part of my learning journey. Running this on any machine without the owner's explicit permission is illegal and unethical. Please use it responsibly.