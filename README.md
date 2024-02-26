# Sistema de domotización y seguridad para una vivienda

El presente proyecto desarrolla y explica el trabajo final de la asignatura Informática Industrial, del Grado en Ingeniería Electrónica, Robótica y Mecatrónica. 

El proyecto se divide en dos partes fundamentales, siendo estas las correspondientes al sistema de domotización y al sistema de seguridad.

Como se explicará en mayor profundidad posteriormente, el sistema de seguridad estará comprendido principalmente por una cámara con un sistema de movimiento que permitirá visualizar su entorno en 360 grados. Esta implementación también ha sido usada para la implementación de un robot móvul para la asignatua Laboratorio de Robótica. Debido a la gran complejidad de este sistema, se ha creado un repositorio a parte donde explica toda esta parte del proyecto con la profundidad que ésta requiere, siendo posible acceder a dicho repositorio a través del siguiente enlace: https://github.com/dxvidlf/Camara-para-Robot-Movil

Todos los códigos mencionados en el documento se encuentran disponibles en la carpeta "Códigos".

## Introducción

En la era digital actual, la conectividad y la automatización desempeñan un papel fundamental en la transformación de nuestros hogares en entornos inteligentes y seguros. Con el rápido avance de la Internet de las Cosas (IoT), la posibilidad de integrar dispositivos y sensores inteligentes en el entorno doméstico ha creado oportunidades significativas para mejorar la seguridad y la gestión eficiente de nuestros hogares.

De este modo, nuestro proyecto implementa un sistema de sensores inteligentes para la monitorización y actuación sobre diferentes elementos de nuestro hogar. Hemos querido darle un plus a nuestro proyecto orientado sobre todo a la seguridad del hogar, tanto de cara a robos o allanamientos, implementando cámaras de seguridad, sensores de movimiento y alarmas, como también de cara a fugas de gas e incendios, añadiendo sensores de gas y humo.

Todo ello será controlable desde el móvil gracias a un bot de Telegram con el que podremos conocer el estado de los sensores y controlar ciertos actuadores. También, por supuesto se podrá interactuar con todo el sistema mediante un Dashboard completo, con capacidad para el control de todas las funcionalidades y el monitoreo de los datos, así como la integración con asistentes virtuales como Alexa o Siri.

## Objetivos
El objetivo principal que se persigue satisfacer con el presente proyecto es el diseño e implementación de un sistema de domotización para una vivienda, facilitando una interfaz amigable con la que el usuario final pueda interactuar con los elementos que constituyen dicho sistema. Además, se implementará de la misma forma un sistema de seguridad para el hogar tanto desde el punto de vista de la protección frente a accidentes como fugas de gas, incendios, etc. como desde el punto de vista de la protección frente a intentos de allanamiento, contando para ello con sensores que detecten la presencia de intrusos y con una cámara de seguridad.

Para llevar a cabo dicho objetivo, se hará uso de entornos de programación tales como NodeRed y Arduino, siendo la primera la principal donde se desarrollará la lógica de programación que rija nuestro proyecto, y quedando por tanto la segunda destinada a la programación de cada parte individual del sistema de domotización.

Para hacer aún más cercana y realista al usuario la experiencia del sistema de domotización se ha creado un entorno de desarrollo para el mismo dentro de una Raspberry Pi 3b+, lo cual tal y como se explicará más adelante facilitará el acceso remoto a su sistema local, dando así la posibilidad a dicho usuario de poder gestionar el control y la seguridad de su vivienda desde cualquier parte.

<p align="center">
  <img src="/fotos/image.png">
</p>

## Configuración del entorno de desarrollo

Como se ha explicado anteriormente, el entorno sobre el que se ha trabajado para el desarrollo del proyecto está contenido dentro de una Raspberry Pi 3b+. El dispositivo es un micrordenador con prestaciones limitadas con respecto a un ordenador común, pero suficientes para el uso que se le dará a este proyecto y, sobre todo, con un consumo muy reducido de energía. 

A continuación, se detallan las fases de configuración seguidas para la creación del entorno de desarrollo contenido en la Raspberry Pi 3b+.

### Configuración inicial de Raspberry Pi OS

El primer paso de la configuración del entorno es la instalación de Raspberry Pi OS (anteriormente conocido como Raspbian). Para ello, se ha hecho uso de la herramienta Raspberry Pi /fotos/imager, la cual se encargará de flashear nuestra tarjeta microSD con el sistema operativo que deseemos para nuestra Raspberry. Una vez completado el proceso de escritura y verificación del sistema, podemos introducir la tarjeta microSD en el dispositivo (no sin antes conectar el mismo a un teclado, ratón y monitor para visualizar la interfaz de usuario) para comenzar con las configuraciones iniciales tales como la creación de un usuario, la contraseña, zona horaria, etc.

Tras la configuración del sistema, se configura una de las cosas más importantes del proceso, una dirección IP estática. Esa característica nos da facilidad para hacer el resto de las configuraciones posteriores según una dirección IP que sabemos que no cambiará. 

<p align="center">
  <img src="/fotos/image-1.png">
</p>

Para llevar a cabo esta tarea, accedemos al archivo /etc/dhcpcd.conf, y creamos la interfaz de red correspondiente para asignar la dirección IP estática. Es importante asignar una dirección alta, para evitar posibles conflictos en la asignación dinámica de IPs del servidor DHCP de nuestro router, aunque posteriormente solucionaremos esto. Nuestra interfaz configurada consta de las siguientes líneas.

```shell
interface eth0
static ip_address=192.168.1.254/24
static routers=192.168.1.1
static domain_name_servers=192.168.1.1 8.8.8.8
```

<p align="center">
  <img src="/fotos/image-2.png">
</p>

Esto además nos permitirá saber en qué dirección estará siempre nuestro dispositivo de cara a su conexión SSH mediante programas como RealVNC (el cual viene ya instalado por defecto en la Raspberry), por lo que a partir de aplicar esta configuración podemos prescindir de los periféricos del dispositivo y visualizar su interfaz de usuario desde cualquier otro dispositivo de nuestra área local.

<p align="center">
  <img src="/fotos/image-3.png">
</p>

### Instalación de Docker y Portainer

Para la instalación de todos los programas que vamos a usar en nuestro proyecto, haremos uso de Docker. Esta es una herramienta muy útil, dado que permite la instalación de contenedores en nuestro dispositivo de una forma simple y cómoda, con una gran comunidad detrás y soporte para muchas plataformas. 

Sin embargo, Docker no dispone actualmente de una interfaz de usuario para Linux Raspbian, por lo que adicionalmente hemos instalado la herramienta Portainer, la cual brinda al usuario una interfaz de control de los contenedores del dispositivo de forma externa al mismo, ya que se puede acceder a ella igualmente desde cualquier dispositivo del área local. 

Para la instalación de Docker, simplemente hay que ejecutar el siguiente comando en la terminal.

```shell
curl -fsSL https://get.docker.com | sh
```
Para la instalación de Portainer hay que ejecutar los siguientes comandos.
```shell
sudo docker pull portainer/portainer-ce:latest
sudo docker run -d -p 9000:9000 --name=portainer --restart=always -v /var/run/docker.sock:/var/run/docker.sock -v portainer_data:/data portainer/portainer-ce:latest
```
Con esto, y tras la respectiva configuración inicial de Docker y Portainer (la cuál no se va a explicar en este proyecto ya que lo alargaría en exceso), obtenemos una interfaz de usuario accesible desde el área local como la siguiente. Desde ahí, se pueden acceder a todos los contenedores para detenerlos, reiniciarlos o lanzarlos, ver los logs de cada uno, ejecutar comandos como usuario root, etc.

<p align="center">
  <img src="/fotos/image-4.png">
</p>

### Instalación de los contenedores del proyecto

Para la instalación de todos los contenedores utilizados en este proyecto, se ha seguido la siguiente metodología:

•	Búsqueda del contenedor necesario y su versión compatible con arm64 (la arquitectura de nuestra Raspberry) en Docker Hub.
•	Configuración de la pila de lanzamiento del contenedor, donde entre otras cosas se especifica la política de reinicio, los puertos expuestos, las variables de entorno necesarias, los volúmenes necesarios, etc.
•	Lanzamiento del contenedor y verificación de su funcionamiento.

Los contenedores instalados son los siguientes:

#### Nodered

Programa principal en el que se va a controlar el flujo de nuestro proyecto. Se accede a él mediante el puerto 1880 de nuestro dispositivo. De cara a su acceso desde fuera del área local, se ha modificado el archivo settings.js para ofrecer un sistema de autenticación con usuario y contraseña, ofreciendo permisos de solo lectura o totales en función del usuario que acceda al sistema.

#### DuckDNS

Aunque nosotros sepamos que nuestro dispositivo tiene una cierta dirección IP dentro de nuestra área local, dicha dirección IP de cara al exterior de nuestra red es dinámica, y puede cambiar cada vez que el router se reinicie. 

Por ello, existe una herramienta denominada Dynamic Domain Name Server, la cual permite asociar una dirección IP dinámica al nombre de un dominio previamente asignado. 

<p align="center">
  <img src="/fotos/image-5.png">
</p>

Dicha asignación queda registrada en unos servidores que ofrecen sus servicios de redireccionamiento DNS al usuario, por lo que para este proyecto haremos uso del servidor DuckDNS para que cada vez que la Raspberry se reinicie, podamos saber la IP pública en la que está operando y poder acceder así a sus puertos mediante el nombre que le hayamos asignado al dominio. En este caso, hemos implantado 3 dominios: 

•infindgrupo10nodered.duckdns.org: este dominio apuntará al puerto donde se encuentra operando NodeRed en nuestra Raspberry, el 1880.

•infindgrupo10portainer.duckdns.org: este dominio apuntará al puerto donde se encuentra operando Portainer en nuestra Raspberry, el 9000.

•infindgrupo10cam.duckdns.org: este dominio apuntará a donde se encuentra retransmitiendo video en streaming nuestra cámara de seguridad, por tanto, la IP a la que apunta será la de nuestra cámara dentro de nuestra área local, y el puerto al que apuntará será el 81.

#### NginxProxyManager

