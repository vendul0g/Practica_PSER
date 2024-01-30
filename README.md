# Protocolos de red en IoT
## MQTT
Tenemos nuestro sensores conectado a un servidor gordo que hace de controlador. Los equipos de usuarios se pueden suscribir a un sensor concreto ==>  les llega la información

# Coap
Es un protoclo ligero derivado de HTTP

# Práctica
Vamos a tener:
- Un ESP32 simulado - WOKWI
- 2 sensores <span style="color: red;">distintos</span>.
- En la pantalla nos tiene que mostrar los valores de los sensores de manera alternativa. <span style="color: red;">NO SIMULTÁNEA</span>.
- Codificamos en JSON - Usando MQTT hacia un servidor
- Cuando llegan al Controlador: hay que almacenar los datos en una BD. <span style="color: aqua;">Tendremos que programar un adaptador</span>.
- Desde la base de datos se consulta con un grafara para mostrar por pantalla.
- El usuario del grafana puede crear una alerta que activa un actuador en el ESP32 (se tiene que hacer algo: alarma, luz, lo que sea)
- De manera manual se tiene que poder parar ese actuador

<span style="color: yellow;">Para aprobar hay que hacer toda la funcionalidad básica bien</span>.

# Informe
En el apartado de diseño tenemos que desarrollar una historia de 'para qué vale nuestra práctica'.

# Código 
```
loop{
    get()
    print()
    publish()
}
```

---

Based on the contents of the unzipped project directory, here is an overview of the project's structure and its components:

### Project Overview: Practical PSER

#### Directories and Files:

1. **.obsidian**:
   - Contains configuration files for the Obsidian application, which is likely used for documentation or note-taking. Files include workspace settings, plugin configurations, appearance settings, and hotkeys.

2. **.swp**:
   - A file, possibly a temporary swap file used by an editor like Vim.

3. **adaptador_mqtt_influxdb**:
   - This directory seems to contain scripts and configuration files related to an adapter that connects MQTT to InfluxDB.
   - Files:
     - `env_export.sh`: A shell script, possibly for setting environment variables.
     - `.env`: Environment variables file.
     - `adapter`: A directory, contents unknown.
     - `adapter_mqtt_influx-db.py`: Python script for the adapter.

4. **alertas**:
   - Related to alerting mechanisms.
   - Files:
     - `create_alert.py`: Python script to create alerts.
     - `alerts`: A directory, contents unknown.
     - `images`: A directory, possibly contains images used in alerts.

5. **docker**:
   - Contains Docker-related files, indicating the use of containerization in the project.
   - Files:
     - `LICENSE`: The license file for the project.
     - `docker-compose.yml`: Docker Compose configuration file.
     - `grafana-provisioning`: A directory, likely for Grafana setup.
     - `install-docker-ubuntu.sh`: Shell script to install Docker on Ubuntu.
     - `env_export.sh`: Another shell script for environment variables.
     - `README.md`: Markdown file, probably providing information about this directory.
     - `.env`: Another environment variables file.

6. **launch_all.sh**:
   - A shell script file, possibly used to start the entire project or various components.

7. **wokwi**:
   - Related to Wokwi, which is often used for Arduino and IoT projects.
   - Files:
     - `wokwi.ino`: Arduino sketch file.
     - `old_wokwi.ino`: An older version of the Arduino sketch file.

#### Project Description:

This project appears to be an IoT (Internet of Things) solution, integrating various technologies like MQTT for messaging, InfluxDB for time-series database management, and Docker for containerization. The project includes alerting mechanisms, possibly for monitoring IoT devices or data streams. Arduino sketches in the Wokwi directory suggest the use of microcontrollers in the project. The inclusion of Grafana (inferred from the docker directory) hints at data visualization capabilities. The setup and management of these components seem to be facilitated through various shell scripts and Python programs. 

The exact functionality of each component and how they interact within the project would require a deeper dive into the individual files and scripts.
