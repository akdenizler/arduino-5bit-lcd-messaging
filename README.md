# Arduino 5-Bit LCD Message Transmitter/Receiver

This project implements a simple custom serial communication system between two Arduino boards. A transmitter encodes characters into 5-bit values, sends them over a single data pin using a framed protocol, and displays the transmission process on a 16x2 I2C LCD. The receiver listens for the signal, validates the frame, decodes the characters, and displays the reconstructed message on its own LCD.

## Features

- Custom 5-bit character encoding
- Framed transmission format:
  - 1 start bit (`HIGH`)
  - 5 data bits
  - 1 stop bit (`LOW`)
- Start-of-message and end-of-message control codes
- Live bit-by-bit visualization on LCD row 2
- Decoded message shown on LCD row 1
- LED feedback during transmission and reception
- End-of-message pulse on the receiver

## Character Encoding

The system uses 5-bit codes:

- `A-Z` -> `0-25`
- `-` -> `26`
- `'` -> `27`
- `.` -> `28`
- `/` -> `29`
- `START_MSG` -> `30` (`11110`)
- `END_MSG` -> `31` (`11111`)

## Hardware

Each Arduino setup uses:

- 1 Arduino board
- 1 16x2 I2C LCD
- 1 LED on pin 13
- 1 data line on pin 8

Additional indicators used in the code:
- Transmitter pulses pin 2 during each bit send
- Receiver pulses pin 6 when a full message is received

## How It Works

### Transmitter
The transmitter:
1. Displays the full message on the first row of the LCD
2. Sends a start-of-message code
3. Encodes each character into 5 bits
4. Sends each bit with timing delays
5. Shows the current 5-bit pattern on LCD row 2
6. Sends an end-of-message code

### Receiver
The receiver:
1. Waits for a start bit
2. Reads 5 incoming bits with fixed timing
3. Checks for a valid stop bit
4. Detects start-of-message and end-of-message codes
5. Decodes normal characters and prints them to LCD row 1
6. Shows incoming bits live on LCD row 2
7. Pulses an output pin when the message finishes

## Message Format

Each transmitted code is framed as:

`[START bit = HIGH] [5 data bits] [STOP bit = LOW]`

A full message looks like:

`START_MSG -> character codes -> END_MSG`

## Example Message

The transmitter currently sends:

`HELLO-WORLD`

## File Structure

- `transmitter.ino` — encodes and sends the message
- `receiver.ino` — reads, validates, and decodes the message

## Notes

- Timing is controlled with `BIT_MS = 300`, so both boards must use the same bit duration
- The receiver ignores frames with invalid stop bits
- Messages longer than 16 characters are truncated to fit the LCD

