/*
 *
 *  Version 29-12-2023 22:00:00
 *
 *  Proyecto Iot GR10
 *
 *
 */

//==================================[Bibliotecas]=================================
#include <string>              // Manejo de cadenas de texto
#include <ESP8266WiFi.h>       // Permiten que el ESP8266 se conecte a redes WiFi
#include <PubSubClient.h>      // Para comunicarse usando el protocolo MQTT
#include <ArduinoJson.h>       // Manejo de objetos JSONs
#include <ESP8266httpUpdate.h> // Actualización del firmware a través de HTTP (OTA)
#include "Button2.h"           // libreria para boton flash (GPIO0)

//=================================[Definiciones]=================================
#define __HTTPS__                                                                                           // Se usa para habilitar o deshabilitar características específicas de HTTPS
#define BUTTON_PIN 0                                                                                        // Define el pin del GPIO al que está conectado el botón, en este caso, el GPIO 0.
#define OTA_URL "https://iot.ac.uma.es:1880/esp8266-ota/update"                                             // Address of OTA update server https://iot.ac.uma.es:1880/esp8266-ota
#define FW_BOARD_NAME ""                                                                                    // Para versiones del IDE >= 2.0 dejar vacío
#define HTTP_OTA_VERSION String(__FILE__).substring(String(__FILE__).lastIndexOf('\\') + 1) + FW_BOARD_NAME // Definie el nombre del firmware para su identificación y seguimiento.
#define DEBUG_STRING "[" + String(__FUNCTION__) + "():" + String(__LINE__) + "] "                           // Macro para indicar función y línea de código en los mensajes

ADC_MODE(ADC_VCC); // Configura el modo del Convertidor Analógico a Digital para leer el voltaje de alimentación (VCC) del propio chip.

//===================================[Instancias]=================================
WiFiClient wClient;
PubSubClient mqtt_client(wClient);
Button2 button;

//=============================[Variables de conexion]============================
// const String ssid = "MOVISTAR_7520";
// const String password = "F4m6UJuLsG9vbyfYE4KC";
// const String ssid = "MiFibra-6982";
// const String password = "3NozhEfp";
// const String ssid = "infind";
// const String password = "1518wifi";
// const String ssid = "HUAWEI P8";
// const String password = "12345678";
const String ssid = "iPhone de Darío";
const String password = "dar123456";
// const String ssid = "MIWIFI_hfhQ";
// const String password = "ERc6a46h";

//=====================================[MQTT]=====================================
// const String mqtt_server = "infindgrupo10mqtt.duckdns.org"; // Servidor privado en raspberry
const String mqtt_server = "iot.ac.uma.es"; // Servidor uma
const String GROUP_USER = "II10";           // para MQTT y mongoDB
const String GROUP_PASS = "wD9IYehI";       // para MQTT y mongoDB
int intentos = 0;

//===================================[Topics]=====================================
String ID_PLACA = "ESP" + String(ESP.getChipId()); // Identificador único para la placa ESP8266.
String TOPIC_BASE = GROUP_USER + "/" + ID_PLACA + "/";
String topic_PUB_conexion = TOPIC_BASE + "conexion";
String topic_PUB_datos = TOPIC_BASE + "datos";
String topic_SUB_config = TOPIC_BASE + "config";
String topic_SUB_motor_cmd = TOPIC_BASE + "motor/cmd";
String topic_PUB_motor_status = TOPIC_BASE + "motor/status";
String topic_SUB_switch_cmd = TOPIC_BASE + "switch/cmd";
String topic_PUB_switch_status = TOPIC_BASE + "switch/status";
String topic_SUB_alarm_cmd = TOPIC_BASE + "alarm/cmd";
String topic_PUB_alarm_status = TOPIC_BASE + "alarm/status";
String topic_SUB_FOTA = TOPIC_BASE + "FOTA";

