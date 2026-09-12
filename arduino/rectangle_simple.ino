#include <Arduino.h>

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

const uint8_t SERVO_DOWN_PULSE = 140;
const uint8_t SERVO_UP_PULSE = 40;

const uint16_t SERVO_SETTLE_MS = 1000;
const uint16_t MOTOR_SETTLE_MS = 1500;
const uint16_t SERVO_TURN_SETTLE_MS = 500;

const float RECT_SIDE1_MM = 200.0f;
const float RECT_SIDE2_MM = 280.0f;

const uint16_t FORWARD_DURATION_S1 = (uint16_t)((RECT_SIDE1_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);
const uint16_t FORWARD_DURATION_S2 = (uint16_t)((RECT_SIDE2_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);

const float TURN_ANGLE_DEG = 90.0f;
const uint16_t TURN_DURATION_MS = (uint16_t)(TURN_ANGLE_DEG * 1.816667f);

const int8_t MOTOR_A_DIRECTION = 1;
const int8_t MOTOR_B_DIRECTION = -1;

void setServo(uint8_t angle) {
  uint16_t pulseWidth = 1000 + (angle * 11);
  digitalWrite(PIN_SERVO, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(PIN_SERVO, LOW);
}

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
  setServo(SERVO_UP_PULSE);
  delay(SERVO_TURN_SETTLE_MS);
  
  setMotorA(pwm);
  setMotorB(-pwm);
  delay(durationMs);
  
  stopMotors();
  delay(MOTOR_SETTLE_MS);
  
  setServo(SERVO_DOWN_PULSE);
  delay(SERVO_TURN_SETTLE_MS);
}

void lowerPen() {
  setServo(SERVO_DOWN_PULSE);
  delay(SERVO_SETTLE_MS);
}

void raisePen() {
  setServo(SERVO_UP_PULSE);
  delay(SERVO_SETTLE_MS);
}

void drawRectangle() {
  Serial.println("DESENHANDO RETANGULO");
  
  lowerPen();

  Serial.println("Lado 1 (curto)");
  driveForward(FORWARD_DURATION_S1, MOTOR_PWM_FORWARD);
  Serial.println("Giro 1");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  Serial.println("Lado 2 (comprido)");
  driveForward(FORWARD_DURATION_S2, MOTOR_PWM_FORWARD);
  Serial.println("Giro 2");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  Serial.println("Lado 3 (curto)");
  driveForward(FORWARD_DURATION_S1, MOTOR_PWM_FORWARD);
  Serial.println("Giro 3");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  Serial.println("Lado 4 (comprido)");
  driveForward(FORWARD_DURATION_S2, MOTOR_PWM_FORWARD);
  Serial.println("Giro 4");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
  
  Serial.println("RETANGULO CONCLUIDO");
}

void setup() {
  Serial.begin(9600);
  Serial.println("ROBO RETANGULO INICIANDO");
  
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  
  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);
  
  pinMode(PIN_STBY, OUTPUT);
  pinMode(PIN_SERVO, OUTPUT);
  digitalWrite(PIN_STBY, LOW);
  stopMotors();
  
  delay(500);
  
  Serial.println("Levantando caneta...");
  raisePen();
  
  delay(500);
  
  Serial.println("Habilitando motores. Iniciando desenho...");
  digitalWrite(PIN_STBY, HIGH);
  delay(200);
  
  drawRectangle();
  
  Serial.println("DESENHO CONCLUIDO");
}

void loop() {
  stopMotors();
  delay(1000);
}
