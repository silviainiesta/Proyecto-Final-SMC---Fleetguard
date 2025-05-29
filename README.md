# Proyecto Final SMC – FleetGuard

Este repositorio contiene todo el código desarrollado para el proyecto final de la asignatura **Sistemas de Monitorización y Control (SMC)**. El sistema diseñado se basa en una plataforma IoT para el seguimiento de variables críticas en vehículos de transporte de mercancías, incluyendo temperatura, humedad, seguridad y aceleración.

##Estructura del Repositorio

### ARCHIVOS ARDUINO

#### ➤ Ruta Simple

##### Baliza A – Arduino MKR WiFi + ENV Shield
- `arduino_secrets.h` → Credenciales de red para la conexión con Arduino Cloud.
- `Iot_apr09a.ino` → Código principal que lee sensores y publica datos en Arduino Cloud.

##### Baliza B – Arduino Mega (Seguridad)
- `ArduinoMegaSeguridad.ino` → Código que gestiona sensores de seguridad: puertas, pánico, presencia y señal de estado.

##### Baliza C – Arduino Nano 33 IoT (Acelerómetro)
- `mqtt_acelerometro.ino` → Código que transmite el valor de aceleración por **MQTT**.
- `serial_acelerometro.ino` → Variante que transmite aceleración por **UART (TX/RX)**.

##### Nodo Central – Raspberry Pi
- `5_iot_thingsboard_ruta_json.py` → Script Python que recibe datos por serial, los guarda en la base de datos **MySQL**, y los publica en **ThingsBoard**.

---

#### Ruta Doble (Solución inalámbrica con LoRa)

##### LoRa Transmisora – Arduino MKR 1310
- `remolqueLoraTrasmisora.ino` → Código que simula sensores y envía datos encriptados vía LoRa.

##### LoRa Receptora – Arduino MKR 1310
- `remolqueLoraReceptora.ino` → Código que recibe datos LoRa, los descifra, combina con sensores locales y opcionalmente guarda en SD.

---

## Notas
- El sistema combina múltiples tecnologías: WiFi, LoRa, MQTT, Arduino Cloud, ThingsBoard y bases de datos MySQL.
- Todos los scripts y firmwares han sido probados en entorno real y están listos para su despliegue.

---

