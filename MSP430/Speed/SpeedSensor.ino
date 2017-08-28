#include <SPI.h>
#include <string.h>
#include <Enrf24.h>

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ
const uint8_t txaddr[] = { 0xE7, 0xD3, 0xF0, 0x35, 0xFF };
 
const int buttonPin = 2;  // the number of the pushbutton pin
volatile unsigned int rpmcount = 0;
unsigned int rpm = 0;
unsigned int prpm =0;
unsigned long timeold = 0;
int x = 0;
String str = "r";

void rpm_fun(){
  rpmcount++;  
} 
 
void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  
  attachInterrupt(buttonPin, rpm_fun, FALLING);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1); // MSB-first
 
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setTXaddress((void*)txaddr);
}
 
void loop() {
   x= 0;
   const unsigned long oneSecond = 1 * 1000UL;
   static unsigned long lastSampleTime = 0 - oneSecond;  
   while(x<120){
      unsigned long now = millis();
      if (now - lastSampleTime >= oneSecond){
       lastSampleTime += oneSecond;
       detachInterrupt(buttonPin);
       rpm = rpm + (60*rpmcount);
       rpmcount = 0;  
       attachInterrupt(buttonPin, rpm_fun, RISING); 
       x++;           
     }  if(x == 59){
           prpm = prpm + (rpm/60);
           rpm = 0;   
       } 
     }
     prpm = prpm + (rpm/60); 
     rpm= 0;
     radio.print("r");
     radio.print(prpm/2);
     radio.print("r");
     radio.flush(); 
     prpm = 0; 
     sleep(1000);
}