//==================================[Variables]===================================
unsigned long ultimo_mensaje = 0;
unsigned long ultComprobacion = 0;
unsigned long ultimo_cambInten = 0;
unsigned long ultima_actualizacion_ota = 0;
unsigned long ahora = 0;
int rango_PWM = 255;       // int rango_PWM = 1024;
int intensidadActual = 0;  // Almacena la intensidad actual del PWM
int intensidadDeseada = 0; // Almacena la intensidad deseada del PWM
int intensidadPrevia = 0;  // Almacena la intensidad antes de apagar con boton
int actualMovim = 0;
int actualMQ7 = 1;
int actualMQ6 = 1;
bool ActualizaFOTA = false; // Variable actualiza fota
String mensaje_conexion = "{\"online\":true}";
String LWT = "{\"online\":false}";

//====================================[GPIOs]=====================================
int LED2 = 16;       // D0
int GPIO_Movim = 5;  // D1
int GPIO_LED = 4;    // D2
int GPIO_Alarm = 2;  // D4   El mismo que el led de la placa
int LED1 = 2;        // D4
int GPIO_Motor = 14; // D5
int GPIO_MQ6 = 12;   // D6    (Butano, GLP)
int GPIO_MQ7 = 13;   // D7    (Monóxido de carbono)

//=================================[Objetos JSON]=================================
DynamicJsonDocument mensaje_datos(512);
DynamicJsonDocument mensaje_status(512);

//=================================[conecta_wifi]=================================
void conecta_wifi()
{
  int initTime = millis();
  Serial.println("Connecting to " + ssid);
  WiFi.mode(WIFI_STA);                  // Configura el módulo en modo estación
  WiFi.begin(ssid, password);           // Inicia la conexión con las credenciales
  while (WiFi.status() != WL_CONNECTED) // Espera hasta que la conexión sea exitosa
  {
    delay(200);
    Serial.print("."); // Imprime un punto como indicador de progreso
    if (millis() > initTime + 50 * 1000)
    { // Si en 50 segundos no ha conectado:
      Serial.println("Error de timeout al intentar conectar a WiFi, reiniciando el dispositivo");
      ESP.restart(); // Reinicia el dispositivo para que trate de conectar a wifi
    }
  }
  Serial.println();
}

//=================================[conecta_mqtt]=================================
void conecta_mqtt()
{
  while (!mqtt_client.connected()) // Bucle que se ejecuta hasta que se establezca la conexión
  {
    Serial.print("Attempting MQTT connection..."); // Attempt to connect
    if (mqtt_client.connect(ID_PLACA.c_str(), GROUP_USER.c_str(), GROUP_PASS.c_str(), topic_PUB_conexion.c_str(), 2, false, LWT.c_str()))
    {
      intentos = 0;
      Serial.println(" conectado a broker: " + mqtt_server);
      mqtt_client.subscribe(topic_PUB_conexion.c_str());
      mqtt_client.subscribe(topic_PUB_datos.c_str());
      mqtt_client.subscribe(topic_SUB_config.c_str());
      mqtt_client.subscribe(topic_PUB_motor_status.c_str());
      mqtt_client.subscribe(topic_SUB_motor_cmd.c_str());
      mqtt_client.subscribe(topic_PUB_switch_status.c_str());
      mqtt_client.subscribe(topic_SUB_switch_cmd.c_str());
      mqtt_client.subscribe(topic_PUB_alarm_status.c_str());
      mqtt_client.subscribe(topic_SUB_alarm_cmd.c_str());
      mqtt_client.subscribe(topic_SUB_FOTA.c_str());
      mqtt_client.subscribe(topic_SUB_FOTA.c_str());
      mqtt_client.publish(topic_PUB_conexion.c_str(), mensaje_conexion.c_str(), true);
    }
    else // Si la conexión falla
    {
      Serial.println("ERROR:" + String(mqtt_client.state()) + " reintento en 5s");
      delay(5000); // Wait 5 seconds before retrying
      intentos += 1;
      if (intentos > 5)
      {
        Serial.println("Error al intentar conectar a MQTT, reiniciando el dispositivo");
        ESP.restart(); // Reinicia el dispositivo para que reconecte mqtt más rápido
      }
    }
  }
}

