#include "HID-Project.h"

const int pinLight = 3;
const int pinChest = 4;
const int pinVol = A2;

void setup() {
  pinMode(pinLight, INPUT);
  //Le pullup permet de ne pas utiliser un cable vers le 5V.
  pinMode(pinChest, INPUT_PULLUP);

  // Initialise la manette
  Gamepad.begin();
}

void loop() {

  //Si l'interrupteur est allumé
  if (digitalRead(pinLight)) {
    Gamepad.press(4);
  } else {
  //Si l'interrupteur est éteint
    Gamepad.release(4);
  }

  //Attention ! Le pull-up inverse la logique
  //On met un ! avant pour gérer cela
  //Si le coffre est fermé
  if (!digitalRead(pinChest)) {
    Gamepad.press(3);
  } else {
  //Si le coffre est ouvert
    Gamepad.release(3);
  }

  //Le joystick va de -32768 à 32768
  Gamepad.yAxis(map(analogRead(pinVol), 0, 1024, 0, 32768));
  //Gamepad.xAxis(map(analogRead(pinVol), 0, 1024, 0, 32768));

  // Functions above only set the values.
  // This writes the report to the host.
  Gamepad.write();
}
