#include <Arduino.h>
#include <Servo.h>
#include <SoftwareSerial.h>

// =====================================================
// ROBÔ COM MENU BLUETOOTH
// =====================================================
// Recebe comandos via Bluetooth para desenhar formas
// HC-06: RX no pino 10, TX no pino 11
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
// PINOS DO BLUETOOTH
// =====================================================
// RX no pino 10, TX no pino 11 (SoftwareSerial)
SoftwareSerial BTSerial(10, 11);  // RX, TX

// =====================================================
// VELOCIDADE E GIRO CALIBRADOS
// =====================================================

const float ROBOT_SPEED_MM_PER_S = 654.0f;

const uint8_t MOTOR_PWM_FORWARD = 150;
const uint8_t MOTOR_PWM_TURN = 150;

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
// DESENHO: TRIÂNGULO
// =====================================================

void drawTriangle() {
  BTSerial.println("Desenhando TRIANGULO...");
  Serial.println("Desenhando TRIANGULO...");

  lowerPen();

  const uint16_t FORWARD_DURATION_MS = 383;  // 250mm
  const uint16_t TURN_DURATION_MS = 218;     // 120°

  for (uint8_t i = 0; i < 3; i++) {
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);
  }

  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
  
  BTSerial.println("TRIANGULO CONCLUIDO!");
  Serial.println("TRIANGULO CONCLUIDO!");
}

// =====================================================
// DESENHO: QUADRADO
// =====================================================

void drawSquare() {
  BTSerial.println("Desenhando QUADRADO...");
  Serial.println("Desenhando QUADRADO...");

  lowerPen();

  const uint16_t FORWARD_DURATION_MS = 306;  // 200mm
  const uint16_t TURN_DURATION_MS = 143;     // 90°

  for (uint8_t i = 0; i < 4; i++) {
    driveForward(FORWARD_DURATION_MS, MOTOR_PWM_FORWARD);
    turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);
  }

  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
  
  BTSerial.println("QUADRADO CONCLUIDO!");
  Serial.println("QUADRADO CONCLUIDO!");
}

// =====================================================
// DESENHO: RETÂNGULO
// =====================================================

void drawRectangle() {
  BTSerial.println("Desenhando RETANGULO...");
  Serial.println("Desenhando RETANGULO...");

  lowerPen();

  const float SIDE_LENGTHS_MM[4] = {150.0f, 250.0f, 150.0f, 250.0f};
  const uint16_t TURN_DURATION_MS = 138;  // 90°

  for (uint8_t i = 0; i < 4; i++) {
    uint16_t forwardDuration = (uint16_t)((SIDE_LENGTHS_MM[i] / ROBOT_SPEED_MM_PER_S) * 1000.0f);
    driveForward(forwardDuration, MOTOR_PWM_FORWARD);
    turnRight(TURN_DURATION_MS, MOTOR_PWM_TURN);
  }

  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
  
  BTSerial.println("RETANGULO CONCLUIDO!");
  Serial.println("RETANGULO CONCLUIDO!");
}

// =====================================================
// DESENHO: CÍRCULO
// =====================================================

void drawCircle() {
  BTSerial.println("Desenhando CIRCULO...");
  Serial.println("Desenhando CIRCULO...");

  lowerPen();

  penServo.write(SERVO_UP_ANGLE);
  delay(SERVO_TURN_SETTLE_MS);

  setMotorA(0);  // MOTOR A TRAVADO
  setMotorB(-MOTOR_PWM_TURN);

  delay(2500);  // 2.5 segundos

  stopMotors();
  delay(MOTOR_SETTLE_MS);

  penServo.write(SERVO_DOWN_ANGLE);
  delay(SERVO_TURN_SETTLE_MS);

  raisePen();
  stopMotors();
  digitalWrite(PIN_STBY, LOW);
  
  BTSerial.println("CIRCULO CONCLUIDO!");
  Serial.println("CIRCULO CONCLUIDO!");
}

// =====================================================
// PROCESSA COMANDO
// =====================================================

void processCommand(String command) {
  command.toUpperCase();
  
  if (command == "TRIANGULO") {
    drawTriangle();
  } 
  else if (command == "QUADRADO") {
    drawSquare();
  } 
  else if (command == "RETANGULO") {
    drawRectangle();
  } 
  else if (command == "CIRCULO") {
    drawCircle();
  } 
  else if (command == "MENU") {
    showMenu();
  }
  else {
    BTSerial.println("Comando invalido!");
    BTSerial.println("Digite: TRIANGULO, QUADRADO, RETANGULO, CIRCULO ou MENU");
    Serial.println("Comando invalido!");
  }
}

// =====================================================
// MENU
// =====================================================

void showMenu() {
  BTSerial.println("\n========== MENU ROBO ==========");
  BTSerial.println("Digite uma das opcoes:");
  BTSerial.println("  TRIANGULO");
  BTSerial.println("  QUADRADO");
  BTSerial.println("  RETANGULO");
  BTSerial.println("  CIRCULO");
  BTSerial.println("  MENU");
  BTSerial.println("===============================\n");
  
  Serial.println("\n========== MENU ROBO ==========");
  Serial.println("Digite uma das opcoes:");
  Serial.println("  TRIANGULO");
  Serial.println("  QUADRADO");
  Serial.println("  RETANGULO");
  Serial.println("  CIRCULO");
  Serial.println("  MENU");
  Serial.println("===============================\n");
}

// =====================================================
// CONFIGURAÇÃO INICIAL
// =====================================================

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);  // HC-06 baud rate

  Serial.println("=== ROBO COM BLUETOOTH INICIANDO ===");

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

  digitalWrite(PIN_STBY, HIGH);
  delay(200);

  showMenu();
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  // Lê dados do Bluetooth
  if (BTSerial.available()) {
    String command = "";
    
    while (BTSerial.available()) {
      char ch = BTSerial.read();
      if (ch != '\n' && ch != '\r') {
        command += ch;
      }
      delay(5);
    }

    if (command.length() > 0) {
      Serial.print("Comando recebido: ");
      Serial.println(command);
      processCommand(command);
      delay(1000);
      showMenu();
    }
  }

  delay(100);
}
