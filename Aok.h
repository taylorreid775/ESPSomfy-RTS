#ifndef AOK_H
#define AOK_H
#include <Arduino.h>

// A-OK / AC136 433.92 MHz OOK (TX-only). Frame layout from public protocol
// documentation: 0xA3 + 24-bit ID + 16-bit channel bitmap + command + checksum + trailing 1.

#define AOK_FREQUENCY 433.92f
#define AOK_BIT_COUNT 65
#define AOK_REPEAT 8

#define AOK_CMD_UP 11
#define AOK_CMD_DOWN 67
#define AOK_CMD_STOP 35
#define AOK_CMD_PROG 83

#define AOK_AGC1_US 5300
#define AOK_AGC2_US 530
#define AOK_SILENCE_US 5030
#define AOK_SHORT_US 270
#define AOK_LONG_US 565

uint16_t aokChannelMask(uint8_t channel);
void aokEncodeBits(uint32_t remoteId, uint8_t channel, uint8_t command, uint8_t bits[AOK_BIT_COUNT]);
void aokTransmit(uint8_t gpioPin, const uint8_t bits[AOK_BIT_COUNT]);

#endif
