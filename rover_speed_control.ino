#include <Servo.h>
#include <SerialCommand.h>

#define MOTOR_DIRECTION_FORWARD 1
#define MOTOR_DIRECTION_BACKWARD 0
#define PIN_LEFT_MOTOR_SPEED 6
#define PIN_RIGHT_MOTOR_SPEED 5
#define PIN_LEFT_MOTOR_DIRECTION 8
#define PIN_RIGHT_MOTOR_DIRECTION 7
#define MOTOR_MAX 255
#define PIN_ARM_SERVO 4
#define PIN_LED_GREEN 13
#define ARM_UP_POSITION 140
#define ARM_DOWN_POSITION 90
#define ANGLE_MAX 90
#define ANGLE_MIN -90

enum motor{
  MOTOR_LEFT,
  MOTOR_RIGHT
};

int serialSteerAngle = 0;
int serialSpeedAngle = 0;
bool turning = false;

Servo arm;
int arm_pos;

SerialCommand sCmd;

void serialNotFound(){
  Serial.println("Unrecognized cmd");
}

void setSpeedAnalog(motor motor, int aval, int direction){
  if(MOTOR_LEFT == motor){
    analogWrite (PIN_LEFT_MOTOR_SPEED, aval);
    digitalWrite(PIN_LEFT_MOTOR_DIRECTION, direction);
  }
  else if (MOTOR_RIGHT == motor){
    analogWrite (PIN_RIGHT_MOTOR_SPEED, aval);
    digitalWrite(PIN_RIGHT_MOTOR_DIRECTION, direction);
  }
}

void setSpeedAngle(motor motor, int angle){
  angle = constrain(angle, ANGLE_MIN, ANGLE_MAX);
  int speed;
  int direction;
  if(angle >= 0){
    direction = MOTOR_DIRECTION_FORWARD;
    speed = map(angle, 0, ANGLE_MAX, 0, MOTOR_MAX);
  }
  else{
    direction = MOTOR_DIRECTION_BACKWARD;
    speed = map(angle, 0, ANGLE_MIN, 0, MOTOR_MAX);
  }
  setSpeedAnalog(motor, speed, direction);
}

void setSteerAngle(int angle){
  angle = constrain(angle, ANGLE_MIN, ANGLE_MAX);
  setSpeedAngle(MOTOR_LEFT, serialSpeedAngle + angle);
  setSpeedAngle(MOTOR_RIGHT, serialSpeedAngle - angle);
}

void serialSetSpeedAngle(){
  Serial.print("Serial set speed angle: ");
  int angle;
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    angle = atoi(arg);
    Serial.println(angle);
    serialSpeedAngle = angle;
    setSpeedAngle(MOTOR_LEFT, angle);
    setSpeedAngle(MOTOR_RIGHT, angle);
  }
  else{
    Serial.println("no value set");
  }
}

void serialSetSteerAngle(){
  Serial.print("Serial set steer angle: ");
  int angle;
  char *arg;
  arg = sCmd.next();
  if (arg != NULL) {
    angle = atoi(arg);
    Serial.println(angle);
    serialSteerAngle = angle;
    setSteerAngle(angle);
  }
  else{
    Serial.println("no value set");
  }
}

void serialStop(){
  setSteerAngle(0);
  setSpeedAngle(MOTOR_LEFT, 0);
  setSpeedAngle(MOTOR_RIGHT, 0);
}

void addSerialCommands(){
  sCmd.addCommand("speed", serialSetSpeedAngle);
  sCmd.addCommand("steer", serialSetSteerAngle);
  sCmd.addCommand("stop", serialStop);
  sCmd.addDefaultHandler(serialNotFound);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  int i;
  for(i=5;i<=8;i++)
    pinMode(i, OUTPUT);
    
  addSerialCommands();
}

void loop() {
  sCmd.readSerial();
}
