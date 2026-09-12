#include <Arduino.h>
#include <Servo.h>

const uint8_t PIN_AIN1 = 2;
const uint8_t PIN_AIN2 = 4;
const uint8_t PIN_PWMA = 5;

const uint8_t PIN_BIN1 = 7;
const uint8_t PIN_BIN2 = 8;
const uint8_t PIN_PWMB = 6;

const uint8_t PIN_STBY = 12;
const uint8_t PIN_SERVO = 9;

const float ROBOT_SPEED_MM_PER_S = 654.0f;
const uint8_t MOTOR_PWM_FORWARD = 180;
const uint8_t MOTOR_PWM_TURN = 180;

const uint8_t SERVO_DOWN_ANGLE = 95;
const uint8_t SERVO_UP_ANGLE = 35;

const uint16_t SERVO_SETTLE_MS = 1000;
const uint16_t MOTOR_SETTLE_MS = 1500;
const uint16_t SERVO_TURN_SETTLE_MS = 500;

const uint8_t NUM_SIDES = 5;
const float SIDE_LENGTH_MM = 180.0f;
const float TURN_ANGLE_DEG = 72.0f;

const uint16_t FORWARD_DURATION_MS = (uint16_t)((SIDE_LENGTH_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);
const uint16_t TURN_DURATION_MS = (uint16_t)(TURN_ANGLE_DEG * 1.816667f);

const int8_t MOTOR_A_DIRECTION = 1;
const int8_t MOTOR_B_DIRECTION = -1;

Servo penServo;

void setMotorA(int16_t speed) {
  speed *= MOTOR_A_DIRECTION;
  speed = constrain(speed, -150, 150);
  
  if (speed > 0) {
    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, speed);
  } else if (speed < 0) {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, HIGH);
    analogWrite(PIN_PWMA, -speed);
  } else {
    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
    analogWrite(PIN_PWMA, 0);
  }
}

void setMotorB(int16_t speed) {
  speed *= MOTOR_B_DIRECTION;
  speed = constrain(speed, -150, 150);
  
  if (speed > 0) {
    digitalWrite(PIN_BIN1, HIGH);
    digitalWrite(PIN_BIN2, LOW);
    analogWrite(PIN_PWMB, speed);
  } else if (speed < 0) {
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, HIGH);
    analogWrite(PIN_PWMB, -speed);
  } else {
    digitalWrite(PIN_BIN1, LOW);
    digitalWrite(PIN_BIN2, LOW);
    analogWrite(PIN_PWMB, 0);
  }
}

void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}

void driveForward(uint16_t durationMs, uint8_t pwm) {
  setMotorA(pwm);
  setMotorB(pwm);
  delay(durationMs);
  stopMotors();
  delay(MOTOR_SETTLE_MS);
}

void turnRight(uint16_t durationMs, uint8_t pwm) {
  penServo.write(SERVO_UP_ANGLE);
  delay(SERVO_TURN_SETTLE_MS);
  
  setMotorA(pwm);
  setMotorB(-pwm);
  delay(durationMs);
  
  stopMotors();
  delay(MOTOR_SETTLE_MS);
  
  penServo.write(SERVO_DOWN_ANGLE);
  delay(SERVO_TURN_SETTLE_MS);
}

void lowerPen() {
  penServo.write(SERVO_DOWN_ANGLE);
  delay(SERVO_SETTLE_MS);
}

void raisePen() {
  penServo.write(SERVO_UP_ANGLE);
  delay(SERVO_SETTLE_MS);
}

void drawShapeOnce() {
  lowerPen();
  
  for (uint8_t i = 0; i < NUM_SIDES; i++) {
    Serial.print("Lado ");
    Serial.println(i + 1);
    
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    
    Serial.print("Giro ");
    Serial.println(i + 1);
    
    turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);
  }
  
  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println("ROBO PENTAGONO INICIANDO");
  
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, LOW);
  stopMotors();
  
  penServo.attach(PIN_SERVO, 1000, 2000);
  delay(500);
  
  Serial.println("Servo anexado. Levantando caneta...");
  raisePen();
  
  delay(500);
  
  Serial.println("Habilitando motores. Iniciando desenho...");
  digitalWrite(PIN_STBY, HIGH);
  delay(200);
  
  drawShapeOnce();
  
  Serial.println("DESENHO CONCLUIDO");
}

void loop() {
  stopMotors();
  delay(1000);
}
