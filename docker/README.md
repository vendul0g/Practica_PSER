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