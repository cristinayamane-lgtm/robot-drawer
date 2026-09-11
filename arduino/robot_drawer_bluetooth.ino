// =====================================================
// ROBÔ DRAWER - CONTROLE VIA BLUETOOTH
// =====================================================
// Recebe comandos do app iOS via HC-06
// Desenha: Quadrado, Triângulo, Pentágono, Retângulo
// =====================================================

#include <Arduino.h>
#include <Servo.h>

// =====================================================
// PINOS DO TB6612FNG
// =====================================================

const uint8_t PIN_AIN1 = 2;
const uint8_t PIN_AIN2 = 4;
const uint8_t PIN_PWMA = 5;

const uint8_t PIN_BIN1 = 7;
const uint8_t PIN_BIN2 = 8;
const uint8_t PIN_PWMB = 6;

const uint8_t PIN_STBY = 12;
const uint8_t PIN_SERVO = 9;

// =====================================================
// CONFIGURAÇÕES GERAIS
// =====================================================

const float ROBOT_SPEED_MM_PER_S = 654.0f;

const uint8_t MOTOR_PWM_FORWARD = 180;
const uint8_t MOTOR_PWM_TURN = 180;

const uint8_t SERVO_DOWN_ANGLE = 95;
const uint8_t SERVO_UP_ANGLE = 35;

const uint16_t SERVO_SETTLE_MS = 1000;
const uint16_t MOTOR_SETTLE_MS = 1500;
const uint16_t SERVO_TURN_SETTLE_MS = 500;

// =====================================================
// DIREÇÃO DOS MOTORES
// =====================================================

const int8_t MOTOR_A_DIRECTION = 1;
const int8_t MOTOR_B_DIRECTION = -1;

Servo penServo;

// =====================================================
// BLUETOOTH COMMANDS
// =====================================================

#define CMD_DRAW_SQUARE 0x01
#define CMD_DRAW_RECTANGLE 0x02
#define CMD_DRAW_TRIANGLE 0x03
#define CMD_DRAW_PENTAGON 0x04
#define CMD_STOP 0x05
#define CMD_RESET 0x09

// =====================================================
// MOTOR A
// =====================================================

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

// =====================================================
// MOTOR B
// =====================================================

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

// =====================================================
// PARADA DOS MOTORES
// =====================================================

void stopMotors() {
  setMotorA(0);
  setMotorB(0);
}

// =====================================================
// MOVIMENTO PARA FRENTE
// =====================================================

void driveForward(uint16_t durationMs, uint8_t pwm) {
  setMotorA(pwm);
  setMotorB(pwm);

  delay(durationMs);

  stopMotors();
  delay(MOTOR_SETTLE_MS);
}

// =====================================================
// GIRO
// =====================================================

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

// =====================================================
// CONTROLE DA CANETA
// =====================================================

void lowerPen() {
  penServo.write(SERVO_DOWN_ANGLE);
  delay(SERVO_SETTLE_MS);
}

void raisePen() {
  penServo.write(SERVO_UP_ANGLE);
  delay(SERVO_SETTLE_MS);
}

// =====================================================
// DESENHO: QUADRADO (200mm)
// =====================================================

void drawSquare() {
  Serial.println(">>> Drawing SQUARE");
  
  const uint8_t NUM_SIDES = 4;
  const float SIDE_LENGTH_MM = 200.0f;
  const float TURN_ANGLE_DEG = 90.0f;
  
  const uint16_t FORWARD_DURATION_MS =
      (uint16_t)((SIDE_LENGTH_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);
  const uint16_t TURN_DURATION_MS =
      (uint16_t)(TURN_ANGLE_DEG * 1.816667f);

  lowerPen();

  for (uint8_t i = 0; i < NUM_SIDES; i++) {
    Serial.print("  Side ");
    Serial.println(i + 1);
    
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);
  }

  raisePen();
  Serial.println("<<< SQUARE complete");
}

// =====================================================
// DESENHO: TRIÂNGULO (250mm)
// =====================================================

