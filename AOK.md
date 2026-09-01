# A-OK blinds on this ESPSomfy-RTS fork

This local fork (`aok-tx`) adds **TX-only A-OK / AC136** control on the **same ESP32 + CC1101** you already use for Somfy RTS.

- Somfy stays at **433.42 MHz** receive (physical remotes still update position).
- Each A-OK command hops the radio to **433.92 MHz**, sends 8 OOK frames, then restores 433.42 MHz RX.
- **Kendal / CHQ7208T fan (303.73 MHz) is not supported.** That band needs different RF matching and antenna.

Do **not** mix A-OK and Somfy shades in the same ESPSomfy **group**.

## Do not GitHub-OTA from rstrouse

Stock ESPSomfy GitHub updates would overwrite this fork. GitHub check/install is disabled. Flash firmware from this tree instead (USB or **System → firmware upload** of a build you compiled).

## Build and flash

This is still an Arduino sketch (`SomfyController.ino`). Follow the upstream [Firmware Guide](https://github.com/rstrouse/ESPSomfy-RTS/wiki/Installing-the-Firmware) with these extras:

1. Open `ESPSomfy-RTS/SomfyController.ino` (this folder, not a downloaded rstrouse zip).
2. Same libraries as upstream: SmartRC-CC1101-Driver-Lib, ArduinoJson, etc.
3. Compile and upload the sketch **and** the `data/` LittleFS image (web UI includes the A-OK protocol option).
4. After flash, Somfy shades already in `/shades.cfg` load as before (config version 25 adds an A-OK channel byte; old files default to channel 1).

## Pair an A-OK / AC136 motor

1. In ESPSomfy: **Add Shade**, protocol **A-OK**, type **Roller Shade**.
2. Set **A-OK Channel** to the AC136 channel (1–6) used for that motor.
3. Leave **Remote Address** unique (factory default is fine). This is the virtual remote ID.
4. Put the motor in learn mode (PROG on the motor; usually ~10 seconds).
5. Open **Send Prog (pair)** / Pair Shade and press **Prog**. The motor should jog.
6. If it does not jog, try **Prog** again, then **Up** from the home screen while still in learn mode (some motors pair on UP).
7. Mark **Shade Paired**. Test Up / Stop / Down.

Home Assistant: reuse the existing ESPSomfy-RTS-HA integration. An A-OK shade is a normal cover (up/stop/down, timed position). No HA code changes.

Physical AC136 remotes are **not** tracked (TX-only). Position is estimated from up/down times, same as unlinked motors.

## Channel bitmap

Channel *n* is encoded as the A-OK 16-bit address bit field used by AC114-style remotes: channel 1 = `0x0100`, channel 2 = `0x0200`, … channel 6 = `0x2000`. If a motor ignores commands, try the other AC136 channels or report it — some AC123 remotes use the opposite bit order.

## Commands

| ESPSomfy | A-OK byte |
|---|---|
| Up | 11 |
| Down | 67 |
| My / Stop | 35 |
| Prog | 83 |

## Test plan

1. **Somfy regression:** existing RTS shades still move. Serial/web frame log still shows `56-S` (or similar) from a physical Somfy remote **after** you have sent an A-OK command.
2. **A-OK:** pair one motor, then Up / Stop / Down from the web UI and from Home Assistant.
3. If nothing moves: check radio enabled, TX pin, unique address, channel, Prog vs Up for pairing, and that you uploaded the new LittleFS `data/` UI.

## Protocol notes

A-OK frame (65 bits): `0xA3` + 24-bit ID + 16-bit channel + 8-bit command + 8-bit checksum + trailing `1`. Checksum is the low 8 bits of the sum of ID, address, and command (not `0xA3`). Encoder is a reimplementation of the public layout, not a copy of GPL sources.