//================================[procesa_mensaje]===============================
void procesa_mensaje(char *topic, byte *payload, unsigned int length)
{
  String mensaje = String(std::string((char *)payload, length).c_str()); // Convierte el payload a una cadena de texto (String) para facilitar su manejo.
  Serial.println("Mensaje recibido [" + String(topic) + "] " + mensaje); // Imprime el mensaje recibido y el topic asociado en el puerto serie.
  if (String(topic) == topic_SUB_motor_cmd)                              // Caso: Mensaje para controlar el motor.
  {
    DynamicJsonDocument mensaje_json(50); // Deserializar el JSON recibido.
    DeserializationError error = deserializeJson(mensaje_json, mensaje);
    if (!error)
    {
      intensidadDeseada = mensaje_json["level"]; // Ajusta la intensidad del motor.
    }
    else
    {
      Serial.println("Error al analizar el comando para el MOTOR");
      return;
    }
  }
  else if (String(topic) == topic_SUB_switch_cmd) // Caso: Mensaje para controlar el led (switch)
  {
    DynamicJsonDocument mensaje_json(50); // Deserializar JSON
    DeserializationError error = deserializeJson(mensaje_json, mensaje);
    bool sw = mensaje_json["switch"];
    if (not error)
    {
      if (sw) // Enciende o apaga el LED según el mensaje recibido.
      {
        digitalWrite(GPIO_LED, LOW);
      }
      else
      {
        digitalWrite(GPIO_LED, HIGH);
      }
      String mensaje_switch_str;
      mqtt_client.publish(topic_PUB_switch_status.c_str(), mensaje.c_str()); // Publica el estado actual del switch en el topic correspondiente.
    }
    else
    {
      Serial.println("Error al analizar el comando para el SWITCH");
      return;
    }
  }
  else if (String(topic) == topic_SUB_alarm_cmd) // Caso: Mensaje para controlar la alarma.
  {
    DynamicJsonDocument mensaje_json(50); // Desealiza JSON
    DeserializationError error = deserializeJson(mensaje_json, mensaje);
    bool al = mensaje_json["alarm"];
    if (not error)
    {
      if (al)
      {
        analogWrite(GPIO_Alarm, 1); // Activa la alarma y con ella el built-in LED
      }
      else
      {
        analogWrite(GPIO_Alarm, 0); // Desactiva la alarma.
        digitalWrite(LED1, HIGH);   // Apaga el LED built in GPIO2.
      }
      String mensaje_alarm_str;
      mqtt_client.publish(topic_PUB_alarm_status.c_str(), mensaje.c_str()); // // Publica el estado actual de la alarma
    }
    else
    {
      Serial.println("Error al analizar el comando para la ALARMA");
      return;
    }
  }
  else if (String(topic) == topic_SUB_FOTA) // Caso: Mensaje para actualizar el firmware mediante FOTA (Firmware Over The Air).
  {
    DynamicJsonDocument mensaje_json(50);
    DeserializationError error = deserializeJson(mensaje_json, mensaje);
    if (!error)
    {
      ActualizaFOTA = mensaje_json["actualizaFOTA"];
      Serial.println("Actualiza fota: " + ActualizaFOTA);
    }
    else
    {
      Serial.println("Error al analizar el comando para actualizar mediante FOTA");
      return;
    }
  }
}

//========================[funciones para progreso de OTA]========================
void inicio_OTA()
{
  Serial.println(DEBUG_STRING + "Nuevo Firmware encontrado. Actualizando...");
}
void final_OTA()
{
  Serial.println(DEBUG_STRING + "Fin OTA. Reiniciando...");
}
void error_OTA(int e)
{
  Serial.println(DEBUG_STRING + "ERROR OTA: " + String(e) + " " + ESPhttpUpdate.getLastErrorString());
}

