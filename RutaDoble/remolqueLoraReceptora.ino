#include <SPI.h>
#include <LoRa.h>
#include <SD.h>

String clave = "iotgrupodsmc";  // Clave compartida para cifrado
const int SD_CS_PIN = 4;        // Pin CS de la tarjeta SD

unsigned long t0 = 0;

// Función para desencriptar mensaje (XOR)
String desencriptar(String cifrado, String clave) {
  String resultado = "";
  for (int i = 0; i < cifrado.length(); i++) {
    resultado += char(cifrado[i] ^ clave[i % clave.length()]);
  }
  return resultado;
}

// Simulación de sensores en remolque 1
String simularSensoresLocales() {
  int puerta = random(0, 2);
  int presencia = random(300, 900);
  float temperatura = random(50, 400) / 10.0;
  int panico = random(0, 2);

  String local = "\"remolque1\":{";
  local += "\"puerta\":" + String(puerta) + ",";
  local += "\"presencia\":" + String(presencia) + ",";
  local += "\"temperatura\":" + String(temperatura, 1) + ",";
  local += "\"panico\":" + String(panico);
  local += "}";

  return local;
}

// Función para guardar texto en SD
void guardarEnSD(String texto) {
  String nombreArchivo = "log.txt";
  File archivo = SD.open(nombreArchivo, FILE_WRITE);
  if (archivo) {
    archivo.println(texto);
    archivo.close();
    Serial.println("Guardado en SD.");
  } else {
    Serial.println("Error al abrir el archivo en SD.");
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(868E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Error al inicializar la tarjeta SD");
    while (1);
  }

  Serial.println("Baliza A (Receptora + Simulación)");
  Serial.println("Tarjeta SD lista.");
}

void loop() {
  // Recepción de datos LoRa
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String recibido = "";
    while (LoRa.available()) {
      recibido += (char)LoRa.read();
    }

    // Desencriptar
    String mensaje = desencriptar(recibido, clave);

    // Simulación local
    String local = simularSensoresLocales();

    // Unir datos remolque1 + remolque2
    String total = "{";
    total += local + ",";
    total += "\"remolque2\":" + mensaje;
    total += "}";

    Serial.println("Recibido y combinado:");
    Serial.println(total);
    guardarEnSD(total);  //Guardar mensaje en tarjeta SD
  }

  // Si no se recibe nada en 5 segundos, simular local
  if (millis() - t0 > 5000) {
    t0 = millis();
    String local = simularSensoresLocales();
    String soloLocal = "{" + local + "}";

    Serial.println(" Solo remolque1:");
    Serial.println(soloLocal);
    guardarEnSD(soloLocal);  //Guardar datos en tarjeta SD
  }
}
