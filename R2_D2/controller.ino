#include "controller.h"

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
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
    ctl->index(),
    ctl->dpad(),
    ctl->buttons(),
    ctl->axisX(),
    ctl->axisY(),
    ctl->axisRX(),
    ctl->axisRY(),
    ctl->brake(),
    ctl->throttle(),
    ctl->miscButtons(),
    ctl->gyroX(),
    ctl->gyroY(),
    ctl->gyroZ(),
    ctl->accelX(),
    ctl->accelY(),
    ctl->accelZ()
  );
}

// ========= GAME CONTROLLER ACTIONS SECTION ========= //

void processGamepad(ControllerPtr ctl) {
  if(!autonomous){
    // Joystick values
    int x = ctl->axisX();
    int y = ctl->axisY();
  
    // Deadzone
    if (abs(x) < 50) x = 0;
    if (abs(y) < 50) y = 0;
  
    // Differential drive
    int leftSpeed = y + x;
    int rightSpeed = y - x;
  
    // Clip
    leftSpeed = constrain(leftSpeed, -512, 512);
    rightSpeed = constrain(rightSpeed, -512, 512);
  
    // Map to PWM
    int leftPwm = map(abs(leftSpeed), 0, 512, 0, 255);
    int rightPwm = map(abs(rightSpeed), 0, 512, 0, 255);
  
    // Set left motor direction
    if (leftSpeed >= 0) {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    } else {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }
  
    // Set right motor direction
    if (rightSpeed >= 0) {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    } else {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }
  
    // Set speeds
    analogWrite(ENABLE_A, leftPwm);
    analogWrite(ENABLE_B, rightPwm);

    Serial.printf("Left: %d Right: %d\n", leftSpeed, rightSpeed);
  }
  // Example button code (you can keep or remove)
  if ((ctl->buttons() & 0x1) == 0x1) {
    if(xPressed){
      
    }else {
      Serial.println("X pressed...");
      autonomous = !autonomous;
      if(autonomous){
          x_hat_plus[0][0] = getDist('R');
          x_hat_plus[1][0] = 0;
          x_hat_plus[2][0] = getDist('R') + getDist('L');
      }
      xPressed = true;
    }
  }else if(xPressed){
    xPressed = false;
  }

  if ((ctl->buttons() & 0x8) == 0x8) {
    Serial.println("Triangle pressed...");
  }

  if ((ctl->buttons() & 0x2) == 0x2) {
    Serial.println("Circle pressed...");
  }

  if (ctl->dpad()) {
    if ((ctl->dpad() & 0x1) == 0x1) {
      Serial.println("Dpad UP pressed...");
    } else if ((ctl->dpad() & 0x2) == 0x2) {
      Serial.println("Dpad DOWN pressed...");
    } else if ((ctl->dpad() & 0x8) == 0x8) {
      Serial.println("Dpad LEFT pressed...");
    } else if ((ctl->dpad() & 0x4) == 0x4) {
      Serial.println("Dpad RIGHT pressed...");
    }
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData()) {
      if (myController->isGamepad()) {
        processGamepad(myController);
      } else {
        Serial.println("Unsupported controller");
      }
    }
  }
}
