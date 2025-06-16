# R2-D2 Version 1

**DISCLAIMER!!! This project is not currently in a working state**

This is a project I hope to work on long term. For now I am starting off with a very simple differential drive robot controlled by an ESP32 and PS4 controller. I am using an L298N dual h-bridge for control of 2 DC motors. I will also be using a few sg90 servos for control of its legs and head. 

I will do my best to put any and all information on how I completed this project in this repository incase someone else would like make this. 

## Connecting the ESP and PS4 controller

To connect my PS4 controller to the ESP I am using the Bluepad32. I used the following tutorial to set it up and run an example.

https://racheldebarros.com/connect-your-game-controller-to-an-esp32/

The tutorial walked me through every step but left out a few small details that I will include here. 
1. To get a PS4 controller into pairing mode you start with the controller off, then hold the share button, and start holding the PS button a second later.
2. After pairing once you will not need to pair again however, the ps4 contoller will turn off when the ESP is reprogrammed (there were a few times i forgot to turn the controller back on but that may ust be me being stupid lol)
3. The processGamepad() function in the example does work but it has a few issues that I fixed in mine
   - you cannot press multiple buttons at the same time
   - when pressing one button on the Dpad it would get immediatly overwritten by the other Dpad buttons not being pressed