Su función es implantar en nuestra Raspberry un servidor de proxy inverso, lo cual facilitará el redireccionamiento de los puertos de las peticiones externas a las IPs y los puertos de nuestra área local. Además, esta herramienta nos ofrece la posibilidad de crear listas de acceso con las que implantar métodos de autenticación a nuestros dominios, y cifrados de conexiones mediante certificados SSL generados con el servicio letsencrypt. Este contenedor usa los puertos 80, 443 y 81, siendo estos los encargados del tráfico http, https y la gestión del servidor, respectivamente.

<p align="center">
  <img src="/fotos/image-6.png">
</p>

### Configuración del router de nuestra red local

Lo primero que hay que configurar en nuestro router es el rango de direcciones IP dinámicas que puede administrar el servidor DHCP implantado en el router. En nuestro caso, hemos impuesto un rango de direcciones desde la 192.168.1.10 hasta la 192.168.1.150, lo cual es más que necesario para nuestra red local. 

<p align="center">
  <img src="/fotos/image-7.png">
</p>

Por tanto, tenemos el resto de las direcciones de nuestra red disponibles para usarse a nuestro gusto, como hemos hecho con la dirección IP fija de la Raspberry. Con esto, nos hemos asegurado de que en ningún momento el servidor DHCP pueda asignarle a un nuevo dispositivo de nuestra red la misma IP que cualquier otro dispositivo con una IP fija, y evitando así un fallo catastrófico en la red.

Una vez solucionado esto, procedemos a la configuración de los puertos NAT/PAT. Gracias al servidor DuckDNS podemos saber dónde se encuentra nuestra Raspberry Pi y podemos acceder a ella desde fuera de nuestra área local. Sin embargo, en los routers domésticos los puertos de nuestra área local están cerrados de cara al exterior, por lo que cualquier petición externa es denegada en primera instancia. 

Para solucionar esto, podemos abrir los puertos que usen cada aplicación para acceder a ellos desde fuera. Sin embargo, esto supondría un compromiso en la seguridad de nuestra área local, dado que cualquier persona podría acceder a nuestra máquina o incluso hacerse pasar por ella al no usar una comunicación cifrada con certificado, quedando así expuestos al robo de contraseñas y datos en general. 

Para solventar este problema, entra en juego NginxProxyManager. Este servidor de proxy inverso hace que todas las peticiones que entren al router, ya sean cifradas o no, pasen a través de él, aportando a dicha conexión la seguridad mediante certificado SSL mencionada anteriormente, ya que hemos configurado dicho servidor para forzar todas las conexiones que entren como http a ser https. 

Además, a ciertos servicios que no ofrecen algún sistema de autenticación de forma nativa como infindgrupo10cam.duckdns.org, se les puede aplicar autenticación básica mediante usuario y contraseña con una lista de acceso configurada en el NginxProxyManager. 

Sin embargo, los otros servicios como son NodeRed y Portainer si implementan un sistema de seguridad de forma nativa, por lo que no es necesario crear un sistema de autenticación para estos.

Por tanto, lo único que nos queda es abrir los puertos de http y https de nuestro router para que las peticiones puedan llegar a NginxProxyManager, siendo estos el 80 y el 443, respectivamente.

Y con esto, hemos creado un entorno de trabajo seguro para trabajar remotamente en la creación del proyecto, además de ofrecer el acceso al proyecto final para el usuario desde cualquier parte. 

<p align="center">
  <img src="/fotos/image-8.png">
</p>

# Diseño hardware y esquema de conexionado

El desarrollo del hardware de este proceso no es complejo, pero es importante que se explique y detalle en cada parte y componente. Para ello se numerarán todos los componentes, se presentará un esquema de conexionado y se explicará lo que se pretende lograr en cada caso.

## Listado de placas de desarrollo usadas

### ESP8266 NODEMCU ESP-12E CP2102

La placa de desarrollo ESP8266 NodeMCU ESP-12E CP2102 es el componente central en este proyecto. Se trata de una plataforma muy versátil y potente que integra el módulo ESP8266 con conectividad Wi-Fi permitiendo la comunicación bidireccional. 

Su amplia gama de pines GPIO (General Purpose Input/Output) ofrece flexibilidad para conectar una variedad de sensores, actuadores y otros dispositivos periféricos. Además, su compatibilidad con el entorno de programación Arduino y el ecosistema de desarrollo de software permite escribir y cargar código de forma sencilla. 

<p align="center">
  <img src="/fotos/image-9.png">
</p>
El ESP8266 NodeMCU ESP-12E CP2102 es una placa de desarrollo versátil que cuenta con 17 pines GPIO digitales, permitiendo la implementación de PWM de 10 bits en los pines GPIO0 al GPIO15. Algunos GPIO, como el GPIO16, tienen usos específicos, como despertar la placa del modo de suspensión profunda. Sin embargo, se recomienda evitar el uso de ciertos pines, como GPIO6 al GPIO11, que no son accesibles a través de la placa de desarrollo (excepto el 9 y el 10). Además, se destaca la presencia de pines dedicados para SPI y la conversión analógica a digital (ADC0) en A0 con un rango de entrada de 0 a 3.3V. La placa también incluye leds conectados a GPIO2 y GPIO16, operando con lógica invertida. Por último, los botones de reset y flash controlan reinicios y modos de carga, siendo este último activado al poner GPIO0 (D3) a nivel bajo.

### Arduino Nano

El Arduino Nano es una placa de desarrollo compacta basada en microcontroladores de la familia ATmega, fabricada por Arduino. Se destaca por su tamaño reducido y su capacidad para realizar diversas tareas de programación y control. El Nano cuenta con entradas y salidas digitales y analógicas, puertos de comunicación, y se alimenta a través de un conector USB o una fuente de alimentación externa. 

Es popular en proyectos de electrónica y programación debido a su versatilidad y tamaño conveniente, siendo especialmente útil en aplicaciones donde el espacio es limitado. Además, el Arduino Nano es compatible con el entorno de desarrollo Arduino, lo que facilita la programación y la creación de prototipos.

<p align="center">
  <img src="/fotos/image-10.png">
</p>

### ESP-32 Wrover Cam

La ESP32 WROVER-CAM, basada en el chip ESP32, destaca por su diseño compacto y versátil, integrando conectividad Wi-Fi y Bluetooth junto con una cámara que emplea el sensor OV-2640. Este módulo es especialmente adecuado para proyectos de IoT, video vigilancia, captura de imágenes y transmisión en tiempo real, gracias a su potente procesador dual-core Xtensa LX6 que garantiza un rendimiento eficiente.

Uno de los puntos destacados de este módulo son sus GPIOs multipropósito, que ofrecen flexibilidad en el diseño y la implementación de proyectos. Estos pines pueden configurarse para diversas funciones, como entrada analógica, salida digital o comunicación serial, lo que amplía las posibilidades de integración con otros dispositivos y sensores.

<p align="center">
  <img src="/fotos/image-11.png">
</p>

### ESP-32 Wroom 32

La ESP32 WROOM-32, basada en el chip ESP32, es un módulo compacto y versátil que incorpora conectividad Wi-Fi y Bluetooth, siendo una opción ideal para proyectos de IoT, automatización, y aplicaciones inalámbricas. Equipada con un procesador dual-core Xtensa LX6, ofrece un rendimiento eficiente para tareas complejas. 

Los GPIOs versátiles proporcionan flexibilidad en el diseño, permitiendo configurar los pines para diversas funciones, como entrada/salida digital, comunicación serial, o lectura analógica. Esto facilita la integración con sensores y otros componentes externos, adaptándose a diferentes requisitos de proyectos.

La capacidad para ejecutar aplicaciones web directamente en el módulo facilita la visualización remota de datos y la configuración a través de interfaces basadas en navegador.

<p align="center">
  <img src="/fotos/image-12.png">
</p>

## Listado de sensores utilizados

### Sensor de movimiento PIR infrarrojo HC-SR501

Este sensor, basado en tecnología de detección de infrarrojos pasivos, identifica cambios en el calor emitido por objetos en su campo de visión. Gracias a su ajuste y sensibilidad, permite detectar movimientos con precisión.

<p align="center">
  <img src="/fotos/image-13.png">
</p>

### Sensor de gas MQ-6

El módulo de sensor de gas MQ-6 está diseñado para detectar gases combustibles como propano, isobutano y butano en el entorno. Basado en su principio de detección de resistencia, este sensor es capaz de identificar la presencia de estos gases en concentraciones específicas. Es importante ya que nos permitirá integrar sistemas de alerta temprana ante la presencia de gases potencialmente peligrosos. 

<p align="center">
  <img src="/fotos/image-14.png">
</p>

### Sensor de monóxido de carbono MQ-7

Este sensor está diseñado específicamente para detectar la presencia de monóxido de carbono (CO) en el ambiente. Igualmente, se basa en un principio de detección de resistencia, y tiene la capacidad de identificar concentraciones específicas de CO, un gas incoloro, inodoro y altamente tóxico. Igual que el anteriormente descrito, su sensibilidad también es ajustable.

<p align="center">
  <img src="/fotos/image-15.png">
</p>

### Sensor DHT11 para humedad y temperatura

Este sensor digital de baja complejidad cuenta con un sensor de temperatura y un sensor de humedad relativa integrados en un solo chip. Tiene un diseño preciso y de bajo costo y proporciona mediciones confiables de temperatura en un rango de 0 a 50 grados Celsius y de humedad relativa entre 20% y 90%. Este sensor es ampliamente utilizado en aplicaciones de monitoreo ambiental.

<p align="center">
  <img src="/fotos/image-16.png">
</p>

### Fotorresitor

El fotorresistor, también conocido como LDR (Light Dependent Resistor), es un componente electrónico que varía su resistencia eléctrica en función de la intensidad de la luz incidente. Esta variación en la resistencia del fotorresistor permite medir los niveles de luminosidad en su entorno. 

Cuando la luz incide sobre el fotorresistor, su resistencia disminuye, y cuando la luz disminuye, su resistencia aumenta.

<p align="center">
  <img src="/fotos/image-17.png">
</p>

### Panel de teclado numérico matricial de membrana 4x4

Se trata de un dispositivo de entrada que consiste en una matriz de botones dispuestos en filas y columnas. Este teclado es utilizado para introducir datos numéricos. Permite ofrecer múltiples entradas a través de una conexión sencilla lo que lo hace ideal para aplicaciones como control de acceso, sistemas de seguridad.

<p align="center">
  <img src="/fotos/image-18.png">
</p>

### Sensor final de carrera

