#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = { 0xE7, 0xD3, 0xF0, 0x35, 0xC2 };
 
float sumax = 0;
float sumay = 0;
float sumaz = 0;
float promediox;
float promedioy;
float promedioz;
int MaxX = 0; 
int MaxY = 0; 
int MaxZ = 0; 
int MinX = 1024;
int MinY = 1024;
int MinZ = 1024;
int xvalue;        // value read from xpin
int yvalue;        // value read from ypin
int zvalue;        // value read from zpin

void setup() {
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1); // MSB-first

  
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setTXaddress((void*)txaddr);
}
 
void loop() {
   for (int i=0; i<100; i++) {
       xvalue= analogRead(A0);
       yvalue= analogRead(A3);
       zvalue= analogRead(A4);
       
            sumax += xvalue/100.000;
            sumay += yvalue/100.000;
            sumaz += zvalue/100.000;
           
       
                  if  (xvalue > MaxX) {
                     MaxX = xvalue;
                       }    
                  if  (xvalue < MinX) {
                     MinX = xvalue;
                       }
                  if  (yvalue > MaxY) {
                     MaxY = yvalue;
                       }    
                  if  (yvalue < MinY) {
                     MinY = yvalue;
                       }
                  if  (zvalue > MaxZ) {
                     MaxZ = zvalue;
                       }    
                  if  (zvalue < MinZ) {
                     MinZ = zvalue;
                       }
       radio.flush();
       sleep(10);
       
}

  promediox = sumax;
  promedioy = sumay;
  promedioz = sumaz;

  
       radio.print("X"); 
       radio.print(int(promediox));
       radio.print(int(MaxX));
       radio.print(int(MinX));  
       radio.print("Y");
       radio.print(int(promedioy));
       radio.print(int(MaxY));  
       radio.print(int(MinY));  
       radio.print("Z");
       radio.print(int(promedioz));
       radio.print(int(MaxZ));
       radio.print(int(MinZ));
       radio.print("F");

        radio.flush();
        sleep(500);
        radio.deepsleep();
        sleep(500);
  
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setTXaddress((void*)txaddr);
       
  sumax = 0;
  MaxX = 0; 
  MinX = 1024;

  sumay = 0;
  MaxY = 0; 
  MinY = 1024;

  sumaz = 0;
  MaxZ = 0; 
  MinZ = 1024;

  sleep(1000);
}
