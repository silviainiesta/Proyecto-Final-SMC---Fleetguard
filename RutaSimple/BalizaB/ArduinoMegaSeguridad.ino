enum EstadoSistema {DESARMADO, ARMADO};
EstadoSistema estado = DESARMADO;

const int PIN_PUERTA = 2;
const int PIN_PANICO = 3;
const int PIN_PRESENCIA = A0;
const int PIN_LED_SALIDA = 13;
const int PIN_SIRENA = 12;
const int PIN_ESTADO = 6;

const int PIN_LED_VERDE = 4;
const int PIN_LED_AZUL = 5;

volatile bool sensorPuerta = false;
volatile bool panicoActivado = false;

bool estadoSenalAnterior = LOW;
unsigned long ultimoCambio = 0;
const unsigned long reboteDelay = 200;

// NUEVAS VARIABLES PARA GUARDAR VALORES ACTUALES
bool valorPuertaActual = false;
bool valorPanicoActual = false;

void setup() {
  pinMode(PIN_PUERTA, INPUT_PULLUP);
  pinMode(PIN_PANICO, INPUT_PULLUP);
  pinMode(PIN_PRESENCIA, INPUT);
  pinMode(PIN_ESTADO, INPUT);
  pinMode(PIN_LED_SALIDA, OUTPUT);
  pinMode(PIN_SIRENA, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_LED_AZUL, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_PUERTA), ISR_puerta, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_PANICO), ISR_panico, FALLING);

  Serial.begin(9600);
}

void loop() {
  // LECTURA DE LA SEÑAL DE ESTADO
  bool estadoSenalActual = digitalRead(PIN_ESTADO);

  if (estadoSenalActual == HIGH && estado == DESARMADO) {
    estado = ARMADO;
    Serial.println("Estado cambiado a ARMADO por señal externa");
  } else if (estadoSenalActual == LOW && estado == ARMADO) {
    estado = DESARMADO;
    Serial.println("Estado cambiado a DESARMADO por señal externa");
  }

  // LEDS DE ESTADO
  digitalWrite(PIN_LED_VERDE, estado == DESARMADO ? HIGH : LOW);
  digitalWrite(PIN_LED_AZUL, estado == ARMADO ? HIGH : LOW);

  // SISTEMA ARMADO
  if (estado == ARMADO) {
    int valorPresencia = analogRead(PIN_PRESENCIA);
    bool presencia = (valorPresencia > 600);

    // Guardar valores en variables
    valorPuertaActual = sensorPuerta;
    valorPanicoActual = panicoActivado;

    Serial.print("Puerta="); Serial.print(valorPuertaActual);
    Serial.print("  Panico="); Serial.print(valorPanicoActual);
    Serial.print("  Presencia="); Serial.println(presencia);

    if (sensorPuerta || panicoActivado || presencia) {
      activarAlarma();
      enviarEvento(sensorPuerta, panicoActivado, presencia);
      sensorPuerta = false;
      panicoActivado = false;
    }
  } else {
    desactivarAlarma();
  }

  enviarValoresJSON();
  delay(500);
}

void ISR_puerta() {
  sensorPuerta = true;
}

void ISR_panico() {
  panicoActivado = true;
}

void activarAlarma() {
  digitalWrite(PIN_SIRENA, HIGH);
  delay(500);
  digitalWrite(PIN_SIRENA, LOW);
  delay(500);
  digitalWrite(PIN_LED_SALIDA, HIGH);
}

void desactivarAlarma() {
  digitalWrite(PIN_SIRENA, LOW);
  digitalWrite(PIN_LED_SALIDA, LOW);
}

void enviarEvento(bool puerta, bool panico, bool presencia) {
  if (puerta) Serial.println("ALERTA: SENSOR DE PUERTA ACTIVADO");
  if (panico) Serial.println("ALERTA: BOTON DE PANICO ACTIVADO");
  if (presencia) Serial.println("ALERTA: SENSOR DE PRESENCIA ACTIVADO");
}


void enviarValoresJSON() {
  Serial.print("{\"eventos\":[");
  Serial.print("{\"id\":1,\"valor\":"); Serial.print(valorPuertaActual); Serial.print("},");
  Serial.print("{\"id\":2,\"valor\":"); Serial.print(valorPanicoActual); Serial.println("}]}");
  Serial.print("{\"id\":3,\"valor\":"); Serial.print(presencia); Serial.println("}]}");
}
