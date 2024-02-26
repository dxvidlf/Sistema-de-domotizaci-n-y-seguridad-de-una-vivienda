/*
 ESP-NOW based sensor
 Envía un JSON a la pasarella ESP-NOW -> MQTT que está escuchando en la MAC 2E:3A:E8:12:7D:0E
 Después del envío o si hay un error o time-out el dispositivo entra en deep-sleep durante 5 minutos + o -
*/
/* IMPORTANTE !!!
   Para que el dispositivo se despierte desde deep-sleep hay que conectar el GPIO16 con RST (reset)
   Pero para programar la placa hay que quitar esa conexión con RST
*/

//==================================[Bibliotecas]=================================
#include <ESP8266WiFi.h> // Permiten que el ESP8266 se conecte a redes WiFi
#include "DHTesp.h"      // Libreria para el sensor de temperatura y humedad
#include "ArduinoJson.h" // Manejo de objetos JSONs
#include <string>        // Manejo de cadenas de texto
extern "C"               // indica al compilador de C++ que trate el código dentro de este bloque como si fuera escrito en C
{
#include <espnow.h> // Incluye la biblioteca ESP-NOW
}

//===================================[Instancias]=================================
DHTesp dht;

//====================================[GPIOs]=====================================
#define LED1 2

//=================================[Definiciones]=================================
#define DEBUG_STRING "[" + String(__FUNCTION__) + "():" + String(__LINE__) + "] " // Macro para indicar función y línea de código en los mensajes
#define WIFI_CHANNEL 6                                                            // Los canales Wi-Fi permiten que diferentes redes operen sin interferir entre sí.
#define SEND_TIMEOUT 2000                                                         // Tiempo de espera (timeout) 2 seg para enviar datos
#define MENSAJE_MAXSIZE 250                                                       // Limita la cantidad de datos que se pueden enviar en un solo mensaje para mantener la sincronización
#define SLEEP_SECS 300                                                            // segundos de reposo (5 minutos)

//==================================[Variables]===================================
const int sensorPin = A0;                                     // Pin analógico A0 al que esta conectado la fotocelula
uint8_t pasarelaMAC[] = {0x2E, 0x3A, 0xE8, 0x12, 0x7D, 0x0E}; // this is the MAC Address of the remote ESP server which receives these sensor readings
char mensaje[MENSAJE_MAXSIZE];
unsigned long bootMs = 0, setupMs = 0, sendMs = 0, waitMs = 0;
RF_PRE_INIT()
{
  bootMs = millis(); // registro del tiempo transcurrido desde el inicio del programa
}

//===================================[byte2HEX]===================================
// Devuelve 2 caracteres HEX para un byte
inline String byte2HEX(byte data)
{
  return (String(data, HEX).length() == 1) ? String("0") + String(data, HEX) : String(data, HEX);
}

//===============================[fin_envio_espnow]===============================
// callback fin envío esp-now
void fin_envio_espnow(uint8_t *mac, uint8_t sendStatus)
{
  Serial.printf("Funcion callback de fin de envio\n");
  String deviceMac = "";
  for (int i = 0; i < 6; i++)
    deviceMac += byte2HEX(mac[i]);
  for (auto &c : deviceMac)
    c = toupper(c);
  Serial.printf("Mensaje enviado a MAC: %s\n", deviceMac.c_str());
  Serial.printf("Recepcion: %s\n", (sendStatus) ? "ERROR" : "OK");
  gotoSleep();
}

