//Unity Wrmhl Alternative controller for Global Game Jame
//https://github.com/relativty/wrmhl

//PortName: COM21
//Baud Rate: 250000
//Read Timeout: -1
//Queue lenght: 10

//16 Circle Neopixel
//I2C LCD 16x2
//Slide Pot (Robotale)
//4 Buttons 12mm
//Open Smart Knob (180°)

/* I2C LCD */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

/* Serial Message */
const String LCD1_MES = "Message";
const String LCD1_MES2 = "1";
const String LCD2_MES = "Message";
const String LCD2_MES2 = "2";
const String LCD3_MES = "Message";
const String LCD3_MES2 = "3";


/* Leds */
#include <FastLED.h>
const int NUM_LEDS = 16;
const int DATA_PIN = 8;
// Define the array of leds
CRGB leds[NUM_LEDS];

/* BUTTONS */
int nb_buttons = 4;
int buttons_pins[4] = {3, 4, 7, 6}; //Pins where buttons are plugged
unsigned long buttons_debounce[4] = {0, 0, 0, 0}; //Time since buttons has changed
unsigned long debounce_delay = 15; //Delay in milleseconds between each check
bool buttons_pressed[4] = {true, true, true, true}; //State of buttons (inverted logic)

/* Analogic */
int nb_analog = 2;
int analog_pins[2] = {A1, A2}; //Pins where analog devices are plugged (ex:knobs/potentiometer)
int analog_values[2] = { -1, -1};

unsigned long analog_debounce[2] = {0, 0}; //
unsigned long analog_debounce_delay = 200;

void setup() {
  Serial.begin(250000);
  Serial.setTimeout(100);
  setup_leds();
  setup_lcd();
  setup_buttons();
}

void loop() {
  buttons_read();
  analog_read();
  serial_read();
}


void setup_buttons() {
  //Setup Buttons
  for (int button = 0; button < nb_buttons; button++) {
    pinMode(buttons_pins[button], INPUT_PULLUP);
    //Serial.println(buttons_pins[button]);
  }
}

void setup_leds() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  LEDS.setBrightness(10);
  change_leds(0);
}


void setup_lcd() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
  lcd.noCursor();
}

void serial_read() {
  if (Serial.available()) {

    String command = Serial.readStringUntil("\n");
    if (command == "LCD1\n") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(LCD1_MES);
      lcd.setCursor(0, 1);
      lcd.print(LCD1_MES2);
      Serial.println("LCD1;OK");
    }
    else if (command == "LCD2\n") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(LCD2_MES);
      lcd.setCursor(0, 1);
      lcd.print(LCD2_MES2);
      Serial.println("LCD2;OK");
    }
    else if (command == "LCD3\n") {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(LCD3_MES);
      lcd.setCursor(0, 1);
      lcd.print(LCD3_MES2);
      Serial.println("LCD3;OK");
    }
    else if (command == "LCDC\n") {
      lcd.clear();
    }
    else if (command[0] = "@") {
      command.remove(0, 1);
      int level = command.toInt();
      //Serial.println(level);
      change_leds(level);
    }
  }
}

//0 : Red
//60 : Yellow
//120: Green
//https://raw.githubusercontent.com/FastLED/FastLED/gh-pages/images/HSV-rainbow-with-desc.jpg
void change_leds(int level) {
  int hue = 0;

  //Check level is between 0 and 16.
  if (level <= 16) {
    if (level >= 0) {

      //Low
      if (level >= 1) {
        if (level <= 5) {
          hue = 0;
        }
      }

      //Med
      if (level >= 6) {
        if (level <= 10) {
          hue = 64;
        }
      }

      //High
      if (level > 10) {
        hue = 96;
      }

      //Turn used leds on
      for (int i = 0; i < level; i++) {
        leds[i] = CHSV(hue, 255, 255);
      }

      //Turn unused leds off
      for (int i = level; i < NUM_LEDS; i++) {
        leds[i] = CHSV(0, 0, 0);
      }

      //Execute commands
      FastLED.show();
      Serial.println("LEDS;OK");
    }
  }
}


void analog_read() {
  for (int analog_input = 0; analog_input < nb_analog; analog_input++) {

    //Print to serial when analog_debounce_delay is finished
    if ((millis() - analog_debounce[analog_input] > analog_debounce_delay)) {
      Serial.print(analog_pins[analog_input]);
      Serial.print(":");
      //Map analog from 0,1024 to 0,255.
      int mapped_analogread = -1;
      if (analog_input == 0) {
        mapped_analogread = map(analogRead(analog_pins[analog_input]), 0, 1024, 0, 180);
      } else {
        mapped_analogread = map(analogRead(analog_pins[analog_input]), 0, 1024, 0, 255);
      }
      //Serial.println(analogRead(analog_pins[analog_input]));
      Serial.println(mapped_analogread);
      analog_debounce[analog_input] = millis();
    }
  }
}

void buttons_read() {
  for (int button = 0; button < nb_buttons; button++) {
    bool button_state = !digitalRead(buttons_pins[button]); //Invert because pullup.


    // If the switch changed, due to noise or pressing:
    if (button_state != buttons_pressed[button]) {
      // reset the debouncing timer
      buttons_debounce[button] = millis();
    }

    if ((millis() - buttons_debounce[button]) > debounce_delay) {
      if (!button_state) { //If buttons is not pressed
        // If buttons wasn't pressed
        if (!buttons_pressed[button]) {
          buttons_pressed[button] = true;
          Serial.print(buttons_pins[button]);
          Serial.println(":0");
        }
      }

      if (button_state) { // If button is pressed
        // If buttons was already pressed
        if (buttons_pressed[button]) {
          buttons_pressed[button] = false;
          Serial.print(buttons_pins[button]);
          Serial.println(":1");
        }
      }
    }
  }
}
