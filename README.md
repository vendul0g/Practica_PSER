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
# Grafana - influxDB
ReadMe
--
# docker-compose-influxdb-grafana

The project relies on https://github.com/jkehres/docker-compose-influxdb-grafana to deploy influxdb and grafana. However, minor updates have been performed.

## Prerequisites

* Clone the repository
```
git clone https://ants-gitlab.inf.um.es/amzarca/docker-compose-influxdb-grafana
```
* cd docker-compose-influxdb-grafana
* Install docker and docker compose if they were not previously installed
```
./install-docker.sh
```
* Deploy the services
```
docker-compose up
```

## Configure grafana
### Configure influxdb data source
Enter to the dashboard
```
http://localhost:3000/login
admin:admin (If you have modified .env, use the new values you have provided)
```
* Configuration-> data source
* Select influxDB
* Query Language -> Select Flux instead of FluxQL
* Provide the following inputs from .env:
	* Organization
	* Token
	* Default Bucket
* Save and test

### Create panel with query
* Create new panel
* You can use the following query as example:
```
from(bucket: "pser_umu_bucket")
  |> range(start: v.timeRangeStart, stop:v.timeRangeStop)
  |> filter(fn: (r) =>
    r._measurement == "iot" and
    (r._field == "temperature" or r._field == "humidity")
  )
```
* Press on query inspector and test the query

## Built With

* [Docker](https://www.docker.com/) - Docker containers
* [influxdb-grafana](https://github.com/jkehres/docker-compose-influxdb-grafana) - Base services

## Authors

* **Alejandro Molina Zarca** - *Main developer/SysAdmin* -

## License

This project is licensed under the MIT License

## Acknowledgments

* Department of Information and Communications Engineering, Faculty of Computer Science, University of Murcia.