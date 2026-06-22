# Arduino TOTP

An Arduino-based TOTP storing the private key in EEPROM and getting time
synchronization information from serial communication.

## Setup

This sketch requires the following libraries:

- [Base32-Decode](https://github.com/dirkx/Arduino-Base32-Decode)
- [TOTP](https://github.com/lucadentella/TOTP-Arduino)

The setup of this system is as follows:

1. Wire the [hardware connections](#hardware-connections) as described below to
   the Arduino
2. Write the `sketches/totp_arduino/totp_arduino.ino` sketch to the Arduino
3. Make the codeset and unixsync utilities for management. Currently only
   support POSIX-compatible systems
4. Use `codeset` to write a base32 private key to the EEPROM
5. Use `unixsync` as a daemon program to continuously sync time to the Arduino

> [!NOTE]
> You can securely clear the key made in `codeset` by using the totp_clearkey
> sketch. Just connect a random source (e.g., a thermistor) to the A0 pin then
> run!

### Hardware connections

```
LCD 1602 (for display)

VSS -> GND
VDD -> 5V
V0  -> POT middle
RS  -> Digital 12
RW  -> GND
E   -> Digital 11
D4  -> Digital 5
D5  -> Digital 4
D6  -> Digital 3
D7  -> Digital 2
A   -> 220Ω resistor -> 5V
K   -> GND
```

```
POTENTIOMETER (for adjusting LCD contrast)

left -> GND
middle -> LCD V0
right -> 5V
```

```
BTN (for reset)

side 1 -> Digital 8
side 2 -> 10kΩ resistor -> 3.3V
```

## Utilities

> [!WARNING]
> These utilities were designed with POSIX-compatible systems in mind (i.e.,
> macOS and Linux-based distros). They will not work in their current form under
> Windows due to differences in how IO is handled at a low level there.

To find the board you want to interact with, do one of the following after connecting them:

1. (with arduino-cli) `arduino-cli board list`
2. (with ls) `ls /dev/cu.*`

The device identifier used in the utilities below is the `/dev/cu.`-prefixed device name

### `codeset`

Used to change the private key being used.

1. Connect the Arduino to a serial port available to the PC
2. Hold the reset button (BTN in [hardware connections](#hardware-connections))
3. Run `codeset <device> <key>` with the arguments:
  1. `<device>`: The device to write to
  2. `<key>`: a base32-encoded private key to write
6. Release the reset button when told to by the program
7. If you followed this correctly, it should say `Waiting for UNIX time...` on the display

### `unixsync`

Used to sync the time on the Arduino with the executing computer

1. Connect the Arduino to a serial port available to the PC
2. Run `unixsync <device> [onetime flag]` with the arguments:
  3. `<device>`: the device name to write to
  4. `[onetime flag]`: optional; set to `1` to make the utility send one sync
     message then exit
3. The utility will keep running until interrupted, regularly sending sync
   messages to the Arduino
4. If you followed this correctly, it should start displaying the code and
   expiration counter on the display
