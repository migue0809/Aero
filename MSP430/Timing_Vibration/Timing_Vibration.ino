#include <SPI.h>
#include <Enrf24.h>
#include <nRF24L01.h>
#include <string.h>

Enrf24 radio(P2_0, P2_1, P2_2);  // P2.0=CE, P2.1=CSN, P2.2=IRQ

const uint8_t txaddr[] = { 0xE7, 0xD3, 0xF0, 0x35, 0xC2 };
const uint8_t rxaddr[] = { 0xE7, 0xD3, 0xF0, 0x35, 0xFF };

unsigned long prev_time;
char *rpm ;
unsigned int rpmNum;

boolean sent=false;

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

void setup()
{
  Serial.begin(9600);
  //Serial.flush();
  //SPI.setModule(2);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(1);
  
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setRXaddress((void*)rxaddr);  
  
  dump_radio_status_to_serialport(radio.radioState()); 
  radio.enableRX();  // Start listening
  delay(100);
}

void loop() {
  rxReed();
  
  radio.end();
  sleep(100);
  
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setTXaddress((void*)txaddr);
  
  txAcc();
  radio.end();
  delay(100);
  
  radio.begin(1000000);  // Defaults 1Mbps, channel 0, max TX power
  radio.setChannel(76);
  radio.setCRC(1,1); 
  radio.setRXaddress((void*)rxaddr);  
  radio.enableRX();  // Start listening
  delay(100);
  
}

void rxReed() {
  char inbuf[33];  
  delta_set();
  
  while (!radio.available(true)&& delta_get()<1000);
   
  if (radio.read(inbuf)) {
      Serial.println(inbuf);
      rpm = strtok(inbuf, "r");
      radio.print(rpm);
      radio.flush();
  }

}

void delta_set() {
  prev_time = millis();
}

// TimeOut  
unsigned long delta_get() {
  unsigned long time;
  unsigned long delta;

  time = millis();
  if (time < prev_time) { // TimeOut
    delta = 0xffffffff - prev_time + time + 1;
  } 
  else {
    delta = time - prev_time;
  }
  return delta;
}

void dump_radio_status_to_serialport(uint8_t status)
{
  Serial.print("Enrf24 radio transceiver status: ");
  switch (status) {
    case ENRF24_STATE_NOTPRESENT:
      Serial.println("NO TRANSCEIVER PRESENT");
      break;

    case ENRF24_STATE_DEEPSLEEP:
      Serial.println("DEEP SLEEP <1uA power consumption");
      break;

    case ENRF24_STATE_IDLE:
      Serial.println("IDLE module powered up w/ oscillators running");
      break;

    case ENRF24_STATE_PTX:
      Serial.println("Actively Transmitting");
      break;

    case ENRF24_STATE_PRX:
      Serial.println("Receive Mode");
      break;

    default:
      Serial.println("UNKNOWN STATUS CODE");
  }
}

void txAcc(){
   
   unsigned long startAcc = micros();
   
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
      
      // Calcula tiempo
     unsigned long endMeasure = micros();
     unsigned long deltaMeasure = end - start;
     Serial.print("Calcula la velocidad con una ventana de: (en microseg) ");
     Seial.println(deltaMeasure);
      
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
      
      // Calcula tiempo que tarda en enviar
     unsigned long endSend = micros();
     unsigned long delta = endSend - endMeasure;
     Serial.print("Tarda en enviar: (en microseg) ");
     Serial.println(delta)
      
      sleep(500);
      
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
      
      //Calcula tiempo de descanso
     unsigned long endIter = micros();
     unsigned long deltaIter = endIter - endSend;
     Serial.print("Tarda en enviar: (en microseg) ");
     Serial.println(deltaIter)
}