Es un dispositivo que conmuta internamente sus conexiones al recibir una pulsación mecánica, haciendo que la corriente de la base pase a circular por el pin NO (Normally Open) en vez de por el pin NC (Normally Closed). En nuestro caso, haremos uso de este dispositivo para detectar cuando el motor que controla la posición de la cámara llega al origen del recorrido y realizar así su calibración.

<p align="center">
  <img src="/fotos/image-19.png">
</p>

## Listado de actuadores usados

### Buzzer pasivo MH-FMD

El buzzer pasivo MH-FMD es un componente acústico que, al recibir una señal eléctrica, produce sonidos audibles. Su funcionamiento se basa en la variación de frecuencia o duración de la señal eléctrica aplicada, generando diferentes tonos audibles.

<p align="center">
  <img src="/fotos/image-20.png">
</p>

### Microservo 9G SG90

Este servo es pequeño y ligero y posee una gran potencia de salida. Este modelo es muy fácil de programar y es idóneo para el uso que desempeñará en el proyecto.

<p align="center">
  <img src="/fotos/image-21.png">
</p>

### Motor de engranaje DC

El motor de engranaje DC es un componente electromecánico que convierte la energía eléctrica en movimiento mecánico. Tiene un tamaño compacto y es bastante versátil, este motor se utiliza para aplicaciones que requieren un torque moderado y un tamaño reducido. Para el control de la velocidad requerirá, de forma adicional, de un transistor NPN, como podría ser el PN2222.

<p align="center">
  <img src="/fotos/image-22.png">
</p>

### Diodos LED

Son utilizados también a modo indicativo en el proyecto tanto para representar la iluminación de un entorno como el estado de ciertos actuadores y sistemas. Para su utilización, serán también necesarias una serie de resistencias que limiten la corriente que pase a través de los diodos.

<p align="center">
  <img src="/fotos/image-23.png">
</p>

### Motor paso a paso 28BYJ-48

Este motor unipolar, aunque no es demasiado potente ni excesivamente rápido, cumple con los requisitos necesarios para el desarrollo de su aplicación en este proyecto. Se trata de un motor de 4 Fases con una resistencia de 50 Ω y un par motor de 34 Newton / metro más o menos 0,34 Kg por cm. Consume unos 55 mA y es de 8 pasos por vuelta por lo que, para completar un giro completo de 360º, necesita 512 impulsos.

Debido a que un motor paso a paso consume una corriente elevada y puede necesitar una tensión mayor a los 5 VDC de nuestro Arduino, no podemos realizar una conexión directa entre ellos. Se requiere un manejador (driver) o interface para brindar al motor la tensión y la corriente necesarias, por eso utilizamos el siguiente módulo.

<p align="center">
  <img src="/fotos/image-24.png">
</p>

### Driver ULN2003A

Este módulo cuenta con el controlador ULN2003 que posee una configuración Darlington y se encarga de incrementar el nivel de corriente para suplir la potencia necesaria que solicita el motor paso a paso. permite la adaptación de cualquier motor paso a paso cuando se conecta directamente. Posee cinco lí­neas de conexión para el motor, además de cuatro fases.

<p align="center">
  <img src="/fotos/image-25.png">
</p>

### Convertidor Buck Mini 360 DC

El convertidor buck Mini 360 DC es un dispositivo compacto y eficiente diseñado para convertir tensiones de entrada mayores a tensiones de salida más bajas de manera controlada. Su tensión de salida se ajusta mediante el potenciómetro integrado. Esto permite adaptar la salida a las necesidades específicas de un proyecto o dispositivo. 

Este tipo de convertidor es comúnmente utilizado en aplicaciones electrónicas para alimentar dispositivos que requieren una tensión de alimentación menor que la disponible. En nuestro caso, lo usaremos para adaptar la salida de 12 Voltios de un cojunto de pilar y así poder alimentar dispositivos como el mando que controle la cámara, o el propio ecosistema de la cámara.

<p align="center">
  <img src="/fotos/image-26.png">
</p>

## Módulos para el sistema de domotización

A continuación, se adjuntan los esquemas de todos los módulos que componen el sistema de domotización de la vivienda.

Placa principal de desarrollo general:

<p align="center">
  <img src="/fotos/image-27.png">
</p>

Placa cliente ESPNOW:

<p align="center">
  <img src="/fotos/image-28.png">
</p>

Placa híbrida, servidor ESPNOW, acceso al hogar:

<p align="center">
  <img src="/fotos/image-29.png">
</p>

El montaje experimental queda de la siguiente forma:

<p align="center">
  <img src="/fotos/image-30.png">
</p>

## Módulos para la seguridad de la vivienda

Toda la información de este apartado se encuentra recogida en el siguiente repositorio:  https://github.com/dxvidlf/Camara-para-Robot-Movil

# Diseño software del sistema de domotización

## Diagrama de bloques o flujo del programa

El diagrama de bloques del sistema de domotización es el siguiente.

<p align="center">
  <img src="/fotos/image-31.png">
</p>

## Descripción del funcionamiento

Los sensores conectados a la placa ESP8266 mandan la información por MQTT hasta NodeRed donde se procesa la información de los sensores y se guarda en bases de dato para su posterior uso. Con Telegram, el Dashboard y los datos de guardados en las bases de datos se mandan de nuevo por MQTT las acciones que los actuadores deban realizar. Todo ello se realiza con 3 placas ESP8266 donde cada una de ellas tiene una función:

1.	Una placa tiene conectada los sensores orientados a la seguridad: humo, gas y movimiento. Esta manda mensajes por MQTT cada 3 segundos.

2.	La segunda placa manda información de las variables atmosféricas y la luminosidad por protocolo ESPNOW cada 5 minutos.

3.	La tercera y última placa recibe los mensajes ESPNOW y los publica por MQTT. Esta placa también tiene incorporado un teclado numérico mediante el cual se da el acceso a la vivienda y un servomotor que actúa como cerradura de la puerta, la cual se abre automáticamente al teclear el código correcto.

## Descripción de los códigos de Arduino

### Librerías usadas

El proyecto con ESP8266 hace uso de varias librerías clave para implementar funcionalidades específicas. A continuación, se presenta una descripción detallada de cada una de estas librerías.


#include <ESP8266WiFi.h>: La librería ESP8266WiFi es fundamental para cualquier proyecto con ESP8266 que requiera conectividad inalámbrica. Proporciona las funciones necesarias para conectar el dispositivo a una red WiFi. Permite realizar tareas como la búsqueda de redes disponibles, la conexión a una red WiFi específica y la gestión de la conexión. 

#include <DHTesp.h>: DHTesp es una librería diseñada para trabajar con los sensores de temperatura y humedad DHT, como en nuestro caso el DHT11. Facilita la lectura de los datos de temperatura y humedad del ambiente.

#include <PubSubClient.h>: La librería PubSubClient proporciona una interfaz cliente para trabajar con protocolos MQTT. Es esencial para el proyecto, permite comunicarnos con un broker MQTT para enviar y recibir mensajes de esta manera podemos procesar esa información con Node-RED . Permite al dispositivo publicar mensajes en topics específicos y suscribirse a topics para recibir datos, lo que es fundamental para la interacción y control en sistemas de automatización y control remoto.

#include <ArduinoJson.h>: ArduinoJson es una librería de manejo de JSON para Arduino y microcontroladores compatibles. Permite serializar y deserializar datos JSON, facilitando el intercambio de datos estructurados.

#include <string>: Librería estándar de C++ para el manejo de cadenas de texto. Ofrece funcionalidades para manipular cadenas, como concatenación, comparación y búsqueda dentro de las mismas.

#include <queue>: Es una librería estándar de C++ que se utiliza para implementar colas de datos. Una cola es una estructura de datos tipo FIFO (First In, First Out) que permite almacenar y recuperar datos de manera ordenada. 

#include <espnow.h>: Es una librería para la implementación del protocolo ESP-NOW en dispositivos ESP. ESP-NOW es un protocolo de comunicación que permite la transmisión de datos de manera directa y eficiente entre dispositivos ESP. Esta librería permite una comunicación rápida y confiable entre dispositivos cercanos sin la necesidad de una red WiFi.

#include <Keypad.h>: Es una librería diseñada para la interfaz con teclados matriciales. Permite leer entradas de teclados de varios tamaños, facilitando la interacción del usuario con el dispositivo. Es útil para una interfaz de entrada manual, como el terminal de entrada numérico.

#include <servo.h>: Es una librería que nos permite controlar una variedad de motores servo y lo hace de manera eficiente al utilizar los temporizadores disponibles en la placa. Esta librería puede controlar hasta 12 servos utilizando solo un temporizador, lo que permite una gestión eficiente de los recursos de la placa.

#include <ESP8266httpUpdate.h>: Permite la actualización del firmware del dispositivo ESP8266 a través de HTTP (Over-The-Air - OTA). Esta funcionalidad es esencial para actualizar el software del dispositivo de forma remota, sin la necesidad de una conexión física.
#include "Button2.h" La librería Button2.h se utiliza para manejar eventos de botones en el ESP8266. Facilita la detección de pulsaciones, doble clic, clic largo, entre otros, permitiendo una interacción avanzada del usuario con el dispositivo. 

Cada una de las placas que componen el proyecto tiene su respectivo código para la ejecución de sus funciones. Queda entonces diferenciado en 3 archivos con extensión “.ino”.


### “PRINCIPAL_PROYIOT_GR10.INO”: ESP-NOW MQTT GATEWAY CON ESP8266 

Las funciones que se busca cubrir son las relacionadas con el sistema de alarma además de ciertos sensores de seguridad, el control domótico de la iluminación y de un ventilador junto con la posibilidad de actualizar este propio código de manera remota. Para ello el flujo de operaciones será el que se describe a continuación.

En primer lugar, se inicializan todas las constantes y variables necesarias para el programa, entre ellas las relacionadas con la conexión (wifi y MQTT), los topics para el envío de los mensajes MQTT y los puertos GPIO que se usan en la placa que son:

