#include <SPI.h>
#include <LoRa.h>

String clave = "iotgrupodsmc";  // Clave compartida
float bateriaLocal = 100.0;     // Batería del remolque 1

// Función para descifrar (XOR)
String desencriptar(String cifrado, String clave) {
  String resultado = "";
  for (int i = 0; i < cifrado.length(); i++) {
    resultado += char(cifrado[i] ^ clave[i % clave.length()]);
  }
  return resultado;
}

// Simular sensores locales (remolque 1)
String simularSensoresLocales() {
  int puerta = random(0, 2);
  int presencia = random(300, 900);
  float temperatura = random(50, 400) / 10.0;
  int panico = random(0, 2);

  // Simular batería
  bateriaLocal -= random(1, 4) / 100.0;
  if (bateriaLocal < 10.0) bateriaLocal = 10.0;

  // Construir JSON del remolque 1
  String local = "\"remolque1\":{";
  local += "\"puerta\":" + String(puerta) + ",";
  local += "\"presencia\":" + String(presencia) + ",";
  local += "\"temperatura\":" + String(temperatura, 1) + ",";
  local += "\"panico\":" + String(panico) + ",";
  local += "\"bateria\":" + String(bateriaLocal, 1);
  local += "}";

  return local;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(868E6)) {
    Serial.println("Error LoRa");
    while (1);
  }

  Serial.println("Baliza A (Receptora + Simulación)");
}

void loop() {
  // Receptor LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String recibido = "";
    while (LoRa.available()) {
      recibido += (char)LoRa.read();
    }

    // Desencriptar
    String mensaje = desencriptar(recibido, clave);

    // Simular sensores locales
    String local = simularSensoresLocales();

    // Combinar ambos remolques
    String total = "{";
    total += local + ",";
    total += "\"remolque2\":" + mensaje;
    total += "}";

    Serial.println("Recibido y combinado:");
    Serial.println(total);
  }
}
