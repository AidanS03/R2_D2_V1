#include "bluetooth.h"

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id, properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
      }
    }

    if (!foundEmptySlot) {
      Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

    if (!foundController) {
      Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// ========= SEE CONTROLLER VALUES IN SERIAL MONITOR ========= //

void dumpGamepad(ControllerPtr ctl) {
  Serial.printf(
  "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
  "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
  ctl->index(),        // Controller Index
  ctl->dpad(),         // D-pad
  ctl->buttons(),      // bitmask of pressed buttons
  ctl->axisX(),        // (-511 - 512) left X Axis
  ctl->axisY(),        // (-511 - 512) left Y axis
  ctl->axisRX(),       // (-511 - 512) right X axis
  ctl->axisRY(),       // (-511 - 512) right Y axis
  ctl->brake(),        // (0 - 1023): brake button
  ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
  ctl->miscButtons(),  // bitmask of pressed "misc" buttons
  ctl->gyroX(),        // Gyro X
  ctl->gyroY(),        // Gyro Y
  ctl->gyroZ(),        // Gyro Z
  ctl->accelX(),       // Accelerometer X
  ctl->accelY(),       // Accelerometer Y
  ctl->accelZ()        // Accelerometer Z
  );
}

// ========= GAME CONTROLLER ACTIONS SECTION ========= //

void processGamepad(ControllerPtr ctl) {
  // There are different ways to query whether a button is pressed.
  // By query each button individually:
  //  a(), b(), x(), y(), l1(), etc...

  // Serial.println("Proccessing Gamepad...");
 
  //== PS4 X button = 0x0001 ==//
  if ((ctl->buttons() & 0x1) == 0x1) {
    // code for when X button is pushed
    digitalWrite(BLUE_LED, HIGH);
    Serial.println("X pressed...");
  }else{
    // code for when X button is released
    digitalWrite(BLUE_LED, LOW);
  }

  //== PS4 Square button = 0x0004 ==//
  if ((ctl->buttons() & 0x4) == 0x4) {
    // code for when square button is pushed
  }else{
  // code for when square button is released
  }

  //== PS4 Triangle button = 0x0008 ==//
  if ((ctl->buttons() & 0x8) == 0x8) {
    // code for when triangle button is pushed
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("Triangle pressed...");
  }else{
    // code for when triangle button is released
    digitalWrite(GREEN_LED, LOW);
  }

  //== PS4 Circle button = 0x0002 ==//
  if ((ctl->buttons() & 0x2) == 0x2) {
    // code for when circle button is pushed
    digitalWrite(RED_LED, HIGH);
    Serial.println("Circle pressed...");
  }else{
    // code for when circle button is released
    digitalWrite(RED_LED, LOW);
  }

  if(ctl->dpad()){
    //== PS4 Dpad UP button = 0x01 ==//
    if ((ctl->dpad() & 0x1) == 0x1) {
      // code for when dpad up button is pushed
      multiLED(GREEN_LED, RED_LED, BLUE_LED);
      Serial.println("Dpad UP pressed...");
    }else if ((ctl->dpad() & 0x2) == 0x2) { //==PS4 Dpad DOWN button = 0x02==//
      // code for when dpad down button is pushed
      multiLED(BLUE_LED, RED_LED);
      Serial.println("Dpad DOWN pressed...");
    }else if ((ctl->dpad() & 0x8) == 0x8) { //== PS4 Dpad LEFT button = 0x08 ==//
      // code for when dpad left button is pushed
      multiLED(BLUE_LED, GREEN_LED);
      Serial.println("Dpad LEFT pressed...");
    }else if ((ctl->dpad() & 0x4) == 0x4) { //== PS4 Dpad RIGHT button = 0x04 ==//
      // code for when dpad right button is pushed
      multiLED(BLUE_LED);
      Serial.println("Dpad RIGHT pressed...");
    }else{
      // code for when dpad right button is released
      digitalWrite(GREEN_LED, LOW);
      digitalWrite(RED_LED, LOW);
      digitalWrite(BLUE_LED, LOW);
    }
  }
  

  //== PS4 R1 trigger button = 0x0020 ==//
  if ((ctl->buttons() & 0x20) == 0x20) {
    // code for when R1 button is pushed
  }
  if ((ctl->buttons() & 0x20) != 0x20) {
    // code for when R1 button is released
  }

  //== PS4 R2 trigger button = 0x0080 ==//
  if ((ctl->buttons() & 0x80) == 0x80) {
    // code for when R2 button is pushed
  }
  if ((ctl->buttons() & 0x80) != 0x80) {
    // code for when R2 button is released
  }

  //== PS4 L1 trigger button = 0x0010 ==//
  if ((ctl->buttons() & 0x10) == 0x10) {
    // code for when L1 button is pushed
  }
  if ((ctl->buttons() & 0x10) != 0x10) {
    // code for when L1 button is released
  }

  //== PS4 L2 trigger button = 0x0040 ==//
  if ((ctl->buttons() & 0x40) == 0x40) {
    // code for when L2 button is pushed
  }
  if ((ctl->buttons() & 0x40) != 0x40) {
    // code for when L2 button is released
  }

  //== LEFT JOYSTICK - UP ==//
  if (ctl->axisY() <= -25) {
    // code for when left joystick is pushed up
    }

  //== LEFT JOYSTICK - DOWN ==//
  if (ctl->axisY() >= 25) {
    // code for when left joystick is pushed down
  }

  //== LEFT JOYSTICK - LEFT ==//
  if (ctl->axisX() <= -25) {
    // code for when left joystick is pushed left
  }

  //== LEFT JOYSTICK - RIGHT ==//
  if (ctl->axisX() >= 25) {
    // code for when left joystick is pushed right
  }

  //== LEFT JOYSTICK DEADZONE ==//
  if (ctl->axisY() > -25 && ctl->axisY() < 25 && ctl->axisX() > -25 && ctl->axisX() < 25) {
    // code for when left joystick is at idle
  }

  //== RIGHT JOYSTICK - LEFT ==//
  if (ctl->axisRX() < -25) {
    // code for when right joystick moves along x-axis
  }

    //== RIGHT JOYSTICK - RIGHT ==//
  if (ctl->axisRX() > 25) {
    // code for when right joystick moves along x-axis
  }

  //== RIGHT JOYSTICK - UP ==//
  if (ctl->axisRY() < -25) {
  // code for when right joystick moves along y-axis
  }

    //== RIGHT JOYSTICK - DOWN ==//
  if (ctl->axisRY() > 25) {
  // code for when right joystick moves along y-axis
  }

  // dumpGamepad(ctl);
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
         processGamepad(myController);
      }
      else {
        Serial.println("Unsupported controller");
      }
    }
  }
}