import paho.mqtt.client as mqtt
import tkinter as tk
from PIL import ImageTk, Image

'''
-------------------------------------------------------------------------------
Variables globales
'''
# Configuración de MQTT
MQTT_BROKER = 'broker.mqttdashboard.com'  # URL del broker MQTT
MQTT_PORT = 1883  # Puerto del broker MQTT
MQTT_TOPIC = 'topicPSER/F1-GrupoN-Alertas'  # Tópico MQTT para suscripción
# Creación del cliente MQTT
client = mqtt.Client()

# Mensaje de alerta
ALERT_MESSAGE_DEL = "{\"alerta\": \"frenos-delanteros\"}"
ALERT_MESSAGE_TRA = "{\"alerta\": \"frenos-traseros\"}"
ALERT_MESSAGE_MOT = "{\"alerta\": \"motor\"}"

'''
-------------------------------------------------------------------------------
Funciones de MQTT
'''
# Función para conectarse al broker MQTT
def mqtt_client_connect():
    try:
        # Conexión al broker MQTT
        client.connect(MQTT_BROKER, MQTT_PORT, 60) 
        
        # Comenzamos el bucle de MQTT
        client.loop_start()

        print(" * Conectado al broker MQTT")
    except:
        print(" * Error de conexión con el broker MQTT")

# Función para enviar un mensaje de alerta por MQTT
def send_alert(client, alert_message):
    try:
        # Publicamos el mensaje de alerta
        client.publish(MQTT_TOPIC, alert_message)

        print(f" * {alert_message} --> {MQTT_TOPIC}")
    except:
        print(" * Error enviando el mensaje de alerta")


'''
-------------------------------------------------------------------------------
Ventana 
'''
# Función para crear un botón de alerta
def create_button(window, text, alert_message, color):
    return tk.Button(window, text=text, \
        command=lambda: send_alert(client, alert_message), \
        bg=color, fg="white", font=("Courier", 16), height=2, width=30)


# Función para crear los botones de alerta
def create_alert_buttons(window):
    # Creamos la configuración de los botones
    button_configs = [
        ("¡Alerta frenos delanteros!", ALERT_MESSAGE_DEL, "green"),
        ("¡Alerta frenos traseros!", ALERT_MESSAGE_TRA, "blue"),
        ("¡Alerta motor!", ALERT_MESSAGE_MOT, "red")
    ]
    
    # Creamos los botones
    for text, alert_message, color in button_configs:
        button = create_button(window, text, alert_message, color)
        button.pack(pady=5)

# Función para inicializar la ventana
def initialize_window():
    # Creamos la ventana principal
    window = tk.Tk()
    window.title("Botones de alerta MQTT - F1-GrupoN")

    # Cargamos el logo de la aplicación
    icon_image = Image.open("images/python_alert_logo.png")
    icon_photo = ImageTk.PhotoImage(icon_image)
    
    # Asignamos el logo a la ventana
    window.iconphoto(False, icon_photo)

    # Damos el tamaño a la ventana
    window.geometry('500x250') # anchura x altura
    window.resizable(False, False) # No se permite cambiar el tamaño de la ventana

    create_alert_buttons(window)

    return window

'''
-------------------------------------------------------------------------------
Main
'''
if __name__ == '__main__':
    # Inicializamos el cliente MQTT
    mqtt_client_connect()

    window = initialize_window()

    # Ejecutamos el bucle de la aplicación
    window.mainloop()

    # Paramos el bucle de MQTT
    client.loop_stop()