//=================================[progreso_OTA]=================================
/*esta función se utiliza para mostrar en la consola serial el progreso de una
actualización OTA, indicando el porcentaje completado y la cantidad de datos
actualizados en kilobytes*/
void progreso_OTA(int x, int todo)
{
  int progreso = (int)((x * 100) / todo);
  String espacio = (progreso < 10) ? "  " : (progreso < 100) ? " "
                                                             : "";
  if (progreso % 10 == 0)
    Serial.println(DEBUG_STRING + "Progreso: " + espacio + String(progreso) + "% - " + String(x / 1024) + "K de " + String(todo / 1024) + "K");
}

//==================================[intenta_OTA]=================================
void intenta_OTA() // Proceso de actualización OTA (Over-The-Air)
{
  Serial.println(DEBUG_STRING + "Comprobando actualización:");
  Serial.println(DEBUG_STRING + "URL: " + OTA_URL);
  ESPhttpUpdate.onStart(inicio_OTA);
  ESPhttpUpdate.onError(error_OTA);
  ESPhttpUpdate.onProgress(progreso_OTA);
  ESPhttpUpdate.onEnd(final_OTA);

#ifdef __HTTPS__
  WiFiClientSecure wClient; // Cliente WiFi seguro para la conexión HTTPS.
  wClient.setTimeout(12);   // timeout en segundos    (La lectura sobre SSL puede ser lenta, poner timeout suficiente)
  wClient.setInsecure();    // no comprobar el certificado del servidor
#else
  WiFiClient wClient; // Cliente WiFi para conexiones no seguras.
#endif

  /*inicia el proceso de actualización utilizando la biblioteca ESPhttpUpdate.
  Toma como parámetros el cliente WiFi (wClient), la URL de actualización (OTA_URL)
  y el nombre/version del firmware actual (HTTP_OTA_VERSION). Esta función intentará
  descargar y actualizar el firmware del dispositivo con la versión disponible en la
  URL proporcionada.*/
  switch (ESPhttpUpdate.update(wClient, OTA_URL, HTTP_OTA_VERSION))
  {
  case HTTP_UPDATE_FAILED:
    Serial.println(DEBUG_STRING + "HTTP update failed: Error (" + String(ESPhttpUpdate.getLastError()) + "): " + ESPhttpUpdate.getLastErrorString());
    break;
  case HTTP_UPDATE_NO_UPDATES:
    Serial.println(DEBUG_STRING + "El dispositivo ya está actualizado");
    break;
  case HTTP_UPDATE_OK:
    Serial.println(DEBUG_STRING + "OK");
    break;
  }
}

void singleClick(Button2 &btn)
{
  Serial.println("click\n");
  if (intensidadDeseada == 0) // si esta apagado lo encendemos a la intensidad previa
  {
    intensidadDeseada = intensidadPrevia;
  }
  else // Si está encendido lo apagamos
  {
    intensidadPrevia = intensidadDeseada; // Guardamos la intensidad previa antes de apagarlo
    intensidadDeseada = 0;
  }
}
void longClickDetected(Button2 &btn)
{
  Serial.println("long click detected");
}
void longClick(Button2 &btn)
{
  Serial.println("long click\n");
  intenta_OTA(); // Comprobamos actualización
}
void doubleClick(Button2 &btn)
{
  Serial.println("double click\n");
  intensidadDeseada = 100;
}
void tripleClick(Button2 &btn)
{
  Serial.println("triple click\n");
}