//-----------------------------------------------------
//     SETUP
//-----------------------------------------------------
void setup()
{
  setupMs = millis();   // Guardar el tiempo actual para referencia futura.
  Serial.begin(115200); // Inicializar la comunicación serial a 115200 baudios.
  Serial.println();
  Serial.println("Empieza setup... Sensor DHT11 ESP-NOW");

  // Configuración de LEDs
  pinMode(LED1, OUTPUT);   // inicializa GPIO como salida
  digitalWrite(LED1, LOW); // enciende el led

  dht.setup(5, DHTesp::DHT11); // Inicializa el sensor DHT11 conectado al GPIO 5.

  // Configuración de WiFi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.println("ESP8266 Board MAC Address:  " + String(WiFi.macAddress())); // Imprimimos por pantalla la mac del dispositivo

  // Inicializa ESP-NOW. Si falla, manda el dispositivo a dormir.
  if (esp_now_init() != 0)
  {
    Serial.println("*** ESP_Now init failed");
    gotoSleep();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);                           // configurar el rol del dispositivo en la red ESP-NOW.
  esp_now_add_peer(pasarelaMAC, ESP_NOW_ROLE_SLAVE, WIFI_CHANNEL, NULL, 0); // agrega un peer (otro dispositivo ESP-NOW).
  esp_now_register_send_cb(fin_envio_espnow);                               // Registra una función de devolución de llamada para cuando se complete un envío.

  // LECTURA DE DATOS DE LOS SENSORES
  waitMs = dht.getMinimumSamplingPeriod(); // Espera el tiempo mínimo de muestreo del sensor DHT.
  delay(waitMs);
  float t = dht.getTemperature(); // Lee la temperatura y la humedad del sensor DHT11.
  float h = dht.getHumidity();
  t = (isnan(t)) ? -255 : t;                    // Si es NaN pongo un valor numérico para que JSON no de problemas
  h = (isnan(h)) ? -255 : h;                    // Para el caso en el que no tengamos conectado el sensor
  int sensorValue = analogRead(sensorPin);      // Leer el valor del sensor (0 a 1023)
  sensorValue = map(sensorValue, 0, 1023, 0, 1000); // Reasignar el valor a un rango de 0 a 1000
  float voltage = sensorValue * (5.0 / 1023.0); // Convertir el valor a voltaje (0 a 5V)

  // Construir el mensaje JSON con los datos del sensor
  StaticJsonDocument<200> mensajeJSON; // Tamaño máximo del mensaje JSON: 200 bytes
  mensajeJSON["topic"] = "datos";
  mensajeJSON["time"] = millis();
  mensajeJSON["DHT11"]["temp"] = t;
  mensajeJSON["DHT11"]["hum"] = h;
  mensajeJSON["lum"] = sensorValue;
  mensajeJSON["status"] = dht.getStatusString(); // Agregar un campo de estado

  // Convertir el mensaje JSON a cadena de texto
  char mensaje[250]; // Tamaño máximo de mensaje: 250 bytes
  serializeJson(mensajeJSON, mensaje);
  // Imprimir el mensaje JSON resultante y la longitud
  Serial.printf("Mensaje a enviar: %s\n    len: %d\n", mensaje, strlen(mensaje));
  // Envía el mensaje JSON a través de ESP-NOW.
  esp_now_send(pasarelaMAC, (uint8_t *)mensaje, strlen(mensaje));
  sendMs = millis(); // Guarda el tiempo de envío para referencia futura.
}
//-----------------------------------------------------
//     LOOP
//-----------------------------------------------------
void loop()
{
  if (millis() > SEND_TIMEOUT) // Comprueba si el tiempo actual ha superado el tiempo de espera para enviar.
  {
    gotoSleep(); // Manda el dispositivo a dormir.
  }
}

//-----------------------------------------------------
void gotoSleep()
{
  Serial.printf("Boot: %ims, setup: %ims, wait4DHT: %ims, send: %ims, now %ims, going to sleep for %i secs...\n", bootMs, setupMs, waitMs, sendMs, millis(), SLEEP_SECS);
  ESP.deepSleepInstant(SLEEP_SECS * 1000000, RF_NO_CAL); // El multiplicador 1000000 convierte segundos en microsegundos.
  // RF_NO_CAL desactiva la calibración de la frecuencia de radio al despertar para ahorrar tiempo y energia
}