```arduino
int LED2 = 16;       // D0
int GPIO_Movim = 5;  // D1
int GPIO_LED = 4;    // D2
int GPIO_Alarm = 2;  // D4
int LED1 = 2;        // D4
int GPIO_Motor = 14; // D5
int GPIO_MQ6 = 12;   // D6
int GPIO_MQ7 = 13;   // D7
```
La función de setup es la primera que se ejecuta en el código y en ella se inicializan todos los pines GPIO como entrada o salida según su función, también los procesos de conexión a la red WiFi y configuración del cliente MQTT (junto con la función de callback para el envío de mensajes MQTT) y se muestra la información de inicio del programa. También se inicializan las funciones del botón flash de la placa con la librería Button2 y, por último, se busca una actualización FOTA (Firmware Over the Air) para actualizar el código de la placa su hubiera alguna versión disponible.

Una vez se ha completado el setup, pasamos al bucle de ejecución principal. En cada una de sus iteraciones se hace lo siguiente:

•	Se comprueba si seguimos teniendo conexión con el cliente MQTT y si no es así, se intenta reconectar.

•	Ordenamos que las librerías mqtt_client y button recuperen el control.

•	Verificamos si es necesario realizar una actualización FOTA cada hora.

•	Enviamos los datos de conexión y de los sensores por MQTT cada 3000 segundos.

•	Ajustamos la intensidad del motor de forma gradual a una velocidad de ±1% cada 10ms.

Para realizar esto, en el código se definen algunas funciones que se encargan de realizar los procesos descritos a continuación:

La función conecta_wifi() inicia la conexión con las credenciales hasta que la conexión sea exitosa. De igual forma, la función conecta_mqtt() inicia la conexión con el broker con las credenciales establecidas y, si falla, lo reintenta cada 5 segundos hasta un máximo de 6 veces. Si falla el intento de conexión 6 veces, reinicia la placa para volver a iniciar y hacer de nuevo el setup. Los topics de publicación y suscripción a los que nos conectamos son:

- Topic publicacion conexión: II10/ESP4173425/conexion
- Topic publicacion datos: II10/ESP4173425/datos
- Topic publicacion motor : II10/ESP4173425/motor/status
- Topic suscripcion motor : II10/ESP4173425/motor/cmd
- Topic publicacion switch : II10/ESP4173425/switch/status
- Topic suscripcion switch : II10/ESP4173425/switch/cmd
- Topic publicacion alarma : II10/ESP4173425/alarm/status
- Topic suscripcion alarma : II10/ESP4173425/alarm/cmd
- Topic suscripcion FOTA : II10/ESP4173425/FOTA

La siguiente, y quizá la más importante, función es la de procesa_mensaje() la cual recibe un topic y un payload y procesa los mensajes que llegan realizando las acciones pertinentes con los actuadores conectados a la placa como actualizar la intensidad deseada del motor y encender o apagar la iluminación o la alarma. También contempla la posibilidad de que el mensaje recibido sea dedicado a actualizar el firmware mediante una actualización FOTA.

Hay otras 3 funciones que gestionan que esta actualización FOTA se realice correctamente que son: inicio_OTA(), progreso_OTA() e intenta_OTA().
Por último, tenemos otra serie de pequeñas funciones para procesar el pulsado del botón.

Un simple click, singleClick(), enciende el ventilador a la intensidad previamente establecido o lo apaga; un click largo, longClick(), comprueba si hay alguna actualización FOTA disponible; y un doble click, doubleClick(), enciende el ventilador a máxima intensidad.

### “ESPNOW_SERVER_PROYIOT_GR10.INO”: ESP-NOW MQTT GATEWAY CON ESP8266

El proyecto configura un ESP8266 para servir como un puente para la transmisión de mensajes entre el protocolo ESP-NOW y un servidor MQTT. Este enfoque permite una comunicación eficiente entre dispositivos IoT que no están conectados a internet y un servidor MQTT. 

ESP-NOW y MQTT: Integración y Flujo de Datos

El ESP8266 se configura para operar bajo el protocolo ESP-NOW, un protocolo de comunicación de bajo consumo desarrollado por Espressif Systems. Este protocolo es ideal para escenarios IoT donde se requiere eficiencia energética y una comunicación rápida y segura entre dispositivos. En este proyecto, el ESP8266 recibe mensajes de otro dispositivo ESP-NOW y los procesa para su posterior transmisión a un servidor MQTT. En este sistema, los mensajes ESP-NOW son convertidos a formato JSON y publicados en un servidor MQTT, donde pueden ser distribuidos y gestionados de manera eficiente.
La operación se divide en varias fases clave:

Teclado Numérico y Gestión de Accesos

Una característica distintiva del proyecto es la incorporación de un teclado numérico. Este teclado permite la interacción directa del usuario con el dispositivo. Mediante la entrada de un PIN, el usuario puede controlar accesos o activar determinadas funciones. Este método de interacción agrega una capa de seguridad y personalización al sistema.

La gestión de la entrada del teclado numérico se realiza continuamente. En cuanto al funcionamiento: Se lee y procesa la tecla presionada. Si se completa la entrada de un PIN de 4 dígitos, el sistema verifica su corrección. Si el Pin introducido es el pin correcto de acceso enciende un LED verde como indicación de una entrada exitosa y se activa el servomotor accionando la cerradura de la puerta y permitiendo el acceso. Para el caso de un pin erróneo se enciende un led rojo mostrando que la operación ha sido fallida. Encapsula la información y la publica por MQTT de forma que queda registrado el acceso en una base de datos especifica con la fecha y hora correspondientes, el pin introducido y se ha accedido.

Para entrar en el modo de configuración, se debe teclear '####', seguido del pin de acceso actual. Una vez realizado esto, el usuario puede introducir el nuevo pin que desea establecer como correcto. Esta secuencia de acciones garantiza un método seguro y personalizable para modificar el pin de acceso, añadiendo una capa adicional de seguridad al sistema.

La inclusión de un teclado numérico y el manejo de PINs aporta una dimensión adicional al sistema. Permite a los usuarios interactuar directamente con el dispositivo, proporcionando una capa adicional de control y personalización.

La lectura de datos del teclado no interfiere en el procesamiento de mensajes ESPNOW. SI un mensaje llega mientras se introduce algún código, este será encolado para su posterior procesamiento. El sistema comprueba si hay mensajes en la cola. Si hay mensajes: Enciende el LED rojo para indicar que está procesando un mensaje. Establece conexiones con la red WiFi y el servidor MQTT. Cada mensaje se envía a través de MQTT. Después del envío, el cliente MQTT se desconecta y el dispositivo se reinicia para mantener el modo ESP- NOW activo.

Los mensajes ESP- NOW se convierten en un formato JSON para ser compatibles con MQTT. El ESP8266 se conecta a una red WiFi y luego al servidor MQTT para enviar los mensajes. Los mensajes se publican en el servidor MQTT bajo un topic específico, permitiendo su distribución y manejo en la red IoT. Para los mensajes recibidos por ESP-NOW el topic es dinámico y depende del campo “topic” recibido. Para registrar los datos del teclado numérico se emplea el topic ‘AccesoPin’.

Actualizaciones Over-The-Air (OTA)

Una funcionalidad avanzada incluida en el proyecto es la capacidad de realizar actualizaciones Over-The-Air del firmware. Esto permite actualizar el software del dispositivo de forma remota, sin necesidad de una conexión física, lo cual es esencial para mantener actualizados y seguros los dispositivos IoT. Si el PIN coincide con ‘PinFOTA’ configurado como ‘#10#’, inicia un proceso de actualización Over-The-Air (OTA), lo cual es una característica que permite la actualización remota del firmware.

Manejo Eficiente del Tiempo y Estados

El sistema ha sido diseñado para manejar de manera eficiente los tiempos y estados, como el reseteo automático de la entrada del PIN si no se detecta actividad durante un tiempo predefinido. Esto previene entradas accidentales o incompletas y mejora la usabilidad del dispositivo.

LEDs Indicadores y Manejo de Estados

El proyecto utiliza LEDs para proporcionar indicaciones visuales del estado del sistema. Por ejemplo, un LED puede encenderse para indicar que un mensaje ha sido procesado correctamente o para señalar el estado de un acceso mediante PIN. Además, se ha implementado una lógica para el apagado automático de los LEDs, contribuyendo a la eficiencia energética del sistema. Después de un cierto tiempo (‘tiempoEncendido’), el estado del LED se cambia a apagado, lo que ayuda a reducir el consumo de energía y a proporcionar una indicación visual clara del estado actual del sistema.

### “ESPNOW_CLIENT_PROYIOT_GR10.INO”: ESP-NOW CLIENTE CON ESP8266

El programa para el ESP8266 diseñado como un cliente ESP-NOW MQTT constituye un ejemplo robusto y eficiente de cómo un dispositivo de Internet de las Cosas (IoT) puede ser optimizado para la transmisión de datos y la gestión eficiente de la energía. Este sistema de sensor utiliza ESP-NOW para la comunicación, enviando datos en formato JSON a una pasarela ESP-NOW -> MQTT. La identificación de la pasarela se realiza mediante su dirección MAC específica, lo que asegura una comunicación dirigida y segura.

Una de las características más notables del programa es su capacidad para entrar en un modo de sueño profundo, lo cual ocurre después de la transmisión de datos o en caso de error o tiempo de espera. Esta funcionalidad es crucial para conservar la energía, especialmente en aplicaciones donde los dispositivos funcionan con baterías y necesitan operar durante largos períodos. El dispositivo se despierta del sueño profundo mediante una conexión específica entre el GPIO16 y el pin RST, aunque esta conexión debe ser retirada durante la programación del dispositivo. La estructura del código se divide en varias funciones clave, cada una con responsabilidades bien definidas:

1.	Función byte2HEX: Esta función es esencial para convertir un byte en una cadena hexadecimal, facilitando la visualización de datos binarios como las direcciones MAC.

2.	Callback fin_envio_espnow: Se encarga de manejar la finalización de la transmisión de datos ESP-NOW. Informa sobre el éxito o fracaso de la transmisión e imprime la dirección MAC del destinatario, proporcionando una retroalimentación valiosa para la depuración.

3.	Función setup: Se inicia con la inicialización del puerto serie para permitir la comunicación y el monitoreo. Luego, el sensor DHT se configura y se inicia, lo que permite la lectura de datos de temperatura y humedad. La configuración de WiFi se establece en modo estación. También se inicializa el protocolo ESP-NOW y se configura el rol del dispositivo en la red, además de registrar un peer con la dirección MAC especificada.

4.	Función loop: Contiene la lógica principal del programa que se ejecuta continuamente. Comprueba el tiempo de espera para el envío de datos y activa el modo de sueño si es necesario.

