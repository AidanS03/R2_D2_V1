#include <MatrixMath.h>
#include <Bluepad32.h>
#include "controller.h"
#include "functions.h"

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

#define IN1 33
#define IN2 25
#define IN3 27
#define IN4 32
#define ENABLE_B 26
#define ENABLE_A 14
#define BOARD_LED 2

// ***************************************************************************
// Timer setup
hw_timer_t * timer = NULL;

volatile bool timerFired = false;
float prevError, u, iTerm = 0;

void IRAM_ATTR onTimer(){
  GPIO.out ^= (1u << BOARD_LED);
  timerFired = true;
}

// ****************************************************************************
// Setup matricies 
mtx_type F[3][3] = {{1, 0.1, 0}, 
                    {0,  1,  0}, 
                    {0,  0,  1}};
mtx_type F_tran[3][3] = {{ 1,  0, 0}, 
                         {0.1, 1, 0}, 
                         { 0,  0, 1}};
mtx_type Q[3][3] = {{0.005,  0,   0   }, 
                    {  0,  0.02,  0   }, 
                    {  0,    0, 0.0001}};
mtx_type H[2][3] = {{1, 0, 0}, 
                    {-1, 0, 1}};
mtx_type H_tran[3][2] = {{1, -1}, 
                         {0, 0 }, 
                         {0, 1 }};
mtx_type R[2][2] = {{0.03, 0  }, 
                    {  0, 0.03}};
mtx_type R_inv[2][2] = {{33.3333,    0   },
                        {   0   , 33.3333}};
mtx_type P_plus[3][3] = {{10, 0,  0}, 
                         { 0, 10, 0}, 
                         { 0, 0, 10}};
mtx_type x_h_plus[3][1] = {{114},       // initial guess, robot at center
                           { 0 },       // of 2.28m (~7.5ft) wide hallway,
                           { 228 }};    // stationary at 0 m/s
mtx_type P_minus[3][3];
mtx_type K[3][2];
mtx_type x_h_minus[3][1];
 mtx_type Z[2][1];
mtx_type Test[2][3];

// **************************************************************************
// Arduino setup function. Runs in CPU 1
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Setup the Bluepad32 callbacks
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // "forgetBluetoothKeys()" should be called when the user performs
  // a "device factory reset", or similar.
  // Calling "forgetBluetoothKeys" in setup() just as an example.
  // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
  // But it might also fix some connection / re-connection issues.
  BP32.forgetBluetoothKeys();

  // Enables mouse / touchpad support for gamepads that support them.
  // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
  // - First one: the gamepad
  // - Second one, which is a "virtual device", is a mouse.
  // By default, it is disabled.
  BP32.enableVirtualDevice(false);

  // GPIO setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENABLE_A, OUTPUT);
  pinMode(ENABLE_B, OUTPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  Serial.println("LEDs setup.");

  pinMode(trig_R, OUTPUT);
  pinMode(echo_R, INPUT);
  pinMode(trig_L, OUTPUT);
  pinMode(echo_L, INPUT);  

  Serial.println("Ultrasonics setup.");

  pinMode(BOARD_LED, OUTPUT);
  digitalWrite(BOARD_LED, LOW);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100000, true);           // 100ms timer, auto reload true
  timerAlarmEnable(timer);
  Serial.println("Timer setup.");
}

