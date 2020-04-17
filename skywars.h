
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define ANIMATION_DELAY 100 // milliseconds
#define MAX_DEVICES 4
#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

#define joyX A1
#define joyY A0
#define btn1 2
#define btn2 3

unsigned char reverse_bits(unsigned char b)
{
  return (b * 0x0202020202ULL & 0x010884422010ULL) % 0x3ff;
}

unsigned char achter[8] = {
  B01000000,
  B10100000,
  B01010000,
  B00101000,
  B00010100,
  B00001010,
  B00000101,
  B00000010,
};


unsigned char midden[8] = {
  B11000000,
  B11100000,
  B01110000,
  B00111000,
  B00011100,
  B00001110,
  B00000111,
  B00000011,
};

unsigned char voor[8] = {
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000100,
  B00000010,
  B00000001,
};