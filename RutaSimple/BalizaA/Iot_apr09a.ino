#include "thingProperties.h"
#include <Arduino_MKRENV.h>
#include <Servo.h>

// Variables
bool alarma = false;

const int outputPin = 6;         // LED/Pin que indica sistema ARMADO


Servo miServo;
const int servoPin = 1;

void setup() {
  Serial.begin(9600);
  delay(1500);

  // Inicializa propiedades de Arduino Cloud
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Pines de salida
  pinMode(outputPin, OUTPUT);


  // Inicializar servo
  miServo.attach(servoPin);
  miServo.write(0);  // Posición inicial
  setupWiFi();
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // Inicializar sensor ENV
  if (!ENV.begin()) {
    Serial.println("Error al iniciar el ENV Shield.");
    while (1);
  }
}

void loop() {
  ArduinoCloud.update();  // Sincroniza con la nube

  // Leer sensores
  float temp = ENV.readTemperature();
  float hum = ENV.readHumidity();
  Serial.println(WiFi.localIP());
  // Mostrar valores
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print(" ºC | Hum: ");
  Serial.print(hum);
  Serial.print(" % | Setpoint: ");
  Serial.print(setpoint);
  Serial.print(" ºC | Alarma: ");

  // Evaluar condición: solo activa la alarma si el sistema está armado
  if (status && temp > setpoint) {
    alarma = true;
    moverServo();
    Serial.println("ACTIVADA");
  } else {
    alarma = false;
    Serial.println("DESACTIVADA");
  }

  // Actualizar variables en la nube
  temperature = temp;
  humidity = hum;
  alarm = alarma;

  delay(3000);
}

// Callback cuando cambia el setpoint
void onSetpointChange() {
  Serial.print("Nuevo setpoint recibido: ");
  Serial.println(setpoint);
}

// Callback cuando cambia el estado (ARMADO/DESARMADO)
void onStatusChange() {
  Serial.print("Nuevo estado de 'status': ");
  Serial.println(status ? "ARMADO" : "DESARMADO");

  digitalWrite(outputPin, status ? HIGH : LOW);
}

// Función que mueve el servo de 0° a 180° y vuelve
void moverServo() {
  Serial.println("Moviendo servo...");

// un ciclo de 180
  for (int angulo = 0; angulo <= 180; angulo++) {
    miServo.write(angulo);
    delay(10);
  }

  for (int angulo = 180; angulo >= 0; angulo--) {
    miServo.write(angulo);
    delay(10);
  }

  Serial.println("Servo completó el movimiento.");
}
void setupWiFi() {
  delay(10);
  Serial.print("Conectando a WiFi...");
  WiFi.begin(SSID, PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" ¡Conectado!");
  Serial.print("IP asignada: ");
  Serial.println(WiFi.localIP());  
}
