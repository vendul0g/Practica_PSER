#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <WiFi.h> // Libreria para WiFi para poder enviar con MQTT
#include <PubSubClient.h> // Libreria para publicar y suscribir con MQTT
#include <ArduinoJson.h> // Libreria para JSON

#define JSON_BUFFER_SIZE 256 // Tamaño del buffer para el objeto JSON
#define MQTT_PORT 1883 // puerto de MQTT
#define MQTT_BROKER "broker.mqttdashboard.com" // broker de HiveMQ
#define TOPIC_DATA "topicPSER/F1-GrupoN" // topic para subir los datos
#define TOPIC_ALERT "topicPSER/F1-GrupoN-Alertas" // topic para los datos

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
char clientId[50]; // cliente que se conecta al MQTT

// Cliente para la conexion WiFi al servidor MQTT
WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 13         // Pin donde está conectado el DHT22
#define DHTTYPE DHT22     // Especifica el tipo de DHT
#define NTC_PIN 32

DHT dht(DHT_PIN, DHTTYPE);

#define BUZZER_PIN 1

Servo myServo;
LiquidCrystal_I2C lcd(0x27, 20, 4); // Ajusta la dirección 0x27 según tu pantalla
int potPin = 35; // Pin analógico para el potenciómetro deslizante
int servoPin = 25; // Pin para el servo
int leds[] = {23, 26, 33, 19, 18, 5, 17, 16, 4};

unsigned long previousMillis = 0;
const long INTERVAL = 1000;
bool showDHT = true;  // Bandera para alternar entre DHT22 y DS18B20
const float BETA = 3950;
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0)); // Se inicializa la semilla de los numeros pseudoaletaorios

  pinMode(PIN_PITO, OUTPUT);
  client.setCallback(receive_mqtt_message);
  // Se conecta al WiFi
  wifiConnect();

  // Se establece el servidor MQTT
  client.setServer(MQTT_BROKER, MQTT_PORT);
  // Se establece la funcion a la que se llamara cuando llegue un mensaje del topic al que nos hemos suscrito
  client.setCallback(receive_mqtt_message);

  myServo.attach(servoPin);
  for (int pin : leds) pinMode(pin, OUTPUT);

  Wire.begin(2, 15);
  lcd.init();
  lcd.backlight();

  dht.begin();
}
// Funcion para conectarse al WiFi
void wifiConnect() {
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  Serial.print("...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Informacion adicional de la conexion WiFi
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

// Funcion para conectarse al servidor MQTT
void mqttReconnect() {
  while (!client.connected()) {
    Serial.print("[*] Intentando conectar al servidor MQTT...");
    long r = random(1000);
    sprintf(clientId, "clientId-%ld", r);
    if (client.connect(clientId)) {
      Serial.println(clientId);
      Serial.println("Attempting MQTT connection...");
      client.subscribe(TOPIC_ALERT);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
    Serial.println();
  }
}
void receive_mqtt_message(char* topic, byte* payload, unsigned int length) {
    Serial.print("Mensaje recibido bajo el tópico [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Procesar el mensaje
    if (String(topic) == TOPIC_DATA) {
        // Asumiendo que el mensaje es un JSON con un campo "alerta"
        StaticJsonDocument<100> doc;
        deserializeJson(doc, payload, length);
        String alerta = doc["alerta"];
        if (alerta == "RPM_alta") {
            activarAlertaRPM();
        }
    }
}
void activarAlertaRPM() {
    // Hacer parpadear todos los LEDs
    for (int i = 0; i < 5; i++) {
        for (int led : leds) {
            digitalWrite(led, HIGH);
        }
        delay(500);
        for (int led : leds) {
            digitalWrite(led, LOW);
        }
        delay(500);
    }

    // Emitir pitido
    tone(PIN_PITO, 1000, 1000); // 1000 Hz durante 1 segundo
}

String serializarJSON(float temperaturaDelanteros, float temperaturaTraseros, int rpm) {
    StaticJsonDocument<100> doc;
    doc["temperaturaDelanteros"] = temperaturaDelanteros;
    doc["temperaturaTraseros"] = temperaturaTraseros;
    doc["rpm"] = rpm;

    String jsonOutput;
    serializeJson(doc, jsonOutput);

    return jsonOutput;
}

void loop() {
  if (!client.connected()) {
      mqttReconnect();
    }
  client.loop();
  int potValue = analogRead(potPin);
  int angle = map(potValue, 0, 4095, 0, 180);
  myServo.write(angle);
  handleLEDs(angle);

  lcd.setCursor(0, 0);
  lcd.print("RPM del motor");
  lcd.setCursor(0, 1);
  if(angle < 100){
    lcd.setCursor(0, 1);
     lcd.print(angle*100);
     delay(200);
     lcd.setCursor(0, 1);
     lcd.print("                    ");
     
  }else{
    lcd.print(angle*100);
  }
  

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    lcd.setCursor(0, 2);
    lcd.print("                    ");  // Limpiar fila 3
    lcd.setCursor(0, 3);
    lcd.print("                    ");  // Limpiar fila 4

    float temperatureDelanteros, temperatureTraseros;
    if (showDHT) {
      // Mostrar datos del DHT22
      temperatureDelanteros = dht.readTemperature();
      lcd.setCursor(0, 2);
      lcd.print("Frenos delanteros:");
      lcd.setCursor(0, 3);
      lcd.print(temperatureDelanteros*13);
      lcd.print(" C");
    } else {
      // Mostrar datos del NTC
      int analogValue  = analogRead(NTC_PIN);
      temperatureTraseros = 1 / (log(1 / (2047. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
      lcd.setCursor(0, 2);
      lcd.print("Frenos traseros:");
      lcd.setCursor(0, 3);
      lcd.print(temperatureTraseros*20);
      lcd.print(" C");
    }
    showDHT = !showDHT;
    // Serializar y publicar datos en formato JSON
    // Se codifica el JSON (se imprime por consola a modo de debug, comentado)
    Serial.println("JSON serializado: ");
    String jsonOutput = serializarJSON(temperatureDelanteros, temperatureTraseros, angle * 100);
    Serial.println(jsonOutput);
    
    
    client.publish(TOPIC_DATA, jsonOutput.c_str());
  }
}

void handleLEDs(int angle) {
  int numLEDsOn = map(angle, 0, 180, 0, 9);
  for (int i = 0; i < 9; i++) {
    digitalWrite(leds[i], i < numLEDsOn ? HIGH : LOW);
  }
}

