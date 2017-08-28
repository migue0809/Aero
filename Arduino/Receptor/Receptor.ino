#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <stdlib.h>

char text[255] = {0};
RF24 radio(9, 10);

const uint64_t pipes[2] = {0xE7D3F035FF,  0xE7D3F035C2 }; 
void setup()
{
  while (!Serial);
  Serial.begin(9600);
  
  radio.begin();
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(76);
  radio.setPALevel(RF24_PA_MAX);  
  radio.enableDynamicPayloads();  
  radio.setCRCLength(RF24_CRC_16);  
  
  radio.openReadingPipe(1, pipes[0]);
  radio.openReadingPipe(2, pipes[1]);
  
  radio.startListening();
}

void loop(){
if (radio.available()){
    radio.read(&text, sizeof(text));   
    Serial.println(text);
   }
}
