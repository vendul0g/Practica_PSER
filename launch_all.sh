#!/bin/bash

# Abrimos VSCode
code .

# Abrimos las ventanas correspondientes a los servicios: Adaptador y Alertas
gnome-terminal --tab -- bash -c "cd adaptador_mqtt_influxdb/ && source adapter/bin/activate && python3 adapter_mqtt_influx-db.py;bash" 
gnome-terminal --tab -- bash -c "cd alertas/ && source alerts/bin/activate && python3 create_alert.py;bash"
gnome-terminal --tab

# Abrimos InfluxDB, Grafana y HiveMQ y Wokwi
firefox http://influxdb:8086/ &
firefox http://localhost:3000/ &
firefox https://www.hivemq.com/demos/websocket-client/ &
firefox https://wokwi.com/projects/382834644618863617 &

# Lanzamos el docker
cd docker/
docker-compose up

