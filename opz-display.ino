/* 
OP-Z screen
Raspberry Pico connected to SSD1306 0.96" monochrome screen
GND - GND
VDD - 3.3V
SCK - GP5
SDA - GP4
Raspberry Pico must be configured to accept arduino code with this guide: https://learn.adafruit.com/rp2040-arduino-with-the-earlephilhower-core/overview

Original code by https://github.com/apiel/opz-display
Modified to work with 0.96" OLED screen by larsoe

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

#define RENDER_SIZE 100
char render[3][RENDER_SIZE] = {"", "", ""};

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Wire
#define PIN_WIRE_SDA        (6u)
#define PIN_WIRE_SCL        (7u)

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//---------Draw funksjon, skift ut u8g2-------------------------------------
unsigned long lastDraw = 0;
void draw()
{
  if (millis() - lastDraw > 100)
  {
    lastDraw = millis();
    display.clearDisplay();
    display.setTextSize(2);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(10, 10);
    display.println(render[0]);
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.println(render[1]);
    display.setCursor(10, 50);
    display.println(render[2]);
    display.display();
  }
}
//-----------------------------------------------------------------------

void setup() {
  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
  #endif

    pinMode(LED_BUILTIN, OUTPUT);

    MIDI.begin(MIDI_CHANNEL_OMNI);

    MIDI.setHandleControlChange(handleControlChange);

  
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(40, 30);
  display.println("Booting!");
  display.display();
  delay(2000);
}

void loop() {
    MIDI.read();
    draw();
  
}

void handleControlChange(byte channel, byte cc, byte value)
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

  switch (cc)
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
    strcpy(render[1], channel == 7 ? "ARP speed" : "LFO Depth");
    break;

  case 10:
    strcpy(render[1], channel == 7 ? "ARP Pattern" : "LFO Rate");
    break;

  case 11:
    strcpy(render[1], channel == 7 ? "ARP Style" : "LFO Dest");
    break;

  case 12:
    strcpy(render[1], channel == 7 ? "ARP Range" : "LFO Shape");
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

  if (cc == 12 && channel != 7)
  {
    if (value < 10)
    {
      strcpy(render[2], "SIN");
    }
    else if (value < 21)
    {
      strcpy(render[2], "TRI");
    }
    else if (value < 32)
    {
      strcpy(render[2], "SSQR");
    }
    else if (value < 43)
    {
      strcpy(render[2], "SAW");
    }
    else if (value < 54)
    {
      strcpy(render[2], "RND");
    }
    else if (value < 64)
    {
      strcpy(render[2], "GYRO");
    }
    else if (value < 75)
    {
      strcpy(render[2], "SIN triggered");
    }
    else if (value < 85)
    {
      strcpy(render[2], "TRI triggered");
    }
    else if (value < 96)
    {
      strcpy(render[2], "SSQR triggered");
    }
    else if (value < 107)
    {
      strcpy(render[2], "SAW triggered");
    }
    else if (value < 118)
    {
      strcpy(render[2], "RND triggered");
    }
    else
    {
      strcpy(render[2], "ONCE triggered");
    }
  }
  else if (cc == 11 && channel != 7)
  {
    if (value < 16)
    {
      strcpy(render[2], "P1");
    }
    else if (value < 32)
    {
      strcpy(render[2], "P2");
    }
    else if (value < 48)
    {
      strcpy(render[2], "CUTOFF");
    }
    else if (value < 64)
    {
      strcpy(render[2], "RES");
    }
    else if (value < 80)
    {
      strcpy(render[2], "ATTACK");
    }
    else if (value < 96)
    {
      strcpy(render[2], "PITCH");
    }
    else if (value < 112)
    {
      strcpy(render[2], "PAN");
    }
    else
    {
      strcpy(render[2], "VOLUME");
    }
  }
  else if (cc == 10 && channel != 7)
  {
    if (value < 8)
    {
      strcpy(render[2], "1/64");
    }
    else if (value < 16)
    {
      strcpy(render[2], "1/32");
    }
    else if (value < 24)
    {
      strcpy(render[2], "1/16");
    }
    else if (value < 32)
    {
      strcpy(render[2], "1/8");
    }
    else if (value < 40)
    {
      strcpy(render[2], "1/4");
    }
    else if (value < 48)
    {
      strcpy(render[2], "1/2");
    }
    else if (value < 56)
    {
      strcpy(render[2], "1/1");
    }
    else if (value < 64)
    {
      strcpy(render[2], "2/1");
    }
    else
    {
      snprintf(render[2], RENDER_SIZE, "%d Hz", value - 64);
    }
  }
  else if (cc == 9 && channel != 7)
  {
    snprintf(render[2], RENDER_SIZE, "%d", value - 64);
  }
  else if (cc == 15)
  {
    if (value == 64)
    {
      strcpy(render[2], "Center");
    }
    else
    {
      snprintf(render[2], RENDER_SIZE, "%d %s", abs(value - 64), value < 64 ? "Left" : "Right");
    }
  }
  else if (cc == 9 && channel == 7)
  {
    if (value == 0)
    {
      strcpy(render[2], "Off");
    }
    else
    {
      snprintf(render[2], RENDER_SIZE, "%d", (int)(value / 16) + 1);
    }
  }
  else if (cc == 10 && channel == 7)
  {
    if (value < 21)
    {
      strcpy(render[2], "manual");
    }
    else if (value < 43)
    {
      strcpy(render[2], "up");
    }
    else if (value < 64)
    {
      strcpy(render[2], "down");
    }
    else if (value < 85)
    {
      strcpy(render[2], "up/down");
    }
    else if (value < 107)
    {
      strcpy(render[2], "down/up");
    }
    else
    {
      strcpy(render[2], "random");
    }
  }
  else if (cc == 11 && channel == 7)
  {
    snprintf(render[2], RENDER_SIZE, "%d", (int)(value / 21) + 1);
  }
  else
  {
    snprintf(render[2], RENDER_SIZE, "%d", value);
  }
}
