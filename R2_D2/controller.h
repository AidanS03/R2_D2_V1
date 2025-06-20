#ifndef CONTROLLER_H
#define CONTROLLER_H

void onConnectedController(ControllerPtr ctl);
void onDisconnectedController(ControllerPtr ctl);
void dumpGamepad(ControllerPtr ctl);
void processGamepad(ControllerPtr ctl);
void processControllers();

#endif 