#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <WiFi.h>         // Librería para conectarse a la red WiFi
#include <PubSubClient.h> // Librería para publicar y suscribirse a mensajes MQTT
#include <ArduinoJson.h>  // Librería para serializar y deserializar JSON

/**
 * -------------------------------------------------------------------------
 * Constantes y variables globales
 */
// MQTT
#define MQTT_PORT 1883
#define MQTT_BROKER "broker.mqttdashboard.com"
char clientId[50]; // Identificador del cliente MQTT
#define TOPIC_DATA "topicPSER/F1-GrupoN"
#define TOPIC_ALERT "topicPSER/F1-GrupoN-Alertas"
#define ALERT_MESSAGE_DEL "frenos-delanteros"
#define ALERT_MESSAGE_TRA "frenos-traseros"
#define ALERT_MESSAGE_MOT "motor"

// WiFi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// Cliente WiFi para conectarse a la red y al servidor MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// DHT y NTC (Temperatura)
#define MULTIPLIER 12
#define DHT_PIN 13    // Pin del sensor DHT
#define DHTTYPE DHT22 // Tipo de sensor DHT
DHT dht(DHT_PIN, DHTTYPE);
bool showDHT = true; // Variable para alternar entre mostrar el DHT y el NTC
#define NTC_PIN 32   // Pin del sensor NTC
#define BETA 3950

// Botón
#define PIN_BOTON 14 // Pin del botón

// Pito
#define PIN_PITO 12    // Pin del pito
#define DOT_LENGTH 200 // Length of a dot in milliseconds

// Motor
Servo motor;        // Objeto para controlar el motor
#define PIN_MOTOR 25 // Pin donde está conectado el motor

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4); // Dirección I2C, 20 caracteres y 4 filas

// Potenciómetro
#define PIN_POTENCIOMETRO 35

// LEDs
int leds[] = {23, 26, 33, 19, 18, 5, 17, 16, 4}; // Array con los pines de los LEDs
#define L_RED 22
#define L_GREEN 21
#define L_BLUE 27

// Variables para controlar el tiempo de espera entre publicaciones
unsigned long previousMillis = 0;
#define INTERVAL 1000 // 1 Segundo

/*
 * -----------------------------------------------------------------------------
 * Funciones de inicialización
 */
// Función para conectar a la red WiFi
void connect_wifi()
{
  Serial.print("Connecting to WiFi ");
  WiFi.mode(WIFI_STA); // Modo cliente
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Esperamos a que se conecte a la red WiFi
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected"); // Conexión establecida
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}

// Función para conectar al servidor MQTT
void initialize_mqtt()
{
  // Establecemos el servidor MQTT
  client.setServer(MQTT_BROKER, MQTT_PORT);

  // Función que se ejecuta cuando se recibe un mensaje en el topic suscrito
  client.setCallback(receive_mqtt_message);

  // Nos conectamos al servidor MQTT
  if (!client.connected())
  {
    connect_mqtt();
  }
}

// Función para inicializar los LEDs
void initialize_leds()
{
  // Inicializamos los pines de los LEDs del motor
  for (int pin : leds)
    pinMode(pin, OUTPUT);

  // Inicializamos los pines de los LEDs de las alertas
  pinMode(L_RED, OUTPUT);
  pinMode(L_GREEN, OUTPUT);
  pinMode(L_BLUE, OUTPUT);
}

// Función para inicializar el LCD
void initialize_lcd()
{
  // Inicializamos el bus I2C para el LCD
  Wire.begin(2, 15);

  // Inicializamos el LCD
  lcd.init();

  // Encendemos la luz de fondo del LCD
  lcd.backlight();
}

/**
 * -------------------------------------------------------------------------
 * JSON
 */
// Función para serializar los datos en formato JSON
String json_serialize(float temperaturaDelanteros, float temperaturaTraseros, int rpm)
{
  // Creamos un documento JSON
  StaticJsonDocument<100> doc;

  // Añadimos los datos al documento
  doc["temperaturaDelanteros"] = temperaturaDelanteros;
  doc["temperaturaTraseros"] = temperaturaTraseros;
  doc["rpm"] = rpm;

  // Serializamos el documento JSON
  String jsonOutput;
  serializeJson(doc, jsonOutput);
  return jsonOutput; // Devolvemos el documento JSON serializado
}

// Función para deserializar los datos de JSON
String json_deserialize(byte *payload, unsigned int length)
{
  // Convertimos el mensaje recibido a String
  String payloadStr((char *)payload, length);

  // Creamos un documento JSON
  StaticJsonDocument<100> doc;

  // Deserializamos el mensaje recibido
  deserializeJson(doc, payloadStr);

  // Obtenemos los datos del documento
  String alerta = doc["alerta"].as<String>();

  // Devolvemos los datos
  return alerta;
}

/*
 * -----------------------------------------------------------------------------
 * MQTT Functions
 */
