#include "Aok.h"

uint16_t aokChannelMask(uint8_t channel) {
  if (channel < 1) channel = 1;
  if (channel > 16) channel = 16;
  // First address byte carries channels 1-8 (LSB = channel 1). Second byte is 9-16.
  if (channel <= 8)
    return static_cast<uint16_t>(1 << (channel - 1)) << 8;
  return static_cast<uint16_t>(1 << (channel - 9));
}

void aokEncodeBits(uint32_t remoteId, uint8_t channel, uint8_t command, uint8_t bits[AOK_BIT_COUNT]) {
  uint8_t frame[8];
  uint16_t addr = aokChannelMask(channel);
  frame[0] = 0xA3;
  frame[1] = static_cast<uint8_t>((remoteId >> 16) & 0xFF);
  frame[2] = static_cast<uint8_t>((remoteId >> 8) & 0xFF);
  frame[3] = static_cast<uint8_t>(remoteId & 0xFF);
  frame[4] = static_cast<uint8_t>((addr >> 8) & 0xFF);
  frame[5] = static_cast<uint8_t>(addr & 0xFF);
  frame[6] = command;
  frame[7] = static_cast<uint8_t>((frame[1] + frame[2] + frame[3] + frame[4] + frame[5] + frame[6]) & 0xFF);
  for (uint8_t i = 0; i < 64; i++) {
    bits[i] = (frame[i / 8] >> (7 - (i % 8))) & 0x01;
  }
  bits[64] = 1;
}

static void aokHigh(uint32_t pinMask, uint16_t us) {
  REG_WRITE(GPIO_OUT_W1TS_REG, pinMask);
  delayMicroseconds(us);
}

static void aokLow(uint32_t pinMask, uint16_t us) {
  REG_WRITE(GPIO_OUT_W1TC_REG, pinMask);
  delayMicroseconds(us);
}

void aokTransmit(uint8_t gpioPin, const uint8_t bits[AOK_BIT_COUNT]) {
  uint32_t pinMask = 1u << gpioPin;
  aokHigh(pinMask, AOK_AGC1_US);
  aokLow(pinMask, AOK_AGC2_US);
  for (uint8_t i = 0; i < AOK_BIT_COUNT; i++) {
    if (bits[i]) {
      aokHigh(pinMask, AOK_LONG_US);
      aokLow(pinMask, AOK_SHORT_US);
    } else {
      aokHigh(pinMask, AOK_SHORT_US);
      aokLow(pinMask, AOK_LONG_US);
    }
  }
  aokLow(pinMask, AOK_SILENCE_US);
}