5.	Función gotoSleep: Gestiona la entrada del dispositivo en el modo de sueño profundo. Registra y muestra el tiempo transcurrido en varias etapas del proceso antes de dormir, optimizando así la vida de la batería.

El proceso de transmisión de datos implica la lectura de los sensores para obtener datos de temperatura, humedad y luminosidad. Estos datos se empaquetan en un mensaje JSON, que luego se envía a la pasarela ESP-NOW especificada. El manejo eficiente del modo de sueño profundo, incluyendo el registro de varios sellos de tiempo, permite un ahorro significativo de energía, lo que es esencial para dispositivos IoT que a menudo operan en condiciones donde el acceso a la energía es limitado.

## Descripción de los flujos de NodeRed

En este primer bloque tenemos todo lo necesario para la captura de datos y almacenamiento de estos en las bases de datos para la Seguridad y los Datos. También se configura la pestaña del Dashboard. Podemos ver que tenemos dos subflows con funciones específicas: un monitor de seguridad y un monitor de datos, estos tienen como función configurar otras pestañas en el Dashboard con el objetivo de visualizar los datos de una manera más sencilla.

<p align="center">
  <img src="/fotos/image-32.png">
</p>
<p align="center">
  <img src="/fotos/image-33.png">
</p>
<p align="center">
  <img src="/fotos/image-34.png">
</p>

En la siguiente figura podemos ver la programación del bot de Telegram. Para el uso de los botones (explicado en el manual de usuario) hay que añadirlos en la función de mensaje de bienvenida. Cada botón está configurado con una callback_query diferente que desencadena la acción especificada. Arriba a la izquierda podemos ver que se guarda el chatId de la persona que lanza cualquier evento con callback_query (cualquier botón) para que la respuesta se envíe solo al usuario que mando la petición.

<p align="center">
  <img src="/fotos/image-35.png">
</p>

Este flujo está diseñado específicamente para integrar dispositivos de domótica con un asistente virtual como Alexa de Amazon. A través de este sistema, los usuarios pueden controlar dispositivos inteligentes en su hogar utilizando comandos de voz, lo que representa un salto significativo en la conveniencia y accesibilidad de la automatización del hogar. Podemos controlar el estado del led, el estado del motor y activar/desactivar la alarma con un simple comando de voz.

<p align="center">
  <img src="/fotos/image-36.png">
</p>

Tras esto tenemos la zona donde podemos hacer consultas a las diferentes bases de datos y borrarlas (estos hilos están desconectados para no borrar bases de datos de manera accidental).

<p align="center">
  <img src="/fotos/image-37.png">
</p>

Otra parte importante del Flow es la zona que nos informa en el dashboard de la última actualización de Firmware y añade el botón para poder actualizar el firmware del dispositivo de manera remota. 

<p align="center">
  <img src="/fotos/image-38.png">
</p>

# Diseño software del sistema de seguridad

## Diagrama de bloques o flujo de programa

El diagrama de bloques del sistema es el siguiente:

<p align="center">
  <img src="/fotos/image-39.png">
</p>

## Descripción del funcionamiento

El sistema controla todas las peticiones que se hacen desde el exterior, tanto los comandos de movimiento para el motor de la cámara como los comandos para hacer fotos y activar o desactivar el flash. Dichas peticiones no entran directamente a nuestra área local por el puerto de cada aplicación, sino que mantienen una conexión segura con nuestra Raspberry, cifrada con certificado SSL, y se redirigen a su puerto destino pasando por el servidor local NginxProxyManager.

Las peticiones pueden hacerse desde cualquiera de los medios implantados, ya sea desde la dashboard de Nodered, desde Telegram, o con los comandos de joystick programados en el mando wifi diseñado. Con ello, se le ofrece al usuario multitud de opciones disponibles para poder gestionar el sistema a su gusto.


### Librerías usadas

Esta parte del proyecto hace uso de las siguientes librerías:

#include <WiFi.h>: proporciona las funciones necesarias para conectar las placas ESP32 a una red WiFi. Permite realizar tareas como la búsqueda de redes disponibles, la conexión a una red WiFi específica y la gestión de la conexión. Además, en este caso también se usará para establecer una IP estática a la conexión de la ESP32 Wrover Cam.

#include <PubSubClient.h>: permite comunicarnos con un broker MQTT para enviar y recibir mensajes de esta manera podemos procesar esa información con Node-RED. Permite al dispositivo publicar mensajes en topics específicos y suscribirse a topics para recibir datos, lo que es fundamental para la interacción y control en sistemas de automatización y control remoto.

#include <ArduinoJson.h>: ArduinoJson es una librería de manejo de JSON para Arduino y microcontroladores compatibles. Permite serializar y deserializar datos JSON, facilitando el intercambio de datos estructurados.

#include <string>: Librería estándar de C++ para el manejo de cadenas de texto. Ofrece funcionalidades para manipular cadenas, como concatenación, comparación y búsqueda dentro de las mismas.

#include <Stepper.h>: esta librería es usada para el control del motor paso a paso que controla el sistema de movimiento de la cámara. Proporciona comandos muy útiles para controlar la velocidad y la dirección del motor.

#include <HTTPClient.h>: esta librería permite hacer la actualización al servidor DuckDNS de la IP pública actual de la cámara cada vez que es encendida o reiniciada.

#include <esp_camera.h> + <camera_pins.h>: estas librerías son específicas de todos los módulos ESP con sensor de cámara. En ellas se definen las funciones y elementos necesarios para el manejo de la comunicación entre el módulo ESP y el propio sensor de la cámara.

Cada una de las placas que componen el proyecto tiene su respectivo código para la ejecución de sus funciones. Queda entonces diferenciado en 3 archivos con extensión “.ino”.

#### “CAMERASERVER_GR10.INO”: IMPLEMENTACIÓN DE SERVIDOR DE VIDEO EN STREAMING, ATENCIÓN A CONTROL DE FOTOS, ENCENDIDO O APAGADO DEL FLASH, CONTROL DEL SISTEMA DE MOVIMIENTO Y ACTUALIZACIÓN DE IP PÚBLICA CON ESP32 WROVER CAM 

En primer lugar, se inicializan todas las constantes y variables necesarias para el programa, entre ellas las relacionadas con la conexión (wifi y MQTT), los topics para el envío de los mensajes MQTT y los puertos GPIO que se usan en la placa que son:

```arduino
const int ledVerde = 33;
const int ledRojo = 32;
const int left = 14;
const int rigth = 12;
const int txPin = 1;
const int pinflash = 15;
```

La asignación de los GPIOs en esta placa es crítica, ya que se ha estudiado detalladamente el pinout de la misma para no causar conflictos entre los pines usados por el propio sensor de la cámara y los pines que quedan libres.

La función de setup es la primera que se ejecuta en el código y en ella se inicializan todos los pines GPIO como entrada o salida según su función, también los procesos de conexión a la red WiFi y configuración del cliente MQTT (junto con la función de callback para el envío de mensajes MQTT), la configuración del sensor de la cámara, la inicialización del servidor del video en streaming y la actualización de la IP pública de la cámara. 

Una vez se ha completado el setup, pasamos al bucle de ejecución principal. En cada una de sus iteraciones se hace lo siguiente:

•	Se comprueba si seguimos teniendo conexión Wifi y conexión con el cliente MQTT y si no es así, se intenta reconectar.

•	Ordenamos que la librería mqtt_client recupere el control.

•	Mandamos una foto a modo de toma de seguridad en un intervalo de tiempo que está determinado mediante una variable que se recibe por MQTT, y corresponde con un slider disponible en la dashboard de Nodered. Por defecto, esta variable está inicializada a 60 minutos.

Para realizar esto, en el código se definen algunas funciones que se encargan de realizar los procesos descritos a continuación:

La función conecta_wifi() inicia la conexión con las credenciales hasta que la conexión sea exitosa. El estado de la conexión Wifi viene determinado por el estado de unos leds rojos y verde, conectados a la placa. De igual forma, la función conecta_mqtt() inicia la conexión con el broker con las credenciales establecidas y, si falla, lo reintenta cada 5 segundos.

Los topics de publicación y suscripción a los que nos conectamos son:

- Topic publicación conexión: II10/ESP 193997127210196/camara/conexion
- Topic publicación flash: II10/ESP 193997127210196/camara/responseFlash
- Topic publicación comando de foto rutinaria: II10/ESP 193997127210196/camara/fotoRutinaria
- Topic subscripción comando flash: II10/ESP 193997127210196/camara/requestFlash
- Topic subscripción tiempo de foto de seguridad: II10/ESP 193997127210196/camara/tiempo
- Topic subscripción comandos de movimiento de cámara:II10/ESP 36221669107172/mando/camara

La función es la de procesa_mensaje() se encarga de recibir el topic y el payload de los mensajes entrantes, para realizar las acciones pertinentes con los actuadores conectados a la placa. Atiende a las peticiones de encendido y apagado del flash de la cámara, a la actualización del tiempo de toma de fotos de seguridad, y a los comandos de movimiento del motor de la cámara, los cuales son codificados y pasados al Arduino Nano a través de los pines definidos como “left” y “rigth”. Realmente, la cámara tiene pines libres suficientes como para poder administrar por si misma el conexionado con el motor paso a paso. Sin embargo, el programa diseñado para el control de dicho motor hace que la rutina principal de la cámara (la gestión del servidor local donde implementa el video en streaming) quede congelada, por lo que se releva la tarea del control del motor al Arduino Nano de cara a tener un flujo de video constante durante el movimiento del motor de la cámara.

La función UpdateDuckDNS() es la encargada de hacer la petición http GET al servidor DuckDns para actualizar la IP pública de la cámara cada vez que ésta se reinicia. Al tener la cámara funcionando dentro de nuestra área local, esto realmente no sería necesario ya que la visualización del video desde la dashboard de NodeRed, aunque se haga desde fuera, se toma desde la información que transmite la cámara localmente. Sin embargo, con la idea de desvincular la dependencia de la dashboard de NodeRed con el servidor de la cámara se ofrece la posibilidad de ver el video en directo desde el mismo servidor nativo que implementa la cámara, para lo que se hace uso de la dirección infindgrupo10cam.duckdns.org para poder acceder a dicho video. Además, la conexión externa a nuestra área local con dicha dirección se gestiona también a través de NginxProxyManager, por lo que se ha creado una lista de acceso con usuario y contraseña para poder acceder al servidor, además de cifrar la conexión de nuevo mediante certificado SSL.

