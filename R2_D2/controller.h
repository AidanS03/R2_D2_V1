#ifndef CONTROLLER_H
#define CONTROLLER_H

bool autonomous = false;
bool xPressed = false;


mtx_type x_hat_plus[3][1];

void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);
void dumpGamepad(ControllerPtr ctl);
void processGamepad(ControllerPtr ctl);
void processControllers();

#endif 
