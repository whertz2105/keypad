# ESP32 Adjustable Macropad

This project provides a basic firmware sketch for an ESP32 based macropad and a simple Windows configurator.

## Firmware

The Arduino sketch is located in `firmware/keypad.ino`. It implements:

- Configurable key matrix size.
- USB HID keyboard output.
- "Burner" mode for updating key mappings. Hold the burner pin (`GPIO0` by default) low at boot and send a JSON mapping over serial.

Example mapping JSON:

```json
{
  "keys": [["A", "B", "C"], ["Ctrl+Alt+Delete", "Enter", "Hello"], ["1", "2", "3"]]
}
```

The JSON is stored in flash using the `Preferences` library.

## Windows Configurator

`windows_configurator.py` is a small `tkinter` GUI for sending mapping JSON files to the board. Install `pyserial` and run the script or build an executable using PyInstaller:

```bash
pip install pyserial
pyinstaller --onefile windows_configurator.py
```

Select the serial port, load a JSON mapping, and send it to the device while it is in burner mode.