// Función para conectarse al servidor MQTT
void connect_mqtt()
{
  while (!client.connected())
  { // Mientras no nos hayamos conectado
    Serial.print("[*] Intentando conectar al servidor MQTT...");

    // Creamos un identificador de cliente aleatorio
    long r = random(10000) + 1000;
    sprintf(clientId, "clientId-%ld", r);

    // Intentamos conectarnos al servidor
    if (client.connect(clientId))
    { // Si hay éxito
      Serial.println(clientId);
      client.subscribe(TOPIC_ALERT); // Nos suscribimos al topic de alertas
    }
    else
    { // Si falla
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

// Funcion para publicar un mensaje en el servidor MQTT
void publish_mqtt_message(float temperaturaDelanteros, float temperaturaTraseros, int rpm)
{
  // Serializamos los datos en formato JSON
  String jsonOutput = json_serialize(temperaturaDelanteros, temperaturaTraseros, rpm);

  // Mostramos por pantalla el mensaje que vamos a publicar
  Serial.print("Publicando mensaje: ");
  Serial.println(jsonOutput);

  // Publicamos el mensaje en el servidor MQTT
  client.publish(TOPIC_DATA, jsonOutput.c_str());
}

// Función que se ejecuta cuando se recibe un mensaje de alerta
void receive_mqtt_message(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");

  // Mostramos por pantalla el mensaje recibido
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Procesamos el mensaje recibido
  // Deserializamos el mensaje recibido
  String alerta = json_deserialize(payload, length);

  // Activamos la alerta correspondiente
  if (alerta == ALERT_MESSAGE_DEL)
  {
    activar_alerta_frenos_delanteros();
  }
  else if (alerta == ALERT_MESSAGE_TRA)
  {
    activar_alerta_frenos_traseros();
  }
  else if (alerta == ALERT_MESSAGE_MOT)
  {
    activar_alerta_motor();
  }
}

/**
 * -------------------------------------------------------------------------
 * NTC
 */
float read_temperatura_NTC()
{
  // ADC = Analog to Digital Converter
  const float ADC_max = 4095; // Máximo valor ADC en ESP32
  const float Vref = 3.3;     // Referencia de voltaje para ADC ESP32
  const float R0 = 10000;     // Resistencia del transmisor NTC
  const float T0 = 298.15;    // Referencia de temperatur en Kelvin (25°C)

  int ADC_value = analogRead(NTC_PIN);                   // Read the analog value from the NTC sensor
  float Vmeasured = (ADC_value / (float)ADC_max) * Vref; // Convert ADC value to voltage

  // Calculate the resistance of the NTC thermistor
  float R = (Vref / Vmeasured) - 1.0;
  R = R0 / R; // Convert the ratio to resistance

  // Calculate the temperature in Kelvin
  float temperatureK = 1.0 / (1.0 / T0 + log(R / R0) / BETA);

  // Convert Kelvin to Celsius
  float temperatureC = temperatureK - 273.15;

  return temperatureC;
}

/**
 * -------------------------------------------------------------------------
 * Pito
 */

void pito_punto()
{
  tone(PIN_PITO, 1000); // Pita
  delay(DOT_LENGTH);    // Espera
  noTone(PIN_PITO);     // Para
}

void pito_raya()
{
  tone(PIN_PITO, 1000);
  delay(DOT_LENGTH * 3);
  noTone(PIN_PITO);
}

void pito_espacio(int units)
{
  delay(DOT_LENGTH * units); // Space between beeps
}

void activar_alerta_frenos_delanteros()
{
  digitalWrite(L_GREEN, HIGH); // Encendemos el LED verde

  // Mientras que el botón no se pulse
  while (digitalRead(PIN_BOTON) == HIGH)
  {
    pito_punto();
    pito_espacio(1);
  }
  digitalWrite(L_GREEN, LOW); // Apagamos el LED verde
}

void activar_alerta_frenos_traseros()
{
  digitalWrite(L_BLUE, HIGH); // Encendemos el LED azul
  // Mientras que el botón no se pulse
  while (digitalRead(PIN_BOTON) == HIGH)
  {
    pito_raya();
    pito_espacio(1);
  }
  digitalWrite(L_BLUE, LOW); // Apagamos el LED azul
}

/**
 * -------------------------------------------------------------------------
 * LEDs
 */
// Función para encender o apagar los LEDs según el ángulo
void handleLEDs(int angle)
{
  // Calculamos el número de LEDs que deben estar encendidos
  int numLEDsOn = map(angle, 0, 180, 0, 9);

  // Encendemos o apagamos los LEDs según el número calculado
  for (int i = 0; i < 9; i++)
  {
    digitalWrite(leds[i], i < numLEDsOn ? HIGH : LOW);
  }
}

// Función para activar la alerta de motor - Señal visual con LEDs
void activar_alerta_motor()
{
  // Encendemos el LED rojo
  digitalWrite(L_RED, HIGH);

  // Mientras no se pulse el botón
  while (digitalRead(PIN_BOTON) == HIGH)
  {
    for (int led : leds)
      digitalWrite(led, HIGH);
    pito_punto();
    pito_espacio(1);
    pito_punto();
    pito_espacio(1);

    for (int led : leds)
      digitalWrite(led, LOW);
    pito_raya();
    pito_espacio(1);
  }
  // Apagamos el LED rojo
  digitalWrite(L_RED, LOW);
}

/**
 * -------------------------------------------------------------------------
 * LCD
 */
// Función para mostrar el ángulo en el LCD
void displayLCD(int angle)
{
  lcd.setCursor(0, 0); // Nos movemos a la primera fila
  lcd.print("RPM del motor");

  lcd.setCursor(0, 1); // Nos movemos a la segunda fila
  if (angle < 100)
  {
    lcd.print(angle * 100); // Mostramos las RPM
    delay(200);

    lcd.setCursor(0, 1);               // Nos movemos a al inicio de la segunda fila
    lcd.print("                    "); // Borramos el texto
  }
  else
  {
    lcd.print(angle * 100); // Mostramos las RPM
  }
}

// Función para limpiar las líneas de temperatura del LCD
void lcd_clear_temperature()
{
  lcd.setCursor(0, 2);               // Nos movemos a la tercera fila
  lcd.print("                    "); // Borramos el texto
  lcd.setCursor(0, 3);               // Nos movemos a la cuarta fila
  lcd.print("                    "); // Borramos el texto
}

// Función para mostrar la temperatura en el LCD
void lcd_print_temperatura(String text, float temperatura)
{
  lcd.setCursor(0, 2);    // Nos movemos a la tercera fila
  lcd.print(text);        // Mostramos el texto
  lcd.setCursor(0, 3);    // Nos movemos a la cuarta fila
  lcd.print(temperatura); // Mostramos la temperatura
  lcd.print(" C");        // Mostramos el símbolo de grados
}

/**
 * -------------------------------------------------------------------------
 * Motor
 */
int manejador_motor()
{
  // Leemos el valor del potenciómetro
  int potValue = analogRead(PIN_POTENCIOMETRO);

  // Mapeamos el valor del potenciómetro a un ángulo entre 0 y 180
  int angle = map(potValue, 0, 4095, 0, 180);

  // Movemos el motor al ángulo calculado
  motor.write(angle);

  // Encendemos los LEDs correspondientes según el ángulo
  handleLEDs(angle);

  // Mostramos el ángulo en el LCD
  displayLCD(angle);

  return angle * 100; // Devolvemos las RPM
}

/**
 * -------------------------------------------------------------------------
 * Main
 */
// Inicializar
void setup()
{
  // Inicializamos el puerto serie
  Serial.begin(115200);

  // Inicializamos la semilla para generar números aleatorios
  randomSeed(analogRead(0));

  // Inicializamos el pin del pito
  pinMode(PIN_PITO, OUTPUT);

  // Inicializamos el pin del botón
  pinMode(PIN_BOTON, INPUT_PULLUP);

  // Conectamos a la red WiFi
  connect_wifi();

  // Conectamos al servidor MQTT
  initialize_mqtt();

  // Inicializamos el motor
  motor.attach(PIN_MOTOR);

  // Inicializamos los LEDs (Pines de salida)
  initialize_leds();

  // Inicializamos el LCD
  initialize_lcd();

  // Inicializamos el sensor DHT
  dht.begin();
}

// Bucle principal
void loop()
{
  // Nos conectamos al servidor MQTT si no estamos conectados
  if (!client.connected())
  {
    connect_mqtt();
  }
  client.loop(); // Mantenemos la conexión con el servidor MQTT

  // Leemos el valor del potenciómetro y movemos el motor
  int rpm = manejador_motor();

  // Leemos los sensores de temperatura y publicamos los datos cada segundo
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= INTERVAL)
  {
    previousMillis = currentMillis;

    lcd_clear_temperature(); // Borramos las líneas de temperatura del LCD

    float temperatura_delanteros = temperatura_delanteros = dht.readTemperature() * MULTIPLIER;;
    float temperatura_traseros = temperatura_traseros = read_temperatura_NTC() * MULTIPLIER;
    // Alternamos entre mostrar el DHT y el NTC
    if (showDHT)
    { // Si toca mostrar el DHT (Delanteros)
      lcd_print_temperatura("Frenos delanteros:", temperatura_delanteros);
    }
    else
    { // Si toca mostrar el NTC (Traseros)
      lcd_print_temperatura("Frenos traseros:", temperatura_traseros);
    }
    showDHT = !showDHT; // Actualizamos el valor para alternar

    // Publicamos los datos en el servidor MQTT
    publish_mqtt_message(temperatura_delanteros, temperatura_traseros, rpm);
  }
}