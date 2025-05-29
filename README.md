# Proyecto-Final-SMC---Fleetguard
El repositorio se orgaiza d el asiguiente forma:
ARCHIVOS ARDUINO
  --> RUTA SIMPLE
      -> Baliza A
        - arduino_secrets.h : credenciales de red para Arduino Cloud
        - Iot_apr09a.ino : código ejecutado en Arduino MKR WIFI
      -> Baliza B
        - ArduinoMegaSeguridad.ino
      -> Baliza C:
        - mqtt_acelerometro.ino : código adaptado para trasmitir el valor de aceleración por MQTT
        - serial_acelerometro.ino : código adaptado para trasmitir valor de aceleración por serial RX TX
      -> Nodo Central:
        - 5_iot_thingsboard_ruta_json.py : código ejecutado en Raspberry Pi que guarda datos de las balizas en BBDD y se conecta a Thingsboard.
  --> RUTA DOBLE
      -> LoRa Trasmisora: 
        - remolqueLoraTrasmisora.ino : código ejecutado en LORA MKR 1310 trasmisora
      -> LoRa Receptora:
        - remolqueLoraReceptora.ino : código ejecutado en LORA MKR 1310 receptora con capacidad de guardar si se le conecta módulo para SD
        
