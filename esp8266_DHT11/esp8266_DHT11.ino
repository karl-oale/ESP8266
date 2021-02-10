#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <Ticker.h>
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin D3
DHT dht(dht_dpin, DHTTYPE); 
Ticker ticker;
float h;
float t;
void setup(void)
{ 
  dht.begin();
  Serial.begin(115200);
  Serial.println("Humidity and temperature\n\n");
  ticker.attach(6,tt);
}

void loop() {
    h = dht.readHumidity();
    t = dht.readTemperature();
    delay(1000);
}

void tt(){
           
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.println("C  ");
}
