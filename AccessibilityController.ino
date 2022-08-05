#include <Adafruit_ST7735.h>

import processing.serial.*;

#define cs   2
#define rst   3
#define dc  4

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

const uint16_t  Black        = 0x0000;
const uint16_t  Blue         = 0x001F;
const uint16_t  Red          = 0xF800;
const uint16_t  Green        = 0x07E0;
const uint16_t  Cyan         = 0x07FF;
const uint16_t  Magenta      = 0xF81F;
const uint16_t  Yellow       = 0xFFE0;
const uint16_t  White        = 0xFFFF;


const uint16_t background_color = Black;

// LCD dimensions in pixels
const int draw_width = 160;
const int draw_height = 128;


const int x_pin = A1;
const int y_pin = A0;

const int sw_draw = 5;
const int sw_clear = 7;

int x_stick = 0;
int y_stick = 0;

int x_cursor = draw_width/2;
int y_cursor = draw_height/2;

int old_x_cursor = draw_width/2;
int old_y_cursor = draw_height/2;

void setup() {
  // put your setup code here, to run once:
  
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(background_color);
  tft.setTextColor(White);
  tft.setTextSize(1);
  tft.setRotation(1);
  tft.setCursor(0,0);
  Serial.begin(9600);

  pinMode(x_pin, INPUT);
  pinMode(y_pin, INPUT);
  pinMode(sw_draw, INPUT_PULLUP);
  pinMode(sw_clear, INPUT_PULLUP);
}

void loop() {

  x_stick = analogRead(x_pin);
  y_stick = analogRead(y_pin);
  
  manageCursor(Red, 3);
  manageButtons();

  delay(25);
}


void manageCursor(uint16_t color, int radius){
  int raw_x_vel = map(x_stick, 0, 1023, -512, 512);
  int raw_y_vel = map(y_stick, 1023, 0, -512, 512);
  int x_vel = 0;
  int y_vel = 0;

  //Create deadzone in both directions, generate new velocity as a function of stick input
  
  if (abs(raw_x_vel) > 100){
    x_vel = raw_x_vel/128;
  }
  if (abs(raw_y_vel) > 100){
    y_vel = raw_y_vel/128;
  }

  // Check that new position is in bounds, if so place dot...
  if (!outOfBounds(x_cursor, y_cursor, x_vel, y_vel, radius)){
    x_cursor += x_vel;
    y_cursor += y_vel;

    tft.fillCircle(x_cursor, y_cursor, radius, color);
    
    // If in cursor mode, should not be leaving trail, erase old values
    if (digitalRead(sw_draw)){
      if (old_x_cursor != x_cursor || old_y_cursor != y_cursor){
         tft.fillCircle(old_x_cursor, old_y_cursor, radius, background_color);
      }
    }
    old_x_cursor = x_cursor;
    old_y_cursor = y_cursor;
  }
}  

void manageButtons(){
  if (invert(digitalRead(sw_clear)) && digitalRead(sw_draw)){
    tft.fillScreen(background_color);
  }
  // Both buttons clicked, interpret as submitting the letter
  if (invert(digitalRead(sw_clear)) && invert(digitalRead(sw_draw))){
    Serial.println("Submit");
  }
}
