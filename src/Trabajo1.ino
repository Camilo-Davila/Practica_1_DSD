#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>

//*******Pantalla****/////
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();

//*******Sensor de temperatura*****//
#include "DHT.h"

#define DHTPIN 27//definicion del pin al que se conecta el sensor 
#define DHTTYPE DHT11//definir el tipo de dht

DHT dht(DHTPIN,DHTTYPE); //constructor

/****************************************
 * Define Constants
 ****************************************/
const char *UBIDOTS_TOKEN = "BBFF-j1TruHugbGjhYBFo9QxRdBRSET2lTo";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "UPBWiFi";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "Temperatura"; // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL2 = "Humedad"; // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 5000; // Update rate in milliseconds

unsigned long timer;
//uint8_t analogPin = 33; // Pin used to read data from GPIO34 ADC_CH6.

Ubidots ubidots(UBIDOTS_TOKEN);

/****************************************
 * Auxiliar Functions
 ****************************************/

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
 * Main Functions
 ****************************************/

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
//Sensor init
  dht. begin();

//Pantalla init
  tft.init();
  //Con este código cambio el color del fondo de la pantalla
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  //Código para imprimir un texto en la pantalla, el primer argumento es la posición x, el segundo es la posición y, el tercero es el tamaño de la fuente
  tft.setTextColor(TFT_BLUE);
  tft.drawString("TRABAJO 1",50,50,4);//X, Y, FONT
  //tft.drawString("Hola Mundo",10,120,2);//EL 2 es la fuente
  
  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

//int i=0;

void loop()
{
  // put your main code here, to run repeatedly:

  delay (2000); //reatrdo de 2seg
  float h= dht.readHumidity(); //lee la humedad en fnc read y la almacena en h 
  float t= dht.readTemperature(); //lee la temperatura en fnc read y la almacena en t 
  if (isnan(h) || isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!")); //isnan nos devuelve un 1 en caso de ue exista un fallo o un error en la lectura de la vble
    return; 
  }
  
  //tft.drawString(String(i), 30, 50, 7);
  //i++;
  //delay(100);
  
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) // triggers the routine every 5 seconds
  {
    //float value = analogRead(analogPin);
    //Serial.print(value);
    //Serial.print();
    ubidots.add(VARIABLE_LABEL, t); // Insert your variable Labels and the value to be sent
    ubidots.add(VARIABLE_LABEL2, h); 
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_CYAN);
  tft.drawString("Humedad (%):",5,0,2);//X, Y, FONT
  tft.drawString(String(h), 90, 15, 7);

  tft.setTextColor(TFT_RED);
  tft.drawString("Temperatura(°C):",5,65,2);//X, Y, FONT
  tft.drawString(String(t), 90, 80, 7);

  
  
}