Para esto último, en la función conectaWifi() se configura una IP estática dentro de nuestra área local para la conexión de la cámara, de cara a indicarle a NginxProxyManager la IP donde estará siempre conectada la cámara y el puerto al que tiene que hacer la redirección cuando se hace una petición de conexión desde el exterior,  siendo en este caso el puerto 81.


#### “CONTROLMOTOR_GR10”: CONTROLADOR DEL MOTOR DE LA CÁMARA CON ARDUINO NANO

Este programa es el encargado de controlar el motor de la cámara. Para ello, se definen primero los pines que se van a usar, siendo los siguientes:

```arduino
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;
const int buttonPin = 6;
const int left = 7;
const int rigth = 8;
```



Los pines INX son los que se conectan al driver ULN2003A del motor, y los que se declaran como tal en la instancia del motor paso a paso dentro del código. El “buttonPin” es la entrada que recoge las pulsaciones del final de carrera del circuito. Los pines “left” y “rigth” son los que reciben la codificación de órdenes desde la ESP32 Wrover Cam, que hace de pasarela MQTT en este caso.

En este programa encontramos dos funciones principales. Primero, se encuentra recuperarOrigen(), la cual se ejecuta después de las declaraciones pertinentes en el setup() para calibrar la posición de la cámara. Para ello, se hace girar el motor en sentido antihorario, hasta que el final de carrera detecte que la cámara está en la posición que hemos definido como origen. Desde aquí, fijamos la posición actual de la cámara a 0, y así podemos controlar los movimientos de esta fijando el rango de giro entre 0 y 360 grados. El objetivo de este sistema es evitar que la cámara pueda girar más de una vuelta sobre sí misma, para si evitar dañar los conectores que van desde ella hasta el circuito que controla el motor y los demás componentes del sistema de movimiento.

La siguiente función principal es controlMotor(), la cual es llamada cada vez que se detecta un comando de movimiento desde la ESP32 Wrover Cam. En función de la codificación de dicho comando, esta función mueve el motor en un sentido u otro, controlando el rango de movimiento como se ha mencionado anteriormente.

También contamos con un par de funciones auxiliares como son degree2steps() y steps2degree(), las cuales se encargan de hacer la conversión de consigna de ángulo a pasos del motor y viceversa según convenga.

Por último, en el bucle principal del programa se atienden los comandos de movimiento codificados. En función de su codificación, el Arduino Nano llevará órdenes de girar el motor en un sentido u otro, detener el movimiento, o llevar la cámara a su posición inicial.

#### “MANDO_GR10”: MANDO WIFI PARA CONTROLAR LA CÁMARA CON ESP32

Este programa es el encargado de convertir las consignas de los joysticks del mando en comandos para el Arduino Nano que controla el motor de la cámara. Para el desarrollo del proyecto de esta asignatura realmente solo es necesario un joystick para controlar el movimiento de la cámara, sin embargo, el sistema general de la cámara de seguridad va a ser reimplementado en otro proyecto de la asignatura Fundamentos de Robótica, a modo de cámara de control de entorno de un robot móvil. Por ello, el segundo joystick se empleará para mandar las consignas de movimiento del robot móvil, y así contener todo lo necesario para controlar todos los elementos del sistema en un único dispositivo.

Para este programa, primero se declaran todos los topics necesarios, siendo los siguientes:

- Topic publicación comando de cámara: II10/ESP 36221669107172/mando/piero
- Topic publicación comando de robot móvil: II10/ESP 36221669107172/mando/camara
- Topic publicación de conexión: II10/ESP 36221669107172/mando/conexion

Además, se definen también los pines que van a ser usados:

```arduino
const int pinRojo = 5;
const int pinVerde = 18;

const int pinxCamara = 33;
const int pinyCamara = 32;
const int pinButtonCamara = 17;

const int pinxPiero = 34;
const int pinyPiero = 39;
const int pinButtonPiero = 16;
```

El primer grupo de pines son los correspondientes a los leds de indicación del estado de conexión. El segundo, son los correspondientes a los pines de lectura del joystick de la cámara, y el tercero son los correspondientes a los pines de lectura del joystick del robot móvil.

Además de las ya mencionadas y explicadas funciones conectaWifi() y conectamqtt() necesarias para la conexión de la placa, este programa cuenta con una función principal denominada leerJoyStick(), la cual es llamada cada 100 ms en el programa principal para comprobar si la posición de alguno de los dos joysticks del circuito ha cambiado, y así mandar dicha posición en forma de comando por mqtt para actuar en consecuencia. 

## Descripción de los flujos de NodeRed

En NodeRed, tenemos un conjunto de bloques para el manejo de la cámara de seguridad y sus funciones, los cuales se detallarán a continuación.

El primer bloque es el dedicado a la lógica para hacer fotos de forma manual, comando que se puede realizar a través de las tres plataformas ofrecidas al usuario, como son NodeRed, Telegram y el mando wifi. Además, se ofrece al usuario la posibilidad de escoger entre hacer la foto con o sin flash. La foto se realiza mediante una petición http al API “/capture” del servidor local de la cámara, gracias al nodo http request. Dicha foto es devuelta en formato buffer de datos, e introducida en el campo content del mensaje de respuesta al nodo Telegram sender. Además, dicha foto es convertida a formato cadena para su almacenamiento en la base de datos denominada “fotos_manual”. Para añadir robustez al programa, se ha añadido una comprobación para verificar si el intento de hacer la foto se ha llevado a cabo satisfactoriamente o no, valorando si el payload que devuelve el http request devuelve error o no, de cara a no almacenar ni enviar fotos corruptas

<p align="center">
  <img src="/fotos/image-40.png">
</p>

El siguiente bloque es el dedicado a la toma de fotos de seguridad, la cual se hace por defecto cada 60 minutos, tal y como se explicó en su correspondiente programa de Arduino. En el dashboard de NodeRed se dispone de un nodo slider para poder configurar el tiempo de guardado de dichas fotos de seguridad, el cual puede configurarse desde 1 a 60 minutos. Dichas fotos se guardan en una base de datos denominada “fotos_seguridad”. 

<p align="center">
  <img src="/fotos/image-41.png">
</p>

El siguiente bloque es el dedicado a la recuperación de las fotos guardas en las dos bases de datos, en función del botón que use el usuario en la petición desde Telegram. De nuevo, se hace una comprobación del estado de la foto antes de mandarla.

<p align="center">
  <img src="/fotos/image-42.png">
</p>

El siguiente bloque es el dedicado a la gestión de las peticiones de control del motor de la cámara y del flash manual, tanto desde la dashboard de NodeRed como desde los botones disponibles en Telegram.

<p align="center">
  <img src="/fotos/image-43.png">
</p>

El siguiente bloque es el encargado de gestionar el video en streaming en la dashboard de NodeRed. Consta de un nodo template en el que hay un código html básico compuesto por un <div> que encierra el contenido de la /fotos/imagen que transmite en directo la cámara de seguridad, haciendo una petición al servidor que implanta la cámara en el área local. Además, también se gestiona la petición desde Telegram para ver el video en streaming, devolviendo ante dicha petición la URL del servidor donde se encuentra alojado el video en streaming de la cámara.

<p align="center">
  <img src="/fotos/image-44.png">
</p>

Por último, nos encontramos unos nodos auxiliares para hacer un vaciado rápido de las fotos de las bases de datos en caso de que necesitemos hacer una depuración. Estos no se utilizarán normalmente.

<p align="center">
  <img src="/fotos/image-45.png">
</p>

# Robustez y prevención de errores

Consideramos que es importante asegurar la fiabilidad y consistencia del sistema, priorizando la prevención de casos inesperados y la gestión efectiva de situaciones imprevistas. Por esto, se ha dado especial atención a la construcción de un entorno que minimiza la ocurrencia de fallos y errores, garantizando una experiencia óptima para el usuario final gracias a la estabilidad del sistema y una funcionalidad predecible y confiable. Por eso, algunas de las medidas que se han tomado son las siguientes.

- Prevención de inicio de las conexiones infinito: Cuando las placas inician una conexión, ya sea a Wifi o a MQTT puede ocurrir que la conexión no se vaya a establecer correctamente por algún fallo ajeno o en el propio setup del inicio del programa. También cuando la placa híbrida se conecta a wifi para enviar por MQTT los datos que ha recibido por ESP-NOW necesitamos que no haya ningún fallo en esas conexiones.

Por esto, hemos introducido un timeout en las funciones de conexión asegurando que, si se está tardando más de lo normal en conectar a WiFi o se están haciendo muchos intentos fallidos de conexión a MQTT, se haga un reseteo del dispositivo y vuelva a realizar el setup y trate de iniciar de nuevo el proceso de conexión.

- No añadir ni mostrar valores incongruentes en los datos: En algún caso se podría dar que el sensor de temperatura y humedad, por ejemplo, de algún fallo y se pierdan los valores estas variables en algún registro. Si se intenta introducir este valor en la base de datos, podría producir resultados erróneos en algunos cálculos que se basen en estos valores. De igual forma, si ya hubiera un valor incongruente o vacío en la base de datos, no queremos que aparezca en las representaciones ni que se trate de procesar por otros nodos pues podría inducir a errores.

Por tanto, al introducir datos en la base de datos siempre nos aseguraremos de que el campo status del mensaje sea "OK" y nunca “TIMEOUT”. A la hora de mostrarlos si en la base de datos hay algún valor vacío, con el payload = undefined, no queremos que sea tenido en cuenta.

- Optimización a las consultas a las bases de datos: En primera instancia quisimos consultar las bases de datos de dos maneras diferentes, para que se viera indiscutiblemente que, al consultar grandes cantidades de datos, hacerlo con agregaciones es realmente óptima, más que la tradicional de pasar un filtro en el payload del mensaje al nodo de MongoDB. 


Sin embargo, la carga computacional que debía soportar NodeRed era, quizá, demasiado grande. Por eso, finalmente optamos por realizar todas las consultas con grandes cantidades de datos mediante agregaciones. Si el periodo de los datos a consultar cubre más del periodo de 3 días, se hará una agregación por horas y se mostrarán los datos medios de cada hora. Si, por otro lado, la consulta es de un rango de fechas inferior a 3 días, sí que se mostrarán todos los datos, permitiéndonos hacer un análisis más exhaustivo de los datos. 