void drawTriangle() {
  Serial.println(">>> Drawing TRIANGLE");
  
  const uint8_t NUM_SIDES = 3;
  const float SIDE_LENGTH_MM = 250.0f;
  
  const uint16_t FORWARD_DURATION_MS =
      (uint16_t)((SIDE_LENGTH_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);

  const uint16_t TURN_DURATIONS[3] = {220, 280, 220};  // Giros calibrados

  lowerPen();

  for (uint8_t i = 0; i < NUM_SIDES; i++) {
    Serial.print("  Side ");
    Serial.println(i + 1);
    
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    
    Serial.print("  Turn ");
    Serial.println(i + 1);
    turnRight(TURN_DURATIONS[i], MOTOR_PWM_TURN);
  }

  raisePen();
  Serial.println("<<< TRIANGLE complete");
}

// =====================================================
// DESENHO: PENTÁGONO (200mm)
// =====================================================

void drawPentagon() {
  Serial.println(">>> Drawing PENTAGON");
  
  const uint8_t NUM_SIDES = 5;
  const float SIDE_LENGTH_MM = 200.0f;
  
  const uint16_t FORWARD_DURATION_MS =
      (uint16_t)((SIDE_LENGTH_MM / ROBOT_SPEED_MM_PER_S) * 1000.0f);

  const uint16_t TURN_DURATIONS[5] = {131, 160, 160, 131, 131};  // Giros calibrados

  lowerPen();

  for (uint8_t i = 0; i < NUM_SIDES; i++) {
    Serial.print("  Side ");
    Serial.println(i + 1);
    
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    
    Serial.print("  Turn ");
    Serial.println(i + 1);
    turnRight(TURN_DURATIONS[i], MOTOR_PWM_TURN);
  }

  raisePen();
  Serial.println("<<< PENTAGON complete");
}

// =====================================================
// DESENHO: RETÂNGULO (customizado)
// =====================================================

void drawRectangle(uint8_t length, uint8_t width) {
  Serial.print(">>> Drawing RECTANGLE ");
  Serial.print(length);
  Serial.print("cm x ");
  Serial.print(width);
  Serial.println("cm");
  
  // Converte para mm
  float length_mm = length * 10.0f;
  float width_mm = width * 10.0f;
  
  const uint16_t FORWARD_DURATION_L =
      (uint16_t)((length_mm / ROBOT_SPEED_MM_PER_S) * 1000.0f);
  const uint16_t FORWARD_DURATION_W =
      (uint16_t)((width_mm / ROBOT_SPEED_MM_PER_S) * 1000.0f);
  
  const uint16_t TURN_DURATION_MS = 400;  // 90°

  lowerPen();

  // Lado 1 (comprimento)
  Serial.println("  Side 1 (length)");
  driveForward(FORWARD_DURATION_L, MOTOR_PWM_FORWARD);
  Serial.println("  Turn 1");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  // Lado 2 (largura)
  Serial.println("  Side 2 (width)");
  driveForward(FORWARD_DURATION_W, MOTOR_PWM_FORWARD);
  Serial.println("  Turn 2");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  // Lado 3 (comprimento)
  Serial.println("  Side 3 (length)");
  driveForward(FORWARD_DURATION_L, MOTOR_PWM_FORWARD);
  Serial.println("  Turn 3");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  // Lado 4 (largura)
  Serial.println("  Side 4 (width)");
  driveForward(FORWARD_DURATION_W, MOTOR_PWM_FORWARD);
  Serial.println("  Turn 4");
  turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);

  raisePen();
  Serial.println("<<< RECTANGLE complete");
}

// =====================================================
// TRATAMENTO DE COMANDOS
// =====================================================

void handleCommand(uint8_t cmd) {
  Serial.print("Command received: 0x");
  Serial.println(cmd, HEX);
  
  switch (cmd) {
    case CMD_DRAW_SQUARE:
      drawSquare();
      break;
      
    case CMD_DRAW_TRIANGLE:
      drawTriangle();
      break;
      
    case CMD_DRAW_PENTAGON:
      drawPentagon();
      break;
      
    case CMD_DRAW_RECTANGLE: {
      // Aguarda 2 bytes: comprimento e largura
      unsigned long timeout = millis() + 1000;
      while (Serial.available() < 2 && millis() < timeout) {
        delay(10);
      }
      if (Serial.available() >= 2) {
        uint8_t length = Serial.read();
        uint8_t width = Serial.read();
        drawRectangle(length, width);
      } else {
        Serial.println("ERROR: Rectangle parameters not received");
      }
      break;
    }
      
    case CMD_STOP:
      Serial.println("STOP command");
      stopMotors();
      raisePen();
      break;
      
    case CMD_RESET:
      Serial.println("RESET command");
      stopMotors();
      raisePen();
      digitalWrite(PIN_STBY, LOW);
      delay(500);
      digitalWrite(PIN_STBY, HIGH);
      break;
      
    default:
      Serial.print("Unknown command: 0x");
      Serial.println(cmd, HEX);
      break;
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(9600);
  
  Serial.println("\n\n");
  Serial.println("================================");
  Serial.println("ROBOT DRAWER - BLUETOOTH CONTROL");
  Serial.println("================================");

  // Configura pinos dos motores
  pinMode(PIN_AIN1, OUTPUT);
  pinMode(PIN_AIN2, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);

  pinMode(PIN_BIN1, OUTPUT);
  pinMode(PIN_BIN2, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);

  // Desabilita motores inicialmente
  pinMode(PIN_STBY, OUTPUT);
  digitalWrite(PIN_STBY, LOW);
  stopMotors();

  // Inicializa servo
  penServo.attach(PIN_SERVO, 1000, 2000);
  delay(500);
  
  Serial.println("Servo attached. Raising pen...");
  raisePen();
  
  delay(500);

  // Habilita motores
  Serial.println("Motors enabled. Waiting for commands...");
  digitalWrite(PIN_STBY, HIGH);
  delay(200);
  
  Serial.println("Ready for Bluetooth commands!");
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  // Aguarda comandos via Bluetooth (Serial)
  if (Serial.available()) {
    uint8_t cmd = Serial.read();
    handleCommand(cmd);
  }
  
  delay(10);
}
