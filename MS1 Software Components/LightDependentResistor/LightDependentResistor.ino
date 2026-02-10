#include <Arduino.h>
#include <Wire.h>
#include <ArtronShop_BH1750.h>

ArtronShop_BH1750 ldr2(0x23, &Wire);
ArtronShop_BH1750 ldr3(0x23, &Wire);
ArtronShop_BH1750 ldr4(0x23, &Wire);
ArtronShop_BH1750 ldr5(0x23, &Wire);

void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(0x70);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setupLDR(ArtronShop_BH1750 ldr, uint8_t tcachannel) {
  tcaselect(tcachannel);
  Serial.print(tcachannel);
  if (ldr.begin()) {
    Serial.println(F("> BH1750 Advanced begin"));
  }
  else {
    Serial.println(F("> Error initialising BH1750"));
  }
}

void printLightData(ArtronShop_BH1750 ldr, uint8_t tcachannel) {
  tcaselect(tcachannel);
  float lux = ldr.light();
  String message = "(" + String(tcachannel) + ") Light: " + String(lux) + " lx";
  Serial.println(message);
}

void setup() {
  Serial.begin(9600);

  Wire.begin();
  setupLDR(ldr2, 2);
  setupLDR(ldr3, 3);
  setupLDR(ldr4, 4);
  setupLDR(ldr5, 5);
}

void loop() {
  printLightData(ldr2, 2);
  printLightData(ldr3, 3);
  printLightData(ldr4, 4);
  printLightData(ldr5, 5);
  delay(1000);
}