De esta forma nos evitamos mucha carga computacional al recorrer cantidades grandes de muestras de datos y hacemos que se muestren los valores en las gráficas de forma inmediata.

- Comprobar la actualización firmware de manera remota periódicamente: Como se ha explicado en la explicación del código cargado en la placa principal, cada hora se comprueba si hay alguna actualización disponible para el firmware. De esta forma, sin tener que intervenir de ninguna forma, nos aseguramos de que el dispositivo siempre estará actualizado a la última versión y que todos los errores se puedan corregir de forma remota.

- Mensajes de Telegram: Se ha implementado un aviso por la aplicación de Telegram si se activa o desactiva algún actuador de manera remota o si alguien trata de acceder al domicilio. Para implementar esto, se envía el mensaje a la última persona que haya intercambiado mensajes con el bot (estableciendo su chatId como una variable global dentro del flujo) aunque se podría definir una lista de usuarios que quieran recibir las notificaciones en todo momento.

- Mostrar las fechas de última actualización: Para asegurarnos de que todo está funcionando correctamente, podemos ver en el Dashboard la hora de última actualización de los datos de temperatura, humedad y luminosidad, así como la última actualización de la conexión y la última actualización mediante FOTA.

# Experiencia de usuario

En este proyecto se ha tenido muy en cuenta la experiencia al usuario ya que es crucial que un sistema como el que se plantea sea fácil de utilizar, controlar y gestionar. Por eso, se ha procurado que estas tareas se puedan realizar de una forma cómoda y lo más automatizadas posible.

En primer lugar, el dashboard, que es parte de la interfaz de usuario de NodeRED, trata de abarcar todos los controles y monitorización de datos necesaria para el uso del sistema de seguridad y domotización. Se puede acceder a través del siguiente enlace.

https://infindgrupo10nodered.duckdns.org/ui

Está optimizado para poder utilizarse igualmente desde cualquier dispositivo, incluidos los dispositivos móviles. Sin embargo, también planteamos la posibilidad del control desde una aplicación móvil. Para esto, se puede realizar de dos maneras. La primera es descargando la aplicación Remote-RED, disponible tanto para Android como para iPhone e incluyendo el flujo a la aplicación para poder utilizar el dashboard desde ahí.

La otra opción es ejecutar este atajo (sólo para iPhone) desde el menú de compartir la página del dashboard (el enlace anterior) desde el navegador con la página abierta. 

https://www.icloud.com/shortcuts/addc1a6f689942379920675ab7379163.

Una vez ejecutado el atajo, al darle a “añadir a pantalla de inicio” (en el menú compartir, de nuevo), se añadirá como una aplicación móvil que funciona de manera fluida y con una interfaz amigable.

<p align="center">
  <img src="/fotos/image-46.png">
</p>

Otro gran valor añadido de cara a mejorar el control del sistema es la integración con los asistentes virtuales como Amazon Alexa. Es completamente funcional mediante los comandos de “Enciende Led”, “Apaga Led”, “Activa Alarma”, “Desactiva alarma”, “Enciende Motor” y “Apaga Motor”. Esto es gracias a la librería node-red-contrib-alexa-home-skill y a la configuración en la aplicación de Amazon Alexa. También se destaca, que dentro de esta aplicación tenemos integrados botones de control de estos dispositivos y que cualquier elemento de casa conectada que podamos tener se puede asociar y personalizar la experiencia del usuario al máximo.

<p align="center">
  <img src="/fotos/image-47.png">
</p>

Como se observa en las figuras anteriores, aparece el control de las luces e interruptores en la parte superior, además de las frases que se pueden utilizar para el control. 

Por otro lado, y para aumentar aún más la compatibilidad con todos los dispositivos incluyendo mejoras y funciones automatizadas, se ha implementado la conexión con los atajos de iPhone, permitiendo crear automatizaciones personales y abrir una rama prácticamente infinita de posibilidades con los dispositivos de Apple. Para hacer esto, simplemente se ha añadido una petición HTTP al flujo de uno de red y desde los atajos de iPhone se ha creado un atajo que ejecute esta petición. 

<p align="center">
  <img src="/fotos/image-48.png">
</p>

Por tanto, conseguiríamos activar la alarma entrando siguiente enlace

https://infindgrupo10nodered.duckdns.org/atajo_alarma

El atajo que se ha creado se pone a disposición y el enlace de descarga se encuentra en el apartado de Manual de usuario junto con una pequeña explicación de como añadir una automatización personalizada.

Por último, destacaremos los indicadores led que se han implementado en el hardware y que nos permiten ver que el sistema está funcionando correctamente. Por ejemplo, el motor tiene conectado un led que también varía en intensidad según la intensidad del motor, por lo que podemos ver de forma visual, lo rápido que estaría girando el ventilador. Al usar el teclado numérico de membrana tenemos un led que se enciende verde cuando el código es correcto y rojo cuando es incorrecto. Este mismo led de color rojo parpadea alternándose con uno de los built-in leds de la placa cada vez que esta se está conectando a wifi y enviando un dato recibido por ESP-NOW o bien un pin introducido.

# Resultados y conclusiones

La realización de este proyecto centrado en la implementación de un programa de seguridad para el hogar basado en la Internet de las Cosas (IoT) nos ha proporcionado valiosas perspectivas y resultados significativos que merecen ser destacados. 

1.	Avances en la Seguridad Residencial con IoT: Se ha logrado con un éxito rotundo la integración de dispositivos y sensores inteligentes en el entorno doméstico y ha demostrado ser una estrategia eficaz para fortalecer la seguridad en un domicilio o una empresa. La capacidad de detectar, alertar y responder automáticamente a eventos críticos, como fugas de gas, humos o intrusiones, abre nuevas posibilidades para la protección del hogar y la seguridad de sus habitantes.

2.	Personalización y Adaptabilidad: Lo que hemos tratado de conseguir con nuestro sistema es que puedes hacerlo tuyo al cien por cien. Es como tener un traje hecho a medida, pero en lugar de ropa, estamos hablando de la seguridad de tu casa. Tú decides cómo y cuándo quieres que te avise si pasa, y ajustas todo para que se ajuste a tu vida y a lo que necesitas.

Con Alexa, el usuario disfruta de la simplicidad y la naturalidad de la interacción por voz, pudiendo controlar dispositivos y consultar información sin interrumpir sus actividades diarias. El Bot de Telegram añade una capa adicional de accesibilidad y seguridad, facilitando el control remoto y la recepción de notificaciones instantáneas sobre eventos del hogar, todo ello dentro de una aplicación de mensajería ampliamente utilizada y apreciada por su encriptación de extremo a extremo. Esta funcionalidad asegura que los usuarios estén siempre informados y en control, incluso cuando no están presentes físicamente en su hogar. 

El dashboard proporciona una visión completa y en tiempo real del estado de la casa. A través de una interfaz gráfica intuitiva y fácil de usar, los usuarios pueden monitorear y controlar los dispositivos conectados, verificar registros de seguridad, estado del ambiente y otros datos relevantes que les permiten tomar decisiones informadas sobre la gestión de su vivienda.

3.	Desafíos de Ciberseguridad y Privacidad: Con el aumento de dispositivos que se manejan desde fuera de casa, la seguridad informática y la privacidad se han vuelto super importantes. Es fundamental proteger nuestros datos y evitar ataques cibernéticos, y eso es algo imprescindible para que todos se sientan cómodos y confiados al usar la tecnología del hogar inteligente.

El empleo de una Raspberry Pi como servidor local en nuestro proyecto de domótica representa una elección estratégica para fortalecer la ciberseguridad y la privacidad. Al ser un centro de procesamiento y almacenamiento de datos en el entorno del hogar, se elimina la necesidad de depender solo de la nube y de terceros, lo que reduce significativamente la exposición a vulnerabilidades externas y ataques cibernéticos ya que localmente hemos hecho uso de medios como NginxProxyManager para proteger la exposición al exterior de nuestra red local. 

4.	Interconexión para una Respuesta Coordinada: La capacidad de interconectar dispositivos y sensores no solo mejora la eficiencia del sistema, sino que también facilita una respuesta coordinada ante situaciones críticas. No solo hacemos que todo funcione mejor y más rápido, sino que también conseguimos que el sistema reaccione en conjunto ante cualquier problema. Esto significa que podemos manejar todo desde un solo lugar y tomar decisiones al instante, lo que hace que todo el sistema de seguridad sea más efectivo.

5.	Potencial para la Evolución Continua: Este proyecto sienta las bases para la evolución continua de la seguridad residencial con la integración de IoT. Las posibilidades de mejora y expansión son vastas, desde la integración de tecnologías emergentes hasta la implementación de algoritmos más avanzados para el análisis de datos y la toma de decisiones automatizada.

En conjunto, esta integración de tecnologías en nuestro proyecto no solo prioriza la comodidad y la eficiencia, sino que también enfatiza la importancia de la seguridad y la privacidad, creando una experiencia de usuario que es profundamente personalizable, segura y cómoda.

# Manual de usuario
## Control mediante Telegram

Una de las partes importantes de nuestro proyecto es la utilización de Telegram para el control de los diferentes actuadores y algunas consultas sobre el ultimo acceso o la temperatura, humedad y luminosidad del ambiente, además del control de la cámara de vigilancia. Todo esto realiza gracias a un bot al que hemos llamado Ultrahouse3000, accesible al usuario mediante el siguiente enlace.

https://t.me/Ultrahouse3000bot

<p align="center">
  <img src="/fotos/image-49.png">
</p>

Una vez iniciamos el bot, este nos saluda y nos ofrece un menú de opciones con el que elegir a qué parte del control de la vivienda queremos acceder.

En función de si entramos al menú de Control Domótica o Seguridad del Hogar, obtendremos las acciones disponibles a realizar en cada caso, siempre teniendo la opción de regresar al menú principal haciendo uso del botón “Volver”.

<p align="center">
  <img src="/fotos/image-50.png">
</p>

### Opciones disponibles del menú de domótica

A continuación, se detallan todas las opciones de control disponibles para el usuario, siendo estas:

•	Encender/Apagar las luces led remotamente
<p align="center">
  <img src="/fotos/image-51.png">
