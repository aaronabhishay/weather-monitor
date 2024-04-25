#include <WiFiMulti.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

// ThingSpeak Credentials
const char* thingspeak_server = "api.thingspeak.com"; //ThingSpeak API Server
String apiKey = ""; // Write API Key
WiFiClient client;

// Network credentials
const char* ssid     =  ""; 
const char* password = "";


float bmp_pressure, bmp_altitude, dht_temperature, dht_humidity;


Adafruit_BMP280 bmp; // Declare the BMP280 object
DHT dht_sensor(23, DHT22); // Declare the DHT22 object with pin 23

void read_sensor() {
  // Get latest sensor readings
  bmp_pressure = bmp.readPressure() / 100.0F; // Convert pressure to hPa
  bmp_altitude = bmp.readAltitude(1013.25);

  // Read DHT22 sensor
  dht_temperature = dht_sensor.readTemperature();
  dht_humidity = dht_sensor.readHumidity();
  Serial.println("Values read");
}

void sendToCloud() //Send data to ThingSpeak Cloud
{
  if(client.connect(thingspeak_server,80)){
    String postStr = apiKey;

    postStr = "&field1=";
    postStr += String(dht_temperature);
    postStr += "&field2=";
    postStr += String(dht_humidity);
    postStr += "&field3=";
    postStr += String(bmp_pressure);
    postStr += "&field4=";
    postStr += String(bmp_altitude);


    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("Success");
  }

}


void setup() {
  Serial.begin(115200);

  //Connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("CONNECTED");

  
  // Initialize BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Initialize DHT sensor
  dht_sensor.begin();

}

void loop() {

  read_sensor();
  sendToCloud();
  // If there is no Wifi signal, try to reconnect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

  // Wait for 10 seconds
  Serial.println("Waiting for 10 seconds");
  delay(10000);

}