// Arduino loop function. Runs in CPU 1.
void loop() {
  // This call fetches all the controllers' data.
  // Call this function in your main loop.
  bool fired = false;
  if (timerFired){
    timerFired = false;
    fired = true;
  }
  bool dataUpdated = BP32.update();
  if (dataUpdated)
    processControllers();

  if(autonomous && fired){
    float distR = getDist('R');
    float distL = getDist('L');
    Z[0][0] = distR;
    Z[1][0] = distL;

    mtx_type temp1[3][3];
    mtx_type temp2[3][3];
    mtx_type temp3[2][3];
    mtx_type temp4[2][2];
    mtx_type temp5[2][2];
    mtx_type temp6[3][2];
    mtx_type temp7[2][1];
    mtx_type temp8[2][1];
    mtx_type temp9[3][1];
    mtx_type temp10[3][2];
    mtx_type temp11[3][3];
    // P minus -----------------------------------------------------------------------
    Matrix.Multiply((mtx_type*)F, (mtx_type*)P_plus, 3, 3, 3, (mtx_type*)temp1); 
    Matrix.Print((mtx_type*)temp1, 3, 3, "F*P+");
    Matrix.Multiply((mtx_type*)temp1, (mtx_type*)F_tran, 3, 3, 3, (mtx_type*)temp2);
    Matrix.Print((mtx_type*)temp2, 3, 3, "(F*P+)*F'"); 
    Matrix.Add((mtx_type*)temp2, (mtx_type*)Q, 3, 3, (mtx_type*)P_minus);
    Matrix.Print((mtx_type*)P_minus, 3, 3, "P_minus");
  
    // K -----------------------------------------------------------------------------
    Matrix.Multiply((mtx_type*)H, (mtx_type*)P_minus, 2, 3, 3, (mtx_type*)temp3);
    Matrix.Print((mtx_type*)temp3, 2, 3, "H*P-");
    Matrix.Multiply((mtx_type*)temp3, (mtx_type*)H_tran, 2, 3, 2, (mtx_type*)temp4);
    Matrix.Print((mtx_type*)temp4, 2, 2, "(H*P-)*H'");
    Matrix.Add((mtx_type*)temp4, (mtx_type*)R, 2, 2, (mtx_type*)temp5);
    Matrix.Print((mtx_type*)temp5, 2, 2, "(H*P-*H')+R");
    Matrix.Invert((mtx_type*)temp5, 2);
    Matrix.Print((mtx_type*)temp5, 2, 2, "inv(H*P-*H'+R)");
    Matrix.Multiply((mtx_type*)P_minus, (mtx_type*)H_tran, 3, 3, 2, (mtx_type*)temp6);
    Matrix.Print((mtx_type*)temp6, 3, 2, "P-*H'");
    Matrix.Multiply((mtx_type*)temp6, (mtx_type*)temp5, 3, 2, 2, (mtx_type*)K);
    Matrix.Print((mtx_type*)K, 3, 2, "K");
  
    // x^- ----------------------------------------------------------------------------
    Matrix.Multiply((mtx_type*)F, (mtx_type*)x_h_plus, 3, 3, 1, (mtx_type*)x_h_minus);
    Matrix.Print((mtx_type*)x_h_minus, 3, 1, "x^-");
    
    // x^+ ----------------------------------------------------------------------------
    Matrix.Multiply((mtx_type*)H, (mtx_type*)x_h_minus, 2, 3, 1, (mtx_type*)temp7);
    Matrix.Print((mtx_type*)temp7, 2, 1, "H*x^-");
    Matrix.Subtract((mtx_type*)Z, (mtx_type*)temp7, 2, 1, (mtx_type*)temp8);
    Matrix.Print((mtx_type*)temp8, 2, 1, "Z-(H*x^-)");
    Matrix.Multiply((mtx_type*)K, (mtx_type*)temp8, 3, 2, 1, (mtx_type*)temp9);
    Matrix.Print((mtx_type*)temp9, 3, 1, "K*(Z-H*x^-)");
    Matrix.Add((mtx_type*)x_h_minus, (mtx_type*)temp9, 3, 1, (mtx_type*)x_h_plus);
    Matrix.Print((mtx_type*)x_h_plus, 3, 1, "x^+");
  
    // P plus -------------------------------------------------------------------------
    Matrix.Invert((mtx_type*)P_minus, 3);
    Matrix.Print((mtx_type*)P_minus, 3, 3, "inv(P-)");
    Matrix.Multiply((mtx_type*)H_tran, (mtx_type*)R_inv, 3, 2, 2, (mtx_type*)temp10);
    Matrix.Print((mtx_type*)temp10, 3, 2, "H'*inv(R)");
    Matrix.Multiply((mtx_type*)temp10, (mtx_type*)H, 3, 2, 3, (mtx_type*)temp11);
    Matrix.Print((mtx_type*)temp11, 3, 3, "(H'*inv(R))*H");
    Matrix.Add((mtx_type*)P_minus, (mtx_type*)temp11, 3, 3, (mtx_type*)P_plus);
    Matrix.Print((mtx_type*)P_plus, 3, 3, "inv(P-) + (H'*inv(R)*H)");
    Matrix.Invert((mtx_type*)P_plus, 3);
    Matrix.Print((mtx_type*)P_plus, 3, 3, "P+");

    float filteredR = x_h_plus[0][0];
    float filteredL = x_h_plus[2][0] - filteredR;
    float error = filteredR - filteredL;
    if(prevError * error < 0){
      iTerm = 0;
    }

    float pTerm = error;
    iTerm = 0.1 * (iTerm + error);
    float dTerm = (prevError - error) / 0.1;
    u = pGain * pTerm + iGain * iTerm + dGain * dTerm;
    prevError = error;
  

    processPID(u);
    Serial.print("PID out, u = ");
    Serial.println(u);
    Serial.print("Dist R: ");
    Serial.println(distR);
    Serial.print("Dist L: ");
    Serial.println(distL);

    // For EKF, variance for sensors is 0.03
  }
}
