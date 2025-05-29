#!/usr/bin/env python
import serial
import mysql.connector
import paho.mqtt.client as mqtt
import json
import time
import random
import math
import threading

# MQTT Configuración
THINGSBOARD_HOST = 'arduo.es'
ACCESS_TOKEN = 'iFasc2WCmsS906FFYiEv'

# Serial Arduino
arduino = serial.Serial('/dev/ttyACM2', 9600)

# MQTT
client = mqtt.Client()
client.username_pw_set(ACCESS_TOKEN)
client.connect(THINGSBOARD_HOST, 1883, 60)
client.loop_start()

# Punto inicial para coordenadas GPS (ej: Madrid)
lat_actual = 40.4168
lng_actual = -3.7038

def generar_punto_cercano(lat, lng, max_dist_m=100):
    R = 6371000
    distancia_rad = max_dist_m / R
    angulo = random.uniform(0, 2 * math.pi)
    dlat = distancia_rad * math.cos(angulo)
    dlng = distancia_rad * math.sin(angulo) / math.cos(math.radians(lat))
    nueva_lat = lat + math.degrees(dlat)
    nueva_lng = lng + math.degrees(dlng)
    return {'lat': nueva_lat, 'lng': nueva_lng}

# Hilo para generar y enviar coordenadas GPS
def enviar_gps():
    global lat_actual, lng_actual
    while True:
        punto = generar_punto_cercano(lat_actual, lng_actual)
        lat_actual = punto['lat']
        lng_actual = punto['lng']
        print("GPS:", punto)

        # Envío a ThingsBoard
        client.publish('v1/devices/me/telemetry', json.dumps(punto), 1)

        # Envío a MySQL
        send_mysql('lat', str(lat_actual))
        send_mysql('lng', str(lng_actual))

        time.sleep(5)

# Procesa y convierte el JSON desde Arduino
def cad_proc(cad):
    try:
        print("\n\nInicio------------------------------------------------>\n" + cad)
        data = json.loads(cad)

        if "eventos" not in data:
            print("No se encontraron eventos en el JSON.")
            return

        for evento in data["eventos"]:
            sensor = str(evento["id"])
            value = evento["valor"]

            print(f"sensor: {sensor}")
            print(f"value: {value}")
            send_mysql(sensor, value)

            # Envío MQTT a ThingsBoard
            if sensor == '1':
                sensor_data = {'puerta': bool(value)}
            elif sensor == '2':
                sensor_data = {'panico': bool(value)}
            elif sensor == '3':
                sensor_data = {'presencia': int(value)}
            else:
                sensor_data = {f'sensor_{sensor}': value}

            client.publish('v1/devices/me/telemetry', json.dumps(sensor_data), 1)

    except json.JSONDecodeError as e:
        print(f"Error al decodificar JSON: {e}")

# Enviar a MySQL
def send_mysql(sensor_, value_):
    try:
        cnx = mysql.connector.connect(user='user', password='1234',
                                      host='127.0.0.1',
                                      database='irene')
        cursor = cnx.cursor()
        query = f"INSERT INTO data (idsensor, value) VALUES ('{sensor_}', {value_});"
        print("MySQL:", query)
        cursor.execute(query)
        cnx.commit()
        cursor.close()
        cnx.close()
    except mysql.connector.Error as err:
        print(f"Error MySQL: {err}")

# Inicia hilo de GPS
gps_thread = threading.Thread(target=enviar_gps, daemon=True)
gps_thread.start()

# Bucle principal: leer datos de Arduino
try:
    while True:
        line = arduino.readline().decode(errors='ignore').strip()
        if line:
            cad_proc(line)
except KeyboardInterrupt:
    print("Programa detenido.")
    arduino.close()
