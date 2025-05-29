#include <Arduino_LSM6DS3.h>

void setup() {
  Serial.begin(9600);      // USB (debug)
  Serial1.begin(9600);     // UART TX/RX físico

  delay(1500);

  if (!IMU.begin()) {
    Serial.println("No se detecta acelerómetro LSM6DS3.");
    while (1);
  }

  Serial.println("IMU listo.");
}

void loop() {
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);  // Se deben pasar los tres ejes

    // Crear JSON solo con eje X
    String payload = "{";
    payload += "\"4\":" + String(x, 3);
    payload += "}";

    // Enviar por TX (Serial1)
    Serial1.println(payload);

    // También imprimir por USB para depurar
    Serial.println("Enviando: " + payload);
  }

  delay(2000);
}
