import paho.mqtt.client as mqtt
import os, json
from datetime import datetime
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS

'''
Variables globales
'''
# Configuración de InfluxDB
INFLUXDB_URL = 'http://localhost:8086'  # URL de InfluxDB
TOKEN = "admin_token"  # Token de acceso a InfluxDB
ORG = "pser_umu"  # Nombre de la organización en InfluxDB
BUCKET = 'pser_umu_bucket'  # Nombre del bucket en InfluxDB

# Configuración de MQTT
MQTT_BROKER = 'broker.mqttdashboard.com'  # URL del broker MQTT
MQTT_PORT = 1883  # Puerto del broker MQTT
MQTT_TOPIC = 'topicPSER/F1-GrupoN'  # Tópico MQTT para suscripción

'''
Funciones de InfluxDB
'''
def influxdb_initialize_client():
    client = InfluxDBClient(url=INFLUXDB_URL, token=TOKEN, org=ORG)
    write_api = client.write_api(write_options=SYNCHRONOUS)
    return write_api

# Función para almacenar datos en InfluxDB
def influxdb_store(tempDelanteros, tempTraseros, rpm):
    # Creación del punto de datos para InfluxDB
    point = Point(f"datos_sensor") \
        .field("temperaturaDelanteros", float(tempDelanteros)) \
        .field("temperaturaTraseros", float(tempTraseros)) \
        .field("rpm", rpm) \
        .time(datetime.utcnow(), WritePrecision.NS)
    # Escritura del punto en InfluxDB
    influxdb_client.write(bucket=BUCKET, org=ORG, record=point)

'''
Funciones de MQTT
'''
# Función para gestionar la conexión al broker MQTT
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(" * Conectado al broker MQTT")
        client.subscribe(MQTT_TOPIC)  # Suscripción al tópico MQTT
        print(f" * Suscrito al tópico MQTT: {MQTT_TOPIC}")
    else:
        print(f" * Error de conexión con el broker MQTT: {rc}")

# Función para manejar los mensajes MQTT recibidos
def on_message(client, userdata, msg):
    print(f" - [MQTT] Mensaje recibido --> {msg.topic}: {str(msg.payload)}")
    try:
        # Decodificación y parseo del mensaje JSON
        data = json.loads(msg.payload.decode('utf-8'))
        # Almacenamos en InfluxDB
        influxdb_store(data['temperaturaDelanteros'], data['temperaturaTraseros'], data['rpm'])
        print(" * Datos guardados en InfluxDB")
    except json.JSONDecodeError:
        print("Error decodificando el JSON del mensaje MQTT")

def initialize_mqtt_client():
    # Creación del cliente MQTT
    client = mqtt.Client()
    
    # Asignación de funciones para gestión de conexión y mensajes
    client.on_connect = on_connect
    client.on_message = on_message
    return client

'''
Main
'''
if __name__ == '__main__':
    # Verificación del token de InfluxDB
    print(" * TOKEN: ", TOKEN)

    # Inicialización del cliente InfluxDB
    influxdb_client = influxdb_initialize_client()

    # Inicialización del cliente MQTT
    mqtt_client = initialize_mqtt_client()

    # Conexión al broker MQTT
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)

    # Bucle infinito para mantener la conexión y recibir mensajes
    mqtt_client.loop_forever()

    # Cierre del cliente InfluxDB
    influxdb_client.close()