void enviarMensaje()
{
  digitalWrite(LED2, LOW); // Enciende el LED para enviar el mensaje.
  // Contruimos mensajes para envío
  mensaje_datos["Uptime"] = millis();          // Tiempo desde que el dispositivo se encendió.
  mensaje_datos["Vcc"] = ESP.getVcc();         // Voltaje del dispositivo.
  mensaje_datos["movim"] = actualMovim;        // Estado del sensor de movimiento.
  mensaje_datos["Wifi"]["SSId"] = WiFi.SSID(); // Información de la conexión WiFi.
  mensaje_datos["Wifi"]["IP"] = WiFi.localIP().toString();
  mensaje_datos["Wifi"]["RSSI"] = WiFi.RSSI();
  mensaje_datos["MQ"]["MQ7"] = actualMQ7; // Estado de sensor de humo.
  mensaje_datos["MQ"]["MQ6"] = actualMQ6; // Estado de sensor de gas
  // Convierte el objeto JSON en una cadena y lo envía a través de MQTT.
  String mensaje_datos_str;
  serializeJson(mensaje_datos, mensaje_datos_str);
  mqtt_client.publish(topic_PUB_datos.c_str(), mensaje_datos_str.c_str());
  // Apaga el LED tras enviar el mensaje.
  digitalWrite(LED2, HIGH);
}

//-----------------------------------------------------
//     SETUP
//-----------------------------------------------------
void setup()
{
  // Inicialización de objetos JSON
  mensaje_datos["Uptime"];                                         // Tiempo desde que el dispositivo se encendió.
  mensaje_datos["Vcc"];                                            // Voltaje de alimentación del dispositivo.
  mensaje_datos["movim"];                                          // Datos de movimiento
  JsonObject info_wifi = mensaje_datos.createNestedObject("Wifi"); // Objeto JSON anidado para datos de WiFi.
  info_wifi["SSId"];                                               // SSID de la red WiFi.
  info_wifi["IP"];                                                 // Dirección IP en la red WiFi.
  info_wifi["RSSI"];                                               // Intensidad de la señal de la red WiFi.
  JsonObject info_mq = mensaje_datos.createNestedObject("MQ");     // Objeto JSON anidado para datos de sensores MQ.
  info_mq["MQ6"];                                                  // Datos del sensor MQ6. (Butano, GLP)
  info_mq["MQ7"];                                                  // Datos del sensor MQ7. (Monóxido de carbono)

  // Comienza el SetUp
  Serial.begin(115200); // Inicia la comunicación serial a 115200 baudios.
  Serial.println();
  Serial.println("Empieza setup...");

  // Impresión de información relevante para la actualización OTA
  Serial.println(DEBUG_STRING + "Fuente del programa: " + String(__FILE__));
  Serial.println(DEBUG_STRING + "Nombre Firmware: " + String(HTTP_OTA_VERSION) + ".bin");
  Serial.println(DEBUG_STRING + "Placa: " + String(ARDUINO_BOARD));

  // Configuración de los pines GPIO como entrada o salida según su función.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(GPIO_MQ6, INPUT);
  pinMode(GPIO_MQ7, INPUT);
  pinMode(GPIO_Movim, INPUT);
  pinMode(GPIO_Motor, OUTPUT);
  pinMode(GPIO_LED, OUTPUT);
  pinMode(GPIO_Alarm, OUTPUT);
  digitalWrite(LED1, HIGH);     // Apaga LED1.
  digitalWrite(LED2, HIGH);     // Apaga LED2.
  digitalWrite(GPIO_LED, HIGH); // Apaga el LED asociado a GPIO_LED.

  // Conexión a la red WiFi y configuración del cliente MQTT.
  conecta_wifi();
  mqtt_client.setServer(mqtt_server.c_str(), 1883); // Configura el servidor MQTT.
  mqtt_client.setBufferSize(512);                   // Establece tamaño del buffer para mensajes MQTT.
  mqtt_client.setCallback(procesa_mensaje);         // Establece la función callback para MQTT.
  conecta_mqtt();

  // Configuración del botón para manejar diferentes tipos de clics.
  button.begin(BUTTON_PIN);
  button.setClickHandler(singleClick);
  button.setLongClickHandler(longClick);
  button.setDoubleClickHandler(doubleClick);

  // Impresión de información relevante sobre los topics MQTT
  Serial.println("Identificador placa: " + ID_PLACA);
  Serial.println("Topic publicacion conexión: " + topic_PUB_conexion);
  Serial.println("Topic publicacion datos: " + topic_PUB_datos);
  Serial.println("Topic publicacion motor : " + topic_PUB_motor_status);
  Serial.println("Topic suscripcion motor : " + topic_SUB_motor_cmd);
  Serial.println("Topic publicacion switch : " + topic_PUB_switch_status);
  Serial.println("Topic suscripcion switch : " + topic_SUB_switch_cmd);
  Serial.println("Topic publicacion alarma : " + topic_PUB_alarm_status);
  Serial.println("Topic suscripcion alarma : " + topic_SUB_alarm_cmd);
  Serial.println("Topic suscripcion FOTA : " + topic_SUB_FOTA);
  Serial.println("Termina setup en " + String(millis()) + " ms");

  // actualización OTA si hay disponible una nueva version
  intenta_OTA();
}

