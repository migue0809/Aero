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
  
  Serial.begin(9600);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1); // MSB-first
 
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setTXaddress((void*)txaddr);
}
 
void loop() {
   unsigned long start = micros();
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
     
     // Calcula tiempo
     unsigned long endMeasure = micros();
     unsigned long deltaMeasure = end - start;
     Serial.print("Calcula la velocidad con una ventana de: (en microseg) ");
     Seial.println(deltaMeasure);
     
     radio.print("r");
     radio.print(prpm/2);
     radio.print("r");
     radio.flush(); 
     prpm = 0; 
     
     // Calcula tiempo que tarda en enviar
     unsigned long endSend = micros();
     unsigned long delta = endSend - endMeasure;
     Serial.print("Tarda en enviar: (en microseg) ");
     Serial.println(delta)
     sleep(1000);
     
     //Calcula tiempo de descanso
     unsigned long endIter = micros();
     unsigned long deltaIter = endIter - endSend;
     Serial.print("Tarda en enviar: (en microseg) ");
     Serial.println(deltaIter)
}
