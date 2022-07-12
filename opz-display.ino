#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE); // EastRising 0.42" OLED

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

#define RENDER_SIZE 100
char render[3][RENDER_SIZE] = {"", "", ""};

unsigned long lastDraw = 0;
void draw()
{
  if (millis() - lastDraw > 100)
  {
    lastDraw = millis();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.drawStr(0, 0, render[0]);
    u8g2.drawStr(0, 14, render[1]);
    u8g2.drawStr(0, 28, render[2]);
    u8g2.sendBuffer();
  }
}

void setup(void)
{
  Wire.setSDA(22);
  Wire.setSCL(23);
  Wire.begin();
  u8g2.begin();

  strcpy(render[0], "OP-Z screen");
  strcpy(render[1], "v0.0.1");

#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
  // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
  TinyUSB_Device_Init(0);
#endif

  pinMode(LED_BUILTIN, OUTPUT);

  MIDI.begin(MIDI_CHANNEL_OMNI);

  MIDI.setHandleControlChange(handleControlChange);

  Serial.begin(115200);

  // wait until device mounted
  while (!TinyUSBDevice.mounted())
    delay(1);
}

void loop(void)
{
  MIDI.read();
  draw();
}

void handleControlChange(byte channel, byte number, byte value)
{
  switch (channel)
  {
  case 1:
    strcpy(render[0], "Kick");
    break;

  case 2:
    strcpy(render[0], "Snares");
    break;

  case 3:
    strcpy(render[0], "Perc");
    break;

  case 4:
    strcpy(render[0], "Sample");
    break;

  case 5:
    strcpy(render[0], "Bass");
    break;

  case 6:
    strcpy(render[0], "Lead");
    break;

  case 7:
    strcpy(render[0], "ARP");
    break;

  case 8:
    strcpy(render[0], "Chord");
    break;

  default:
    break;
  }

  switch (number)
  {
  case 1:
    strcpy(render[1], "P1");
    break;

  case 2:
    strcpy(render[1], "P2");
    break;

  case 3:
    strcpy(render[1], "Cutoff");
    break;

  case 4:
    strcpy(render[1], "Res");
    break;

  case 5:
    strcpy(render[1], "Attack");
    break;

  case 6:
    strcpy(render[1], "Decay");
    break;

  case 7:
    strcpy(render[1], "Substain");
    break;

  case 8:
    strcpy(render[1], "Release");
    break;

  case 9:
    strcpy(render[1], channel == 7 ? "ARP speed" :"LFO Depth");
    break;

  case 10:
    strcpy(render[1], channel == 7 ? "ARP Pattern" :"LFO Rate");
    break;

  case 11:
    strcpy(render[1], channel == 7 ? "ARP Style" :"LFO Dest");
    break;

  case 12:
    strcpy(render[1], channel == 7 ? "ARP Range" :"LFO Shape");
    break;

      case 13:
    strcpy(render[1], "FX Send 1");
    break;

  case 14:
    strcpy(render[1], "FX Send 2");
    break;

  case 15:
    strcpy(render[1], "Pan");
    break;

  case 16:
    strcpy(render[1], "Level");
    break;

  default:
    break;
  }
  snprintf(render[2], RENDER_SIZE, "%d", value);
}