</p>

•	Activar/Desactivar la alarma remotamente. Si los sensores detectan movimiento cuando la alarma está activa, nuestro bot avisa de que tenemos un intruso en casa, dándonos a elegir las opciones de acción a tomar.
<p align="center">
  <img src="/fotos/image-52.png">
</p>

•	Comprobar los datos de temperatura, humedad y luminosidad del hogar actualmente.

<p align="center">
  <img src="/fotos/image-53.png">
</p>

•	Controlar la velocidad del ventilador remotamente, teniendo la posibilidad de asignarle una velocidad mínima, media o máxima, y pararlo.

<p align="center">
  <img src="/fotos/image-54.png">
</p>

•	Consultar el último acceso a la vivienda. Además, si alguien intenta entrar en nuestra propiedad e introduce incorrectamente la clave de acceso, nuestro bot nos avisará al instante de este suceso y nos dará a elegir una serie de acciones a tomar en consecuencia, tales como enviarnos una foto tomada desde la cámara de seguridad, activar (o desactivar si ya se encuentra activa) la alarma o consultar el último acceso. Sin embargo, al introducir correctamente la clave de acceso, nuestro bot nos dará la bienvenida, nos ofrecerá activar o desactivar la alarma y encender el ventilador para refrescar el ambiente.

<p align="center">
  <img src="/fotos/image-55.png">
</p>

### Opciones disponibles del menú de seguridad del hogar

A continuación, se detallan todas las opciones de control disponibles para el usuario, siendo estas:

•	Ver el video en directo de la cámara de seguridad. Para ello, se nos devuelve un enlace al servidor donde se está retransmitiendo dicho video, donde se requiere introducir las credenciales de acceso para acceder al mismo, medida de seguridad impuesta de cara a que solo nosotros podamos acceder a él.
<p align="center">
  <img src="/fotos/image-56.png">
</p>

•	Hacer fotos con la cámara de seguridad con o sin flash. Además, se ofrece la posibilidad de recuperar tanto la última foto realizada manualmente como la última foto realizada automáticamente.

<p align="center">
  <img src="/fotos/image-57.png">
</p>

•	Activar o desactivar remotamente el flash de la cámara. En el caso de que se requiera hacer una foto con o sin flash, la cámara encenderá o apagará el flash en función de su estado actual, recuperando finalmente el estado inicial del flash manual configurado mediante los botones.

<p align="center">
  <img src="/fotos/image-58.png">
</p>

•	Control de la posición de la cámara. Para ello, se dispone de los botones que hacen que la cámara rote hacia la derecha o hacia la izquierda, deteniendo su movimiento con la pulsación del botón “Stop”. Además, siempre se puede devolver la cámara a su posición inicial haciendo uso del botón “Centrar”.

<p align="center">
  <img src="/fotos/image-59.png">
</p>

## Control mediante el Dashboard de NodeRed

Para este proyecto, un Dashboard amigable es uno de los requisitos indispensables si queremos que el usuario se encuentre cómodo usando nuestro sistema. Para ello hemos implementado el Dashboard con diferentes secciones para que sea lo más intuitivo y organizado posible.

<p align="center">
  <img src="/fotos/image-60.png">
</p>

### Panel General

En esta sección encontramos un resumen de todo lo que abarca nuestro sistema:

•	En la primera columna tenemos la parte de control de los actuadores, donde podemos cambiar la velocidad del motor, seleccionar la temperatura a la saltará el motor (simulando un ventilador) y el switch que apaga y enciende las luces LED

•	En la segunda columna se encuentra el estado de todos los sensores. Los sensores de gas y de humo cambian el color del indicador cuando estos detectan gas y humo respectivamente, en función de si las medidas sensoriales de estos sobrepasan el umbral al que se han ajustado. El led de intruso se activa si la alarma está armada y se detecta movimiento. 

•	En la tercera columna podemos ver información que referente al estado de la placa principal, encontrado datos tales como la última actualización de los sensores, el tiempo de funcionamiento de la placa, etc. También podemos ver si la alarma está conectada o no. 

•	En la cuarta columna aparece reflejado un resumen de las variables atmosféricas actuales dentro de la vivienda.

<p align="center">
  <img src="/fotos/image-61.png">
</p>

### Panel de Seguridad

En esta sección encontramos toda la información referente al estado de la seguridad de nuestra vivienda:

•	En la primera columna encontramos el historial de la evolución de los sensores de humo y gas y el sensor de movimiento. El alcance de la visualización de se configura con los botones de la parte superior, y el intervalo de fechas de los datos a visualizar se configura usando los selectores disponibles. Seguidamente, encontramos un resumen de los datos en forma de lista, con la fecha y el valor leído de los sensores.

•	En la segunda columna encontramos toda la información referente al historial del control de acceso a la vivienda, el cual nos permite conocer el último acceso a la casa solo si conocemos el pin de la alarma. 

<p align="center">
  <img src="/fotos/image-62.png">
</p>

### Panel de datos del ambiente

En esta sección encontramos toda la información referente al ambiente de la vivienda:

•	La primera columna es la dedicada principalmente a la gestión de la visualización de los datos. En ella, podemos ver un resumen de los datos ambientales recogidos por nuestros sensores, configurar el intervalo de tiempo de datos a mostrar en las siguientes columnas, y la opción de descargar todos los datos recogidos en un fichero de extensión .csv haciendo uso de un botón 

•	En la segunda columna encontramos una gráfica que reúne los datos con la evolución en el tiempo de la temperatura y la humedad en la vivienda

•	En la tercera columna encontramos todos los datos referentes al ambiente de la vivienda en gráficas individuales.

•	En la cuarta columna encontramos de nuevo todos los datos referentes al ambiente de la vivienda en gráficas individuales, conteniendo cada gráfica los valores mínimo, medio y máximo de cada magnitud.

•	En la quinta columna encontramos el registro de todos los datos recibidos por los sensores en tiempo real.

<p align="center">
  <img src="/fotos/image-63.png">
</p>

### Panel de la cámara de seguridad

En esta sección encontramos el sistema de visualización y control de la cámara:

•	En la primera columna encontramos el video en de la cámara de seguridad.

•	En la segunda columna encontramos todos los elementos de control de la cámara. Con los botones disponibles, podemos controlar la orientación de la cámara, encender o apagar el flash, mandar una foto a Telegram, y configurar el tiempo entre tomas de seguridad que por defecto es 60 minutos.

<p align="center">
  <img src="/fotos/image-64.png">
</p>

## Control mediante asistentes virtuales

### Uso de Amazon Alexa

1.	Control de Actuador LED: La integración permite a los usuarios emitir comandos de voz como "Alexa, enciende led" o "Alexa, apaga led". Estos comandos serían formateados por Node-RED para ser comprendidos por el actuador LED, que ejecuta la acción correspondiente. Por ejemplo, si el comando es "encender", Node-RED enviaría una señal al módulo ESP conectado al LED para que se active.

2.	Sistema de Alarma: Para el sistema de alarma, los comandos pueden estructurarse para la activación y desactivación, tales como "Alexa, activa la alarma" o "Alexa, desactiva la alarma". Node-RED traduciría estos comandos de voz en señales apropiadas que el sistema de alarma pueda entender, permitiendo así activar o desactivar la alarma según se indique.

3.	Dispositivo Motor: El control del dispositivo motor es un poco más complejo, ya que permite no solo el control binario de encendido/apagado, sino también el control basado en porcentajes. Esto significa que un usuario puede emitir comandos como "Alexa, activa motor a la mitad" o "Alexa, ajusta la velocidad del motor al 75%". Node-RED formatearía estos comandos en una señal que ajuste la operación del motor al porcentaje especificado, para controlar la velocidad de un ventilador o cualquier otro dispositivo motorizado.

Para la integración con Alexa se ha empleado como librería de Node-Red: “node-red-contrib-alexa-home-skill”.  Para poder emplear nodos que se conecten a nuestros dispositivos de casa, tenemos que registrarlos accediendo al siguiente enlace o bien escaneando el código QR siguiente.

<p align="center">
  <img src="/fotos/image-65.png">
</p>

https://alexa-node-red.bm.hardill.me.uk/

Ahí, podemos gestionar los dispositivos que tenemos en casa. Con esta librería, conseguimos comunicar los dispositivos de Node-Red con nuestra cuenta personal de Alexa y el dispositivo Amazon echo.

### Uso de atajo de iPhone para la activación de la alarma

Para comenzar a utilizar esta funcionalidad habrá que definir un atajo y crear una automatización a partir de él. En primer lugar, descargamos el atajo desde el siguiente enlace: 

https://www.icloud.com/shortcuts/f43c0bdc7a494f7c88358459005c39c6 



Al añadirlo, aparecen los cuadros de dialogo de las figuras de la derecha. Simplemente se deben seguir los pasos indicados para la configuración del entorno, y finalmente añadir el atajo. En este caso, el token del bot de Telegram ya va añadido por defecto.

<p align="center">
  <img src="/fotos/image-66.png">
</p>

Una vez se tenga añadido el atajo, configurado con su respectivo chatID y nombre, se debe crear una nueva automatización personal en la app de atajos como la siguiente, tal y como se ve en las siguientes figuras.

<p align="center">
  <img src="/fotos/image-67.png">
</p>

Al crear esta nueva automatización personal, se seleccionará la opción salir. En el apartado de ubicación, se debe seleccionar la ubicación de “casa” o del sitio donde se encuentre el sistema de alarma. A continuación, pulse en siguiente y defina una automatización similar a la que aparece en la figura 72. El único matiz indispensable que debe tener en cuenta a la hora de configurar la automatización es el de ejecutar el atajo “Activar Alarma Ultrahouse”, pero se puede añadir además todas las automatizaciones que se deseen.
También se puede configurar el atajo para su uso con la función nativa de IOS “Oye, Siri”. Para ello, simplemente ejecute comandos de voz tales como “Oye Siri, ejecuta el atajo Activar Alarma Ultrahouse”.

Finalmente, solo resta asegurarnos de que la automatización está activada. Si es así, a partir de ese momento nos llegará un mensaje del bot de Telegram cada vez que salgamos de casa recordando el encendido de la alarma, quedando el sistema de seguridad automáticamente habilitado, tal y como se ve en la siguiente figura.

<p align="center">
  <img src="/fotos/image-68.png">
</p>