//-----------------------------------------------------
//     LOOP
//-----------------------------------------------------
void loop()
{
  if (!mqtt_client.connected()) // Verifica si el cliente MQTT está conectado.
  {                             // Si no, intenta reconectar.
    conecta_mqtt();
  }
  mqtt_client.loop(); // Esta llamada para que la librería recupere el control
  button.loop();      // Permite que la librería Button2 maneje sus tareas
  ahora = millis();   // Obtiene el tiempo actual en milisegundos.

  // Verifica si es necesario realizar una actualización OTA cada hora.
  if (ahora - ultima_actualizacion_ota >= 60 * 60 * 1000 or ActualizaFOTA)
  {
    ultima_actualizacion_ota = ahora;
    intenta_OTA(); // Toggle the LED
    ActualizaFOTA = false;
  }

  if (ahora - ultComprobacion > 1000) // Comprueba datos de los sensores cada segundo
  { 
    ultComprobacion = ahora;
    actualMovim = int(digitalRead(GPIO_Movim));
    actualMQ7 = int(digitalRead(GPIO_MQ7));
    actualMQ6 = int(digitalRead(GPIO_MQ6));

    // Envia datos periódicamente (cada 5 minutos) o de manera instantanea si ha abido algun cambio.
    if (ahora - ultimo_mensaje >= 5 * 60 * 1000)
    {
      ultimo_mensaje = ahora;
      enviarMensaje(); // Envío después de un minuto
    }
    else
    {
      if (actualMovim == 1 || actualMQ7 == 0 || actualMQ6 == 0) // Si se activa alguno de los sensores
      {
        ultimo_mensaje = ahora;
        enviarMensaje(); // Enviar mensaje si se activa algun sensor
      }
    }
  }


  // Ajusta la intensidad del motor
  if (intensidadActual != intensidadDeseada)
  {                                                               // de forma gradual a una velocidad de ±1% cada 10ms
    int cambio = (intensidadDeseada > intensidadActual) ? 1 : -1; // Determina la dirección del cambio de intensidad.
    if (millis() - ultimo_cambInten >= 10)                        // Cambia la intensidad cada 10ms
    {
      ultimo_cambInten = millis();
      intensidadActual += cambio;
      analogWrite(GPIO_Motor, map(intensidadActual, 0, 100, 0, rango_PWM)); // Aplica la nueva intensidad.
      if (intensidadActual == intensidadDeseada)                            // Publica el nuevo estado si ha alcanzado la intensidad deseada.
      {
        mqtt_client.publish(topic_PUB_motor_status.c_str(), String(intensidadDeseada).c_str());
      }
    }
  }

  // Apaga el LED para visulizar el cambio de intensidad del motor.
  if (digitalRead(LED2) == LOW && ahora - ultimo_cambInten >= 100)
  {
    digitalWrite(LED2, HIGH);
  }
}
